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
 *   Main Developer : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef CORE_CONSTANTS_TOKENS_SETTINGS_H
#define CORE_CONSTANTS_TOKENS_SETTINGS_H

/**
 * \file constants_tokensandsettings.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.4.0
 * \date 13 June 2010
*/


namespace Core {

namespace Constants {

// TEXT COMPLETION tokens

// For Documents
const char * const  TOKEN_DOCUMENTTITLE     = "DOCUMENTTITLE";


// For Patients
const char * const  TOKEN_PATIENTAGE        = "PATIENTAGE";
const char * const  TOKEN_PATIENTYEARSOLD   = "PATIENTYEARSOLD";
const char * const  TOKEN_PATIENTFULLNAME   = "PATIENTFULLNAME";
const char * const  TOKEN_PATIENTNAME       = "PATIENTNAME";
const char * const  TOKEN_PATIENTSECONDNAME = "PATIENTSECONDNAME";
const char * const  TOKEN_PATIENTFIRSTNAME  = "PATIENTFIRSTNAME";
const char * const  TOKEN_PATIENTTITLE      = "PATIENTTITLE";
const char * const  TOKEN_PATIENTDATEOFBIRTH= "PATIENTDATEOFBIRTH";
const char * const  TOKEN_WEIGHT            = "WEIGHT";
const char * const  TOKEN_WEIGHT_UNIT       = "WEIGHT_UNIT";
const char * const  TOKEN_HEIGHT            = "HEIGHT";
const char * const  TOKEN_HEIGHT_UNIT       = "HEIGHT_UNIT";
const char * const  TOKEN_CLCR              = "CLCR";
const char * const  TOKEN_CLCR_UNIT         = "CLCR_UNIT";
const char * const  TOKEN_DATE              = "DATE";
const char * const  TOKEN_NUMBEROFDRUGS     = "NBDRUGS";

// For Users
const char * const  TOKEN_USERAGE            = "USERAGE";
const char * const  TOKEN_USERYEARSOLD       = "USERYEARSOLD";
const char * const  TOKEN_USERFULLNAME       = "USERFULLNAME";
const char * const  TOKEN_USERTITLE          = "USERTITLE";
const char * const  TOKEN_USERNAME           = "USERNAME";
const char * const  TOKEN_USERSECONDNAME     = "USERSECONDNAME";
const char * const  TOKEN_USERFIRSTNAME      = "USERFIRSTNAME";
const char * const  TOKEN_USERSPECIALITIES   = "USERSPECIALITIES";
const char * const  TOKEN_USERIDENTIFIANTS   = "USERIDENTIFIANTS";
const char * const  TOKEN_USERQUALIFICATIONS = "USERQUALIFICATIONS";
const char * const  TOKEN_USERADRESS         = "USERADRESS";
const char * const  TOKEN_USERMAIL           = "USERMAIL";
const char * const  TOKEN_USERTEL1           = "USERTEL1";
const char * const  TOKEN_USERTEL2           = "USERTEL2";
const char * const  TOKEN_USERTEL3           = "USERTEL3";
const char * const  TOKEN_USERFAX            = "USERFAX";

const char * const  TOKEN_USERFULLADRESS     = "USERFULLADRESS";
const char * const  TOKEN_USERFULLCONTACT    = "USERFULLCONTACT";

// Some path tags
const char * const  TAG_APPLICATION_RESOURCES_PATH = "__appResources__";
const char * const  TAG_APPLICATION_THEME_PATH     = "__theme__";
const char * const  TAG_APPLICATION_COMPLETEFORMS_PATH = "__completeForms__";
const char * const  TAG_APPLICATION_SUBFORMS_PATH = "__subForms__";
const char * const  TAG_APPLICATION_USER_COMPLETEFORMS_PATH = "__userCompleteForms__";
const char * const  TAG_APPLICATION_USER_SUBFORMS_PATH = "__userSubForms__";
const char * const  TAG_DATAPACK_COMPLETEFORMS_PATH = "__datapackCompleteForms__";
const char * const  TAG_DATAPACK_SUBFORMS_PATH = "__datapackSubForms__";
const char * const  TAG_USER_DOCUMENT_PATH = "__userDocuments__";

// Settings keys
const char * const  S_RECENTFILES_GROUP = "RecentFiles";
const char * const  S_RECENTFILES_KEY   = "File";
const char * const  S_PATIENT_UUID_HISTORY = "PatientsUuids";
const char * const  S_SAVEINTERVAL      = "Core/SaveInterval";
const char * const  S_OPENLAST          = "Core/OpenLastOpenedFile";
const char * const  S_SHOWHELPTEXT      = "Core/ShowFormHelpText";
const char * const  S_LASTLOGIN         = "Login/userLogin";
const char * const  S_LASTPASSWORD      = "Login/userPassword";
const char * const  S_DATEFORMAT        = "Dates/Format";
const char * const  S_PREFEREDLANGUAGE  = "Core/PreferedLanguage";

const char * const  S_DEF_PATIENTFORMS_FILENAME  = "gp_basic1";

const char * const  S_USE_EXTERNAL_DATABASE  = "ExternalDatabase/UseIt";
const char * const  S_EXTERNAL_DATABASE_LOG  = "ExternalDatabase/Log";
const char * const  S_EXTERNAL_DATABASE_PASS = "ExternalDatabase/Pass";
const char * const  S_EXTERNAL_DATABASE_HOST = "ExternalDatabase/Host";
const char * const  S_EXTERNAL_DATABASE_PORT = "ExternalDatabase/Port";

const char * const  S_ALWAYS_SAVE_WITHOUT_PROMPTING = "Core/SaveInDatabaseWithoutPrompringUser";
const char * const  S_ALLOW_VIRTUAL_DATA = "Core/AllowAndUseVirtuals";

const char * const  S_PROXY = "Core/Proxy";

}  // end Constants
} // end Core

#endif // CORE_CONSTANTS_TOKENS_SETTINGS_H
