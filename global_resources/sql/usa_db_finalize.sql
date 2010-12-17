-- /***************************************************************************
-- *   FreeMedicalForms                                                      *
-- *   (C) 2008-2010 by Eric MAEKER, MD                                      *
-- *   eric.maeker@free.fr                                                   *
-- *   All rights reserved.                                                  *
-- *                                                                         *
-- *   This program is a free and open source software.                      *
-- *   It is released under the terms of the new BSD License.                *
-- *                                                                         *
-- *   Redistribution and use in source and binary forms, with or without    *
-- *   modification, are permitted provided that the following conditions    *
-- *   are met:                                                              *
-- *   - Redistributions of source code must retain the above copyright      *
-- *   notice, this list of conditions and the following disclaimer.         *
-- *   - Redistributions in binary form must reproduce the above copyright   *
-- *   notice, this list of conditions and the following disclaimer in the   *
-- *   documentation and/or other materials provided with the distribution.  *
-- *   - Neither the name of the FreeMedForms' organization nor the names of *
-- *   its contributors may be used to endorse or promote products derived   *
-- *   from this software without specific prior written permission.         *
-- *                                                                         *
-- *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
-- *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
-- *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
-- *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
-- *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
-- *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
-- *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
-- *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
-- *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
-- *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
-- *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
-- *   POSSIBILITY OF SUCH DAMAGE.                                           *
-- ***************************************************************************/
-- /**************************************************************************
-- *   Main Developper : Jim Busser, MD <jbusser@interchange.ubc.ca>         *
-- *   Contributors :                                                        *
-- *       Eric MAEKER, MD <eric.maeker@free.fr>                             *
-- ***************************************************************************/

-- /**
--  * \file usa_db_finalize.sql
--  * \author Eric MAEKER, MD <eric.maeker@free.fr>
--  * \version 0.4.4
--  * \date 18 aug 2010
--  */


-- Insert Databases informations;
INSERT INTO `INFORMATIONS` (
    `VERSION`,
    `NAME`,
    `IDENTIFIANT` ,
    `COMPAT_VERSION`,
    `PROVIDER`,
    `WEBLINK`,
    `COMPLEMENTARY_WEBSITE`,
    `AUTHOR`,
    `LICENSE`,
    `LICENSE_TERMS`,
    `DATE`,
    `DRUG_UID_NAME`,
    `PACK_MAIN_CODE_NAME`,
    `ATC`,
    `INTERACTIONS`,
    `AUTHOR_COMMENTS`,
    `LANGUAGE_COUNTRY`,
    `DRUGS_NAME_CONSTRUCTOR`
    ) VALUES (
    "0.5.2",
    "fr=Base de données thérapeutique Américaine
xx=FDA's Drug Product Database
",
    "FDA_US",
    "0.5.2",
    "FDA",
    "http://www.fda.gov/Drugs/",
    "",
    "Eric Maeker, MD",
    "FDA",
    "",
    CURRENT_DATE,
    "DRUG_CODE",
    "PACKAGE",
    1,
    1,
    "",
    "en_US",
    "NAME (FORM, GLOBAL_STRENGTH)"
);

vacuum;
