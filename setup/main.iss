; Script de Inno Setup para Heza.exe

[Setup]
AppName=Heza
AppVersion=1.5
AppPublisher=Bello's Projects
AppPublisherURL=https://heza.com
DefaultDirName={localappdata}\Programs\Bello's Pojects Heza
DefaultGroupName=Heza
OutputBaseFilename=HezaSetup-Windows
Compression=lzma
SolidCompression=yes
WizardStyle=modern
SetupIconFile=HezaLogo.ico
UninstallDisplayIcon=HezaLogo.ico
PrivilegesRequired=lowest
AppCopyright=Bello's Projects 2026
VersionInfoCompany=Bello's Projects
ChangesEnvironment=yes


[Files]
Source: "Heza.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Heza"; Filename: "{app}\Heza.exe"
Name: "{group}\Heza Console"; Filename: "{app}\Heza.exe"; Parameters: "-console"

[Registry]
; Registra la extensión .hz
Root: HKCU; Subkey: ".hz"; ValueType: string; ValueName: ""; ValueData: "Heza.File"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Heza.File"; ValueType: string; ValueName: ""; ValueData: "Archivo de Heza"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Heza.File\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\Heza.exe,0"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Heza.File\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\Heza.exe"" ""%1"""; Flags: uninsdeletekey

[Code]
const
  EnvironmentKey = 'Environment';
  PathVar = 'Path';
  WM_WININICHANGE = $001A;

procedure AddToPath(NewPath: string);
var
  Paths: string;
  ExpandedPath: string;
begin
  ExpandedPath := ExpandConstant(NewPath);
  if not RegQueryStringValue(HKCU, EnvironmentKey, PathVar, Paths) then
    Paths := '';
  if Pos(';' + ExpandedPath + ';', ';' + Paths + ';') = 0 then
  begin
    if Paths <> '' then
      Paths := Paths + ';' + ExpandedPath
    else
      Paths := ExpandedPath;
    if RegWriteStringValue(HKCU, EnvironmentKey, PathVar, Paths) then
      SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0, 0);  // <--- Cambio aquí
  end;
end;

procedure RemoveFromPath(OldPath: string);
var
  Paths: string;
  ExpandedPath: string;
  NewPaths: string;
  i: Integer;
  PathList: TStringList;
begin
  ExpandedPath := ExpandConstant(OldPath);
  if not RegQueryStringValue(HKCU, EnvironmentKey, PathVar, Paths) then
    Exit;
  PathList := TStringList.Create;
  try
    PathList.Delimiter := ';';
    PathList.StrictDelimiter := True;
    PathList.DelimitedText := Paths;
    NewPaths := '';
    for i := 0 to PathList.Count - 1 do
    begin
      if CompareText(PathList[i], ExpandedPath) <> 0 then
      begin
        if NewPaths = '' then
          NewPaths := PathList[i]
        else
          NewPaths := NewPaths + ';' + PathList[i];
      end;
    end;
    if NewPaths = '' then
      RegDeleteValue(HKCU, EnvironmentKey, PathVar)
    else
      RegWriteStringValue(HKCU, EnvironmentKey, PathVar, NewPaths);
    SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0, 0);  // <--- Cambio aquí
  finally
    PathList.Free;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
    AddToPath('{app}');
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
    RemoveFromPath('{app}');
end;