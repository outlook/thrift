/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "thrift/parse/t_field.h"

#include <algorithm>
#include <cctype>

namespace {

bool ci_find(const std::string& haystack, const std::string& needle) {
  auto it = std::search(
      haystack.begin(),
      haystack.end(),
      needle.begin(),
      needle.end(),
      [](char lhs, char rhs) { return std::tolower(lhs) == std::tolower(rhs); });

  return it != haystack.end();
}

}

bool t_field::is_redacted() const {
  return has_doc_text("@redacted") || has_anno("redacted") || has_anno("thrifty.redacted");
}

bool t_field::is_obfuscated() const {
  return has_doc_text("@obfuscated") || has_anno("obfuscated") || has_anno("thrifty.obfuscated");
}

bool t_field::has_doc_text(const std::string& text) const {
  bool result = false;
  if (has_doc()) {
    result = ci_find(get_doc(), text);
  }
  return result;
}

bool t_field::has_anno(const std::string& anno) const {
  auto it = annotations_.find(anno);
  return it != annotations_.end();
}
