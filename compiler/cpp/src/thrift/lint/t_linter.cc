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

#include <iostream>
#include <regex>
#include "thrift/lint/t_linter.h"
using namespace std;

/**
 * Framework linter method
 * Returns true if lints
 */
bool t_linter::lint() {
  bool contains_failure = false;

  if (!validate_enum_names()) {
    contains_failure = true;
  }

  if (!validate_struct_names()) {
    contains_failure = true;
  }

  if (!validate_enum_constant_names()) {
    contains_failure = true;
  }

  return !contains_failure;
}

bool t_linter::validate_enum_names() {
  std::regex regex(R"(^OT\w*)");
  bool contains_failure = false;

  const vector<t_enum*>& enums = program_->get_enums();
  vector<t_enum*>::const_iterator e_iter;
  for (e_iter = enums.begin(); e_iter != enums.end(); ++e_iter) {
    t_enum* en = *e_iter;

    if (!std::regex_match(en->get_name(), regex)) {
      cout << "Failed regex for enum name: " << en->get_name() << endl;
      contains_failure = true;
    }
  }

  return !contains_failure;
}

bool t_linter::validate_struct_names() {
  std::regex regex(R"(^OT\w*)");
  bool contains_failure = false;

  const vector<t_enum*>& enums = program_->get_enums();
  vector<t_enum*>::const_iterator e_iter;
  for (e_iter = enums.begin(); e_iter != enums.end(); ++e_iter) {
    t_enum* en = *e_iter;

    if (!std::regex_match(en->get_name(), regex)) {
      cout << "Failed regex for enum name: " << en->get_name() << endl;
      contains_failure = true;
    }
  }

  return !contains_failure;
}

bool t_linter::validate_enum_constant_names() {
  std::regex regex(R"(^[a-z0-9_]+$)");
  bool contains_failure = false;

  const vector<t_enum*>& enums = program_->get_enums();
  vector<t_enum*>::const_iterator e_iter;
  for (e_iter = enums.begin(); e_iter != enums.end(); ++e_iter) {
    t_enum* en = *e_iter;

    vector<t_enum_value*> constants = en->get_constants();
    vector<t_enum_value*>::iterator c_iter;

    for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {

      if (!std::regex_match((*c_iter)->get_name(), regex)) {
        cout << "Failed regex for enum constant name: " << (*c_iter)->get_name() << endl;
        contains_failure = true;
      }
    }
  }

  return !contains_failure;
}