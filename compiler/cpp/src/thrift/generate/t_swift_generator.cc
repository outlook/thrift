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

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>

#include <stdlib.h>
#include <sys/stat.h>
#include <sstream>
#include "thrift/platform.h"
#include "thrift/generate/t_oop_generator.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>

using std::map;
using std::ostream;
using std::ofstream;
using std::ostringstream;
using std::set;
using std::string;
using std::stringstream;
using std::vector;

static const string endl = "\n"; // avoid ostream << std::endl flushes

/**
 * Swift code generator.
 *
 * Designed from the Objective-C (aka Cocoa) generator.
 */
class t_swift_generator : public t_oop_generator {
public:
  t_swift_generator(t_program* program,
                    const map<string, string>& parsed_options,
                    const string& option_string)
    : t_oop_generator(program) {
    (void)option_string;
    map<string, string>::const_iterator iter;

    log_unexpected_ = false;
    async_clients_ = false;
    promise_kit_ = false;
    debug_descriptions_ = false;
    exclude_thrift_types_ = false;
    telemetry_object_ = false;
    exclude_empty_init_ = false;
    exclude_equatable_ = false;
    exclude_printable_ = false;
    separate_files_ = false;
    struct_ = false;

    for( iter = parsed_options.begin(); iter != parsed_options.end(); ++iter) {
      if( iter->first.compare("log_unexpected") == 0) {
        log_unexpected_ = true;
      } else if( iter->first.compare("async_clients") == 0) {
        async_clients_ = true;
      } else if( iter->first.compare("promise_kit") == 0) {
        promise_kit_ = true;
      } else if( iter->first.compare("debug_descriptions") == 0) {
        debug_descriptions_ = true;
      } else if( iter->first.compare("exclude_thrift_types") == 0) {
        exclude_thrift_types_ = true;
      } else if( iter->first.compare("telemetry_object") == 0) {
        telemetry_object_ = true;
      } else if( iter->first.compare("exclude_empty_init") == 0) {
        exclude_empty_init_ = true;
      } else if( iter->first.compare("exclude_equatable") == 0) {
        exclude_equatable_ = true;
      } else if( iter->first.compare("exclude_printable") == 0) {
        exclude_printable_ = true;
      } else if( iter->first.compare("separate_files") == 0) {
        separate_files_ = true;
      } else if( iter->first.compare("struct") == 0) {
        struct_ = true;
      }
      else {
        throw "unknown option swift:" + iter->first;
      }
    }

    out_dir_base_ = "gen-swift";
  }

  /**
   * Init and close methods
   */

  void init_generator();
  void close_generator();

  void generate_consts(vector<t_const*> consts);

  /**
   * Program-level generation functions
   */

  void generate_typedef(t_typedef* ttypedef);
  void generate_enum(t_enum* tenum);
  void generate_enum(ofstream& f_enum_decl, ofstream& f_enum_impl, t_enum* tenum);
  void generate_struct(t_struct* tstruct);
  void generate_struct(ofstream& f_struct_decl, ofstream& f_struct_impl, t_struct* tstruct);
  void generate_xception(t_struct* txception);
  void generate_xception(ofstream& f_xception_decl, ofstream& f_xception_impl, t_struct* txception);
  void generate_service(t_service* tservice);
  void generate_service(ofstream& f_service_impl, t_service* tservice);

  void print_const_value(ostream& out,
                         string name,
                         t_type* type,
                         t_const_value* value,
                         bool defval = false,
                         bool is_property = false);
  void render_const_value(ostream& out,
                          t_type* type,
                          t_const_value* value);
  void print_doc(ostream& out, t_doc* tdoc, bool should_indent);
  void print_struct_init_doc(ostream& out, t_struct* tstruct, const vector<t_field*>& fields, bool all);

  void generate_swift_struct(ofstream& out,
                             t_struct* tstruct,
                             bool is_private);
  void generate_swift_struct_init(ofstream& out,
                                  t_struct* tstruct,
                                  bool all,
                                  bool is_private);

  void generate_swift_struct_implementation(ofstream& out,
                                            t_struct* tstruct,
                                            bool is_result,
                                            bool is_private);
  void generate_swift_struct_hashable_extension(ofstream& out,
                                                t_struct* tstruct,
                                                bool is_private);
  void generate_swift_struct_equatable_extension(ofstream& out,
                                                 t_struct* tstruct,
                                                 bool is_private);
  void generate_swift_struct_telemetry_object_extension(ofstream& out, t_struct* tstruct);
  void generate_swift_struct_telemetry_event_extension(ofstream& out, t_struct* tstruct);
  void telemetry_dictionary_value(ofstream& out, t_type* type, string property_name);
  void generate_swift_struct_thrift_extension(ofstream& out,
                                              t_struct* tstruct,
                                              bool is_result,
                                              bool is_private);
  void generate_swift_struct_reader(ofstream& out, t_struct* tstruct, bool is_private);
  void generate_swift_struct_writer(ofstream& out,t_struct* tstruct, bool is_private);
  void generate_swift_struct_result_writer(ofstream& out, t_struct* tstruct);
  void generate_swift_struct_printable_extension(ofstream& out, t_struct* tstruct);

  string function_result_helper_struct_type(t_service *tservice, t_function* tfunction);
  string function_args_helper_struct_type(t_service* tservice, t_function* tfunction);
  void generate_function_helpers(ofstream& f_service_impl, t_service *tservice, t_function* tfunction);

  /**
   * Service-level generation functions
   */

  void generate_swift_service_protocol(ofstream& out, t_service* tservice);
  void generate_swift_service_protocol_async(ofstream& out, t_service* tservice);

  void generate_swift_service_client(ofstream& out, t_service* tservice);
  void generate_swift_service_client_async(ofstream& out, t_service* tservice);

  void generate_swift_service_client_send_function_implementation(ofstream& out,
                                                                  t_service* tservice,
                                                                  t_function* tfunction,
                                                                  bool needs_protocol);
  void generate_swift_service_client_send_function_invocation(ofstream& out, t_function* tfunction);
  void generate_swift_service_client_send_async_function_invocation(ofstream& out,
                                                                    t_function* tfunction);
  void generate_swift_service_client_recv_function_implementation(ofstream& out,
                                                                  t_service* tservice,
                                                                  t_function* tfunction,
                                                                  bool needs_protocol);
  void generate_swift_service_client_implementation(ofstream& out, t_service* tservice);
  void generate_swift_service_client_async_implementation(ofstream& out, t_service* tservice);

  void generate_swift_service_server(ofstream& out, t_service* tservice);
  void generate_swift_service_server_implementation(ofstream& out, t_service* tservice);
  void generate_swift_service_helpers(ofstream& f_service_impl, t_service* tservice);

  void create_file(ofstream& out, string file_name);

  /**
   * Helper rendering functions
   */

  string telemetry_object_protocols();
  string swift_imports();
  string swift_thrift_imports();
  string type_name(t_type* ttype, bool is_optional=false, bool is_forced=false);
  string base_type_name(t_base_type* tbase);
  string declare_property(t_field* tfield, bool is_private);
  string function_signature(t_function* tfunction);
  string async_function_signature(t_function* tfunction);
  string promise_function_signature(t_function* tfunction);
  string function_name(t_function* tfunction);
  string argument_list(t_struct* tstruct, string protocol_name, bool is_internal);
  string type_to_enum(t_type* ttype, bool qualified=false);
  string maybe_escape_identifier(const string& identifier);
  void populate_reserved_words();
  string enum_value_name(t_enum_value* tenumvalue);
  string struct_property_name(t_field* field);
  string camel_case_from_underscore(string underscore_name);

private:

  void block_open(ostream& out) {
    out << " {" << endl;
    indent_up();
  }

  void block_close(ostream& out, bool end_line=true) {
    indent_down();
    indent(out) << "}";
    if (end_line) out << endl;
  }


  bool field_is_optional(t_field* tfield) {
    return tfield->get_req() == t_field::T_OPTIONAL;
  }

  bool struct_has_required_fields(t_struct* tstruct) {
    const vector<t_field*>& members = tstruct->get_members();
    vector<t_field*>::const_iterator m_iter;
    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      if (!field_is_optional(*m_iter)) {
        return true;
      }
    }
    return false;
  }

  bool struct_has_optional_fields(t_struct* tstruct) {
    const vector<t_field*>& members = tstruct->get_members();
    vector<t_field*>::const_iterator m_iter;
    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      if (field_is_optional(*m_iter)) {
        return true;
      }
    }
    return false;
  }

  string constants_declarations_;

  /**
   * File streams
   */

  ofstream f_decl_;
  ofstream f_impl_;

  string f_decl_name_;
  string f_impl_name_;

  bool log_unexpected_;
  bool async_clients_;
  bool promise_kit_;
  bool debug_descriptions_;
  bool exclude_thrift_types_;
  bool telemetry_object_;
  bool exclude_empty_init_;
  bool exclude_equatable_;
  bool exclude_printable_;
  bool separate_files_;
  bool struct_;

  set<string> swift_reserved_words_;
};

/**
 * Prepares for file generation by opening up the necessary file output
 * streams.
 */
void t_swift_generator::init_generator() {
  // Make output directory
  MKDIR(get_out_dir().c_str());

  populate_reserved_words();

  // we have a declarations file...
  f_decl_name_ = get_out_dir() + capitalize(program_name_) + ".swift";
  create_file(f_decl_, capitalize(program_name_));

  // ...and a implementation extensions file
  f_impl_name_ = get_out_dir() + capitalize(program_name_) + "+Exts.swift";
  create_file(f_impl_, capitalize(program_name_) + "+Exts");

  if (telemetry_object_) {
    f_impl_ << telemetry_object_protocols() << endl << endl;
  }
}

