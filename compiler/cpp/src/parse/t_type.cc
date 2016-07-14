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

#include "t_type.h"

#include "t_visitor.h"

#include <boost/lexical_cast.hpp>
#include <string>

using namespace boost;

void t_type::accept(t_visitor &visitor) {
  if (is_base_type()) {
    t_base_type *self = static_cast< t_base_type*>(this);
    t_base_type::t_base base = self->get_base();
    if (base == t_base_type::TYPE_STRING) {
      if (self->is_binary()) {
        visitor.visitBinary(self);
      } else {
        visitor.visitString(self);
      }
    } else if (base == t_base_type::TYPE_BOOL) {
      visitor.visitBool(self);
    } else if (base == t_base_type::TYPE_BYTE) {
      visitor.visitByte(self);
    } else if (base == t_base_type::TYPE_I16) {
      visitor.visitI16(self);
    } else if (base == t_base_type::TYPE_I32) {
      visitor.visitI32(self);
    } else if (base == t_base_type::TYPE_I64) {
      visitor.visitI64(self);
    } else if (base == t_base_type::TYPE_DOUBLE) {
      visitor.visitDouble(self);
    } else {
      throw "unexpected base type: " + lexical_cast<std::string>(base);
    }
  } else if (is_list()) {
    visitor.visitList(static_cast< t_list*>(this));
  } else if (is_set()) {
    visitor.visitSet(static_cast< t_set*>(this));
  } else if (is_map()) {
    visitor.visitMap(static_cast< t_map*>(this));
  } else if (is_struct() || is_xception()) {
    visitor.visitStruct(static_cast< t_struct*>(this));
  } else if (is_enum()) {
    visitor.visitEnum(static_cast< t_enum*>(this));
  } else if (is_service()) {
    visitor.visitService(static_cast< t_service*>(this));
  } else {
    throw "unexpected type: " + get_name();
  }
}

