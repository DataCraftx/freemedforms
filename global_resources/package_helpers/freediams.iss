; Inno Setup compiler file for
; FreeDiams
; (C) Eric Maeker, MD, GPLv3, FreeMedForms project

[Setup]
AppName=FreeDiams
AppId=FreeDiams
AppVerName=FreeDiams,__version__
DefaultDirName={pf}\FreeDiams
DefaultGroupName=FreeDiams
UninstallDisplayIcon={app}\freediams.exe
VersionInfoDescription=FreeDiams __version__
VersionInfoProductName=FreeDiams
OutputDir=FreeDiams
;ShowLanguageDialog=no
;ShowUndisplayableLanguages=yes
AppCopyright=Copyright (C) 2008-2012 Eric Maeker, MD, {cm:FreeMedFormsProject}
AppPublisher={cm:FreeMedFormsProject}
AppPublisherURL=http://www.freemedforms.com/
ChangesAssociations=yes
;LicenseFile=
;OutputDir=SourceDir
;WizardSmallImageFile=mysmallimage.bmp
SourceDir=e:\freediams-__version__\packages\win\freediams
SetupIconFile=e:\freediams-__version__\freediams\freediams.ico
WizardSmallImageFile=e:\freediams-__version__\global_resources\pixmap\svg\freediams-128.bmp
WizardImageFile=e:\freediams-__version__\global_resources\pixmap\svg\freediams-128.bmp
;WizardImageStretch=no
Compression=lzma/normal

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: fr; MessagesFile: "compiler:Languages\French.isl"
Name: de; MessagesFile: "compiler:Languages\German.isl"

[Messages]
en.BeveledLabel=English
fr.BeveledLabel=Fran�ais
de.BeveledLabel=Deutsch

[CustomMessages]
en.FreeDiamsSetup=Free pharmaceutical prescriber and drugs interactions checker.
fr.FreeDiamsSetup=Assistant de prescription m�dicamenteuse et de recherche d'interactions m�dicamenteuse.
de.FreeDiamsSetup=Freie Arzt-und pharmazeutischen Medikamenten Wechselwirkungen checker.

en.FreeMedFormsProject=The FreeMedForms project
fr.FreeMedFormsProject=Le projet FreeMedForms
en.FreeMedFormsProject=Das Projekt FreeMedForms

en.FullInstall=Full Install
de.FullInstall=Vollst�ndige Installation
fr.FullInstall=Installation compl�te

en.CustomInstall=Custom Install
de.CustomInstall=Benutzerdefinierte Installation
fr.CustomInstall=Installation personnalis�e

en.UninstallProgram=Uninstall %1
fr.UninstallProgram=D�sinstaller %1
de.UninstallProgram=Deinstallieren %1

;en.OneDbInstall=One drugs database install
;de.OneDbInstall=
;fr.OneDbInstall=Installation avec une seule base de donn�es th�rapeutique

en.CompDbFr=Drugs database
de.CompDbFr=Drugs database
fr.CompDbFr=Base de donn�es th�rapeutique

;en.TypeDbFr=custom
;de.TypeDbFr=custom
;fr.TypeDbFr=custom onedb
;en.TypeDbCa=custom onedb
;de.TypeDbCa=custom
;fr.TypeDbCa=custom
;en.TypeDbUs=custom onedb
;de.TypeDbUs=custom
;fr.TypeDbUs=custom
;en.TypeDbZa=custom
;de.TypeDbZa=custom
;fr.TypeDbZa=custom

en.CompHelpEn=English help
de.CompHelpEn=Englisch Hilfe
fr.CompHelpEn=Aide : traduction anglaise
en.CompHelpFr=French help translations
de.CompHelpFr=Franz�sisch Hilfe
fr.CompHelpFr=Aide fran�aise

en.StartApplication=Launch FreeDiams
de.StartApplication=Starten FreeDiams
fr.StartApplication=D�marrer FreeDiams

[Types]
Name: "full"; Description: {cm:FullInstall}
;Name: "onedb"; Description: {cm:OneDbInstall}
Name: "custom"; Description: {cm:CustomInstall}; Flags: iscustom

[Components]
Name: "program"; Description: "FreeDiams"; Types: full custom; Flags: fixed
Name: "dbFr"; Description: {cm:CompDbFr}; Types: full
Name: "helpEn"; Description: {cm:CompHelpEn}; Types: full custom
Name: "helpFr"; Description: {cm:CompHelpFr}; Types: full custom


[Files]
Source: "freediams.exe"; DestDir: "{app}"; Components: program
Source: "*.dll"; DestDir: "{app}"; Components: program
Source: "plugins/*"; DestDir: "{app}/plugins"; Components: program; Flags: recursesubdirs
Source: "Resources/datapacks/appinstalled/drugs/*"; DestDir: "{app}/Resources/datapacks/appinstalled/drugs"; Components: dbFr
Source: "Resources/pixmap/*"; DestDir: "{app}/Resources/pixmap"; Components: program; Flags: recursesubdirs
Source: "Resources/textfiles/*"; DestDir: "{app}/Resources/textfiles"; Components: program; Flags: recursesubdirs
Source: "Resources/translations/*"; DestDir: "{app}/Resources/translations"; Components: program; Flags: recursesubdirs
Source: "Resources/doc/freediams/en/*"; DestDir: "{app}/Resources/doc/freediams/en"; Components: helpEn; Flags: recursesubdirs
Source: "Resources/doc/freediams/fr/*"; DestDir: "{app}/Resources/doc/freediams/fr"; Components: helpFr; Flags: recursesubdirs
Source: "Resources/package_helpers/freemedforms.url"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Components: program  ; Flags: isreadme
Source: "COPYING.txt"; DestDir: "{app}"; Components: program

[Icons]
Name: "{group}\FreeDiams"; Filename: "{app}\FreeDiams.exe"
Name: "{group}\{cm:UninstallProgram,FreeDiams}"; Filename: "{uninstallexe}"
Name: "{group}\{cm:FreeMedFormsProject}"; FileName: "{app}\freemedforms.url"
Name: "{group}\README.txt"; FileName: "{app}\README.txt"
Name: "{group}\COPYING.txt"; FileName: "{app}\COPYING.txt"

