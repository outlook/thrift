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

#include <algorithm>
#include <regex>
#include "thrift/parse/t_program.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;
using namespace std;

/**
 * Class for a thrift linter.
 */
class t_linter {
public:
  t_linter(t_program* program, string lint_file) {
    program_ = program;
    lint_file_ = lint_file;
  }

  virtual ~t_linter() {}

  /**
   * Framework linter method
   * Returns true if lints
   */
  bool lint();

private:
  /**
   * The program being generated
   */
  t_program* program_;

  string lint_file_;

  bool validate_enum_names(string message, string raw_regex, set<string> enum_exceptions);
  bool validate_struct_names(string message, string raw_regex, set<string> struct_exceptions);
  bool validate_enum_constant_names(string message, string raw_regex, set<string> enum_exceptions, set<string> exceptions);
  bool validate_struct_member_names(string message, string raw_regex, set<string> struct_exceptions, set<string> exceptions);
  bool validate_struct_member_values(string message, string raw_regex, set<string> struct_exceptions, set<string> exceptions);

  bool validate_override_struct_member_names(string message, vector<map<string, string>> member_name_by_struct_exceptions);
  bool validate_override_struct_member_names(
    string message,
    t_struct* tstruct,
    vector<map<string, string>> member_name_by_struct_exceptions);
  bool validate_override_struct_member_names(
    string message,
    t_struct* tstruct,
    vector<map<string, string>> member_name_by_struct_exceptions,
    map<string, string> &member_name_by_struct);

  bool validate_struct_member_order(string message, set<string> struct_exceptions);

  bool validate_required_before_optional(string message, set<string> struct_exceptions);

  bool validate_enum_implicit_value(string message, set<string> enum_exceptions);

  template<typename T>
  set<T> as_set(pt::ptree pt, string key);

  template<typename T>
  vector<map<string, T>> as_map_array(pt::ptree pt, string key);

  bool contains_value(vector<map<string, string>> map_array, string key);
};