/**
 * Prints protocols necessary for telemetry objects
 *
 * @return Protocols for telemetry objects
 */
string t_swift_generator::telemetry_object_protocols() {
  return R"objc(
public typealias TelemetryDictionary = [String: TelemetryValue]

public protocol TelemetryObject {
  func telemetryDictionary() -> TelemetryDictionary
}

public protocol TelemetryEvent: TelemetryObject { }

public enum TelemetryValue: Equatable {
  case string(String)
  case bool(Bool)
  case dictionary(TelemetryDictionary)

  init(_ value: Any) {
    if let string = value as? String {
      self = .string(string)
    }
    else if let bool = value as? Bool {
      self = .bool(bool)
    }
    else if let telemetryObject = value as? TelemetryObject {
      self = .dictionary(telemetryObject.telemetryDictionary())
    }
    else {
      // Convert other types to string
      self = .string("\(value)")
    }
  }
}

)objc";
}

/**
 * Prints standard Cocoa imports
 *
 * @return List of imports for Cocoa libraries
 */
string t_swift_generator::swift_imports() {

  vector<string> includes_list;
  includes_list.push_back("Foundation");

  ostringstream includes;

  vector<string>::const_iterator i_iter;
  for (i_iter=includes_list.begin(); i_iter!=includes_list.end(); ++i_iter) {
    includes << "import " << *i_iter << endl;
  }

  includes << endl;

  return includes.str();
}

/**
 * Prints Thrift runtime imports
 *
 * @return List of imports necessary for Thrift runtime
 */
string t_swift_generator::swift_thrift_imports() {

  vector<string> includes_list;
  if (!exclude_thrift_types_) {
    includes_list.push_back("Thrift");
  }

  if (promise_kit_) {
    includes_list.push_back("PromiseKit");
  }

  ostringstream includes;

  vector<string>::const_iterator i_iter;
  for (i_iter=includes_list.begin(); i_iter!=includes_list.end(); ++i_iter) {
    includes << "import " << *i_iter << endl;
  }

  includes << endl;

  return includes.str();
}

/**
 * Finish up generation.
 */
void t_swift_generator::close_generator() {
  // stick our constants declarations at the end of the header file
  // since they refer to things we are defining.
  f_decl_ << constants_declarations_ << endl;
}

/**
 * Generates a typedef. This is just a simple 1-liner in Swift
 *
 * @param ttypedef The type definition
 */
void t_swift_generator::generate_typedef(t_typedef* ttypedef) {
  if (boost::algorithm::ends_with(ttypedef->get_symbolic(), "TeleMetadataOnly")) {
    return;
  }

  f_decl_ << indent() << "public typealias " << ttypedef->get_symbolic()
          << " = " << type_name(ttypedef->get_type()) << endl;
  f_decl_ << endl;
}

/**
 * Generates code for an enumerated type. In Swift, this is
 * essentially the same as the thrift definition itself, using
 * Swift syntax.
 *
 * @param tenum The enumeration
 */
void t_swift_generator::generate_enum(t_enum* tenum) {
  if (separate_files_) {
    ofstream f_enum;
    create_file(f_enum, tenum->get_name());

    generate_enum(f_enum, f_enum, tenum);
  }
  else {
    generate_enum(f_decl_, f_impl_, tenum);
  }
}

void t_swift_generator::generate_enum(ofstream& f_enum_decl, ofstream& f_enum_impl, t_enum* tenum) {
  print_doc(f_enum_decl, tenum, false);

  f_enum_decl << indent() << "public enum " << tenum->get_name() << " : Int32";
  block_open(f_enum_decl);

  vector<t_enum_value*> constants = tenum->get_constants();
  vector<t_enum_value*>::iterator c_iter;

  for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {
    print_doc(f_enum_decl, *c_iter, true);
    f_enum_decl << indent() << "case " << enum_value_name(*c_iter)
                << " = " << (*c_iter)->get_value() << endl;
  }

  if (!exclude_empty_init_) {
    f_enum_decl << endl;
    f_enum_decl << indent() << "public init() { self.init(rawValue: " << constants.front()->get_value() << ")! }" << endl;
  }

  block_close(f_enum_decl);
  f_enum_decl << endl;

  f_enum_impl << indent() << "extension " << tenum->get_name();
  if (!exclude_thrift_types_) {
    f_enum_impl << " : TEnum";
  }
  block_open(f_enum_impl);

  f_enum_impl << endl;

  if (!exclude_thrift_types_) {
    f_enum_impl << indent() << "public static func readValueFromProtocol(proto: TProtocol) throws -> " << tenum->get_name();
    block_open(f_enum_impl);
    f_enum_impl << indent() << "var raw = Int32()" << endl
                << indent() << "try proto.readI32(&raw)" << endl
                << indent() << "return " << tenum->get_name() << "(rawValue: raw)!" << endl;
    block_close(f_enum_impl);
    f_enum_impl << endl;

    f_enum_impl << indent() << "public static func writeValue(value: " << tenum->get_name() << ", toProtocol proto: TProtocol) throws";
    block_open(f_enum_impl);
    f_enum_impl << indent() << "try proto.writeI32(value.rawValue)" << endl;
    block_close(f_enum_impl);
    f_enum_impl << endl;
  }

  if (telemetry_object_) {
    f_enum_impl << indent() << "public func telemetryName() -> String";
    block_open(f_enum_impl);
    if (boost::algorithm::ends_with(tenum->get_name(), "AsInt")) {
      f_enum_impl << indent() << "return \"\\(rawValue)\"" << endl;
    }
    else {
      f_enum_impl << indent() << "switch self {" << endl;
      for (const auto& value : tenum->get_constants()) {
        f_enum_impl << indent() << "case ." << enum_value_name(value) << ": return \"" << value->get_name() << "\"" << endl;
      }
      f_enum_impl << indent() << "}" << endl;
    }
    block_close(f_enum_impl);
  }

  block_close(f_enum_impl);
  f_enum_impl << endl;
}

/**
 * Generates public constants for all Thrift constants.
 *
 * @param consts Constants to generate
 */
void t_swift_generator::generate_consts(vector<t_const*> consts) {

  ostringstream const_interface;

  // Public constants for base types & strings
  vector<t_const*>::iterator c_iter;
  for (c_iter = consts.begin(); c_iter != consts.end(); ++c_iter) {
    t_type* type = (*c_iter)->get_type();
    if (boost::algorithm::ends_with(type_name(type), "TeleMetadataOnly")) {
      continue;
    }
    const_interface << "public let " << capitalize((*c_iter)->get_name()) << " : " << type_name(type) << " = ";
    render_const_value(const_interface, type, (*c_iter)->get_value());
    const_interface << endl << endl;
  }

  // this gets spit into the header file in ::close_generator
  constants_declarations_ = const_interface.str();

}

/**
 * Generates a struct definition for a thrift data type. This is a struct
 * with public members. Optional types are used for optional properties to
 * allow them to be tested for availability. Separate inits are included for
 * required properties & all properties.
 *
 * Generates extensions to provide conformance to TStruct, TSerializable,
 * Hashable & Equatable
 *
 * @param tstruct The struct definition
 */
void t_swift_generator::generate_struct(t_struct* tstruct) {
  if (separate_files_) {
    ofstream f_struct;
    create_file(f_struct, tstruct->get_name());

    generate_struct(f_struct, f_struct, tstruct);
  }
  else {
    generate_struct(f_decl_, f_impl_, tstruct);
  }
}

void t_swift_generator::generate_struct(ofstream& f_struct_decl, ofstream& f_struct_impl, t_struct* tstruct) {
  generate_swift_struct(f_struct_decl, tstruct, false);
  generate_swift_struct_implementation(f_struct_impl, tstruct, false, false);
}

/**
 * Exceptions are structs, but they conform to ErrorType
 *
 * @param tstruct The struct definition
 */
void t_swift_generator::generate_xception(t_struct* txception) {
  if (separate_files_) {
    ofstream f_xception;
    create_file(f_xception, txception->get_name());

    generate_xception(f_xception, f_xception, txception);
  }
  else {
    generate_xception(f_decl_, f_impl_, txception);
  }
}

void t_swift_generator::generate_xception(ofstream& f_xception_decl, ofstream& f_xception_impl, t_struct* txception) {
  generate_swift_struct(f_xception_decl, txception, false);
  generate_swift_struct_implementation(f_xception_impl, txception, false, false);
}

/**
 * Generate the interface for a struct. Only properties and
 * init methods are included.
 *
 * @param tstruct The struct definition
 * @param is_private
 *                Is the struct public or private
 */
void t_swift_generator::generate_swift_struct(ofstream& out,
                                              t_struct* tstruct,
                                              bool is_private) {
  print_doc(out, tstruct, false);

  string visibility = is_private ? "private" : "public";
  string object_type = struct_ ? "struct" : "final class";

  out << indent() << visibility << " " << object_type << " " << tstruct->get_name();

  if (tstruct->is_xception()) {
    out << " : ErrorType";
  }

  block_open(out);

  // properties
  const vector<t_field*>& members = tstruct->get_members();
  vector<t_field*>::const_iterator m_iter;

  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    out << endl;
    out << declare_property(*m_iter, is_private) << endl;
  }

  out << endl;

  // init

  if (!exclude_empty_init_ || !struct_has_required_fields(tstruct)) {
    indent(out) << visibility << " init()";
    block_open(out);
    block_close(out);

    out << endl;
  }

  if (struct_has_required_fields(tstruct)) {
    generate_swift_struct_init(out, tstruct, false, is_private);
  }
  if (struct_has_optional_fields(tstruct)) {
    generate_swift_struct_init(out, tstruct, true, is_private);
  }

  block_close(out);

  out << endl;
}

