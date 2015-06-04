/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Logger.h"
#include "Util.h"
#include "XmlDom.h"
#include "XmlPullParser.h"

#include <cassert>
#include <memory>
#include <stack>
#include <string>
#include <tuple>

namespace aapt {
namespace xml {

constexpr char kXmlNamespaceSep = 1;

struct Stack {
    std::unique_ptr<xml::Node> root;
    std::stack<xml::Node*> nodeStack;
    std::u16string pendingComment;
};

/**
 * Extracts the namespace and name of an expanded element or attribute name.
 */
static void splitName(const char* name, std::u16string* outNs, std::u16string* outName) {
    const char* p = name;
    while (*p != 0 && *p != kXmlNamespaceSep) {
        p++;
    }

    if (*p == 0) {
        outNs->clear();
        *outName = util::utf8ToUtf16(name);
    } else {
        *outNs = util::utf8ToUtf16(StringPiece(name, (p - name)));
        *outName = util::utf8ToUtf16(p + 1);
    }
}

static void addToStack(Stack* stack, XML_Parser parser, std::unique_ptr<Node> node) {
    node->lineNumber = XML_GetCurrentLineNumber(parser);
    node->columnNumber = XML_GetCurrentColumnNumber(parser);

    Node* thisNode = node.get();
    if (!stack->nodeStack.empty()) {
        stack->nodeStack.top()->addChild(std::move(node));
    } else {
        stack->root = std::move(node);
    }

    if (thisNode->type != NodeType::kText) {
        stack->nodeStack.push(thisNode);
    }
}

static void XMLCALL startNamespaceHandler(void* userData, const char* prefix, const char* uri) {
    XML_Parser parser = reinterpret_cast<XML_Parser>(userData);
    Stack* stack = reinterpret_cast<Stack*>(XML_GetUserData(parser));

    std::unique_ptr<Namespace> ns = util::make_unique<Namespace>();
    if (prefix) {
        ns->namespacePrefix = util::utf8ToUtf16(prefix);
    }

    if (uri) {
        ns->namespaceUri = util::utf8ToUtf16(uri);
    }

    addToStack(stack, parser, std::move(ns));
}

static void XMLCALL endNamespaceHandler(void* userData, const char* prefix) {
    XML_Parser parser = reinterpret_cast<XML_Parser>(userData);
    Stack* stack = reinterpret_cast<Stack*>(XML_GetUserData(parser));

    assert(!stack->nodeStack.empty());
    stack->nodeStack.pop();
}

static bool lessAttribute(const Attribute& lhs, const Attribute& rhs) {
    return std::tie(lhs.namespaceUri, lhs.name, lhs.value) <
            std::tie(rhs.namespaceUri, rhs.name, rhs.value);
}

static void XMLCALL startElementHandler(void* userData, const char* name, const char** attrs) {
    XML_Parser parser = reinterpret_cast<XML_Parser>(userData);
    Stack* stack = reinterpret_cast<Stack*>(XML_GetUserData(parser));

    std::unique_ptr<Element> el = util::make_unique<Element>();
    splitName(name, &el->namespaceUri, &el->name);

    while (*attrs) {
        Attribute attribute;
        splitName(*attrs++, &attribute.namespaceUri, &attribute.name);
        attribute.value = util::utf8ToUtf16(*attrs++);

        // Insert in sorted order.
        auto iter = std::lower_bound(el->attributes.begin(), el->attributes.end(), attribute,
                                     lessAttribute);
        el->attributes.insert(iter, std::move(attribute));
    }

    el->comment = std::move(stack->pendingComment);
    addToStack(stack, parser, std::move(el));
}

static void XMLCALL endElementHandler(void* userData, const char* name) {
    XML_Parser parser = reinterpret_cast<XML_Parser>(userData);
    Stack* stack = reinterpret_cast<Stack*>(XML_GetUserData(parser));

    assert(!stack->nodeStack.empty());
    stack->nodeStack.top()->comment = std::move(stack->pendingComment);
    stack->nodeStack.pop();
}

static void XMLCALL characterDataHandler(void* userData, const char* s, int len) {
    XML_Parser parser = reinterpret_cast<XML_Parser>(userData);
    Stack* stack = reinterpret_cast<Stack*>(XML_GetUserData(parser));

    if (!s || len <= 0) {
        return;
    }

    // See if we can just append the text to a previous text node.
    if (!stack->nodeStack.empty()) {
        Node* currentParent = stack->nodeStack.top();
        if (!currentParent->children.empty()) {
            Node* lastChild = currentParent->children.back().get();
            if (lastChild->type == NodeType::kText) {
                Text* text = static_cast<Text*>(lastChild);
                text->text += util::utf8ToUtf16(StringPiece(s, len));
                return;
            }
        }
    }

    std::unique_ptr<Text> text = util::make_unique<Text>();
    text->text = util::utf8ToUtf16(StringPiece(s, len));
    addToStack(stack, parser, std::move(text));
}

static void XMLCALL commentDataHandler(void* userData, const char* comment) {
    XML_Parser parser = reinterpret_cast<XML_Parser>(userData);
    Stack* stack = reinterpret_cast<Stack*>(XML_GetUserData(parser));

    if (!stack->pendingComment.empty()) {
        stack->pendingComment += '\n';
    }
    stack->pendingComment += util::utf8ToUtf16(comment);
}

std::unique_ptr<Node> inflate(std::istream* in, SourceLogger* logger) {
    Stack stack;

    XML_Parser parser = XML_ParserCreateNS(nullptr, kXmlNamespaceSep);
    XML_SetUserData(parser, &stack);
    XML_UseParserAsHandlerArg(parser);
    XML_SetElementHandler(parser, startElementHandler, endElementHandler);
    XML_SetNamespaceDeclHandler(parser, startNamespaceHandler, endNamespaceHandler);
    XML_SetCharacterDataHandler(parser, characterDataHandler);
    XML_SetCommentHandler(parser, commentDataHandler);

    char buffer[1024];
    while (!in->eof()) {
        in->read(buffer, sizeof(buffer) / sizeof(buffer[0]));
        if (in->bad() && !in->eof()) {
            stack.root = {};
            logger->error() << strerror(errno) << std::endl;
            break;
        }

        if (XML_Parse(parser, buffer, in->gcount(), in->eof()) == XML_STATUS_ERROR) {
            stack.root = {};
            logger->error(XML_GetCurrentLineNumber(parser))
                    << XML_ErrorString(XML_GetErrorCode(parser)) << std::endl;
            break;
        }
    }

    XML_ParserFree(parser);
    return std::move(stack.root);
}

static void copyAttributes(Element* el, android::ResXMLParser* parser) {
    const size_t attrCount = parser->getAttributeCount();
    if (attrCount > 0) {
        el->attributes.reserve(attrCount);
        for (size_t i = 0; i < attrCount; i++) {
            Attribute attr;
            size_t len;
            const char16_t* str16 = parser->getAttributeNamespace(i, &len);
            if (str16) {
                attr.namespaceUri.assign(str16, len);
            }

            str16 = parser->getAttributeName(i, &len);
            if (str16) {
                attr.name.assign(str16, len);
            }

            str16 = parser->getAttributeStringValue(i, &len);
            if (str16) {
                attr.value.assign(str16, len);
            }
            el->attributes.push_back(std::move(attr));
        }
    }
}

std::unique_ptr<Node> inflate(const void* data, size_t dataLen, SourceLogger* logger) {
    std::unique_ptr<Node> root;
    std::stack<Node*> nodeStack;

    android::ResXMLTree tree;
    if (tree.setTo(data, dataLen) != android::NO_ERROR) {
        return {};
    }

    android::ResXMLParser::event_code_t code;
    while ((code = tree.next()) != android::ResXMLParser::BAD_DOCUMENT &&
            code != android::ResXMLParser::END_DOCUMENT) {
        std::unique_ptr<Node> newNode;
        switch (code) {
            case android::ResXMLParser::START_NAMESPACE: {
                std::unique_ptr<Namespace> node = util::make_unique<Namespace>();
                size_t len;
                const char16_t* str16 = tree.getNamespacePrefix(&len);
                if (str16) {
                    node->namespacePrefix.assign(str16, len);
                }

                str16 = tree.getNamespaceUri(&len);
                if (str16) {
                    node->namespaceUri.assign(str16, len);
                }
                newNode = std::move(node);
                break;
            }

            case android::ResXMLParser::START_TAG: {
                std::unique_ptr<Element> node = util::make_unique<Element>();
                size_t len;
                const char16_t* str16 = tree.getElementNamespace(&len);
                if (str16) {
                    node->namespaceUri.assign(str16, len);
                }

                str16 = tree.getElementName(&len);
                if (str16) {
                    node->name.assign(str16, len);
                }

                copyAttributes(node.get(), &tree);

                newNode = std::move(node);
                break;
            }

            case android::ResXMLParser::TEXT: {
                std::unique_ptr<Text> node = util::make_unique<Text>();
                size_t len;
                const char16_t* str16 = tree.getText(&len);
                if (str16) {
                    node->text.assign(str16, len);
                }
                newNode = std::move(node);
                break;
            }

            case android::ResXMLParser::END_NAMESPACE:
            case android::ResXMLParser::END_TAG:
                assert(!nodeStack.empty());
                nodeStack.pop();
                break;

            default:
                assert(false);
                break;
        }

        if (newNode) {
            newNode->lineNumber = tree.getLineNumber();

            Node* thisNode = newNode.get();
            if (!root) {
                assert(nodeStack.empty());
                root = std::move(newNode);
            } else {
                assert(!nodeStack.empty());
                nodeStack.top()->addChild(std::move(newNode));
            }

            if (thisNode->type != NodeType::kText) {
                nodeStack.push(thisNode);
            }
        }
    }
    return std::move(root);
}

Node::Node(NodeType type) : type(type), parent(nullptr), lineNumber(0), columnNumber(0) {
}

void Node::addChild(std::unique_ptr<Node> child) {
    child->parent = this;
    children.push_back(std::move(child));
}

Namespace::Namespace() : BaseNode(NodeType::kNamespace) {
}

std::unique_ptr<Node> Namespace::clone() const {
    Namespace* ns = new Namespace();
    ns->lineNumber = lineNumber;
    ns->columnNumber = columnNumber;
    ns->comment = comment;
    ns->namespacePrefix = namespacePrefix;
    ns->namespaceUri = namespaceUri;
    for (auto& child : children) {
        ns->addChild(child->clone());
    }
    return std::unique_ptr<Node>(ns);
}

Element::Element() : BaseNode(NodeType::kElement) {
}

std::unique_ptr<Node> Element::clone() const {
    Element* el = new Element();
    el->lineNumber = lineNumber;
    el->columnNumber = columnNumber;
    el->comment = comment;
    el->namespaceUri = namespaceUri;
    el->name = name;
    el->attributes = attributes;
    for (auto& child : children) {
        el->addChild(child->clone());
    }
    return std::unique_ptr<Node>(el);
}

Attribute* Element::findAttribute(const StringPiece16& ns, const StringPiece16& name) {
    for (auto& attr : attributes) {
        if (ns == attr.namespaceUri && name == attr.name) {
            return &attr;
        }
    }
    return nullptr;
}

Element* Element::findChild(const StringPiece16& ns, const StringPiece16& name) {
    return findChildWithAttribute(ns, name, nullptr);
}

Element* Element::findChildWithAttribute(const StringPiece16& ns, const StringPiece16& name,
                                         const Attribute* reqAttr) {
    for (auto& childNode : children) {
        Node* child = childNode.get();
        while (child->type == NodeType::kNamespace) {
            if (child->children.empty()) {
                break;
            }
            child = child->children[0].get();
        }

        if (child->type == NodeType::kElement) {
            Element* el = static_cast<Element*>(child);
            if (ns == el->namespaceUri && name == el->name) {
                if (!reqAttr) {
                    return el;
                }

                Attribute* attrName = el->findAttribute(reqAttr->namespaceUri, reqAttr->name);
                if (attrName && attrName->value == reqAttr->value) {
                    return el;
                }
            }
        }
    }
    return nullptr;
}

std::vector<Element*> Element::getChildElements() {
    std::vector<Element*> elements;
    for (auto& childNode : children) {
        Node* child = childNode.get();
        while (child->type == NodeType::kNamespace) {
            if (child->children.empty()) {
                break;
            }
            child = child->children[0].get();
        }

        if (child->type == NodeType::kElement) {
            elements.push_back(static_cast<Element*>(child));
        }
    }
    return elements;
}

Text::Text() : BaseNode(NodeType::kText) {
}

std::unique_ptr<Node> Text::clone() const {
    Text* el = new Text();
    el->lineNumber = lineNumber;
    el->columnNumber = columnNumber;
    el->comment = comment;
    el->text = text;
    return std::unique_ptr<Node>(el);
}

} // namespace xml
} // namespace aapt
