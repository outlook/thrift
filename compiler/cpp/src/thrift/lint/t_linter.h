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
using namespace std;

/**
 * Class for a thrift linter.
 */
class t_linter {
public:
  t_linter(t_program* program) {
    program_ = program;
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

  bool validate_enum_names();
  bool validate_struct_names();
  bool validate_enum_constant_names();
  bool validate_struct_member_names();
  bool validate_struct_member_values();

  bool validate_override_struct_member_names();
  bool validate_override_struct_member_names(
    regex regex,
    t_struct* tstruct,
    map<string, string> member_name_by_struct_exceptions,
    map<string, string> member_name_by_struct);
};