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

#include "ResourceTable.h"
#include "ResourceValues.h"
#include "ValueVisitor.h"

#include "link/TableMerger.h"
#include "util/Comparators.h"
#include "util/Util.h"

#include <cassert>

namespace aapt {

TableMerger::TableMerger(IAaptContext* context, ResourceTable* outTable) :
        mContext(context), mMasterTable(outTable) {
    // Create the desired package that all tables will be merged into.
    mMasterPackage = mMasterTable->createPackage(
            mContext->getCompilationPackage(), mContext->getPackageId());
    assert(mMasterPackage && "package name or ID already taken");
}

/**
 * This will merge packages with the same package name (or no package name).
 */
bool TableMerger::merge(const Source& src, ResourceTable* table, bool overrideExisting) {
    const uint8_t desiredPackageId = mContext->getPackageId();

    bool error = false;
    for (auto& package : table->packages) {
        // Warn of packages with an unrelated ID.
        if (package->id && package->id.value() != 0x0 && package->id.value() != desiredPackageId) {
            mContext->getDiagnostics()->warn(DiagMessage(src)
                                             << "ignoring package " << package->name);
            continue;
        }

        if (package->name.empty() || mContext->getCompilationPackage() == package->name) {
            // Merge here. Once the entries are merged and mangled, any references to
            // them are still valid. This is because un-mangled references are
            // mangled, then looked up at resolution time.
            // Also, when linking, we convert references with no package name to use
            // the compilation package name.
            if (!doMerge(src, table, package.get(), false, overrideExisting)) {
                error = true;
            }
        }
    }
    return !error;
}

/**
 * This will merge and mangle resources from a static library.
 */
bool TableMerger::mergeAndMangle(const Source& src, const StringPiece16& packageName,
                                 ResourceTable* table) {
    bool error = false;
    for (auto& package : table->packages) {
        // Warn of packages with an unrelated ID.
        if (packageName != package->name) {
            mContext->getDiagnostics()->warn(DiagMessage(src)
                                             << "ignoring package " << package->name);
            continue;
        }

        bool mangle = packageName != mContext->getCompilationPackage();
        mMergedPackages.insert(package->name);
        if (!doMerge(src, table, package.get(), mangle, false)) {
            error = true;
        }
    }
    return !error;
}

bool TableMerger::doMerge(const Source& src, ResourceTable* srcTable,
                          ResourceTablePackage* srcPackage, const bool manglePackage,
                          const bool overrideExisting) {
    bool error = false;

    for (auto& srcType : srcPackage->types) {
        ResourceTableType* dstType = mMasterPackage->findOrCreateType(srcType->type);
        if (srcType->symbolStatus.state == SymbolState::kPublic) {
            if (dstType->symbolStatus.state == SymbolState::kPublic && dstType->id && srcType->id
                    && dstType->id.value() == srcType->id.value()) {
                // Both types are public and have different IDs.
                mContext->getDiagnostics()->error(DiagMessage(src)
                                                  << "can not merge type '"
                                                  << srcType->type
                                                  << "': conflicting public IDs");
                error = true;
                continue;
            }

            dstType->symbolStatus = std::move(srcType->symbolStatus);
            dstType->id = srcType->id;
        }

        for (auto& srcEntry : srcType->entries) {
            ResourceEntry* dstEntry;
            if (manglePackage) {
                dstEntry = dstType->findOrCreateEntry(NameMangler::mangleEntry(
                        srcPackage->name, srcEntry->name));
            } else {
                dstEntry = dstType->findOrCreateEntry(srcEntry->name);
            }

            if (srcEntry->symbolStatus.state != SymbolState::kUndefined) {
                if (srcEntry->symbolStatus.state == SymbolState::kPublic) {
                    if (dstEntry->symbolStatus.state == SymbolState::kPublic &&
                            dstEntry->id && srcEntry->id &&
                            dstEntry->id.value() != srcEntry->id.value()) {
                        // Both entries are public and have different IDs.
                        mContext->getDiagnostics()->error(DiagMessage(src)
                                                          << "can not merge entry '"
                                                          << srcEntry->name
                                                          << "': conflicting public IDs");
                        error = true;
                        continue;
                    }

                    if (srcEntry->id) {
                        dstEntry->id = srcEntry->id;
                    }
                }

                if (dstEntry->symbolStatus.state != SymbolState::kPublic &&
                        dstEntry->symbolStatus.state != srcEntry->symbolStatus.state) {
                    dstEntry->symbolStatus = std::move(srcEntry->symbolStatus);
                }
            }

            for (ResourceConfigValue& srcValue : srcEntry->values) {
                auto iter = std::lower_bound(dstEntry->values.begin(), dstEntry->values.end(),
                                             srcValue.config, cmp::lessThanConfig);

                if (iter != dstEntry->values.end() && iter->config == srcValue.config) {
                    const int collisionResult = ResourceTable::resolveValueCollision(
                            iter->value.get(), srcValue.value.get());
                    if (collisionResult == 0 && !overrideExisting) {
                        // Error!
                        ResourceNameRef resourceName(srcPackage->name,
                                                     srcType->type,
                                                     srcEntry->name);

                        mContext->getDiagnostics()->error(DiagMessage(srcValue.value->getSource())
                                                          << "resource '" << resourceName
                                                          << "' has a conflicting value for "
                                                          << "configuration ("
                                                          << srcValue.config << ")");
                        mContext->getDiagnostics()->note(DiagMessage(iter->value->getSource())
                                                         << "originally defined here");
                        error = true;
                        continue;
                    } else if (collisionResult < 0) {
                        // Keep our existing value.
                        continue;
                    }

                } else {
                    // Insert a place holder value. We will fill it in below.
                    iter = dstEntry->values.insert(iter, ResourceConfigValue{ srcValue.config });
                }

                if (manglePackage) {
                    iter->value = cloneAndMangle(srcTable, srcPackage->name, srcValue.value.get());
                } else {
                    iter->value = clone(srcValue.value.get());
                }
            }
        }
    }
    return !error;
}

std::unique_ptr<Value> TableMerger::cloneAndMangle(ResourceTable* table,
                                                   const std::u16string& package,
                                                   Value* value) {
    if (FileReference* f = valueCast<FileReference>(value)) {
        // Mangle the path.
        StringPiece16 prefix, entry, suffix;
        if (util::extractResFilePathParts(*f->path, &prefix, &entry, &suffix)) {
            std::u16string mangledEntry = NameMangler::mangleEntry(package, entry.toString());
            std::u16string newPath = prefix.toString() + mangledEntry + suffix.toString();
            mFilesToMerge.push(FileToMerge{ table, *f->path, newPath });
            std::unique_ptr<FileReference> fileRef = util::make_unique<FileReference>(
                    mMasterTable->stringPool.makeRef(newPath));
            fileRef->setComment(f->getComment());
            fileRef->setSource(f->getSource());
            return std::move(fileRef);
        }
    }
    return clone(value);
}

std::unique_ptr<Value> TableMerger::clone(Value* value) {
    return std::unique_ptr<Value>(value->clone(&mMasterTable->stringPool));
}

} // namespace aapt
