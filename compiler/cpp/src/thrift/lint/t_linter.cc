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
#include "thrift/parse/t_base_type.h"
#include "thrift/lint/t_linter.h"

/**
 * Framework linter method
 * Returns true if lints
 */
bool t_linter::lint() {
  bool contains_failure = false;

  pt::ptree lint_file_root;
  pt::read_json(lint_file_, lint_file_root);

  // Iterator over all rules
  for (auto& rule : lint_file_root.get_child("rules")) {
    string lint_name = rule.second.get<string>("name");
    auto regex = rule.second.get_optional<string>("regex");

    set<string> enum_exceptions = as_set<string>(rule.second, "enum_exceptions");
    set<string> struct_exceptions = as_set<string>(rule.second, "struct_exceptions");
    set<string> exceptions = as_set<string>(rule.second, "exceptions");

    vector<map<string, string>> member_name_by_struct_exceptions = as_map_array<string>(rule.second, "member_name_by_struct_exceptions");

    if (lint_name == "enum_name") {
      if (!regex) {
        failure("Should have provided regex for enum_name");
      }

      if (!validate_enum_names(*regex, enum_exceptions)) {
        contains_failure = true;
      }
    } else if (lint_name == "enum_constant_name") {
      if (!regex) {
        failure("Should have provided regex for enum_constant_name");
      }

      if (!validate_enum_constant_names(*regex, enum_exceptions, exceptions)) {
        contains_failure = true;
      }
    } else if (lint_name == "struct_name") {
      if (!regex) {
        failure("Should have provided regex for struct_name");
      }

      if (!validate_struct_names(*regex, struct_exceptions)) {
        contains_failure = true;
      }
    } else if (lint_name == "struct_member_name") {
      if (!regex) {
        failure("Should have provided regex for struct_member_name");
      }

      if (!validate_struct_member_names(*regex, struct_exceptions, exceptions)) {
        contains_failure = true;
      }
    } else if (lint_name == "struct_member_value") {
      if (!regex) {
        failure("Should have provided regex for struct_member_value");
      }

      if (!validate_struct_member_values(*regex, struct_exceptions, exceptions)) {
        contains_failure = true;
      }
    } else if (lint_name == "override_struct_member") {
      if (!validate_override_struct_member_names(member_name_by_struct_exceptions)) {
        contains_failure = true;
      }
    } else if (lint_name == "struct_member_order") {
      if (!validate_struct_member_order(struct_exceptions)) {
        contains_failure = true;
      }
    } else if (lint_name == "required_before_optional") {
      if (!validate_required_before_optional(struct_exceptions)) {
        contains_failure = true;
      }
    }
    else {
      failure("Unknown lint name: ", lint_name);
    }
  }

  return !contains_failure;
}

bool t_linter::validate_enum_names(string raw_regex, set<string> enum_exceptions) {
  std::regex regex(raw_regex);
  bool contains_failure = false;

  const vector<t_enum*>& enums = program_->get_enums();
  vector<t_enum*>::const_iterator e_iter;
  for (e_iter = enums.begin(); e_iter != enums.end(); ++e_iter) {
    t_enum* en = *e_iter;

    if (enum_exceptions.find(en->get_name()) != enum_exceptions.end()) {
      continue;
    }

    if (!std::regex_match(en->get_name(), regex)) {
      cerr << "Failed regex for enum name: " << en->get_name() << endl;
      contains_failure = true;
    }
  }

  return !contains_failure;
}

bool t_linter::validate_struct_names(string raw_regex, set<string> struct_exceptions) {
  std::regex regex(raw_regex);
  bool contains_failure = false;

  const vector<t_struct*>& structs = program_->get_structs();
  vector<t_struct*>::const_iterator s_iter;
  for (s_iter = structs.begin(); s_iter != structs.end(); ++s_iter) {
    t_struct* tstruct = *s_iter;

    if (struct_exceptions.find(tstruct->get_name()) != struct_exceptions.end()) {
      continue;
    }

    if (!std::regex_match(tstruct->get_name(), regex)) {
      cerr << "Failed regex for struct name: " << tstruct->get_name() << endl;
      contains_failure = true;
    }
  }

  return !contains_failure;
}