/**
 * Generate struct init for properties
 *
 * @param tstruct The structure definition
 * @param all     Generate init with all or just required properties
 * @param is_private
 *                Is the initializer public or private
 */
void t_swift_generator::generate_swift_struct_init(ofstream& out,
                                                   t_struct* tstruct,
                                                   bool all,
                                                   bool is_private) {

  const vector<t_field*>& members = tstruct->get_members();

  print_struct_init_doc(out, tstruct, members, all);

  string visibility = is_private ? "private" : "public";

  indent(out) << visibility << " init(";

  vector<t_field*>::const_iterator m_iter;

  bool first=true;
  for (m_iter = members.begin(); m_iter != members.end();) {
    if ((all || !field_is_optional(*m_iter)) && !boost::algorithm::ends_with(type_name((*m_iter)->get_type()), "TeleMetadataOnly")) {
      if (first) {
        first = false;
      }
      else {
        out << ", ";
      }
      out << struct_property_name(*m_iter) << ": "
          << maybe_escape_identifier(type_name((*m_iter)->get_type(), field_is_optional(*m_iter)));
    }
    ++m_iter;
  }
  out << ")";

  block_open(out);

  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    if ((all || (*m_iter)->get_req() == t_field::T_REQUIRED || (*m_iter)->get_req() == t_field::T_OPT_IN_REQ_OUT) &&
      !boost::algorithm::ends_with(type_name((*m_iter)->get_type()), "TeleMetadataOnly")) {
      out << indent() << "self." << struct_property_name(*m_iter) << " = "
          << struct_property_name(*m_iter) << endl;
    }
  }

  block_close(out);

  out << endl;
}

/**
 * Generate the hashable protocol implmentation
 *
 * @param tstruct The structure definition
 * @param is_private
 *                Is the struct public or private
 */
void t_swift_generator::generate_swift_struct_hashable_extension(ofstream& out,
                                                                 t_struct* tstruct,
                                                                 bool is_private) {

  string visibility = is_private ? "private" : "public";

  indent(out) << "extension " << tstruct->get_name() << " : Hashable";

  block_open(out);

  out << endl;

  indent(out) << visibility << " func hash(into hasher: inout Hasher)";

  block_open(out);


  const vector<t_field*>& members = tstruct->get_members();
  vector<t_field*>::const_iterator m_iter;

  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    t_field* tfield = *m_iter;
    indent(out) << "hasher.combine(" << struct_property_name(tfield) << ")" << endl;
  }

  block_close(out);

  out << endl;

  block_close(out);

  out << endl;
}

/**
 * Generate the equatable protocol implementation
 *
 * @param tstruct The structure definition
 * @param is_private
 *                Is the struct public or private
 */
void t_swift_generator::generate_swift_struct_equatable_extension(ofstream& out,
                                                                  t_struct* tstruct,
                                                                  bool is_private) {

  string visibility = is_private ? "private" : "public";

  indent(out) << visibility << " func ==(lhs: " << type_name(tstruct) << ", rhs: " << type_name(tstruct) << ") -> Bool";

  block_open(out);

  indent(out) << "return";

  const vector<t_field*>& members = tstruct->get_members();
  vector<t_field*>::const_iterator m_iter;

  if (members.size()) {

    out << endl;

    indent_up();

    for (m_iter = members.begin(); m_iter != members.end();) {
      t_field* tfield = *m_iter;
      indent(out) << "(lhs." << struct_property_name(tfield)
                  << " == rhs." << struct_property_name(tfield) << ")";
      if (++m_iter != members.end()) {
        out << " &&";
      }
      out << endl;
    }

    indent_down();

  }
  else {
    out << " true" << endl;
  }

  block_close(out);

  out << endl;
}

/**
 * Generate struct implementation. Produces extensions that
 * fulfill the requisite protocols to complete the value.
 *
 * @param tstruct The struct definition
 * @param is_result
 *                If this is a result it needs a different writer
 * @param is_private
 *                Is the struct public or private
 */
void t_swift_generator::generate_swift_struct_implementation(ofstream& out,
                                                             t_struct* tstruct,
                                                             bool is_result,
                                                             bool is_private) {

  if (!exclude_equatable_) {
    generate_swift_struct_equatable_extension(out, tstruct, is_private);
  }

  if (!is_private && !is_result && !exclude_printable_) {
    generate_swift_struct_printable_extension(out, tstruct);
  }

  if (!exclude_equatable_) {
    generate_swift_struct_hashable_extension(out, tstruct, is_private);
  }

  if (!exclude_thrift_types_) {
    generate_swift_struct_thrift_extension(out, tstruct, is_result, is_private);
  }
  if (telemetry_object_) {
    generate_swift_struct_telemetry_object_extension(out, tstruct);
    generate_swift_struct_telemetry_event_extension(out, tstruct);
  }
  out << endl << endl;
}

/**
 * Generate the TelemetryEvent protocol implementation
 *
 * @param tstruct The structure definition
 */
void t_swift_generator::generate_swift_struct_telemetry_event_extension(ofstream& out, t_struct* tstruct) {
  bool contains_event_name = false;

  for (const auto& member : tstruct->get_members()) {
    if (member->get_name() == "event_name") {
      contains_event_name = true;
      break;
    }
  }

  if (!contains_event_name) {
    // This is not an event struct, do not add the protocol
    return;
  }

  indent(out) << "extension " << tstruct->get_name() << " : TelemetryEvent";
  block_open(out);
  block_close(out);

  out << endl;
}

/**
 * Generate the TelemetryObject protocol implementation
 *
 * @param tstruct The structure definition
 */
void t_swift_generator::generate_swift_struct_telemetry_object_extension(ofstream& out, t_struct* tstruct) {
  indent(out) << "extension " << tstruct->get_name() << " : TelemetryObject";
  block_open(out);

  out << endl;

  out << indent() << "public func telemetryDictionary() -> TelemetryDictionary";
  block_open(out);

  out << endl;

  out << indent() << "var telemetryData = TelemetryDictionary()" << endl;

  for (const auto& member : tstruct->get_members()) {
    bool optional = field_is_optional(member);

    // types labeled as NonTelemetry will not be in the resulting telemetry dictionary
    if (boost::algorithm::ends_with(type_name(member->get_type()), "NonTelemetry") ||
      boost::algorithm::ends_with(type_name(member->get_type()), "TeleMetadataOnly")) {
      continue;
    }

    if (optional) {
      out << indent() << "if let " << struct_property_name(member) << " = " << struct_property_name(member);
      block_open(out);
    }

    out << indent() << "telemetryData[\"" << member->get_name() << "\"] = ";
    telemetry_dictionary_value(out, member->get_type(), struct_property_name(member));
    out << endl;

    if (optional) {
      block_close(out);
    }
  }

  out << indent() << "return telemetryData" << endl;

  block_close(out);
  block_close(out);

  out << endl;
}

void t_swift_generator::telemetry_dictionary_value(ofstream& out, t_type* type, string property_name) {
  type = get_true_type(type);

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_STRING:
    case t_base_type::TYPE_BOOL:
    case t_base_type::TYPE_I8:
    case t_base_type::TYPE_I16:
    case t_base_type::TYPE_I32:
    case t_base_type::TYPE_I64:
    case t_base_type::TYPE_DOUBLE:
      out << "TelemetryValue(" << property_name << ")";
      break;
    default:
      throw "compiler error: invalid base type " + type->get_name();
      break;
    }
  } else if (type->is_map()) {
    t_map *tmap = (t_map*)type;

    out << ".dictionary({";

    indent_up();
    out << endl;

    out << indent() << "var dictionary = TelemetryDictionary()" << endl;
    out << indent() << "for (key, value) in " << property_name;

    block_open(out);

    out << indent() << "dictionary[";

    t_type* key_type = get_true_type(tmap->get_key_type());
    if (key_type->is_string()) {
      out << "key";
    } else if (key_type->is_enum()) {
      out << "key.telemetryName()";
    }
    else {
      throw "compiler error: unsupported key type for map " + key_type->get_name();
    }

    out << "] = ";

    telemetry_dictionary_value(out, tmap->get_val_type(), "value");

    out << endl;

    block_close(out);

    out << indent() << "return dictionary" << endl;

    block_close(out, false);
    out << "())";
  } else if (type->is_enum()) {
    out << ".string(" << property_name << ".telemetryName())";
  } else if (type->is_struct()) {
    out << "TelemetryValue(" << property_name << ")";
  }
  else {
    throw "compiler error: invalid type (" + type_name(type) + ") for property \"" + property_name + "\"";
  }
}

/**
 * Generate the TStruct protocol implementation.
 *
 * @param tstruct The structure definition
 * @param is_result
 *                Is the struct a result value
 * @param is_private
 *                Is the struct public or private
 */
void t_swift_generator::generate_swift_struct_thrift_extension(ofstream& out,
                                                               t_struct* tstruct,
                                                               bool is_result,
                                                               bool is_private) {

  indent(out) << "extension " << tstruct->get_name() << " : TStruct";
  block_open(out);

  out << endl;

  generate_swift_struct_reader(out, tstruct, is_private);

  if (is_result) {
    generate_swift_struct_result_writer(out, tstruct);
  }
  else {
    generate_swift_struct_writer(out, tstruct, is_private);
  }

  block_close(out);

  out << endl;
}

