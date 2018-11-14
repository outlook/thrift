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

#ifndef T_VISITOR_H
#define T_VISITOR_H

#include <string>

#include "thrift/parse/t_type.h"
#include "thrift/parse/t_base_type.h"
#include "thrift/parse/t_list.h"
#include "thrift/parse/t_set.h"
#include "thrift/parse/t_map.h"
#include "thrift/parse/t_enum.h"
#include "thrift/parse/t_struct.h"
#include "thrift/parse/t_service.h"
#include "thrift/parse/t_typedef.h"

#include <boost/lexical_cast.hpp>

template <typename T>
class t_visitor;

template <typename T>
T visit(t_type* type, t_visitor<T>& visitor);

template <typename T>
class t_visitor {
public:
  virtual ~t_visitor() = default;

  virtual T visit_void(t_base_type *) {
    throw "visit_void: unimplemented";
  }

  virtual T visit_binary(t_base_type *) {
    throw "visit_binary: unimplemented";
  }

  virtual T visit_string(t_base_type *) {
    throw "visit_string: unimplemented";
  }

  virtual T visit_bool(t_base_type *) {
    throw "visit_bool: unimplemented";
  }

  virtual T visit_i8(t_base_type *) {
    throw "visit_i8: unimplemented";
  }

  virtual T visit_i16(t_base_type *) {
    throw "visit_i16: unimplemented";
  }

  virtual T visit_i32(t_base_type *) {
    throw "visit_i32: unimplemented";
  }

  virtual T visit_i64(t_base_type *) {
    throw "visit_i64: unimplemented";
  }

  virtual T visit_double(t_base_type *) {
    throw "visit_double: unimplemented";
  }

  virtual T visit_list(t_list *) {
    throw "visit_list: unimplemented";
  }

  virtual T visit_set(t_set *) {
    throw "visit_set: unimplemented";
  }

  virtual T visit_map(t_map *) {
    throw "visit_map: unimplemented";
  }

  virtual T visit_struct(t_struct *) {
    throw "visit_struct: unimplemented";
  }

  virtual T visit_enum(t_enum *) {
    throw "visit_enum: unimplemented";
  }

  virtual T visit_service(t_service *) {
    throw "visit_service: unimplemented";
  }

  virtual T visit_typedef(t_typedef *td) {
    // As a convenience, we'll provide a default implementation for
    // typedefs that forwards to their aliased type.
    return visit<T>(td->get_true_type(), *this);
  }
};

template <typename T>
class t_simple_visitor : public t_visitor<T> {
public:
  virtual ~t_simple_visitor() = default;

  virtual T visit_base(t_base_type*) {
    throw "visit_base: unimplemented";
  }

  virtual T visit_void(t_base_type *t) {
    return visit_base(t);
  }

  virtual T visit_binary(t_base_type *t) {
    return visit_base(t);
  }

  virtual T visit_string(t_base_type *t) {
    return visit_base(t);
  }

  virtual T visit_bool(t_base_type *t) {
    return visit_base(t);
  }

  virtual T visit_i8(t_base_type *t) {
    return visit_base(t);
  }

  virtual T visit_i16(t_base_type *t) {
    return visit_base(t);
  }

  virtual T visit_i32(t_base_type *t) {
    return visit_base(t);
  }

  virtual T visit_i64(t_base_type *t) {
    return visit_base(t);
  }
};

template <typename T>
T visit(t_type* type, t_visitor<T>& visitor) {
  if (type->is_base_type()) {
    t_base_type* base_type = static_cast<t_base_type*>(type);
    switch (base_type->get_base()) {
    case t_base_type::TYPE_VOID: return visitor.visit_void(base_type);
    case t_base_type::TYPE_BOOL: return visitor.visit_bool(base_type);
    case t_base_type::TYPE_I8: return visitor.visit_i8(base_type);
    case t_base_type::TYPE_I16: return visitor.visit_i16(base_type);
    case t_base_type::TYPE_I32: return visitor.visit_i32(base_type);
    case t_base_type::TYPE_I64: return visitor.visit_i64(base_type);
    case t_base_type::TYPE_DOUBLE: return visitor.visit_double(base_type);
    case t_base_type::TYPE_STRING: return base_type->is_binary()
        ? visitor.visit_binary(base_type)
        : visitor.visit_string(base_type);
    default:
        throw "Unexpected base value: " + boost::lexical_cast<std::string>(base_type->get_base());
    }
  } else if (type->is_list()) {
    return visitor.visit_list(static_cast<t_list*>(type));
  } else if (type->is_set()) {
    return visitor.visit_set(static_cast<t_set*>(type));
  } else if (type->is_map()) {
    return visitor.visit_map(static_cast<t_map*>(type));
  } else if (type->is_enum()) {
    return visitor.visit_enum(static_cast<t_enum*>(type));
  } else if (type->is_struct()) {
    return visitor.visit_struct(static_cast<t_struct*>(type));
  } else if (type->is_typedef()) {
    return visitor.visit_typedef(static_cast<t_typedef*>(type));
  } else if (type->is_service()) {
    return visitor.visit_service(static_cast<t_service*>(type));
  } else {
    throw std::string{"Unexpected type: "} + type->get_name();
  }
}

#endif
