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

#ifndef AAPT_DEBUG_H
#define AAPT_DEBUG_H

#include "Resource.h"
#include "ResourceTable.h"
#include "xml/XmlDom.h"

// Include for printf-like debugging.
#include <iostream>

namespace aapt {

struct DebugPrintTableOptions {
  bool showSources = false;
};

struct Debug {
  static void printTable(ResourceTable* table,
                         const DebugPrintTableOptions& options = {});
  static void printStyleGraph(ResourceTable* table,
                              const ResourceName& targetStyle);
  static void dumpHex(const void* data, size_t len);
  static void dumpXml(xml::XmlResource* doc);
};

}  // namespace aapt

#endif  // AAPT_DEBUG_H