/**
 * Generates a function to read a struct from
 * from a protocol. (TStruct compliance)
 *
 * @param tstruct The structure definition
 * @param is_private
 *                Is the struct public or private
 */
void t_swift_generator::generate_swift_struct_reader(ofstream& out,
                                                     t_struct* tstruct,
                                                     bool is_private) {

  string visibility = is_private ? "private" : "public";

  indent(out) << visibility << " static func readValueFromProtocol(__proto: TProtocol) throws -> "
              << tstruct->get_name();

  block_open(out);

  out << endl;

  indent(out) << "try __proto.readStructBegin()" << endl << endl;

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    bool optional = field_is_optional(*f_iter);
    indent(out) << "var " << struct_property_name(*f_iter) << " : "
                << type_name((*f_iter)->get_type(), optional, !optional) << endl;
  }

  out << endl;

  // Loop over reading in fields
  indent(out) << "fields: while true";

  block_open(out);

  out << endl;

  indent(out) << "let (_, fieldType, fieldID) = try __proto.readFieldBegin()" << endl << endl;
  indent(out) << "switch (fieldID, fieldType)";

  block_open(out);

  indent(out) << "case (_, .STOP):" << endl;
  indent_up();
  indent(out) << "break fields" << endl << endl;
  indent_down();

  // Generate deserialization code for known cases
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {

    indent(out) << "case (" << (*f_iter)->get_key() << ", " << type_to_enum((*f_iter)->get_type()) << "):" << endl;
    indent_up();
    indent(out) << maybe_escape_identifier((*f_iter)->get_name()) << " = try __proto.readValue() as "
                << type_name((*f_iter)->get_type()) << endl << endl;
    indent_down();

  }

  indent(out) << "case let (_, unknownType):" << endl;
  indent_up();
  indent(out) << "try __proto.skipType(unknownType)" << endl;
  indent_down();

  block_close(out);

  out << endl;

  // Read field end marker
  indent(out) << "try __proto.readFieldEnd()" << endl;

  block_close(out);

  out << endl;

  indent(out) << "try __proto.readStructEnd()" << endl;

  out << endl;

  if (struct_has_required_fields(tstruct)) {
    // performs various checks (e.g. check that all required fields are set)
    indent(out) << "// Required fields" << endl;

    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      if (field_is_optional(*f_iter)) {
        continue;
      }
      indent(out) << "try __proto.validateValue(" << (*f_iter)->get_name() << ", "
                  << "named: \"" << (*f_iter)->get_name() << "\")" << endl;
    }
  }

  out << endl;

  indent(out) << "return " << tstruct->get_name() << "(";
  for (f_iter = fields.begin(); f_iter != fields.end();) {
    out << (*f_iter)->get_name() << ": " << maybe_escape_identifier((*f_iter)->get_name());
    if (++f_iter != fields.end()) {
      out << ", ";
    }
  }
  out << ")" << endl;

  block_close(out);

  out << endl;
}

/**
 * Generates a function to write a struct to
 * a protocol. (TStruct compliance)
 *
 * @param tstruct The structure definition
 * @param is_private
 *                Is the struct public or private
 */
void t_swift_generator::generate_swift_struct_writer(ofstream& out,
                                                     t_struct* tstruct,
                                                     bool is_private) {

  string visibility = is_private ? "private" : "public";

  indent(out) << visibility << " static func writeValue(__value: " << tstruct->get_name() << ", toProtocol __proto: TProtocol) throws";

  block_open(out);

  out << endl;

  string name = tstruct->get_name();
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  indent(out) << "try __proto.writeStructBeginWithName(\"" << name << "\")" << endl;

  out << endl;

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    t_field *tfield = *f_iter;

    bool optional = field_is_optional(tfield);
    if (optional) {
      indent(out) << "if let " << struct_property_name(tfield)
                  << " = __value." << struct_property_name(tfield);
      block_open(out);
    }

    indent(out) << "try __proto.writeFieldValue("
                << (optional ? "" : "__value.") << maybe_escape_identifier(tfield->get_name()) << ", "
                << "name: \"" << tfield->get_name() << "\", "
                << "type: " << type_to_enum(tfield->get_type()) << ", "
                << "id: " << tfield->get_key() << ")" << endl;

    if (optional) {
      block_close(out);
    }

    out << endl;
  }

  indent(out) << "try __proto.writeFieldStop()" << endl << endl;

  indent(out) << "try __proto.writeStructEnd()" << endl;

  block_close(out);

  out << endl;
}

/**
 * Generates a function to read a struct from
 * from a protocol. (TStruct compliance)
 *
 * This is specifically a function result. Only
 * the first available field is written.
 *
 * @param tstruct The structure definition
 */
void t_swift_generator::generate_swift_struct_result_writer(ofstream& out, t_struct* tstruct) {

  indent(out) << "private static func writeValue(__value: " << tstruct->get_name() << ", toProtocol __proto: TProtocol) throws";

  block_open(out);

  out << endl;

  string name = tstruct->get_name();
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  indent(out) << "try __proto.writeStructBeginWithName(\"" << name << "\")" << endl;

  out << endl;

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    t_field *tfield = *f_iter;

    indent(out) << "if let result = __value." << struct_property_name(*f_iter);

    block_open(out);

    indent(out) << "try __proto.writeFieldValue(result, "
                << "name: \"" << tfield->get_name() << "\", "
                << "type: " << type_to_enum(tfield->get_type()) << ", "
                << "id: " << tfield->get_key() << ")" << endl;

    block_close(out);
  }
  // Write the struct map
  indent(out) << "try __proto.writeFieldStop()" << endl << endl;

  indent(out) << "try __proto.writeStructEnd()" << endl;

  block_close(out);

  out << endl;
}

/**
 * Generates a description method for the given struct
 *
 * @param tstruct The struct definition
 */
void t_swift_generator::generate_swift_struct_printable_extension(ofstream& out, t_struct* tstruct) {

  // Allow use of debugDescription so the app can add description via a cateogory/extension

  indent(out) << "extension " << tstruct->get_name() << " : "
              << (debug_descriptions_ ? "CustomDebugStringConvertible" : "CustomStringConvertible");

  block_open(out);

  out << endl;

  indent(out) << "public var description : String";

  block_open(out);

  indent(out) << "var desc = \"" << tstruct->get_name() << "(\"" << endl;

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  for (f_iter = fields.begin(); f_iter != fields.end();) {
    indent(out) << "desc += \"" << struct_property_name(*f_iter)
                << "=\\(String(describing: self." << struct_property_name(*f_iter) << "))";
    if (++f_iter != fields.end()) {
      out << ", ";
    }
    out << "\"" << endl;
  }
  indent(out) << "desc += \")\"" << endl;
  indent(out) << "return desc" << endl;

  block_close(out);

  out << endl;

  block_close(out);

  out << endl;
}

/**
 * Generates a thrift service.  In Swift this consists of a
 * protocol definition and a client (with it's implementation
 * separated into exts file).
 *
 * @param tservice The service definition
 */
void t_swift_generator::generate_service(t_service* tservice) {
  if (separate_files_) {
    ofstream f_service;
    create_file(f_service, tservice->get_name());

    generate_service(f_service, tservice);
  }
  else {
    generate_service(f_impl_, tservice);
  }
}

void t_swift_generator::generate_service(ofstream& f_service_impl, t_service* tservice) {
  generate_swift_service_protocol(f_decl_, tservice);
  generate_swift_service_client(f_decl_, tservice);
  if (async_clients_) {
    generate_swift_service_protocol_async(f_decl_, tservice);
    generate_swift_service_client_async(f_decl_, tservice);
  }
  generate_swift_service_server(f_decl_, tservice);

  generate_swift_service_helpers(f_service_impl, tservice);

  generate_swift_service_client_implementation(f_service_impl, tservice);
  if (async_clients_) {
    generate_swift_service_client_async_implementation(f_service_impl, tservice);
  }
  generate_swift_service_server_implementation(f_service_impl, tservice);
}

/**
 * Generates structs for all the service return types
 *
 * @param tservice The service
 */
void t_swift_generator::generate_swift_service_helpers(ofstream& f_service_impl, t_service* tservice) {
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {

    t_struct* ts = (*f_iter)->get_arglist();

    string qname = function_args_helper_struct_type(tservice, *f_iter);

    t_struct qname_ts = t_struct(ts->get_program(), qname);

    const vector<t_field*>& members = ts->get_members();
    vector<t_field*>::const_iterator m_iter;
    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      qname_ts.append(*m_iter);
    }

    generate_swift_struct(f_service_impl, &qname_ts, true);
    generate_swift_struct_implementation(f_service_impl, &qname_ts, false, true);
    generate_function_helpers(f_service_impl, tservice, *f_iter);
  }
}

string t_swift_generator::function_result_helper_struct_type(t_service *tservice, t_function* tfunction) {
  if (tfunction->is_oneway()) {
    return tservice->get_name() + "_" + tfunction->get_name();
  } else {
    return tservice->get_name() + "_" + tfunction->get_name() + "_result";
  }
}

string t_swift_generator::function_args_helper_struct_type(t_service *tservice, t_function* tfunction) {
  return tservice->get_name() + "_" + tfunction->get_name() + "_args";
}

/**
 * Generates a struct and helpers for a function.
 *
 * @param tfunction The function
 */
