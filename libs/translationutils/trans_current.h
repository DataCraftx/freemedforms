/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef CONSTANTTRANSLATIONS_CURRENT_H
#define CONSTANTTRANSLATIONS_CURRENT_H

#include <QCoreApplication>

namespace Trans {
namespace Constants {

// Widely used words
const char* const THE_FREEMEDFORMS_COMMUNITY         = QT_TRANSLATE_NOOP("tkConstants", "The FreeMedForms community");
const char* const CONTACT_DEV_TEAM                   = QT_TRANSLATE_NOOP("tkConstants", "Please contact the development team.");
const char* const TYPE                               = QT_TRANSLATE_NOOP("tkConstants", "Type");
const char* const STATUS                             = QT_TRANSLATE_NOOP("tkConstants", "Status");
const char* const ICD10                              = QT_TRANSLATE_NOOP("tkConstants", "ICD10");
const char* const NOW                                = QT_TRANSLATE_NOOP("tkConstants", "now");
const char* const LOW                                = QT_TRANSLATE_NOOP("tkConstants", "low");
const char* const MEDIUM                             = QT_TRANSLATE_NOOP("tkConstants", "medium");
const char* const HIGH                               = QT_TRANSLATE_NOOP("tkConstants", "high");
const char* const MODIFY                             = QT_TRANSLATE_NOOP("tkConstants", "modify");
const char* const SELECTOR                           = QT_TRANSLATE_NOOP("tkConstants", "Selector");
const char* const FORMS                              = QT_TRANSLATE_NOOP("tkConstants", "Forms");
const char* const SUBFORMS                           = QT_TRANSLATE_NOOP("tkConstants", "Subforms");
const char* const VIEWER                             = QT_TRANSLATE_NOOP("tkConstants", "Viewer");
const char* const SERVER                             = QT_TRANSLATE_NOOP("tkConstants", "Server");
const char* const PACKAGE                            = QT_TRANSLATE_NOOP("tkConstants", "Package");
const char* const SERVERS                            = QT_TRANSLATE_NOOP("tkConstants", "Servers");
const char* const PACKAGES                           = QT_TRANSLATE_NOOP("tkConstants", "Packages");
const char* const SERVER_S                           = QT_TRANSLATE_NOOP("tkConstants", "Server(s)");
const char* const PACKAGE_S                          = QT_TRANSLATE_NOOP("tkConstants", "Package(s)");
const char* const UNKNOWN_SERVER                     = QT_TRANSLATE_NOOP("tkConstants", "Unknown server");
const char* const INSTALLED                          = QT_TRANSLATE_NOOP("tkConstants", "Installed");
const char* const CURRENTLY_INSTALLED                = QT_TRANSLATE_NOOP("tkConstants", "Currently installed");
const char* const UNDEFINED                          = QT_TRANSLATE_NOOP("tkConstants", "Undefined");
const char* const LICENSE                            = QT_TRANSLATE_NOOP("tkConstants", "License");
const char* const LABEL                              = QT_TRANSLATE_NOOP("tkConstants", "Label");
const char* const UNIQUE_IDENTIFIER                  = QT_TRANSLATE_NOOP("tkConstants", "Unique identifier");
const char* const URL                                = QT_TRANSLATE_NOOP("tkConstants", "URL");

const char* const TOKEN                              = QT_TRANSLATE_NOOP("tkConstants", "Token");
const char* const TOKEN_1                            = QT_TRANSLATE_NOOP("tkConstants", "Token: %1");
const char* const EDIT_TOKEN                         = QT_TRANSLATE_NOOP("tkConstants", "Edit token");
const char* const VALIDATE                           = QT_TRANSLATE_NOOP("tkConstants", "Validate");
const char* const OVERRIDE                           = QT_TRANSLATE_NOOP("tkConstants", "Override");
const char* const ADD_ALERT                          = QT_TRANSLATE_NOOP("tkConstants", "Add alert");
const char* const EDIT_ALERT                         = QT_TRANSLATE_NOOP("tkConstants", "Edit alert");
const char* const REMIND_LATER                       = QT_TRANSLATE_NOOP("tkConstants", "Remind later");
const char* const _1_ALERTS                           = QT_TRANSLATE_NOOP("tkConstants", "%1 alerts");

const char* const FROM                          = QT_TRANSLATE_NOOP("tkConstants", "from");
const char* const TO                            = QT_TRANSLATE_NOOP("tkConstants", "to");
const char* const FROM_1_TO_2                   = QT_TRANSLATE_NOOP("tkConstants", "from %1 to %2");
const char* const DURING                        = QT_TRANSLATE_NOOP("tkConstants", "during");
const char* const EACH                          = QT_TRANSLATE_NOOP("tkConstants", "each");
const char* const AND                           = QT_TRANSLATE_NOOP("tkConstants", "and");
const char* const ALERT                         = QT_TRANSLATE_NOOP("tkConstants", "Alert");
const char* const ALERTS                        = QT_TRANSLATE_NOOP("tkConstants", "Alerts");
const char* const DYNAMIC_ALERT                 = QT_TRANSLATE_NOOP("tkConstants", "Dynamic alert");
const char* const BLOCKING_ALERT                = QT_TRANSLATE_NOOP("tkConstants", "Blocking alert");
const char* const NON_BLOCKING_ALERT                = QT_TRANSLATE_NOOP("tkConstants", "Non-blocking alert");

// The "related to"
const char* const RELATED_TO                    = QT_TRANSLATE_NOOP("tkConstants", "related to");
const char* const RELATED_TO_PATIENT_1          = QT_TRANSLATE_NOOP("tkConstants", "related to patient %1");
const char* const RELATED_TO_CURRENT_PATIENT    = QT_TRANSLATE_NOOP("tkConstants", "related to current patient");
const char* const RELATED_TO_ALL_PATIENTS       = QT_TRANSLATE_NOOP("tkConstants", "related to all patients");
const char* const RELATED_TO_USER_1             = QT_TRANSLATE_NOOP("tkConstants", "related to user %1");
const char* const RELATED_TO_CURRENT_USER       = QT_TRANSLATE_NOOP("tkConstants", "related to current user");
const char* const RELATED_TO_ALL_USERS          = QT_TRANSLATE_NOOP("tkConstants", "related to all users");
const char* const RELATED_TO_APPLICATION        = QT_TRANSLATE_NOOP("tkConstants", "related to application");
const char* const RELATED_TO_PATIENT_FAMILY_1   = QT_TRANSLATE_NOOP("tkConstants", "related to the family %1");
const char* const RELATED_TO_USER_GROUP_1       = QT_TRANSLATE_NOOP("tkConstants", "related to user group %1");

// Settings related messages
const char* const SETTINGS_RECOVERED                = QT_TRANSLATE_NOOP("tkConstants", "Settings recovered" );
const char* const LOADING_SETTINGS                  = QT_TRANSLATE_NOOP("tkConstants", "Reading settings file");
const char* const SAVING_SETTINGS                   = QT_TRANSLATE_NOOP("tkConstants", "Settings recovered" );
const char* const CREATING_DEFAULT_SETTINGS_FOR_1   = QT_TRANSLATE_NOOP("tkConstants", "Creating default settings for %1");
const char* const SHOW_DETAILS                      = QT_TRANSLATE_NOOP("tkConstants", "Show details");
const char* const MORE_INFORMATION                  = QT_TRANSLATE_NOOP("tkConstants", "More information");
const char* const HIDE_EXTRA_INFORMATION            = QT_TRANSLATE_NOOP("tkConstants", "Hide extra information");

// Print related
const char* const HEADER                            = QT_TRANSLATE_NOOP("tkConstants", "Header");
const char* const FOOTER                            = QT_TRANSLATE_NOOP("tkConstants", "Footer");
const char* const WATERMARK                         = QT_TRANSLATE_NOOP("tkConstants", "Watermark");
const char* const PRINTING                          = QT_TRANSLATE_NOOP("tkConstants", "Printing");

// DataPack related
const char* const DATAPACK                          = QT_TRANSLATE_NOOP("tkConstants", "Data pack");
const char* const DATAPACK_MANAGER                  = QT_TRANSLATE_NOOP("tkConstants", "Data pack manager");
const char* const LAST_MODIFICATION                 = QT_TRANSLATE_NOOP("tkConstants", "Last modification");
const char* const DATA_TYPE                         = QT_TRANSLATE_NOOP("tkConstants", "Data type");

// DataPack types
const char* const ZIP_CODES                         = QT_TRANSLATE_NOOP("tkConstants", "Zip codes");
const char* const USER_DOCUMENTS                    = QT_TRANSLATE_NOOP("tkConstants", "User documents");
const char* const FORMS_FULL_SET                    = QT_TRANSLATE_NOOP("tkConstants", "Full set of forms");
const char* const FORMS_SUBFORMS                    = QT_TRANSLATE_NOOP("tkConstants", "Subforms");
const char* const DISEASES_ICD                      = QT_TRANSLATE_NOOP("tkConstants", "International classification of diseases");
const char* const DISEASES_CLASSIFICATIONS          = QT_TRANSLATE_NOOP("tkConstants", "Diseases classifications");
const char* const VARIOUS                           = QT_TRANSLATE_NOOP("tkConstants", "Various");

} // end Constants
} // end TranslationUtils

#endif // CONSTANTTRANSLATIONS_CURRENT_H
