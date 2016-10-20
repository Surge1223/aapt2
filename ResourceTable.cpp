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
#include "ConfigDescription.h"
#include "NameMangler.h"
#include "ResourceValues.h"
#include "ValueVisitor.h"
#include "util/Util.h"

#include <androidfw/ResourceTypes.h>
#include <algorithm>
#include <memory>
#include <string>
#include <tuple>

namespace aapt {

static bool lessThanType(const std::unique_ptr<ResourceTableType>& lhs,
                         ResourceType rhs) {
  return lhs->type < rhs;
}

template <typename T>
static bool lessThanStructWithName(const std::unique_ptr<T>& lhs,
                                   const StringPiece& rhs) {
  return lhs->name.compare(0, lhs->name.size(), rhs.data(), rhs.size()) < 0;
}

ResourceTablePackage* ResourceTable::findPackage(const StringPiece& name) {
  const auto last = packages.end();
  auto iter = std::lower_bound(packages.begin(), last, name,
                               lessThanStructWithName<ResourceTablePackage>);
  if (iter != last && name == (*iter)->name) {
    return iter->get();
  }
  return nullptr;
}

ResourceTablePackage* ResourceTable::findPackageById(uint8_t id) {
  for (auto& package : packages) {
    if (package->id && package->id.value() == id) {
      return package.get();
    }
  }
  return nullptr;
}

ResourceTablePackage* ResourceTable::createPackage(const StringPiece& name,
                                                   Maybe<uint8_t> id) {
  ResourceTablePackage* package = findOrCreatePackage(name);
  if (id && !package->id) {
    package->id = id;
    return package;
  }

  if (id && package->id && package->id.value() != id.value()) {
    return nullptr;
  }
  return package;
}

ResourceTablePackage* ResourceTable::findOrCreatePackage(
    const StringPiece& name) {
  const auto last = packages.end();
  auto iter = std::lower_bound(packages.begin(), last, name,
                               lessThanStructWithName<ResourceTablePackage>);
  if (iter != last && name == (*iter)->name) {
    return iter->get();
  }

  std::unique_ptr<ResourceTablePackage> newPackage =
      util::make_unique<ResourceTablePackage>();
  newPackage->name = name.toString();
  return packages.emplace(iter, std::move(newPackage))->get();
}

ResourceTableType* ResourceTablePackage::findType(ResourceType type) {
  const auto last = types.end();
  auto iter = std::lower_bound(types.begin(), last, type, lessThanType);
  if (iter != last && (*iter)->type == type) {
    return iter->get();
  }
  return nullptr;
}

ResourceTableType* ResourceTablePackage::findOrCreateType(ResourceType type) {
  const auto last = types.end();
  auto iter = std::lower_bound(types.begin(), last, type, lessThanType);
  if (iter != last && (*iter)->type == type) {
    return iter->get();
  }
  return types.emplace(iter, new ResourceTableType(type))->get();
}

ResourceEntry* ResourceTableType::findEntry(const StringPiece& name) {
  const auto last = entries.end();
  auto iter = std::lower_bound(entries.begin(), last, name,
                               lessThanStructWithName<ResourceEntry>);
  if (iter != last && name == (*iter)->name) {
    return iter->get();
  }
  return nullptr;
}

ResourceEntry* ResourceTableType::findOrCreateEntry(const StringPiece& name) {
  auto last = entries.end();
  auto iter = std::lower_bound(entries.begin(), last, name,
                               lessThanStructWithName<ResourceEntry>);
  if (iter != last && name == (*iter)->name) {
    return iter->get();
  }
  return entries.emplace(iter, new ResourceEntry(name))->get();
}

ResourceConfigValue* ResourceEntry::findValue(const ConfigDescription& config) {
  return findValue(config, StringPiece());
}

struct ConfigKey {
  const ConfigDescription* config;
  const StringPiece& product;
};

bool ltConfigKeyRef(const std::unique_ptr<ResourceConfigValue>& lhs,
                    const ConfigKey& rhs) {
  int cmp = lhs->config.compare(*rhs.config);
  if (cmp == 0) {
    cmp = StringPiece(lhs->product).compare(rhs.product);
  }
  return cmp < 0;
}

ResourceConfigValue* ResourceEntry::findValue(const ConfigDescription& config,
                                              const StringPiece& product) {
  auto iter = std::lower_bound(values.begin(), values.end(),
                               ConfigKey{&config, product}, ltConfigKeyRef);
  if (iter != values.end()) {
    ResourceConfigValue* value = iter->get();
    if (value->config == config && StringPiece(value->product) == product) {
      return value;
    }
  }
  return nullptr;
}

ResourceConfigValue* ResourceEntry::findOrCreateValue(
    const ConfigDescription& config, const StringPiece& product) {
  auto iter = std::lower_bound(values.begin(), values.end(),
                               ConfigKey{&config, product}, ltConfigKeyRef);
  if (iter != values.end()) {
    ResourceConfigValue* value = iter->get();
    if (value->config == config && StringPiece(value->product) == product) {
      return value;
    }
  }
  ResourceConfigValue* newValue =
      values
          .insert(iter, util::make_unique<ResourceConfigValue>(config, product))
          ->get();
  return newValue;
}

std::vector<ResourceConfigValue*> ResourceEntry::findAllValues(
    const ConfigDescription& config) {
  std::vector<ResourceConfigValue*> results;

  auto iter = values.begin();
  for (; iter != values.end(); ++iter) {
    ResourceConfigValue* value = iter->get();
    if (value->config == config) {
      results.push_back(value);
      ++iter;
      break;
    }
  }

  for (; iter != values.end(); ++iter) {
    ResourceConfigValue* value = iter->get();
    if (value->config == config) {
      results.push_back(value);
    }
  }
  return results;
}

std::vector<ResourceConfigValue*> ResourceEntry::findValuesIf(
    const std::function<bool(ResourceConfigValue*)>& f) {
  std::vector<ResourceConfigValue*> results;
  for (auto& configValue : values) {
    if (f(configValue.get())) {
      results.push_back(configValue.get());
    }
  }
  return results;
}

/**
 * The default handler for collisions.
 *
 * Typically, a weak value will be overridden by a strong value. An existing
 * weak
 * value will not be overridden by an incoming weak value.
 *
 * There are some exceptions:
 *
 * Attributes: There are two types of Attribute values: USE and DECL.
 *
 * USE is anywhere an Attribute is declared without a format, and in a place
 * that would
 * be legal to declare if the Attribute already existed. This is typically in a
 * <declare-styleable> tag. Attributes defined in a <declare-styleable> are also
 * weak.
 *
 * DECL is an absolute declaration of an Attribute and specifies an explicit
 * format.
 *
 * A DECL will override a USE without error. Two DECLs must match in their
 * format for there to be
 * no error.
 */
ResourceTable::CollisionResult ResourceTable::resolveValueCollision(
    Value* existing, Value* incoming) {
  Attribute* existingAttr = valueCast<Attribute>(existing);
  Attribute* incomingAttr = valueCast<Attribute>(incoming);
  if (!incomingAttr) {
    if (incoming->isWeak()) {
      // We're trying to add a weak resource but a resource
      // already exists. Keep the existing.
      return CollisionResult::kKeepOriginal;
    } else if (existing->isWeak()) {
      // Override the weak resource with the new strong resource.
      return CollisionResult::kTakeNew;
    }
    // The existing and incoming values are strong, this is an error
    // if the values are not both attributes.
    return CollisionResult::kConflict;
  }

  if (!existingAttr) {
    if (existing->isWeak()) {
      // The existing value is not an attribute and it is weak,
      // so take the incoming attribute value.
      return CollisionResult::kTakeNew;
    }
    // The existing value is not an attribute and it is strong,
    // so the incoming attribute value is an error.
    return CollisionResult::kConflict;
  }

  assert(incomingAttr && existingAttr);

  //
  // Attribute specific handling. At this point we know both
  // values are attributes. Since we can declare and define
  // attributes all-over, we do special handling to see
  // which definition sticks.
  //
  if (existingAttr->typeMask == incomingAttr->typeMask) {
    // The two attributes are both DECLs, but they are plain attributes
    // with the same formats.
    // Keep the strongest one.
    return existingAttr->isWeak() ? CollisionResult::kTakeNew
                                  : CollisionResult::kKeepOriginal;
  }

  if (existingAttr->isWeak() &&
      existingAttr->typeMask == android::ResTable_map::TYPE_ANY) {
    // Any incoming attribute is better than this.
    return CollisionResult::kTakeNew;
  }

  if (incomingAttr->isWeak() &&
      incomingAttr->typeMask == android::ResTable_map::TYPE_ANY) {
    // The incoming attribute may be a USE instead of a DECL.
    // Keep the existing attribute.
    return CollisionResult::kKeepOriginal;
  }
  return CollisionResult::kConflict;
}

static constexpr const char* kValidNameChars = "._-";
static constexpr const char* kValidNameMangledChars = "._-$";

bool ResourceTable::addResource(const ResourceNameRef& name,
                                const ConfigDescription& config,
                                const StringPiece& product,
                                std::unique_ptr<Value> value,
                                IDiagnostics* diag) {
  return addResourceImpl(name, {}, config, product, std::move(value),
                         kValidNameChars, resolveValueCollision, diag);
}

bool ResourceTable::addResource(const ResourceNameRef& name,
                                const ResourceId& resId,
                                const ConfigDescription& config,
                                const StringPiece& product,
                                std::unique_ptr<Value> value,
                                IDiagnostics* diag) {
  return addResourceImpl(name, resId, config, product, std::move(value),
                         kValidNameChars, resolveValueCollision, diag);
}

bool ResourceTable::addFileReference(const ResourceNameRef& name,
                                     const ConfigDescription& config,
                                     const Source& source,
                                     const StringPiece& path,
                                     IDiagnostics* diag) {
  return addFileReferenceImpl(name, config, source, path, nullptr,
                              kValidNameChars, diag);
}

bool ResourceTable::addFileReferenceAllowMangled(
    const ResourceNameRef& name, const ConfigDescription& config,
    const Source& source, const StringPiece& path, io::IFile* file,
    IDiagnostics* diag) {
  return addFileReferenceImpl(name, config, source, path, file,
                              kValidNameMangledChars, diag);
}

bool ResourceTable::addFileReferenceImpl(
    const ResourceNameRef& name, const ConfigDescription& config,
    const Source& source, const StringPiece& path, io::IFile* file,
    const char* validChars, IDiagnostics* diag) {
  std::unique_ptr<FileReference> fileRef =
      util::make_unique<FileReference>(stringPool.makeRef(path));
  fileRef->setSource(source);
  fileRef->file = file;
  return addResourceImpl(name, ResourceId{}, config, StringPiece{},
                         std::move(fileRef), validChars, resolveValueCollision,
                         diag);
}

bool ResourceTable::addResourceAllowMangled(const ResourceNameRef& name,
                                            const ConfigDescription& config,
                                            const StringPiece& product,
                                            std::unique_ptr<Value> value,
                                            IDiagnostics* diag) {
  return addResourceImpl(name, ResourceId{}, config, product, std::move(value),
                         kValidNameMangledChars, resolveValueCollision, diag);
}

bool ResourceTable::addResourceAllowMangled(const ResourceNameRef& name,
                                            const ResourceId& id,
                                            const ConfigDescription& config,
                                            const StringPiece& product,
                                            std::unique_ptr<Value> value,
                                            IDiagnostics* diag) {
  return addResourceImpl(name, id, config, product, std::move(value),
                         kValidNameMangledChars, resolveValueCollision, diag);
}

bool ResourceTable::addResourceImpl(
    const ResourceNameRef& name, const ResourceId& resId,
    const ConfigDescription& config, const StringPiece& product,
    std::unique_ptr<Value> value, const char* validChars,
    const CollisionResolverFunc& conflictResolver, IDiagnostics* diag) {
  assert(value && "value can't be nullptr");
  assert(diag && "diagnostics can't be nullptr");

  auto badCharIter =
      util::findNonAlphaNumericAndNotInSet(name.entry, validChars);
  if (badCharIter != name.entry.end()) {
    diag->error(DiagMessage(value->getSource())
                << "resource '" << name << "' has invalid entry name '"
                << name.entry << "'. Invalid character '"
                << StringPiece(badCharIter, 1) << "'");
    return false;
  }

  ResourceTablePackage* package = findOrCreatePackage(name.package);
  if (resId.isValid() && package->id &&
      package->id.value() != resId.packageId()) {
    diag->error(DiagMessage(value->getSource())
                << "trying to add resource '" << name << "' with ID " << resId
                << " but package '" << package->name << "' already has ID "
                << std::hex << (int)package->id.value() << std::dec);
    return false;
  }

  ResourceTableType* type = package->findOrCreateType(name.type);
  if (resId.isValid() && type->id && type->id.value() != resId.typeId()) {
    diag->error(DiagMessage(value->getSource())
                << "trying to add resource '" << name << "' with ID " << resId
                << " but type '" << type->type << "' already has ID "
                << std::hex << (int)type->id.value() << std::dec);
    return false;
  }

  ResourceEntry* entry = type->findOrCreateEntry(name.entry);
  if (resId.isValid() && entry->id && entry->id.value() != resId.entryId()) {
    diag->error(DiagMessage(value->getSource())
                << "trying to add resource '" << name << "' with ID " << resId
                << " but resource already has ID "
                << ResourceId(package->id.value(), type->id.value(),
                              entry->id.value()));
    return false;
  }

  ResourceConfigValue* configValue = entry->findOrCreateValue(config, product);
  if (!configValue->value) {
    // Resource does not exist, add it now.
    configValue->value = std::move(value);

  } else {
    switch (conflictResolver(configValue->value.get(), value.get())) {
      case CollisionResult::kTakeNew:
        // Take the incoming value.
        configValue->value = std::move(value);
        break;

      case CollisionResult::kConflict:
        diag->error(DiagMessage(value->getSource())
                    << "duplicate value for resource '" << name << "' "
                    << "with config '" << config << "'");
        diag->error(DiagMessage(configValue->value->getSource())
                    << "resource previously defined here");
        return false;

      case CollisionResult::kKeepOriginal:
        break;
    }
  }

  if (resId.isValid()) {
    package->id = resId.packageId();
    type->id = resId.typeId();
    entry->id = resId.entryId();
  }
  return true;
}

bool ResourceTable::setSymbolState(const ResourceNameRef& name,
                                   const ResourceId& resId,
                                   const Symbol& symbol, IDiagnostics* diag) {
  return setSymbolStateImpl(name, resId, symbol, kValidNameChars, diag);
}

bool ResourceTable::setSymbolStateAllowMangled(const ResourceNameRef& name,
                                               const ResourceId& resId,
                                               const Symbol& symbol,
                                               IDiagnostics* diag) {
  return setSymbolStateImpl(name, resId, symbol, kValidNameMangledChars, diag);
}

bool ResourceTable::setSymbolStateImpl(const ResourceNameRef& name,
                                       const ResourceId& resId,
                                       const Symbol& symbol,
                                       const char* validChars,
                                       IDiagnostics* diag) {
  assert(diag && "diagnostics can't be nullptr");

  auto badCharIter =
      util::findNonAlphaNumericAndNotInSet(name.entry, validChars);
  if (badCharIter != name.entry.end()) {
    diag->error(DiagMessage(symbol.source)
                << "resource '" << name << "' has invalid entry name '"
                << name.entry << "'. Invalid character '"
                << StringPiece(badCharIter, 1) << "'");
    return false;
  }

  ResourceTablePackage* package = findOrCreatePackage(name.package);
  if (resId.isValid() && package->id &&
      package->id.value() != resId.packageId()) {
    diag->error(DiagMessage(symbol.source)
                << "trying to add resource '" << name << "' with ID " << resId
                << " but package '" << package->name << "' already has ID "
                << std::hex << (int)package->id.value() << std::dec);
    return false;
  }

  ResourceTableType* type = package->findOrCreateType(name.type);
  if (resId.isValid() && type->id && type->id.value() != resId.typeId()) {
    diag->error(DiagMessage(symbol.source)
                << "trying to add resource '" << name << "' with ID " << resId
                << " but type '" << type->type << "' already has ID "
                << std::hex << (int)type->id.value() << std::dec);
    return false;
  }

  ResourceEntry* entry = type->findOrCreateEntry(name.entry);
  if (resId.isValid() && entry->id && entry->id.value() != resId.entryId()) {
    diag->error(DiagMessage(symbol.source)
                << "trying to add resource '" << name << "' with ID " << resId
                << " but resource already has ID "
                << ResourceId(package->id.value(), type->id.value(),
                              entry->id.value()));
    return false;
  }

  if (resId.isValid()) {
    package->id = resId.packageId();
    type->id = resId.typeId();
    entry->id = resId.entryId();
  }

  // Only mark the type state as public, it doesn't care about being private.
  if (symbol.state == SymbolState::kPublic) {
    type->symbolStatus.state = SymbolState::kPublic;
  }

  if (symbol.state == SymbolState::kUndefined &&
      entry->symbolStatus.state != SymbolState::kUndefined) {
    // We can't undefine a symbol (remove its visibility). Ignore.
    return true;
  }

  if (symbol.state == SymbolState::kPrivate &&
      entry->symbolStatus.state == SymbolState::kPublic) {
    // We can't downgrade public to private. Ignore.
    return true;
  }

  entry->symbolStatus = std::move(symbol);
  return true;
}

Maybe<ResourceTable::SearchResult> ResourceTable::findResource(
    const ResourceNameRef& name) {
  ResourceTablePackage* package = findPackage(name.package);
  if (!package) {
    return {};
  }

  ResourceTableType* type = package->findType(name.type);
  if (!type) {
    return {};
  }

  ResourceEntry* entry = type->findEntry(name.entry);
  if (!entry) {
    return {};
  }
  return SearchResult{package, type, entry};
}

}  // namespace aapt