void t_swift_generator::generate_function_helpers(ofstream& f_service_impl, t_service *tservice, t_function* tfunction) {
  if (tfunction->is_oneway()) {
    return;
  }

  // create a result struct with a success field of the return type,
  // and a field for each type of exception thrown
  t_struct result(program_, function_result_helper_struct_type(tservice, tfunction));
  if (!tfunction->get_returntype()->is_void()) {
    t_field* success = new t_field(tfunction->get_returntype(), "success", 0);
    success->set_req(t_field::T_OPTIONAL);
    result.append(success);
  }

  t_struct* xs = tfunction->get_xceptions();
  const vector<t_field*>& fields = xs->get_members();
  vector<t_field*>::const_iterator f_iter;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    t_field *x = *f_iter;
    t_field *ox = new t_field(x->get_type(), x->get_name(), x->get_key());
    ox->set_req(t_field::T_OPTIONAL);
    result.append(ox);
  }

  // generate the result struct
  generate_swift_struct(f_service_impl, &result, true);
  generate_swift_struct_implementation(f_service_impl, &result, true, true);

  for (f_iter = result.get_members().begin(); f_iter != result.get_members().end(); ++f_iter) {
    delete *f_iter;
  }
}

/**
 * Generates a service protocol definition.
 *
 * @param tservice The service to generate a protocol definition for
 */
void t_swift_generator::generate_swift_service_protocol(ofstream& out, t_service* tservice) {

  indent(out) << "public protocol " << tservice->get_name();

  block_open(out);

  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    out << endl;
    indent(out) << function_signature(*f_iter) << "  // exceptions: ";
    t_struct* xs = (*f_iter)->get_xceptions();
    const vector<t_field*>& xceptions = xs->get_members();
    vector<t_field*>::const_iterator x_iter;
    for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
      out << type_name((*x_iter)->get_type()) + ", ";
    }
    out << endl;
  }

  block_close(out);

  out << endl;
}

/**
 * Generates an asynchronous service protocol definition.
 *
 * @param tservice The service to generate a protocol definition for
 */
void t_swift_generator::generate_swift_service_protocol_async(ofstream& out, t_service* tservice) {

  indent(out) << "public protocol " << tservice->get_name() << "Async";

  block_open(out);

  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    out << endl;
    indent(out) << async_function_signature(*f_iter) << endl;
    if (promise_kit_) {
      indent(out) << promise_function_signature(*f_iter) << endl;
    }
    out << endl;
  }

  block_close(out);

  out << endl;
}

/**
 * Generates a service client interface definition.
 *
 * @param tservice The service to generate a client interface definition for
 */
void t_swift_generator::generate_swift_service_client(ofstream& out,
                                                                t_service* tservice) {

  indent(out) << "public class " << tservice->get_name() << "Client /* : " << tservice->get_name() << " */";

  block_open(out);

  out << endl;

  indent(out) << "let __inProtocol : TProtocol" << endl << endl;

  indent(out) << "let __outProtocol : TProtocol" << endl << endl;

  indent(out) << "public init(inoutProtocol: TProtocol)";

  block_open(out);

  indent(out) << "__inProtocol = inoutProtocol" << endl;

  indent(out) << "__outProtocol = inoutProtocol" << endl;

  block_close(out);

  out << endl;

  indent(out) << "public init(inProtocol: TProtocol, outProtocol: TProtocol)";

  block_open(out);

  indent(out) << "__inProtocol = inProtocol" << endl;

  indent(out) << "__outProtocol = outProtocol" << endl;

  block_close(out);

  out << endl;

  block_close(out);

  out << endl;
}

/**
 * Generates a service client interface definition.
 *
 * @param tservice The service to generate a client interface definition for
 */
void t_swift_generator::generate_swift_service_client_async(ofstream& out,
                                                                      t_service* tservice) {

  indent(out) << "public class " << tservice->get_name() << "AsyncClient /* : " << tservice->get_name() << " */";

  block_open(out);

  out << endl;

  indent(out) << "let __protocolFactory : TProtocolFactory" << endl << endl;

  indent(out) << "let __transportFactory : TAsyncTransportFactory" << endl << endl;

  indent(out) << "public init(protocolFactory: TProtocolFactory, transportFactory: TAsyncTransportFactory)";

  block_open(out);

  indent(out) << "__protocolFactory = protocolFactory" << endl;

  indent(out) << "__transportFactory = transportFactory" << endl;

  block_close(out);

  out << endl;

  block_close(out);

  out << endl;
}

/**
 * Generates a service server interface definition. In other words,
 * the TProcess implementation for the service definition.
 *
 * @param tservice The service to generate a client interface definition for
 */
void t_swift_generator::generate_swift_service_server(ofstream& out,
                                                                t_service* tservice) {

  indent(out) << "public class " << tservice->get_name() << "Processor : NSObject /* " << tservice->get_name() << " */";

  block_open(out);

  out << endl;

  out << indent() << "typealias ProcessorHandlerDictionary = "
                  << "[String: (Int, TProtocol, TProtocol, " << tservice->get_name() << ") throws -> Void]" << endl
      << endl
      << indent() << "let service : " << tservice->get_name() << endl
      << endl
      << indent() << "public init(service: " << tservice->get_name() << ")";
  block_open(out);
  indent(out) << "self.service = service" << endl;
  block_close(out);

  out << endl;

  block_close(out);

  out << endl;
}

/**
 * Generates a function that will send the arguments
 * for a service function via a protocol.
 *
 * @param tservice  The service to generate
 * @param tfunction The function to generate
 * @param needs_protocol
 *                  Wether the first parameter must be a protocol or if
 *                  the protocol is to be assumed
 */
void t_swift_generator::generate_swift_service_client_send_function_implementation(ofstream& out,
                                                                                   t_service *tservice,
                                                                                   t_function* tfunction,
                                                                                   bool needs_protocol) {

  string funname = tfunction->get_name();

  t_function send_function(g_type_bool,
                           "send_" + tfunction->get_name(),
                           tfunction->get_arglist());

  string argsname = function_args_helper_struct_type(tservice, tfunction);
  t_struct* arg_struct = tfunction->get_arglist();

  // Open function
  indent(out) << "private func " << send_function.get_name() << "(" << argument_list(tfunction->get_arglist(), needs_protocol ? "__outProtocol" : "", true) << ") throws";
  block_open(out);

  out << endl;

  // Serialize the request
  indent(out) << "try __outProtocol.writeMessageBeginWithName(\"" << funname << "\", "
              << "type: " << (tfunction->is_oneway() ? ".ONEWAY" : ".CALL") << ", "
              << "sequenceID: 0)" << endl;

  out << endl;

  indent(out) << "let __args = " << argsname << "(";

  // write out function parameters

  const vector<t_field*>& fields = arg_struct->get_members();
  vector<t_field*>::const_iterator f_iter;

  for (f_iter = fields.begin(); f_iter != fields.end();) {
    t_field *tfield = (*f_iter);
    out << struct_property_name(tfield) << ": " << struct_property_name(tfield);
    if (++f_iter != fields.end()) {
      out << ", ";
    }
  }
  out << ")" << endl;
  indent(out) << "try " << argsname << ".writeValue(__args, toProtocol: __outProtocol)" << endl << endl;

  indent(out) << "try __outProtocol.writeMessageEnd()" << endl;

  block_close(out);

  out << endl;
}

/**
 * Generates a function that will recv the result for a
 * service function via a protocol.
 *
 * @param tservice  The service to generate
 * @param tfunction The function to generate
 * @param needs_protocol
 *                  Wether the first parameter must be a protocol or if
 *                  the protocol is to be assumed
 */
void t_swift_generator::generate_swift_service_client_recv_function_implementation(ofstream& out,
                                                                                   t_service* tservice,
                                                                                   t_function* tfunction,
                                                                                   bool needs_protocol) {

  // Open function
  indent(out) << "private func recv_" << tfunction->get_name() << "(";

  if (needs_protocol) {
    out << "__inProtocol: TProtocol";
  }

  out << ") throws";

  if (!tfunction->get_returntype()->is_void()) {
    out << " -> " << type_name(tfunction->get_returntype());
  }

  block_open(out);

  // check for an exception

  out << endl;

  indent(out) << "try __inProtocol.readResultMessageBegin() " << endl << endl;

  string resultname = function_result_helper_struct_type(tservice, tfunction);
  indent(out);
  if (!tfunction->get_returntype()->is_void() || !tfunction->get_xceptions()->get_members().empty()) {
    out << "let __result = ";
  }
  out << "try " << resultname << ".readValueFromProtocol(__inProtocol)" << endl << endl;

  indent(out) << "try __inProtocol.readMessageEnd()" << endl << endl;

  // Careful, only return _result if not a void function
  if (!tfunction->get_returntype()->is_void()) {
    indent(out) << "if let __success = __result.success";
    block_open(out);
    indent(out) << "return __success" << endl;
    block_close(out);
  }

  t_struct* xs = tfunction->get_xceptions();
  const vector<t_field*>& xceptions = xs->get_members();
  vector<t_field*>::const_iterator x_iter;

  for (x_iter = xceptions.begin(); x_iter != xceptions.end(); ++x_iter) {
    indent(out) << "if let " << struct_property_name(*x_iter) << " = __result." << struct_property_name(*x_iter);
    block_open(out);
    indent(out) << "throw " << struct_property_name(*x_iter) << endl;
    block_close(out);
  }

  // If you get here it's an exception, unless a void function
  if (!tfunction->get_returntype()->is_void()) {
    indent(out) << "throw NSError(" << endl;
    indent_up();
    indent(out) << "domain: TApplicationErrorDomain, " << endl;
    indent(out) << "code: Int(TApplicationError.MissingResult.rawValue)," << endl;
    indent(out) << "userInfo: [TApplicationErrorMethodKey: \"" << tfunction->get_name() << "\"])" << endl;
    indent_down();
  }

  // Close function
  block_close(out);

  out << endl;
}

