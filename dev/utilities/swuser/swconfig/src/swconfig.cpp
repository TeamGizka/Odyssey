#include <vcl.h>
#pragma hdrstop

#include <IniFiles.hpp>

#include "frmmain.h"
#include "Frmsysteminfo.h"
#include "Swutil.h"

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    HANDLE Instance = AcquireInstanceMutex();
    if (Instance == NULL)
        return 0;

    if (!IsGameInstalled())
    {
        AnsiString Text;
        AnsiString Caption;
        AnsiString dir = Sysutils::ExtractFilePath(Application->ExeName);
        Inifiles::TIniFile* lang = new Inifiles::TIniFile(dir + "Language.ini");
        Text = lang->ReadString("Messages", "GameNotInstalledText", "");
        Caption = lang->ReadString("Messages", "GameNotInstalledCaption", "");
        if (Text != "" && Caption != "")
        {
            MessageBoxA(NULL, Text.c_str(), Caption.c_str(), MB_ICONERROR);
        }
        delete lang;
        CloseHandle(Instance);
        return 0;
    }

    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TTFrmMain), &TFrmMain);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    CloseHandle(Instance);
    return 0;
}

