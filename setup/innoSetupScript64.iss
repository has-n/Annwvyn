; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "AnnwvynSDK64"
#define MyAppVersion "0.1.0"
#define MyAppPublisher "Arthur (Ybalrid) Brainville"
#define MyAppURL "http://annwvyn.org/"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
CompressionThreads=8
AppId={{B94D9F2F-7B8F-4B38-933A-16F6220B656B}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName=C:\Oculus\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=C:\Oculus\AnnwvynSDK64\Annwvyn\LICENSE
InfoAfterFile=C:\Oculus\AnnwvynSDK64\README.TXT
OutputDir=C:\Oculus\AnnwvynSetup64
OutputBaseFilename=setupAnnwvynSDKx64
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "C:\Oculus\AnnwvynSDK64\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs ; Excludes: "*.sdf,*.ipch,*.pch"
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\RUNME-64.BAT"