/**
 * Generates an invocation of a given the send function for the
 * service function.
 *
 * @param tfunction The service to generate an implementation for
 */
void t_swift_generator::generate_swift_service_client_send_function_invocation(ofstream& out,
                                                                               t_function* tfunction) {

  indent(out) << "try send_" << tfunction->get_name() << "(";

  t_struct* arg_struct = tfunction->get_arglist();

  const vector<t_field*>& fields = arg_struct->get_members();
  vector<t_field*>::const_iterator f_iter;

  for (f_iter = fields.begin(); f_iter != fields.end();) {
    out << (*f_iter)->get_name() << ": " << struct_property_name(*f_iter);
    if (++f_iter != fields.end()) {
      out << ", ";
    }
  }

  out << ")" << endl;
}

/**
 * Generates an invocation of a given the send function for the
 * service function. This is for asynchronous protocols.
 *
 * @param tfunction The service to generate an implementation for
 */
void t_swift_generator::generate_swift_service_client_send_async_function_invocation(ofstream& out,
                                                                                     t_function* tfunction) {

  t_struct* arg_struct = tfunction->get_arglist();
  const vector<t_field*>& fields = arg_struct->get_members();
  vector<t_field*>::const_iterator f_iter;

  indent(out) << "try send_" << tfunction->get_name() << "(__protocol";

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    out << ", " << struct_property_name(*f_iter) << ": " << struct_property_name(*f_iter);
  }

  out << ")" << endl;
}

/**
 * Generates a service client protocol implementation via extension.
 *
 * @param tservice The service to generate an implementation for
 */
void t_swift_generator::generate_swift_service_client_implementation(ofstream& out,
                                                                     t_service* tservice) {

  string name = tservice->get_name() + "Client";

  indent(out) << "extension " << name << " : " << tservice->get_name();

  block_open(out);

  out << endl;

  // generate client method implementations
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::const_iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {

    generate_swift_service_client_send_function_implementation(out, tservice, *f_iter, false);

    if (!(*f_iter)->is_oneway()) {
      generate_swift_service_client_recv_function_implementation(out, tservice, *f_iter, false);
    }

    // Open function
    indent(out) << "public " << function_signature(*f_iter);

    block_open(out);

    out << endl;

    generate_swift_service_client_send_function_invocation(out, *f_iter);

    out << endl;

    indent(out) << "try __outProtocol.transport().flush()" << endl << endl;

    if (!(*f_iter)->is_oneway()) {
      if ((*f_iter)->get_returntype()->is_void()) {
        indent(out) << "try recv_" << (*f_iter)->get_name() << "()" << endl;
      } else {
        indent(out) << "return try recv_" << (*f_iter)->get_name() << "()" << endl;
      }
    }

    block_close(out);

    out << endl;
  }

  block_close(out);

  out << endl;
}

/**
 * Generates a service asynchronous client protocol implementation via extension.
 *
 * @param tservice The service to generate an implementation for
 */
void t_swift_generator::generate_swift_service_client_async_implementation(ofstream& out,
                                                                           t_service* tservice) {

  string name = tservice->get_name() + "AsyncClient";
  string protocol_name = tservice->get_name() + "Async";

  indent(out) << "extension " << name << " : " << protocol_name;

  block_open(out);

  out << endl;

  // generate client method implementations
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::const_iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {

    generate_swift_service_client_send_function_implementation(out, tservice, *f_iter, true);

    if (!(*f_iter)->is_oneway()) {
      generate_swift_service_client_recv_function_implementation(out, tservice, *f_iter, true);
    }

    indent(out) << "public " << async_function_signature(*f_iter);
    block_open(out);

    out << endl;

    out << indent() << "let __transport = __transportFactory.newTransport()" << endl
        << indent() << "let __protocol = __protocolFactory.newProtocolOnTransport(__transport)" << endl
        << endl;

    generate_swift_service_client_send_async_function_invocation(out, *f_iter);

    out << endl;

    indent(out) << "__transport.flushWithCompletion(";

    if ((*f_iter)->is_oneway()) {
      out << "success, failure: failure)" << endl;
    }
    else {
      block_open(out);
      indent(out) << "do";
      block_open(out);

      indent(out);
      if (!(*f_iter)->get_returntype()->is_void()) {
        out << "let result = ";
      }
      out << "try self.recv_" << (*f_iter)->get_name() << "(__protocol)" << endl;

      out << indent() << "success(";
      if (!(*f_iter)->get_returntype()->is_void()) {
        out << "result";
      }
      out << ")" << endl;

      block_close(out);
      indent(out) << "catch let error";
      block_open(out);
      indent(out) << "failure(error as NSError)" << endl;
      block_close(out);
      block_close(out);
      indent(out) << ", failure: failure)" << endl;
    }


    block_close(out);

    out << endl;

    // Promise function
    if (promise_kit_) {

      indent(out) << "public " << promise_function_signature(*f_iter);
      block_open(out);

      out << indent() << "let (__promise, __fulfill, __reject) = Promise<" << type_name((*f_iter)->get_returntype()) << ">.pendingPromise()" << endl << endl
          << indent() << "let __transport = __transportFactory.newTransport()" << endl
          << indent() << "let __protocol = __protocolFactory.newProtocolOnTransport(__transport)" << endl
          << endl;

      generate_swift_service_client_send_async_function_invocation(out, *f_iter);

      out << endl;

      indent(out) << "__transport.flushWithCompletion(";

      if ((*f_iter)->is_oneway()) {
        out << "{ __fulfill() }, failure: { __reject($0) })" << endl;
      }
      else {
        block_open(out);
        indent(out) << "do";
        block_open(out);

        indent(out);
        if (!(*f_iter)->get_returntype()->is_void()) {
          out << "let result = ";
        }
        out << "try self.recv_" << (*f_iter)->get_name() << "(__protocol)" << endl;

        out << indent() << "__fulfill(";
        if (!(*f_iter)->get_returntype()->is_void()) {
          out << "result";
        }
        out << ")" << endl;

        block_close(out);
        indent(out) << "catch let error";
        block_open(out);
        indent(out) << "__reject(error)" << endl;
        block_close(out);
        block_close(out);

        indent(out) << ", failure: { error in " << endl;
        indent_up();
        indent(out) << "__reject(error)" << endl;
        indent_down();
        indent(out) << "})" << endl;
      }

      indent(out) << "return __promise" << endl;

      block_close(out);

      out << endl;

    }

  }

  block_close(out);

  out << endl;
}

/**
 * Generates a service server implementation.
 *
 * Implemented by generating a block for each service function that
 * handles the processing of that function. The blocks are stored in
 * a map and looked up via function/message name.
 *
 * @param tservice The service to generate an implementation for
 */
void t_swift_generator::generate_swift_service_server_implementation(ofstream& out,
                                                                     t_service* tservice) {

  string name = tservice->get_name() + "Processor";

  indent(out) << "extension " << name << " : TProcessor";
  block_open(out);

  out << endl;

  indent(out) << "static let processorHandlers : ProcessorHandlerDictionary =";
  block_open(out);

  out << endl;

  out << indent() << "var processorHandlers = ProcessorHandlerDictionary()" << endl << endl;

  // generate method map for routing incoming calls
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::const_iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {

    t_function* tfunction = *f_iter;

    string args_type = function_args_helper_struct_type(tservice, *f_iter);

    out << indent() << "processorHandlers[\"" << tfunction->get_name() << "\"] = { sequenceID, inProtocol, outProtocol, handler in" << endl
        << endl;

    indent_up();
    out << indent() << "let args = try " << args_type << ".readValueFromProtocol(inProtocol)" << endl
        << endl
        << indent() << "try inProtocol.readMessageEnd()" << endl
        << endl;

    if (!tfunction->is_oneway() ) {
      string result_type = function_result_helper_struct_type(tservice, tfunction);
      indent(out) << "var result = " << result_type << "()" << endl;

      indent(out) << "do";
      block_open(out);

      indent(out);
      if (!tfunction->get_returntype()->is_void()) {
        out << "result.success = ";
      }
      out << "try handler." << function_name(tfunction) << "(";

      t_struct* arg_struct = tfunction->get_arglist();
      const vector<t_field*>& fields = arg_struct->get_members();
      vector<t_field*>::const_iterator f_iter;

      for (f_iter = fields.begin(); f_iter != fields.end();) {
        string fieldName = struct_property_name(*f_iter);
        if (f_iter != fields.begin()) {
          out << fieldName << ": ";
        }
        out << "args." << fieldName;
        if (++f_iter != fields.end()) {
          out << ", ";
        }
      }

      out << ")" << endl;

      block_close(out);

      t_struct* xs = tfunction->get_xceptions();
      const vector<t_field*>& xfields = xs->get_members();
      vector<t_field*>::const_iterator x_iter;

      for (x_iter = xfields.begin(); x_iter != xfields.end(); ++x_iter) {
        indent(out) << "catch let error as " << (*x_iter)->get_type()->get_name();
        block_open(out);
        indent(out) << "result." << struct_property_name(*x_iter) << " = error" << endl;
        block_close(out);
      }

      indent(out) << "catch let error";
      block_open(out);
      out << indent() << "throw error" << endl;
      block_close(out);

      out << endl;

      if (!tfunction->is_oneway()) {
        out << indent() << "try outProtocol.writeMessageBeginWithName(\"" << tfunction->get_name() << "\", type: .REPLY, sequenceID: sequenceID)" << endl
            << indent() << "try " << result_type << ".writeValue(result, toProtocol: outProtocol)" << endl
            << indent() << "try outProtocol.writeMessageEnd()" << endl;
      }
    }
    block_close(out);

  }

  indent(out) << "return processorHandlers" << endl;

  block_close(out,false);
  out << "()" << endl;

  out << endl;

  indent(out) << "public func processOnInputProtocol(inProtocol: TProtocol, outputProtocol outProtocol: TProtocol) throws";
  block_open(out);

  out << endl;

  out << indent() << "let (messageName, _, sequenceID) = try inProtocol.readMessageBegin()" << endl
      << endl
      << indent() << "if let processorHandler = " << name << ".processorHandlers[messageName]";
  block_open(out);
  out << indent() << "do";
  block_open(out);
  out << indent() << "try processorHandler(sequenceID, inProtocol, outProtocol, service)" << endl;
  block_close(out);
  out << indent() << "catch let error as NSError";
  block_open(out);
  out << indent() << "try outProtocol.writeExceptionForMessageName(messageName, sequenceID: sequenceID, ex: error)" << endl;
  block_close(out);
  block_close(out);
  out << indent() << "else";
  block_open(out);
  out << indent() << "try inProtocol.skipType(.STRUCT)" << endl
      << indent() << "try inProtocol.readMessageEnd()" << endl
      << indent() << "try outProtocol.writeExceptionForMessageName(messageName," << endl;
  indent_up();
  out << indent() << "sequenceID: sequenceID," << endl
      << indent() << "ex: NSError(" << endl;
  indent_up();
  out << indent() << "domain: TApplicationErrorDomain, " << endl
      << indent() << "code: Int(TApplicationError.UnknownMethod.rawValue), " << endl
      << indent() << "userInfo: [TApplicationErrorMethodKey: messageName]))" << endl;
  indent_down();
  indent_down();
  block_close(out);

  block_close(out);

  block_close(out);
  out << endl;
}

