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