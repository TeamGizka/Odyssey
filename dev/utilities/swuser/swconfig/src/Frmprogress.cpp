#include <vcl.h>
#pragma hdrstop

#include <IniFiles.hpp>

#include "Frmprogress.h"
#include "Frmsysteminfo.h"
#include "Swsettings.h"
#include "Swutil.h"

#pragma package(smart_init)
TTfrmProgress* TfrmProgress;

__fastcall TTfrmProgress::TTfrmProgress(Classes::TComponent* Owner)
    : Forms::TForm(Owner)
{
    FatalError = false;
    LoadLocalization();
}

void __fastcall TTfrmProgress::FormPaint(System::TObject*  )
{
    if (FatalError)
    {
        Hide();
        MessageBoxA(WindowHandle, String1.c_str(), String2.c_str(),
                    MB_ICONERROR);
        Application->Terminate();
    }
}

void __fastcall TTfrmProgress::LoadLocalization()
{
    CTalkTable tlk;
    AnsiString dir;
    if (!GetGameInstallDir(dir))
        dir = Sysutils::ExtractFilePath(Application->ExeName);
    if (dir[dir.Length()] != '\\')
        dir += "\\";
    bool ok;
    Inifiles::TIniFile* lang;
    if (Sysutils::FileExists(dir + "Language.ini"))
        lang = new Inifiles::TIniFile(dir + "Language.ini");
    else
        lang = new Inifiles::TIniFile(dir + "Utils\\" + "Language.ini");
    tlk.Open((dir + "Dialog.tlk").c_str());
    String1 = lang->ReadString("Global", "ErrorText", "");
    String2 == lang->ReadString("Global", "ErrorCaption", "");
    ok = LocalizeTree(this, Name, lang, &tlk);
    if (String1 == "" || String2 == "" || !ok)
        FatalError = true;
}