/**
 * Returns an Swift name
 *
 * @param ttype The type
 * @param class_ref Do we want a Class reference istead of a type reference?
 * @return Swift type name, i.e. Dictionary<Key,Value>
 */
string t_swift_generator::type_name(t_type* ttype, bool is_optional, bool is_forced) {
  string result;
  if (ttype->is_base_type()) {
    result = base_type_name((t_base_type*)ttype);
  } else if (ttype->is_map()) {
    t_map *map = (t_map *)ttype;
    if (exclude_thrift_types_) {
      result = "[" + type_name(map->get_key_type()) + ": " + type_name(map->get_val_type()) + "]";
    }
    else {
      result = "TMap<" + type_name(map->get_key_type()) + ", " + type_name(map->get_val_type()) + ">";
    }
  } else if (ttype->is_set()) {
    t_set *set = (t_set *)ttype;
    if (exclude_thrift_types_) {
      result = "Set<" + type_name(set->get_elem_type()) + ">";
    }
    else {
      result = "TSet<" + type_name(set->get_elem_type()) + ">";
    }
  } else if (ttype->is_list()) {
    t_list *list = (t_list *)ttype;
    result = "TList<" + type_name(list->get_elem_type()) + ">";
  }
  else {
    result = ttype->get_name();
  }

  if (is_optional) {
    result += "?";
  }

  if (is_forced) {
    result += "!";
  }

  return result;
}

/**
 * Returns the Swift type that corresponds to the thrift type.
 *
 * @param tbase The base type
 */
string t_swift_generator::base_type_name(t_base_type* type) {
  t_base_type::t_base tbase = type->get_base();

  switch (tbase) {
  case t_base_type::TYPE_VOID:
    return "Void";
  case t_base_type::TYPE_STRING:
    if (type->is_binary()) {
      return "TBinary";
    } else {
      return "String";
    }
  case t_base_type::TYPE_BOOL:
    return "Bool";
  case t_base_type::TYPE_I8:
    return "Int8";
  case t_base_type::TYPE_I16:
    return "Int16";
  case t_base_type::TYPE_I32:
    return "Int32";
  case t_base_type::TYPE_I64:
    return "Int64";
  case t_base_type::TYPE_DOUBLE:
    return "Double";
  default:
    throw "compiler error: no Swift name for base type " + t_base_type::t_base_name(tbase);
  }
}

/**
 * Renders full constant value (as would be seen after an '=')
 *
 */
void t_swift_generator::render_const_value(ostream& out,
                                             t_type* type,
                                             t_const_value* value) {
  type = get_true_type(type);

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_STRING:
      out << "\"" << get_escaped_string(value) << "\"";
      break;
    case t_base_type::TYPE_BOOL:
      out << ((value->get_integer() > 0) ? "true" : "false");
      break;
    case t_base_type::TYPE_I8:
    case t_base_type::TYPE_I16:
    case t_base_type::TYPE_I32:
    case t_base_type::TYPE_I64:
      out << type_name(type) << "(" << value->get_integer() << ")";
      break;
    case t_base_type::TYPE_DOUBLE:
      out << type_name(type) << "(";
      if (value->get_type() == t_const_value::CV_INTEGER) {
        out << value->get_integer();
      } else {
        out << value->get_double();
      }
      out << ")";
      break;
    default:
      throw "compiler error: no const of base type " + t_base_type::t_base_name(tbase);
    }
  } else if (type->is_enum()) {
    out << type->get_name() << "." << camel_case_from_underscore(value->get_identifier_name());
  } else if (type->is_struct() || type->is_xception()) {
    out << type_name(type) << "(";

    const vector<t_field*>& fields = ((t_struct*)type)->get_members();
    vector<t_field*>::const_iterator f_iter;

    const map<t_const_value*, t_const_value*>& val = value->get_map();
    map<t_const_value*, t_const_value*>::const_iterator v_iter;

    for (f_iter = fields.begin(); f_iter != fields.end();) {
      t_field* tfield = *f_iter;
      t_const_value* value = NULL;
      for (v_iter = val.begin(); v_iter != val.end(); ++v_iter) {
        if (struct_property_name(tfield) == v_iter->first->get_string()) {
          value = v_iter->second;
        }
      }

      if (value) {
        out << tfield->get_name() << ": ";
        render_const_value(out, tfield->get_type(), value);
      }
      else if (!field_is_optional(tfield)) {
        throw "constant error: required field " + type->get_name() + "." + tfield->get_name() + " has no value";
      }

      if (++f_iter != fields.end()) {
        out << ", ";
      }
    }

    out << ")";

  } else if (type->is_map()) {

    out << "[";

    t_type* ktype = ((t_map*)type)->get_key_type();
    t_type* vtype = ((t_map*)type)->get_val_type();

    const map<t_const_value*, t_const_value*>& val = value->get_map();
    map<t_const_value*, t_const_value*>::const_iterator v_iter;

    for (v_iter = val.begin(); v_iter != val.end();) {

      render_const_value(out, ktype, v_iter->first);
      out << ": ";
      render_const_value(out, vtype, v_iter->second);

      if (++v_iter != val.end()) {
        out << ", ";
      }
    }

    out << "]";
  } else if (type->is_set()) {
    t_type* etype = ((t_set*)type)->get_elem_type();

    const vector<t_const_value*>& val = value->get_list();
    vector<t_const_value*>::const_iterator v_iter;

    if (val.empty()) {
      out << "Set<" + type_name(etype) + ">()";
    }
    else {
      out << "Set([";
      for (v_iter = val.begin(); v_iter != val.end();) {

        render_const_value(out, etype, *v_iter);

        if (++v_iter != val.end()) {
          out << ", ";
        }
      }

      out << "])";
    }
  } else {
    throw "compiler error: no const of type " + type->get_name();
  }

}

/**
 * If the provided documentable object has documentation attached, this
 * will emit it to the output stream.
 */
void t_swift_generator::print_doc(ostream& out, t_doc* tdoc, bool should_indent) {
  if (!tdoc->has_doc()) {
    return;
  }

  vector<string> strs;
  boost::split(strs, tdoc->get_doc(), boost::is_any_of("\n"));

  if (should_indent) {
    out << indent();
  }
  out << "/**" << endl;

  for (size_t i = 0; i < strs.size(); i++) {
    if (strs[i].length() == 0 || strs[i] == "\n") {
      continue;
    }

    if (should_indent) {
      out << indent();
    }
    out << " " << strs[i] << endl;
  }

  if (should_indent) {
    out << indent();
  }
  out << " */" << endl;
}

/**
 * If the provided struct has documentation attached, this
 * will emit it and its parameters to the output stream.
 */
void t_swift_generator::print_struct_init_doc(ostream& out, t_struct* tstruct, const vector<t_field*>& fields, bool all) {
  bool any_field_has_doc = false;
  vector<t_field*>::const_iterator f_iter;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if ((*f_iter)->has_doc()) {
      any_field_has_doc = true;
      break;
    }
  }

  if (!tstruct->has_doc() && !any_field_has_doc) {
    return;
  }

  vector<string> struct_docs;
  boost::split(struct_docs, tstruct->get_doc(), boost::is_any_of("\n"));

  out << indent() << "/**" << endl;

  for (size_t i = 0; i < struct_docs.size(); i++) {
    if (struct_docs[i].length() == 0 || struct_docs[i] == "\n") {
      continue;
    }

    out << indent() << " " << struct_docs[i] << endl;
  }

  if (any_field_has_doc) {
    if (tstruct->has_doc()) {
      out << endl;
    }
    out << indent() << " - Parameters:" << endl;

    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      t_field* arg = *f_iter;

      if ((all || !field_is_optional(arg)) && arg->has_doc()) {
        out << indent() << indent() << " - " << struct_property_name(arg) << ":" << endl;

        vector<string> arg_docs;
        boost::split(arg_docs, arg->get_doc(), boost::is_any_of("\n"));

        for (size_t i = 0; i < arg_docs.size(); i++) {
          if (arg_docs[i].length() == 0 || arg_docs[i] == "\n") {
            continue;
          }

          out << indent() << indent() << indent() << indent() << " " << arg_docs[i] << endl;
        }
      }
    }
  }

  out << indent() << " */" << endl;
}

