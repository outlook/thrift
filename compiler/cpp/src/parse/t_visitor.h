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

#ifndef THRIFT_COMPILER_PARSE_VISITOR_H__
#define THRIFT_COMPILER_PARSE_VISITOR_H__

#include "t_type.h"
#include "t_base_type.h"
#include "t_list.h"
#include "t_set.h"
#include "t_map.h"
#include "t_struct.h"
#include "t_enum.h"
#include "t_service.h"

struct t_visitor {
  virtual void visitVoid(t_base_type *) {
    throw "visitVoid: unimplemented";
  }

  virtual void visitBinary(t_base_type *) {
    throw "visitBinary: unimplemented";
  }

  virtual void visitString(t_base_type *) {
    throw "visitString: unimplemented";
  }

  virtual void visitBool(t_base_type *) {
    throw "visitBool: unimplemented";
  }

  virtual void visitByte(t_base_type *) {
    throw "visitByte: unimplemented";
  }

  virtual void visitI16(t_base_type *) {
    throw "visitI16: unimplemented";
  }

  virtual void visitI32(t_base_type *) {
    throw "visitI32: unimplemented";
  }

  virtual void visitI64(t_base_type *) {
    throw "visitI64: unimplemented";
  }

  virtual void visitDouble(t_base_type *) {
    throw "visitDouble: unimplemented";
  }

  virtual void visitList(t_list *) {
    throw "visitList: unimplemented";
  }

  virtual void visitSet(t_set *) {
    throw "visitSet: unimplemented";
  }

  virtual void visitMap(t_map *) {
    throw "visitMap: unimplemented";
  }

  virtual void visitStruct(t_struct *) {
    throw "visitStruct: unimplemented";
  }

  virtual void visitEnum(t_enum *) {
    throw "visitEnum: unimplemented";
  }

  virtual void visitService(t_service *) {
    throw "visitService: unimplemented";
  }
};

#endif