bool t_linter::validate_enum_constant_names(string raw_regex, set<string> enum_exceptions, set<string> exceptions) {
  std::regex regex(raw_regex);
  bool contains_failure = false;

  const vector<t_enum*>& enums = program_->get_enums();
  vector<t_enum*>::const_iterator e_iter;
  for (e_iter = enums.begin(); e_iter != enums.end(); ++e_iter) {
    t_enum* en = *e_iter;

    if (enum_exceptions.find(en->get_name()) != enum_exceptions.end()) {
      continue;
    }

    vector<t_enum_value*> constants = en->get_constants();
    vector<t_enum_value*>::iterator c_iter;

    for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {

      string name = (*c_iter)->get_name();
      if (exceptions.find(name) != exceptions.end()) {
        continue;
      }

      if (!std::regex_match(name, regex)) {
        cerr << "Failed regex for enum constant name: " << name << endl;
        contains_failure = true;
      }
    }
  }

  return !contains_failure;
}

bool t_linter::validate_struct_member_names(string raw_regex, set<string> struct_exceptions, set<string> exceptions) {
  std::regex regex(raw_regex);
  bool contains_failure = false;

  const vector<t_struct*>& structs = program_->get_structs();
  vector<t_struct*>::const_iterator s_iter;
  for (s_iter = structs.begin(); s_iter != structs.end(); ++s_iter) {
    t_struct* tstruct = *s_iter;

    if (struct_exceptions.find(tstruct->get_name()) != struct_exceptions.end()) {
      continue;
    }

    vector<t_field*> members = tstruct->get_members();
    vector<t_field*>::iterator m_iter;

    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {

      string name = (*m_iter)->get_name();
      if (exceptions.find(name) != exceptions.end()) {
        continue;
      }

      if (!std::regex_match(name, regex)) {
        cerr << "Failed regex for struct member name: " << name << endl;
        contains_failure = true;
      }
    }
  }

  return !contains_failure;
}

bool t_linter::validate_struct_member_values(string raw_regex, set<string> struct_exceptions, set<string> exceptions) {
  std::regex regex(raw_regex);
  bool contains_failure = false;

  const vector<t_struct*>& structs = program_->get_structs();
  vector<t_struct*>::const_iterator s_iter;
  for (s_iter = structs.begin(); s_iter != structs.end(); ++s_iter) {
    t_struct* tstruct = *s_iter;

    if (struct_exceptions.find(tstruct->get_name()) != struct_exceptions.end()) {
      continue;
    }

    vector<t_field*> members = tstruct->get_members();
    vector<t_field*>::iterator m_iter;

    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      t_field* tfield = *m_iter;

      if (tfield->get_value() == NULL) {
        continue;
      }

      if (!tfield->get_type()->is_base_type()) {
        continue;
      }

      t_base_type::t_base tbase = ((t_base_type*)tfield->get_type())->get_base();
      switch (tbase) {
      case t_base_type::TYPE_STRING:
        {
          string value = tfield->get_value()->get_string();
          if (exceptions.find(value) != exceptions.end()) {
            continue;
          }

          if (!std::regex_match(value, regex)) {
            cerr << "Failed regex for struct member value: " << value << endl;
            contains_failure = true;
          }
          break;
        }
      default:
        break;
      }
    }
  }

  return !contains_failure;
}

bool t_linter::validate_override_struct_member_names(vector<map<string, string>> member_name_by_struct_exceptions) {
  bool contains_failure = false;

  const vector<t_struct*>& structs = program_->get_structs();
  vector<t_struct*>::const_iterator s_iter;
  for (s_iter = structs.begin(); s_iter != structs.end(); ++s_iter) {
    if (!validate_override_struct_member_names(*s_iter, member_name_by_struct_exceptions, map<string, string>())) {
      contains_failure = true;
    }
  }

  return !contains_failure;
}