/**
 * Declares an Swift property.
 *
 * @param tfield The field to declare a property for
 */
string t_swift_generator::declare_property(t_field* tfield, bool is_private) {
  ostringstream render;

  if (boost::algorithm::ends_with(type_name(tfield->get_type()), "TeleMetadataOnly")) {
    return render.str();
  }

  print_doc(render, tfield, true);

  string visibility = is_private ? "private" : "public";

  render << indent() << visibility << " var " << struct_property_name(tfield);

  bool is_optional = field_is_optional(tfield);

  if (tfield->get_value() != NULL) {
    t_type* type = tfield->get_type();
    if (is_optional) {
      render << " : " << type_name(type, is_optional);
    }
    render << " = ";
    render_const_value(render, type, tfield->get_value());
  }
  else {
    if (is_optional || exclude_empty_init_) {
      render << " : " << type_name(tfield->get_type(), is_optional);
    }
    else {
      render << " = " << type_name(tfield->get_type(), false) << "()";
    }
  }

  return render.str();
}

/**
 * Renders a function signature
 *
 * @param tfunction Function definition
 * @return String of rendered function definition
 */
string t_swift_generator::function_signature(t_function* tfunction) {

  string result = "func " + function_name(tfunction);

  result += "(" + argument_list(tfunction->get_arglist(), "", false) + ") throws";

  t_type* ttype = tfunction->get_returntype();
  if (!ttype->is_void()) {
    result += " -> " + type_name(ttype);
  }

  return result;
}

/**
 * Renders a function signature that returns asynchronously via blocks.
 *
 * @param tfunction Function definition
 * @return String of rendered function definition
 */
string t_swift_generator::async_function_signature(t_function* tfunction) {
  t_type* ttype = tfunction->get_returntype();
  t_struct* targlist = tfunction->get_arglist();
  string response_param = "(" + ((ttype->is_void()) ? "" : type_name(ttype)) + ") -> Void";
  string result = "func " + function_name(tfunction);
  result += "(" + argument_list(tfunction->get_arglist(), "", false)
          + (targlist->get_members().size() ? ", " : "")
          + "success: " + response_param + ", "
          + "failure: (NSError) -> Void) throws";
  return result;
}

/**
 * Renders a function signature that returns asynchronously via promises.
 *
 * @param tfunction Function definition
 * @return String of rendered function definition
 */
string t_swift_generator::promise_function_signature(t_function* tfunction) {
  return "func " + function_name(tfunction) + "(" + argument_list(tfunction->get_arglist(), "", false) + ") throws "
          + "-> Promise<" + type_name(tfunction->get_returntype()) + ">";
}

/**
 * Renders a verbose function name suitable for a Swift method
 */
string t_swift_generator::function_name(t_function* tfunction) {
  string name = tfunction->get_name();
  if (!tfunction->get_arglist()->get_members().empty()) {
    string first_arg = tfunction->get_arglist()->get_members().front()->get_name();
    if (name.size() < first_arg.size() ||
        lowercase(name.substr(name.size()-first_arg.size())) != lowercase(first_arg)) {
      name += "With" + capitalize(tfunction->get_arglist()->get_members()[0]->get_name());
    }
  }
  return name;
}

/**
 * Renders a Swift method argument list
 */
string t_swift_generator::argument_list(t_struct* tstruct, string protocol_name, bool is_internal) {
  string result = "";
  bool include_protocol = !protocol_name.empty();

  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;

  if (include_protocol && !exclude_thrift_types_) {
    result += protocol_name + ": TProtocol";
    if (!fields.empty()) {
      result += ", ";
    }
  }
  else if (!fields.empty() && is_internal) {
    // Force first argument to be named
    result += fields.front()->get_name() + " ";
  }

  for (f_iter = fields.begin(); f_iter != fields.end();) {
    t_field* arg = *f_iter;
    result += struct_property_name(arg) + ": " + type_name(arg->get_type());

    if (++f_iter != fields.end()) {
      result += ", ";
    }
  }
  return result;
}

/**
 * https://developer.apple.com/library/ios/documentation/Swift/Conceptual/Swift_Programming_Language/LexicalStructure.html
 *
 */

void t_swift_generator::populate_reserved_words() {
  swift_reserved_words_.insert("Self");
  swift_reserved_words_.insert("associatedtype");
  swift_reserved_words_.insert("defer");
  swift_reserved_words_.insert("deinit");
  swift_reserved_words_.insert("dynamicType");
  swift_reserved_words_.insert("enum");
  swift_reserved_words_.insert("extension");
  swift_reserved_words_.insert("fallthrough");
  swift_reserved_words_.insert("false");
  swift_reserved_words_.insert("func");
  swift_reserved_words_.insert("guard");
  swift_reserved_words_.insert("init");
  swift_reserved_words_.insert("inout");
  swift_reserved_words_.insert("internal");
  swift_reserved_words_.insert("let");
  swift_reserved_words_.insert("operator");
  swift_reserved_words_.insert("protocol");
  swift_reserved_words_.insert("repeat");
  swift_reserved_words_.insert("rethrows");
  swift_reserved_words_.insert("struct");
  swift_reserved_words_.insert("subscript");
  swift_reserved_words_.insert("throws");
  swift_reserved_words_.insert("true");
  swift_reserved_words_.insert("typealias");
  swift_reserved_words_.insert("where");
}

string t_swift_generator::struct_property_name(t_field* tfield) {
  return maybe_escape_identifier(camel_case_from_underscore(tfield->get_name()));
}

string t_swift_generator::enum_value_name(t_enum_value* tenumvalue) {
  return maybe_escape_identifier(camel_case_from_underscore(tenumvalue->get_name()));
}

string t_swift_generator::camel_case_from_underscore(string underscore_name) {
  // Convert to camel case
  std::string cap_value_name = underscore_name;
  cap_value_name[0] = tolower(underscore_name[0]);

  // Underscores separate words
  bool cap_next = false;
  for (string::iterator iter = cap_value_name.begin(); iter < cap_value_name.end(); iter++) {
    if (cap_next) {
      *iter = toupper(*iter);
      cap_next = false;
    }
    else if (*iter == '_') {
      cap_next = true;
    }
  }

  boost::replace_all(cap_value_name, "_", "");

  return cap_value_name;
}

string t_swift_generator::maybe_escape_identifier(const string& identifier) {
  if (swift_reserved_words_.find(identifier) != swift_reserved_words_.end()) {
    return "`" + identifier + "`";
  }
  return identifier;
}

/**
 * Converts the parse type to a Swift TType enumeration.
 */
string t_swift_generator::type_to_enum(t_type* type, bool qualified) {
  type = get_true_type(type);

  string result = qualified ? "TType." : ".";

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "NO T_VOID CONSTRUCT";
    case t_base_type::TYPE_STRING:
      return result + "STRING";
    case t_base_type::TYPE_BOOL:
      return result + "BOOL";
    case t_base_type::TYPE_I8:
      return result + "BYTE";
    case t_base_type::TYPE_I16:
      return result + "I16";
    case t_base_type::TYPE_I32:
      return result + "I32";
    case t_base_type::TYPE_I64:
      return result + "I64";
    case t_base_type::TYPE_DOUBLE:
      return result + "DOUBLE";
    }
  } else if (type->is_enum()) {
    return result + "I32";
  } else if (type->is_struct() || type->is_xception()) {
    return result + "STRUCT";
  } else if (type->is_map()) {
    return result + "MAP";
  } else if (type->is_set()) {
    return result + "SET";
  } else if (type->is_list()) {
    return result + "LIST";
  }

  throw "INVALID TYPE IN type_to_enum: " + type->get_name();
}

/**
 * Creates a file in the output directory with the given name.
 */
void t_swift_generator::create_file(ofstream& out, string file_name) {
  string file_stream_fullname = get_out_dir() + file_name + ".swift";
  out.open(file_stream_fullname.c_str());

  out << autogen_comment() << endl;

  out << swift_imports() << swift_thrift_imports() << endl;
}


THRIFT_REGISTER_GENERATOR(
    swift,
    "Swift",
    "    log_unexpected:  Log every time an unexpected field ID or type is encountered.\n"
    "    debug_descriptions:\n"
    "                     Allow use of debugDescription so the app can add description via a cateogory/extension\n"
    "    async_clients:   Generate clients which invoke asynchronously via block syntax.\n"
    "    promise_kit:     Generate clients which invoke asynchronously via promises.\n"
    "    exclude_thrift_types:\n"
    "                     Do not use Thrift types in the generated code\n"
    "    telemetry_object:\n"
    "                     Create protocols in order to send a dictionary of values for telemetry.\n"
    "    exclude_empty_init:\n"
    "                     Do not generate empty initializers\n"
    "    exclude_equatable:\n"
    "                     Do not generate Equatable and Hashable implementations\n"
    "    exclude_printable:\n"
    "                     Do not generate CustomStringConvertible implementation\n"
    "    separate_files:  Create a separate file for each type\n"
    "    struct:          Create structs instead of classes\n")
