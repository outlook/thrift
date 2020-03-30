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
#include <iostream>
#include <regex>
#include "thrift/parse/t_base_type.h"
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

  if (!validate_struct_member_names()) {
    contains_failure = true;
  }

  if (!validate_struct_member_values()) {
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
  vector<string> enum_exceptions = {
    "OTPrivacyLevel",
    "OTDiagnosticConsentLevelAsInt",
    "OTPrivacyServiceStateAsInt",
    "OTPrivacySettingSourceLocationAsInt",
    "OTPrivacyIdentitySpace",
    "OTCIDType",
    "OTAccountType",
    "OTAccountCloud",
    "OTFolderType",
    "OTPrivacyAccountType",
    "OTPrivacySettingsAction",
    "OTPrivacySettingsResult",
    "OTPrivacySettingType",
    "OTPrivacyRoamingService",
    "OTPrivacySettingsFailureReason",
  };

  vector<string> exceptions = {
    "underSubmit",
    "bottomOfScreen",
    "actionCard",
    "hideAd",
    "GROUPS",
    "OTHER",
    "GROUP",
    "OTHER",
    "fileDetails",
    "unifiedInboxHeader",
    "singleInboxHeader",
    "keyStroke",
    "NoResponse",
    "Organizer",
    "Tentative",
    "Accepted",
    "Declined",
    "ProposedNewTime",
    "Accept",
    "Reply",
    "Edit",
    "NoResponse",
    "Folder",
    "Group",
    "Persona",
    "Unknown",
    "missing_xAnchor_mailbox",
    "Mail",
    "Calendar",
    "clearButton",
    "contextChanged",
    "keyboardInput",
    "zeroQuery",
    "includeDeletedToggle",
    "voiceAssistant",
    "spellerSuggestion",
    "closedWithSearch",
    "closedWithoutSearch",
    "closedWithTimeout",
    "closedWithError",
    "deniedPermissions",
    "latencyDiffFirstWord",
    "startedListening",
    "microphoneShown",
    "initializingStateShown",
    "peoplePhone",
    "peopleEmail",
    "peopleOfficeLocation",
    "MFU",
    "MRU",
    "microsoftGraph",
    "lokiAccessToken",
    "substrateToken",
    "cortanaToken",
    "featureAwareness",
    "newAccount",
    "changeServerSettings",
    "timeoutError",
    "authenticationError",
    "unknownError",
    "qrIntroShown",
    "qrIntroScanQrCode",
    "qrIntroSignInManually",
    "qrScanShown",
    "qrScanSignInManually",
    "qrScanSuccess",
    "qrScanFail",
    "lensSDK_camera",
    "enterBackgroundEarly",
    "remoteNotification",
    "significantTimeChange",
    "backgroundFetch",
    "archiveAndMarkAsRead",
    "messageDetail",
    "messageListBulkAction",
    "messageListSwipe",
    "ComposeAttachment",
    "ComposeInline",
    "ComposeRecipient",
    "Attachment",
    "MessagesList",
    "MessageListInSearchResults",
    "FilePicker",
    "FilesInZeroQuery",
    "EventsAgendaView",
    "EventsDayView",
    "ContactsInZeroQuery",
    "PeopleList",
    "BE",
    "FE",
  };
  std::regex regex(R"(^[a-z0-9_]+$)");
  bool contains_failure = false;

  const vector<t_enum*>& enums = program_->get_enums();
  vector<t_enum*>::const_iterator e_iter;
  for (e_iter = enums.begin(); e_iter != enums.end(); ++e_iter) {
    t_enum* en = *e_iter;

    if (std::find(enum_exceptions.begin(), enum_exceptions.end(), en->get_name()) != enum_exceptions.end()) {
      continue;
    }

    vector<t_enum_value*> constants = en->get_constants();
    vector<t_enum_value*>::iterator c_iter;

    for (c_iter = constants.begin(); c_iter != constants.end(); ++c_iter) {

      string name = (*c_iter)->get_name();
      if (std::find(exceptions.begin(), exceptions.end(), name) != exceptions.end()) {
        continue;
      }

      if (!std::regex_match(name, regex)) {
        cout << "Failed regex for enum constant name: " << name << endl;
        contains_failure = true;
      }
    }
  }

  return !contains_failure;
}

bool t_linter::validate_struct_member_names() {
  vector<string> struct_exceptions = {
    "OTPrivacyTags",
    "OTPrivacyConsentNonAADProperties",
    "OTPrivacyConsentAADProperties",
    "OTPrivacyConsentEvent",
    "OTPrivacySettingsEvent",
    "OTBootTimeEvent",
  };

  vector<string> exceptions = {
    "DiagnosticPrivacyLevel",
    "byteCount",
    "reachabilityType",
    "unselectedMessageAction",
    "taskId",
    "otherInboxAdsData",
    "is_IRM_protected",
    "clientName",
    "cloudFile_response_data",
    "smimeCertType",
    "isHxAccount",
    "traceID",
    "logicalID",
    "AccountType",
    "num_accounts_in_DB",
    "errorDescription",
    "createAccount",
    "authType",
    "currentVC",
    "sqlError",
    "errorSource",
    "incidentIdentifier",
    "reporterKey",
    "exceptionName",
    "crashTime",
    "isAppKill",
    "memoAry_used_percentage",
    "systemFlagSet",
    "glEsVersion",
  };

  std::regex regex(R"(^[a-z0-9_]+$)");
  bool contains_failure = false;

  const vector<t_struct*>& structs = program_->get_structs();
  vector<t_struct*>::const_iterator s_iter;
  for (s_iter = structs.begin(); s_iter != structs.end(); ++s_iter) {
    t_struct* tstruct = *s_iter;

    if (std::find(struct_exceptions.begin(), struct_exceptions.end(), tstruct->get_name()) != struct_exceptions.end()) {
      continue;
    }

    vector<t_field*> members = tstruct->get_members();
    vector<t_field*>::iterator m_iter;

    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {

      string name = (*m_iter)->get_name();
      if (std::find(exceptions.begin(), exceptions.end(), name) != exceptions.end()) {
        continue;
      }

      if (!std::regex_match(name, regex)) {
        cout << "Failed regex for struct member name: " << name << endl;
        contains_failure = true;
      }
    }
  }

  return !contains_failure;
}

bool t_linter::validate_struct_member_values() {
  vector<string> struct_exceptions;

  vector<string> exceptions = {
    "watchAppV2",
    "OEM_INSTALL",
  };

  std::regex regex(R"(^[a-z0-9_]+$)");
  bool contains_failure = false;

  const vector<t_struct*>& structs = program_->get_structs();
  vector<t_struct*>::const_iterator s_iter;
  for (s_iter = structs.begin(); s_iter != structs.end(); ++s_iter) {
    t_struct* tstruct = *s_iter;

    if (std::find(struct_exceptions.begin(), struct_exceptions.end(), tstruct->get_name()) != struct_exceptions.end()) {
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
          if (std::find(exceptions.begin(), exceptions.end(), value) != exceptions.end()) {
            break;
          }

          if (!std::regex_match(value, regex)) {
            cout << "Failed regex for struct member value: " << value << endl;
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