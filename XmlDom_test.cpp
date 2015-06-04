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

#include "XmlDom.h"

#include <gtest/gtest.h>
#include <sstream>
#include <string>

namespace aapt {

constexpr const char* kXmlPreamble = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";

TEST(XmlDomTest, Inflate) {
    std::stringstream in(kXmlPreamble);
    in << R"EOF(
        <Layout xmlns:android="http://schemas.android.com/apk/res/android"
                android:layout_width="match_parent"
                android:layout_height="wrap_content">
            <TextView android:id="@+id/id"
                      android:layout_width="wrap_content"
                      android:layout_height="wrap_content" />
        </Layout>
    )EOF";

    SourceLogger logger(Source{ "/test/path" });
    std::unique_ptr<xml::Node> root = xml::inflate(&in, &logger);
    ASSERT_NE(root, nullptr);

    EXPECT_EQ(root->type, xml::NodeType::kNamespace);
    xml::Namespace* ns = static_cast<xml::Namespace*>(root.get());
    EXPECT_EQ(ns->namespaceUri, u"http://schemas.android.com/apk/res/android");
    EXPECT_EQ(ns->namespacePrefix, u"android");
}

} // namespace aapt
