; MathMap InnoSetup Installer File
; (C) 2009 Yuval Levy, licensed under GPL V2
; if possible, let the Make process edit AppVerName to have a proper, automated numbering

[Setup]
AppName=MathMap GIMP Plug-In
; ApId is checked to determine wheter to append to a particular existing uninstall log
; keep it the same for updates of the same installation
AppId=MathMap-{code:Version}
AppVerName=MathMap-{code:Version}
AppPublisher=Mark Probst
AppPublisherURL=http://www.complang.tuwien.ac.at/schani/mathmap/
AppVersion={code:Version}
AppSupportURL=http://groups.google.com/group/mathmap/
AppUpdatesURL=http://www.complang.tuwien.ac.at/schani/mathmap/
; should default to yes, but reports indicate that the entry was not created by the previous installer
CreateUninstallRegKey=yes
; default yes, but for updaters better no. play with it
UpdateUninstallLogAppName=yes
DefaultDirName={userdesktop}\..\.gimp-2.6
; AllowNoIcons does not affect entries in the Tasks section that have their own checkboxes
AllowNoIcons=yes
; show list of types only to reduce complexity unless customizing
AlwaysShowComponentsList=no
LicenseFile=COPYING
AppCopyright=Copyright (C) 1997-2009 Mark Probst
FlatComponentsList=No
; TODO: add icon and other images to the mix
;SetupIconFile=icon.ico
;WizardSmallImageFile=smallimage.bmp
;WizardImageStretch=no
;WizardImageFile=wizardimage.bmp
Compression=lzma/ultra64
SolidCompression=yes


[Types]
Name: "default";           Description: "Default installation"
Name: "custom";            Description: "Custom installation"; Flags: iscustom

[Components]
Name: "core";              Description: "MathMap";                                                     Types: default custom; Flags: fixed
Name: "expressions";       Description: "Expressions that come with MathMap";                          Types: default custom;

[Files]
; executable
Source: "plug-ins\mathmap.exe";                DestDir: "{userdesktop}\..\.gimp-2.6\plug-ins";            Components: core;         Flags: overwritereadonly
; dlls
Source: "plug-ins\libfftw3-3.dll";               DestDir: "{userdesktop}\..\.gimp-2.6\plug-ins";            Components: core;         Flags: overwritereadonly
Source: "plug-ins\libgsl.dll";               DestDir: "{userdesktop}\..\.gimp-2.6\plug-ins";            Components: core;         Flags: overwritereadonly
Source: "plug-ins\libgslcblas.dll";          DestDir: "{userdesktop}\..\.gimp-2.6\plug-ins";            Components: core;         Flags: overwritereadonly
Source: "plug-ins\libgtksourceview-2.0-0.dll"; DestDir: "{userdesktop}\..\.gimp-2.6\plug-ins";            Components: core;         Flags: overwritereadonly
Source: "plug-ins\share\*";              DestDir: "{userdesktop}\..\.gimp-2.6\plug-ins\share"; Components: core;  Flags:  overwritereadonly recursesubdirs
; template
Source: "mathmap\llvm_template.o";            DestDir: "{userdesktop}\..\.gimp-2.6\mathmap";             Components: core;         Flags: overwritereadonly
Source: "mathmap\error.png";                  DestDir: "{userdesktop}\..\.gimp-2.6\mathmap";             Components: core;         Flags: overwritereadonly
Source: "mathmap\mathmap.png";                DestDir: "{userdesktop}\..\.gimp-2.6\mathmap";             Components: core;         Flags: overwritereadonly
; expressions
Source: "mathmap\expressions\*";              DestDir: "{userdesktop}\..\.gimp-2.6\mathmap\expressions"; Components: expressions;  Flags:  overwritereadonly recursesubdirs


; install redirect URL to welcome page
;Source: "url.txt";                             DestDir: "{app}";               DestName: "test.url";     Flags:  deleteafterinstall;   Attribs: hidden
; release notes for this installer
;Source: "Release_Notes.txt";                   DestDir: "{app}\doc"; DestName: "Release Notes.txt"; Components: core;         Flags:  isreadme overwritereadonly


[Code]
function Version (Param: String) : String;
begin
  Result := '20090809';
end;


[Run]
; install counter
;Filename: "{app}\test.url"; Flags: shellexec
; here it would also be possible to launch the execution of post-install VBS scripts
;Filename: "{app}\bin\hugin.exe"; Description: "Launch hugin"; Flags: nowait postinstall skipifsilent unchecked