bool t_linter::validate_override_struct_member_names(
  t_struct* tstruct,
  vector<map<string, string>> member_name_by_struct_exceptions,
  map<string, string> member_name_by_struct) {

  bool contains_failure = false;

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    t_field* field = *f_iter;

    if (contains_value(member_name_by_struct_exceptions, tstruct->get_name())) {
      continue;
    }

    string name = field->get_name();

    std::map<string, string>::iterator existing_member_name_struct = member_name_by_struct.find(name);
    if (existing_member_name_struct != member_name_by_struct.end()) {
      cerr << "Multiple instances member value: " << name;
      cerr << ", struct: " << tstruct->get_name();
      cerr << ", struct: " << existing_member_name_struct->second << endl;
      contains_failure = true;
    }

    member_name_by_struct[name] = tstruct->get_name();
  }

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    t_field* field = *f_iter;

    if (!field->get_type()->is_struct()) {
      continue;
    }

    t_struct* sub_struct = (t_struct*)field->get_type();

    if (!validate_override_struct_member_names(sub_struct, member_name_by_struct_exceptions, member_name_by_struct)) {
      contains_failure = true;
    }
  }

  return !contains_failure;
}

bool t_linter::validate_struct_member_order(set<string> struct_exceptions) {
  bool contains_failure = false;

  const vector<t_struct*>& structs = program_->get_structs();
  vector<t_struct*>::const_iterator s_iter;
  for (s_iter = structs.begin(); s_iter != structs.end(); ++s_iter) {
    t_struct* tstruct = *s_iter;

    if (struct_exceptions.find(tstruct->get_name()) != struct_exceptions.end()) {
      continue;
    }

    int cur_index = 1;

    const vector<t_field*>& fields = tstruct->get_members();
    vector<t_field*>::const_iterator f_iter;
    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      t_field* tfield = *f_iter;

      if (tfield->get_key() != cur_index) {
        cerr << "Unexpected key: " << tfield->get_key();
        cerr << " for struct: " << tstruct->get_name();
        cerr << " member: " << tfield->get_name() << endl;
        contains_failure = true;
      }

      ++cur_index;
    }
  }

  return !contains_failure;
}

bool t_linter::validate_required_before_optional(set<string> struct_exceptions) {
  bool contains_failure = false;

  const vector<t_struct*>& structs = program_->get_structs();
  vector<t_struct*>::const_iterator s_iter;
  for (s_iter = structs.begin(); s_iter != structs.end(); ++s_iter) {
    t_struct* tstruct = *s_iter;

    if (struct_exceptions.find(tstruct->get_name()) != struct_exceptions.end()) {
      continue;
    }

    bool has_optional = false;

    const vector<t_field*>& fields = tstruct->get_members();
    vector<t_field*>::const_iterator f_iter;
    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      t_field* tfield = *f_iter;

      if (tfield->get_req() == t_field::T_OPTIONAL) {
        has_optional = true;
      }
      else {
        if (has_optional) {
          cerr << "Required member after optional: " << tfield->get_name();
          cerr << ", struct: " << tstruct->get_name() << endl;
          contains_failure = true;
        }
      }
    }
  }

  return !contains_failure;
}

template<typename T>
set<T> t_linter::as_set(pt::ptree pt, string key) {
  set<T> s;

  auto value = pt.get_child_optional(key);
  if (!value) {
    return s;
  }

  for (auto& item : *value) {
    s.insert(item.second.get_value<T>());
  }
  return s;
}

template<typename T>
vector<map<string, T>> t_linter::as_map_array(pt::ptree pt, string key) {
  vector<map<string, T>> v;

  auto value = pt.get_child_optional(key);
  if (!value) {
    return v;
  }

  for (auto& item : *value) {
    map<string, T> m;
    for (auto& child_item : item.second) {
      m[child_item.first] = child_item.second.get_value<T>();
    }

    v.push_back(m);
  }
  return v;
}

bool t_linter::contains_value(vector<map<string, string>> map_array, string key) {
  for (map<string, string> map : map_array) {
    if (map.find(key) != map.end()) {
      return true;
    }
  }
  return false;
}