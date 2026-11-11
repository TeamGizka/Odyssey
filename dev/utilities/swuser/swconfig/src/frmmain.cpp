#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <IniFiles.hpp>
#include <string.h>

#include "frmmain.h"
#include "Frmprogress.h"
#include "Frmscanresult.h"
#include "Frmsysteminfo.h"
#include "Swsettings.h"
#include "Swutil.h"
#include "SysScan.h"

#pragma package(smart_init)
TTFrmMain* TFrmMain;

bool __fastcall LocalizeTree(Controls::TWinControl* Root,                  const System::AnsiString& Section,
 Inifiles::TIniFile* Ini,                  CTalkTable* Tlk)
{
    if (Root == NULL)
        return false;
    bool OwnIni = false;
    if (Ini == NULL)
    {
        AnsiString dir = Sysutils::ExtractFilePath(Application->ExeName);
        if (dir[dir.Length()] != '\\')
            dir += "\\";
        if (Sysutils::FileExists(dir + "Language.ini"))
            Ini = new Inifiles::TIniFile(dir + "Language.ini");
        else
            Ini = new Inifiles::TIniFile(dir + "Utils\\" + "Language.ini");
        OwnIni = true;
    }
    bool Result = LocalizeControl(Root, Section, Ini, Tlk) != false;
    for (int i = 0; i < Root->ControlCount; ++i)
    {
        Controls::TControl* Control = Root->Controls[i];
        if (Control != NULL)
        {
            Controls::TWinControl* win =
                dynamic_cast<Controls::TWinControl*>(Control);
            if (win != NULL)
                Result =                    (LocalizeTree(win, Section, Ini, Tlk) != false)
 && Result;
            Result = (LocalizeControl(Control, Section, Ini, Tlk) != false)
 &&                     Result;
        }
    }
    if (OwnIni)
    {
        Ini->UpdateFile();
        delete Ini;
    }
    return Result;
}

bool __fastcall LocalizeControl(Controls::TControl* Control,
                     const System::AnsiString& Section,
 Inifiles::TIniFile* Ini,                     CTalkTable* Tlk)
{
    int len = Control->GetTextLen() + 1;
    char* buf = new char[len];
    memset(buf, 0, len);
    Control->GetTextBuf(buf, len);
    AnsiString Text = buf;
    delete[] buf;
    AnsiString Value = "";
    if (Ini != NULL)
        Value = Ini->ReadString(Section, Control->Name, "");
    if (Value == "" && Control->Tag != 0 && Tlk != NULL)
        Value = Tlk->GetString(Control->Tag);
    bool Result = true;
    if (Value == "" && Text != "")
    {
        Result = false;
        Value = Text;
    }
    Control->SetTextBuf(Value.c_str());
    return Result;
}

int __fastcall ShowLocalizedMessageBox(const System::AnsiString Section,
                            const System::AnsiString TextIdent,
                            const System::AnsiString CaptionIdent,
                            const int& TextStrRef,
 const int& CaptionStrRef,
                            const unsigned& Flags)
{
    AnsiString Text;
    AnsiString Caption;
    AnsiString dir = Sysutils::ExtractFilePath(Application->ExeName);
    if (dir[dir.Length()] != '\\')
        dir += "\\";
    Inifiles::TIniFile* Ini;
    if (Sysutils::FileExists(dir + "Language.ini"))
        Ini = new Inifiles::TIniFile(dir + "Language.ini");
    else
        Ini = new Inifiles::TIniFile(dir + "Utils\\" + "Language.ini");
    CTalkTable tlk;
    if (!tlk.Open((dir + "Dialog.tlk").c_str()))
    {
        Text = Ini->ReadString(Section, TextIdent, "");
        if (Text == "")
        {
            delete Ini;
            return -1;
        }
        Caption = Ini->ReadString(Section, CaptionIdent, "");
        if (Caption == "")
        {
            delete Ini;
            return -1;
        }
    }
    else
    {
        Text = Ini->ReadString(Section, TextIdent, "");
        if (Text == "")
            Text = tlk.GetString(TextStrRef);
        Caption = Ini->ReadString(Section, CaptionIdent, "");
        if (Caption == "")
            Caption = tlk.GetString(CaptionStrRef);
    }
    delete Ini;
    return MessageBoxA(NULL, Text.c_str(), Caption.c_str(), Flags);
}

bool __fastcall ReadSettings(int* Settings)
{
    AnsiString dir = "";
    if (!GetGameInstallDir(dir))
        dir = ExtractFilePath(Application->ExeName);
    int len = dir.Length();
    dir.Unique();
    if (dir[len] != '\\')
        dir += "\\";
    AnsiString path = dir + "swkotor.ini";
    if (!FileExists(path))
    {
        TFileStream* stream = new TFileStream(path, fmCreate);
        delete stream;
    }

    TIniFile* ini = new TIniFile(path);
    Settings[15] =        ini->ReadBool("Graphics Options", "EnableHardwareMouse", true);
    Settings[3] = ini->ReadInteger("Graphics Options", "RefreshRate", 60);
    Settings[0] = ini->ReadInteger("Graphics Options", "Width", 800);
    Settings[1] = ini->ReadInteger("Graphics Options", "Height", 600);
    Settings[5] = ini->ReadInteger("Graphics Options", "Emitters", 1);
    Settings[6] = ini->ReadInteger("Graphics Options", "Anisotropy", 1);
    Settings[7] = ini->ReadInteger("Graphics Options", "Frame Buffer", 1);
    Settings[8] = ini->ReadInteger("Graphics Options", "Soft Shadows", 1);
    Settings[9] = ini->ReadInteger("Graphics Options", "Brightness", 57);
    Settings[10] = ini->ReadInteger("Graphics Options", "FullScreen", 1);
    Settings[11] =        ini->ReadBool("Graphics Options", "AllowHighMonitorFrequency", false);
    Settings[14] = ini->ReadBool("Sound Options", "Force Software", false);
    Settings[12] = ini->ReadBool("Graphics Options", "V-Sync", false);
    Settings[4] = ini->ReadInteger("Graphics Options", "Texture Quality", 2);
    Settings[2] = ini->ReadInteger("Graphics Options", "Anti Aliasing", 0);
    Settings[16] = ini->ReadInteger("Game Options", "Disable Movies", 0);
    Settings[13] = ini->ReadBool("Sound Options", "Disable Sound", false);
    delete ini;
    return true;
}

bool __fastcall WriteSettings(int* Settings)
{
    AnsiString dir = "";
    if (!GetGameInstallDir(dir))
        dir = ExtractFilePath(Application->ExeName);
    int len = dir.Length();
    dir.Unique();
    if (dir[len] != '\\')
        dir += "\\";
    AnsiString path = dir + "swkotor.ini";
    if (FileExists(path))
        SetFileAttributesA(path.c_str(), FILE_ATTRIBUTE_NORMAL);
    else
    {
        TFileStream* stream = new TFileStream(path, fmCreate);
        delete stream;
    }

    TIniFile* ini = new TIniFile(path);
    ini->WriteBool("Graphics Options", "EnableHardwareMouse",                   Settings[15] != 0);
    ini->WriteInteger("Graphics Options", "RefreshRate", Settings[3]);
    ini->WriteInteger("Graphics Options", "Width", Settings[0]);
    ini->WriteInteger("Graphics Options", "Height", Settings[1]);

    if (*(bool*)((char*)Settings + 72))
    {
        ini->WriteInteger("Graphics Options", "Emitters", Settings[5]);
        ini->WriteInteger("Graphics Options", "Anisotropy", Settings[6]);
        ini->WriteInteger("Graphics Options", "Frame Buffer", Settings[7]);
        ini->WriteInteger("Graphics Options", "Soft Shadows", Settings[8]);
        ini->WriteInteger("Graphics Options", "Brightness", Settings[9]);
        ini->WriteInteger("Graphics Options", "FullScreen", Settings[10]);
    }
    if (Settings[11])
        ini->WriteBool("Graphics Options", "AllowHighMonitorFrequency", true);
    else
        ini->DeleteKey("Graphics Options", "AllowHighMonitorFrequency");
    ini->WriteBool("Sound Options", "Disable Sound", Settings[13] != 0);
    ini->WriteBool("Sound Options", "Force Software", Settings[14] != 0);
    ini->WriteBool("Graphics Options", "V-Sync", Settings[12] != 0);
    ini->WriteInteger("Graphics Options", "Texture Quality", Settings[4]);
    ini->WriteInteger("Graphics Options", "Anti Aliasing", Settings[2]);
    ini->WriteBool("Game Options", "Disable Movies", Settings[16] != 0);
    ini->UpdateFile();
    delete ini;

    int old[17];
    memcpy(old, Settings, sizeof(old));
    ReadSettings(Settings);
    return old[0] == Settings[0] && old[1] == Settings[1]
 &&           old[2] == Settings[2] && old[3] == Settings[3]
 &&           old[4] == Settings[4] && old[12] == Settings[12]
 &&           old[13] == Settings[13] && old[5] == Settings[5]
 &&           old[6] == Settings[6] && old[7] == Settings[7]
 &&           old[8] == Settings[8] && old[9] == Settings[9]
 &&           old[10] == Settings[10] && old[11] == Settings[11]
 &&           old[14] == Settings[14] && old[15] == Settings[15]
 &&           old[16] == Settings[16];
}

void __fastcall TRequirements::SplitList(const AnsiString& Value, TStrings* List)
{
    char* buf = new char[Value.Length() + 1];
    strcpy(buf, Value.c_str());

    char* p = buf;
    TList* offsets = new TList;
    offsets->Add(NULL);
    for (; *p != 0; p++)
    {
        if (*p == ',')
        {
            *p = 0;
            offsets->Add((void*)(p + 1 - buf));
        }
    }
    for (int i = 0; i < offsets->Count; i++)
    {
        p = &buf[(int)offsets->Items[i]];
        if (*p != 0)
            List->Add(Trim(AnsiString(p)));
    }
    delete buf;
}

TVersion __fastcall TRequirements::ParseVersion(const AnsiString& Value)
{
    int part[4];
    part[0] = 0;
    part[1] = 0;
    part[2] = 0;
    part[3] = 0;

    char* buf = new char[Value.Length() + 1];
    strcpy(buf, Value.c_str());

    int count = 1;
    char* p = buf;
    for (; *p != 0; p++)
    {
        if (*p == '.')
        {
            *p = 0;
            count++;
        }
    }

    p = buf;
    for (int j = 0; j < count && j < 4; j++)
    {
        switch (j)
        {
        case 0:
            {
                char* String = p;
                part[0] = atol(String);
            }
            break;
        case 1:
            {
                char* String = p;
                part[1] = atol(String);
            }
            break;
        case 2:
            {
                char* String = p;
                part[2] = atol(String);
            }
            break;
        case 3:
            {
                char* String = p;
                part[3] = atol(String);
            }
            break;
        }
        p = p + strlen(p) + 1;
    }

    delete buf;
    return TVersion(part);
}

void ParseVersionDriver(TVersion* Result, const AnsiString& Value)
{
    int len = Value.Length();
    char* buf = new char[len + 1];
    strcpy(buf, Value.c_str());

    for (char* p = strrchr(buf, '.'); p != 0; p = strrchr(buf, '.'))
        *p = 0;

    Result->Part[0] = 0;
    Result->Part[1] = 0;
    Result->Part[2] = 0;
    Result->Part[3] = 0;
    Result->Part[0] = atol(buf);
    int l0 = strlen(buf);
    char* p1 = buf + l0 + 1;
    if (l0 + 1 < len)
    {
        Result->Part[1] = atol(p1);
        char* p2 = p1 + strlen(p1) + 1;
        if (p2 - buf < len)
        {
            Result->Part[2] = atol(p2);
            char* p3 = p2 + strlen(p2) + 1;
            if (p3 - buf < len)
                Result->Part[3] = atol(p3);
        }
    }
}

bool __fastcall TRequirements::CardListContains(TStrings* List, TSysScanData* Data)
{
    for (int i = 0; i < List->Count; i++)
    {
        if (Trim(Data->VideoCardName).Pos(List->Strings[i]) != 0)
            return true;
        if (Trim(Data->OpenGLRenderer).Pos(List->Strings[i]) != 0)
            return true;
    }
    return false;
}

bool __fastcall TRequirements::ReadRequirements()
{
    AnsiString path = GetCurrentDir();
    if (path[path.Length()] != '\\')
        path += "\\";
    path += "requirements.ini";
    if (!FileExists(path))
    {
        path = GetCurrentDir();
        if (path[path.Length()] != '\\')
            path += "\\";
        path += AnsiString("Utils\\") + "requirements.ini";
        if (!FileExists(path))
            return false;
    }

    TIniFile* ini = new TIniFile(path);
    if (ini != 0)
    {
    AnsiString section;
    AnsiString ident;
    AnsiString value;
    AnsiString s;

    section = "OS";
    ident = "Required";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    SplitList(s, OSRequired);
    ident = "Recommended";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    SplitList(s, OSRecommended);
    ident = "FailedMessage";
    OSFailedMessage = ini->ReadString(section, ident,
                        "Your OS does not meet the minimum requirements.");
    ident = "WarningMessage";
    OSWarningMessage = ini->ReadString(section, ident,
                        "Your OS does not meet the recommended specs.");

    section = "CPU";
    ident = "Required";
    CPURequiredSpeed = ini->ReadInteger(section, ident, 0);
    if (CPURequiredSpeed == 0)    {        delete ini;        return false;    }
    ident = "Recommended";
    CPURecommendedSpeed = ini->ReadInteger(section, ident, 0);
    if (CPURecommendedSpeed == 0)    {        delete ini;        return false;    }
    ident = "FailedMessage";
    CPUFailedMessage = ini->ReadString(section, ident,
                        "Your CPU does not meet the minimum requirements.");
    ident = "WarningMessage";
    CPUWarningMessage = ini->ReadString(section, ident,
                        "Your CPU does not meet the recommended specs.");

    section = "RAM";
    ident = "Required_Win4";
    RAMRequiredWin4 = ini->ReadInteger(section, ident, 0);
    if (RAMRequiredWin4 == 0)    {        delete ini;        return false;    }
    ident = "Recommended_Win4";
    RAMRecommendedWin4 = ini->ReadInteger(section, ident, 0);
    if (RAMRecommendedWin4 == 0)    {        delete ini;        return false;    }
    ident = "Required_Win5";
    RAMRequiredWin5 = ini->ReadInteger(section, ident, 0);
    if (RAMRequiredWin5 == 0)    {        delete ini;        return false;    }
    ident = "Recommended_Win5";
    RAMRecommendedWin5 = ini->ReadInteger(section, ident, 0);
    if (RAMRecommendedWin5 == 0)    {        delete ini;        return false;    }
    ident = "FailedMessage_Win4";
    RAMFailedMessageWin4 = ini->ReadString(section, ident,
                        "Your RAM does not meet the minimum requirements.");
    ident = "WarningMessage_Win4";
    RAMWarningMessageWin4 = ini->ReadString(section, ident,
                        "Your RAM does not meet the recommended specs.");
    ident = "FailedMessage_Win5";
    RAMFailedMessageWin5 = ini->ReadString(section, ident,
                        "Your RAM does not meet the minimum requirements.");
    ident = "WarningMessage_Win5";
    RAMWarningMessageWin5 = ini->ReadString(section, ident,
                        "Your RAM does not meet the recommended specs.");

    if (IsGameInstalled())
        section = "HD_GAMEINSTALLED";
    else
        section = "HD_GAMENOTINSTALLED";
    ident = "Required";
    HDRequiredMB = ini->ReadInteger(section, ident, 0);
    if (HDRequiredMB == 0)    {        delete ini;        return false;    }
    ident = "Recommended";
    HDRecommendedMB = ini->ReadInteger(section, ident, 0);
    if (HDRecommendedMB == 0)    {        delete ini;        return false;    }
    ident = "FailedMessage";
    HDFailedMessage = ini->ReadString(section, ident,
        "Your disk space does not meet the minimum requirements.");
    ident = "WarningMessage";
    HDWarningMessage = ini->ReadString(section, ident,
                        "Your disk space does not meet the recommended specs.");

    section = "VRAM";
    ident = "Required";
    VRAMRequiredMB = ini->ReadInteger(section, ident, 0);
    if (VRAMRequiredMB == 0)    {        delete ini;        return false;    }
    ident = "Recommended";
    VRAMRecommendedMB = ini->ReadInteger(section, ident, 0);
    if (VRAMRecommendedMB == 0)    {        delete ini;        return false;    }
    ident = "RecommendedCards";
    s = ini->ReadString(section, ident, "");
    SplitList(s, RecommendedCards);
    ident = "RequiredCards";
    s = ini->ReadString(section, ident, "");
    SplitList(s, RequiredCards);
    ident = "RecommendedCards_nvidia";
    s = ini->ReadString(section, ident, "");
    SplitList(s, RecommendedCardsNvidia);
    ident = "RequiredCards_nvidia";
    s = ini->ReadString(section, ident, "");
    SplitList(s, RequiredCardsNvidia);
    ident = "RecommendedCards_ati";
    s = ini->ReadString(section, ident, "");
    SplitList(s, RecommendedCardsAti);
    ident = "RequiredCards_ati";
    s = ini->ReadString(section, ident, "");
    SplitList(s, RequiredCardsAti);
    ident = "RequiredDriver_nvidia";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    RequiredDriverNvidia = ParseVersion(s);
    ident = "RequiredDriver_ati";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    RequiredDriverAti = ParseVersion(s);
    ident = "DriverWebSite_ati";
    DriverWebSiteAti = ini->ReadString(section, ident,
                                          "http://www.ati.com/");
    ident = "DriverWebSite_nvidia";
    DriverWebSiteNvidia = ini->ReadString(section, ident,
                                          "http://www.nvidia.com/");
    ident = "FailedMessage";
    VRAMFailedMessage = ini->ReadString(section, ident,
        "Your Video Card does not meet the minimum requirements.");
    ident = "FailedMessage2";
    VRAMFailedMessage2 = ini->ReadString(section, ident, "");
    ident = "FailedMessageDrv";
    VRAMFailedMessageDrv = ini->ReadString(section, ident,
        "Your Video Card driver does not meet the minimum requirements.");
    ident = "FailedMessageDrv_nvidia";
    VRAMFailedMessageDrvNvidia = ini->ReadString(section, ident, "");
    ident = "FailedMessageDrv_ati";
    VRAMFailedMessageDrvAti = ini->ReadString(section, ident, "");
    ident = "WarningMessage";
    VRAMWarningMessage = ini->ReadString(section, ident,
                        "Your Video Card does not meet the recommended specs.");
    ident = "WarningMessage2";
    VRAMWarningMessage2 = ini->ReadString(section, ident, "");

    section = "AUDIO";
    ident = "WarningMessage";
    AudioWarningMessage = ini->ReadString(section, ident,
        "Your Sound Card does not meet the minimum requirements.");
    ident = "FailedMessage";
    AudioFailedMessage = ini->ReadString(section, ident,
        "Your Sound Card does not meet the minimum requirements.");

    section = "GL";
    ident = "Required";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    GLRequired = ParseVersion(s);
    ident = "Recommended";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    GLRecommended = ParseVersion(s);
    ident = "BadRenderer";
    GLBadRenderer = ini->ReadString(section, ident, "GDI Generic");
    ident = "FailedMessage";
    GLFailedMessage = ini->ReadString(section, ident,
        "Your OpenGL version does not meet the minimum requirements.");
    ident = "WarningMessage";
    GLWarningMessage = ini->ReadString(section, ident,
        "Your OpenGL version does not meet the recommended specs.");

    section = "GL";
    ident = "Required_nvidia";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    GLRequiredNvidia = ParseVersion(s);
    ident = "Recommended_nvidia";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    GLRecommendedNvidia = ParseVersion(s);
    ident = "BadRenderer_nvidia";
    GLBadRendererNvidia = ini->ReadString(section, ident,
                                          "GDI Generic");
    ident = "FailedMessage_nvidia";
    GLFailedMessageNvidia = ini->ReadString(section, ident,
        "Your OpenGL version does not meet the minimum requirements.");
    ident = "WarningMessage_nvidia";
    GLWarningMessageNvidia = ini->ReadString(section, ident,
        "Your OpenGL version does not meet the recommended specs.");

    section = "GL";
    ident = "Required_ati";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    GLRequiredAti = ParseVersion(s);
    ident = "Recommended_ati";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    GLRecommendedAti = ParseVersion(s);
    ident = "BadRenderer_ati";
    GLBadRendererAti = ini->ReadString(section, ident,
                                       "GDI Generic");
    ident = "FailedMessage_ati";
    GLFailedMessageAti = ini->ReadString(section, ident,
        "Your OpenGL version does not meet the minimum requirements.");
    ident = "WarningMessage_ati";
    GLWarningMessageAti = ini->ReadString(section, ident,
        "Your OpenGL version does not meet the recommended specs.");

    section = "DX";
    ident = "Required";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    DXRequired = ParseVersion(s);
    ident = "Recommended";
    s = ini->ReadString(section, ident, "");
    if (s == "")    {        delete ini;        return false;    }
    DXRecommended = ParseVersion(s);
    ident = "FailedMessage";
    DXFailedMessage = ini->ReadString(section, ident,
        "Your DirectX version does not meet the minimum requirements.");
    ident = "WarningMessage";
    DXWarningMessage = ini->ReadString(section, ident,
        "Your DirectX version does not meet the recommended specs.");

    delete ini;
    return true;
    }
    else
        return false;
}

unsigned char* __fastcall TRequirements::CheckRequirements(TSysScanData* Data,
                                                           unsigned char* Results)
{

    if (OSRequired->IndexOf(Trim(Data->OSName)) == -1)
        Results[0] = 1;
    else if (OSRecommended->IndexOf(Trim(Data->OSName)) == -1)
        Results[0] = 2;
    else
        Results[0] = 0;

    if (Data->CPUSpeed < CPURequiredSpeed)
        Results[1] = 1;
    else if (Data->CPUSpeed < CPURecommendedSpeed)
        Results[1] = 2;
    else
        Results[1] = 0;

    if (Data->OSName.Pos("9") != 0 || Trim(Data->OSName).Pos("nt") != 0)
    {
        RAMFailedMessageSelected = RAMFailedMessageWin4;
        RAMWarningMessageSelected = RAMWarningMessageWin4;
        if (Data->RAM < RAMRequiredWin4)
            Results[2] = 1;
        else if (Data->RAM < RAMRecommendedWin4)
            Results[2] = 2;
        else
            Results[2] = 0;
    }
    else
    {
        RAMFailedMessageSelected = RAMFailedMessageWin5;
        RAMWarningMessageSelected = RAMWarningMessageWin5;
        if (Data->RAM < RAMRequiredWin5)
            Results[2] = 1;
        else if (Data->RAM < RAMRecommendedWin5)
            Results[2] = 2;
        else
            Results[2] = 0;
    }

    int i;
    for (i = 0; i < Data->DiskNames->Count; i++)
    {
        unsigned __int64 freeKB = *(__int64*)Data->DiskNames->Objects[i];
        freeKB /= 1024u;
        double freeMB = (double)freeKB * 0.0009765625;
        if ((double)HDRequiredMB <= freeMB)
            break;
    }
    if (i == Data->DiskNames->Count)
    {
        Results[3] = 1;
    }
    else
    {
        int j;
        for (j = 0; j < Data->DiskNames->Count; j++)
        {
            unsigned __int64 freeKB = *(__int64*)Data->DiskNames->Objects[j];
            freeKB /= 1024u;
            double freeMB = (double)freeKB * 0.0009765625;
            if ((double)HDRecommendedMB <= freeMB)
                break;
        }
        if (j == Data->DiskNames->Count)
            Results[3] = 2;
        else
            Results[3] = 0;
    }

    bool containsReqNvidia = CardListContains(RequiredCardsNvidia, Data);
    bool containsRecNvidia = CardListContains(RecommendedCardsNvidia, Data);
    bool containsReqAti = CardListContains(RequiredCardsAti, Data);
    bool containsRecAti = CardListContains(RecommendedCardsAti, Data);
    bool containsReq = CardListContains(RequiredCards, Data);
    bool containsRec = CardListContains(RecommendedCards, Data);
    bool anyRequired = RequiredCards->Count > 0
 ||                       RequiredCardsAti->Count > 0
 ||                       RequiredCardsNvidia->Count > 0;
    bool anyRecommended = RecommendedCards->Count > 0
 ||                          RecommendedCardsAti->Count > 0
 ||                          RecommendedCardsNvidia->Count > 0;
    if (Data->VideoMemory == 0)
        Results[4] = 2;
    else if (Data->VideoMemory < VRAMRequiredMB)
        Results[4] = 1;
    else if (Data->VideoMemory < VRAMRecommendedMB)
    {
        if (!anyRequired || containsReq || containsReqAti ||
            containsReqNvidia)
            Results[4] = 2;
        else
            Results[4] = 1;
    }
    else if (!anyRecommended || containsRec || containsRecAti ||
             containsRecNvidia)
        Results[4] = 0;
    else
        Results[4] = 2;

    Results[8] = 0;
    if (Results[4] != 1)
    {
        TVersion driver;
        ParseVersionDriver(&driver, Data->NVidiaDriver);
        if (Data->GpuVendor == 0)
        {
            if (RequiredDriverNvidia.Part[3] > driver.Part[3])
                Results[8] = 1;
        }
        else if (Data->GpuVendor == 1)
        {
            if (RequiredDriverAti.Part[2] > driver.Part[2])
                Results[8] = 1;
        }
    }

    if (Data->SoundCardName == "")
        Results[5] = 2;
    else
        Results[5] = 0;

    if (containsRecNvidia || containsReqNvidia)
    {
        GLRequired = GLRequiredNvidia;
        GLRecommended = GLRecommendedNvidia;
        GLFailedMessage = GLFailedMessageNvidia;
        GLWarningMessage = GLWarningMessageNvidia;
        GLBadRenderer = GLBadRendererNvidia;
    }
    else if (containsRecAti || containsReqAti)
    {
        GLRequired = GLRequiredAti;
        GLRecommended = GLRecommendedAti;
        GLFailedMessage = GLFailedMessageAti;
        GLWarningMessage = GLWarningMessageAti;
        GLBadRenderer = GLBadRendererAti;
    }

    TVersion gl = ParseVersion(Data->OpenGLVersion);
    bool requiredNewer = GLRequired > gl;
    if (requiredNewer || Data->OpenGLRenderer.AnsiCompare(GLBadRenderer) == 0)
        Results[6] = 1;
    else
    {
        bool recommendedNewer = GLRecommended > gl;
        if (recommendedNewer)
            Results[6] = 2;
        else
            Results[6] = 0;
    }

    TVersion dx = ParseVersion(Data->DirectXFriendly);
    bool dxRequiredNewer = DXRequired > dx;
    if (dxRequiredNewer)
        Results[7] = 1;
    else
    {
        bool dxRecommendedNewer = DXRecommended > dx;
        if (dxRecommendedNewer)
            Results[7] = 2;
        else
            Results[7] = 0;
    }

    return Results;
}

__fastcall TTFrmMain::TTFrmMain(Classes::TComponent* Owner)    : Forms::TForm(Owner)
{
    InhibitChanges = true;
    FatalError = false;
    HardwareScanned = false;
    cbResolution->Text = "";
    cbRefreshRate->Text = "";
    cbTexturePack->Text = "";
    LoadLocalization();
    Application->Title = Caption;
    FillResolutions();
    FillTexturePacks();
    LoadSettings();
    ApplyToUI();
    bnSaveSettings->Enabled = false;
    LayoutForm();
    ClearFirstRunFlag();
    InhibitChanges = false;
    Recommend = false;
    if (_argc == 2 && stricmp(_argv[1], "-firstrun") == 0)
    {
        SetBusy(false);
        RunScan();
        bnRecommendSettingsClick(this);
        bool WasEnabled = bnSaveSettings->Enabled;
        SetBusy(true);
        bnSaveSettings->Enabled = WasEnabled;
    }
    CheckSoftShadows();
}

void __fastcall TTFrmMain::bnCancelClick(System::TObject*  )
{
    Close();
}

void __fastcall TTFrmMain::bnRecommendSettingsClick(System::TObject* Sender)
{
    TSysScan Scan(NULL);
    int Recommended[17];
    Recommend = true;
    if (HardwareScanned)
    {
        if (GetRecommendedDefaults(&Data, Recommended))
        {
            memcpy(Settings, Recommended, sizeof(Recommended));
            ApplyToUI();
            ActItemChange(Sender);
        }
    }
    else if (GetRecommendedSettings(&Scan, &Data, Recommended))
    {
        memcpy(Settings, Recommended, sizeof(Recommended));
        ApplyToUI();
        ActItemChange(Sender);
        HardwareScanned = true;
    }
}

void __fastcall TTFrmMain::bnSaveSettingsClick(System::TObject*  )
{
    SaveSettings();
}

void __fastcall TTFrmMain::bnScanHardwareClick(System::TObject*  )
{
    bool WasEnabled = bnSaveSettings->Enabled;
    SetBusy(false);
    int Result = ShowLocalizedMessageBox("Messages", "ScanHardwareText",
                                         "ScanHardwareCaption", 48243, 48242,                                         MB_YESNO | MB_ICONWARNING);
    if (Result == -1)
        MessageBoxA(WindowHandle, ErrorText.c_str(), ErrorCaption.c_str(),
                    MB_ICONERROR);
    else if (Result == IDYES)
        RunScan();
    SetBusy(true);
    bnSaveSettings->Enabled = WasEnabled;
}

void __fastcall TTFrmMain::FormPaint(System::TObject*  )
{
    if (FatalError)
    {
        Hide();
        MessageBoxA(WindowHandle, "", "", MB_ICONERROR);
        Application->Terminate();
    }
}

void __fastcall TTFrmMain::ActItemChange(System::TObject* Sender)
{
    if (!InhibitChanges)
    {
        if (Sender == cbResolution)
            UpdateRefreshRates();
        if (Sender == chkDisableSound)
        {
            if (chkDisableSound->State == Stdctrls::cbChecked)
            {
                chkForceSoftwareSound->State = Stdctrls::cbUnchecked;
                chkForceSoftwareSound->Enabled = false;
            }
            else
                chkForceSoftwareSound->Enabled = true;
        }
        ReadSettingsFromUI();
        int Old[17];
        memcpy(Old, Settings, sizeof(Old));
        ReadSettings(Settings);
        bnSaveSettings->Enabled =
            Old[0] != Settings[0] || Old[1] != Settings[1] ||
            Old[2] != Settings[2] || Old[3] != Settings[3] ||
            Old[4] != Settings[4] || Old[12] != Settings[12] ||
            Old[13] != Settings[13] || Old[5] != Settings[5] ||
            Old[6] != Settings[6] || Old[7] != Settings[7] ||
            Old[8] != Settings[8] || Old[9] != Settings[9] ||
            Old[10] != Settings[10] || Old[11] != Settings[11] ||
            Old[14] != Settings[14] || Old[15] != Settings[15] ||
            Old[16] != Settings[16];
        memcpy(Settings, Old, sizeof(Settings));
    }
}

void __fastcall TTFrmMain::FormCloseQuery(System::TObject*  ,
                                          bool& CanClose)
{
    if (bnSaveSettings->Enabled)
    {
        switch (ShowLocalizedMessageBox("Messages", "ExitWithoutSaveText",
                                        "ExitWithoutSaveCaption", 48535, 48534,
                                        MB_YESNOCANCEL | MB_ICONWARNING))
        {
        case IDYES:
            CanClose = SaveSettings();
            break;
        case IDNO:
            CanClose = true;
            break;
        case IDCANCEL:
            CanClose = false;
            break;
        }
    }
    else
        CanClose = true;
}

void __fastcall TTFrmMain::FillResolutions()
{
    DEVMODE dm;
    AnsiString s;
    memset(&dm, 0, sizeof(dm));
    dm.dmSize = sizeof(dm);
    cbResolution->Items->Clear();
    for (DWORD i = 0; EnumDisplaySettingsA(NULL, i, &dm); ++i)
    {
        if (dm.dmBitsPerPel == 32
 &&            ((dm.dmDisplayFrequency >= 60
 &&              (dm.dmDisplayFrequency <= 85 || Settings[11] != 0))
 ||             dm.dmDisplayFrequency <= 1)
 &&            ((dm.dmPelsWidth == 800 && dm.dmPelsHeight == 600)
 ||             (dm.dmPelsWidth == 1024 && dm.dmPelsHeight == 768)
 ||             (dm.dmPelsWidth == 1280 && dm.dmPelsHeight == 960)
 ||             (dm.dmPelsWidth == 1280 && dm.dmPelsHeight == 1024)
 ||             (dm.dmPelsWidth == 1600 && dm.dmPelsHeight == 1200)))
        {
            System::TObject* obj =
                (System::TObject*)(dm.dmPelsHeight | (dm.dmPelsWidth << 16));
            s.sprintf("%ix%i", dm.dmPelsWidth, dm.dmPelsHeight);
            if (cbResolution->Items->IndexOf(s) == -1)
                cbResolution->Items->AddObject(s, obj);
        }
        memset(&dm, 0, sizeof(dm));
        dm.dmSize = sizeof(dm);
    }
}

void __fastcall TTFrmMain::UpdateRefreshRates()
{
    DEVMODE dm;
    AnsiString s;
    memset(&dm, 0, sizeof(dm));
    dm.dmSize = sizeof(dm);
    cbRefreshRate->Items->Clear();
    cbRefreshRate->Items->AddObject("60", (System::TObject*)60);
    int width, height;
    if (cbResolution->ItemIndex < 0)
    {
        width = 800;
        height = 600;
    }
    else
    {
        int obj =
 (int)cbResolution->Items->Objects[cbResolution->ItemIndex];
        width = obj >> 16;
        height = (unsigned short)obj;
    }
    for (DWORD i = 0; EnumDisplaySettingsA(NULL, i, &dm); ++i)
    {
        if (dm.dmBitsPerPel == 32
 && dm.dmDisplayFrequency >= 60
 &&            (dm.dmDisplayFrequency <= 85 || Settings[11] != 0)
 &&            (int)dm.dmPelsWidth == width
 && (int)dm.dmPelsHeight == height)
        {
            s.sprintf("%u", dm.dmDisplayFrequency);
            if (cbRefreshRate->Items->IndexOf(s) == -1)
                cbRefreshRate->Items->AddObject(
                    s, (System::TObject*)dm.dmDisplayFrequency);
        }
        memset(&dm, 0, sizeof(dm));
        dm.dmSize = sizeof(dm);
    }
    int Want = Settings[3];
    int BestDiff = abs(Want - 60);
    int Best = 0;
    for (int i = 1; i < cbRefreshRate->Items->Count; ++i)
    {
        int Diff = abs(Want - (int)cbRefreshRate->Items->Objects[i]);
        if (Diff < BestDiff)
        {
            BestDiff = Diff;
            Best = i;
        }
    }
    cbRefreshRate->ItemIndex = Best;
}

void __fastcall TTFrmMain::FillTexturePacks()
{
    cbTexturePack->Clear();
    cbTexturePack->Items->AddObject(Low, (System::TObject*)0);
    cbTexturePack->Items->AddObject(Medium, (System::TObject*)1);
    cbTexturePack->Items->AddObject(High, (System::TObject*)2);
}

void __fastcall TTFrmMain::LoadSettings()
{
    ReadSettings(Settings);
    ApplyToUI();
}

void __fastcall TTFrmMain::ApplyToUI()
{
    int wh = Settings[1] | (Settings[0] << 16);
    int i;
    for (i = cbResolution->Items->Count - 1; i >= 0; --i)
        if ((int)cbResolution->Items->Objects[i] == wh)
            break;
    if (i < 0)
        i = 0;
    cbResolution->ItemIndex = i;
    UpdateRefreshRates();
    int j;
    for (j = 0; j < cbTexturePack->Items->Count; ++j)
        if ((int)cbTexturePack->Items->Objects[j] == Settings[4])
            break;
    if (j == cbTexturePack->Items->Count)
        --j;
    cbTexturePack->ItemIndex = j;
    chkEnableVSync->State =
 (Stdctrls::TCheckBoxState)(Settings[12] != 0);
    chkDisableSound->State =
 (Stdctrls::TCheckBoxState)(Settings[13] != 0);
    if (chkDisableSound->State != Stdctrls::cbUnchecked)
        chkForceSoftwareSound->State = Stdctrls::cbUnchecked;
    else
        chkForceSoftwareSound->State =
            (Stdctrls::TCheckBoxState)(Settings[14] != 0);
    chkEnableHardwareMouse->State =
        (Stdctrls::TCheckBoxState)(Settings[15] != 0);
    chkDisableMovies->State =
 (Stdctrls::TCheckBoxState)(Settings[16] != 0);
    Update();
}

void __fastcall TTFrmMain::ReadSettingsFromUI()
{
    int i = cbResolution->ItemIndex;
    if (i >= 0)
    {
        Settings[0] = (int)cbResolution->Items->Objects[i] >> 16;
        Settings[1] = (unsigned short)(int)cbResolution->Items->Objects[i];
    }
    int j = cbRefreshRate->ItemIndex;
    if (j >= 0)
        Settings[3] = (int)cbRefreshRate->Items->Objects[j];
    int k = cbTexturePack->ItemIndex;
    if (k >= 0)
        Settings[4] = (int)cbTexturePack->Items->Objects[k];
    Settings[12] = chkEnableVSync->State;
    Settings[13] = chkDisableSound->State;
    Settings[14] = chkForceSoftwareSound->State;
    Settings[15] = chkEnableHardwareMouse->State;
    Settings[16] = chkDisableMovies->State;
}

bool __fastcall TTFrmMain::SaveSettings()
{
    ReadSettingsFromUI();
    if (WriteSettings(Settings))
    {
        bnSaveSettings->Enabled = false;
        return true;
    }
    if (ShowLocalizedMessageBox("Messages", "SaveFailedText",
                                "SaveFailedCaption", 48511, 48510,                                MB_ICONERROR) == -1)
        MessageBoxA(WindowHandle, ErrorText.c_str(), ErrorCaption.c_str(),
                    MB_ICONERROR);
    return false;
}

void __fastcall TTFrmMain::LoadLocalization()
{
    CTalkTable tlk;
    AnsiString dir;
    if (!GetGameInstallDir(dir))
        dir = Sysutils::ExtractFilePath(Application->ExeName);
    if (dir[dir.Length()] != '\\')
        dir += "\\";
    AnsiString iniName;
    if (Sysutils::FileExists(dir + "Language.ini"))
        iniName = dir + "Language.ini";
    else
        iniName = dir + "Utils\\" + "Language.ini";
    Inifiles::TIniFile* lang = new Inifiles::TIniFile(iniName);
    tlk.Open((dir + "Dialog.tlk").c_str());
    ErrorText = lang->ReadString("Global", "ErrorText", "");
    ErrorCaption = lang->ReadString("Global", "ErrorCaption", "");
    Low = lang->ReadString("MainForm", "Low", "");
    if (Low == "")
        Low = tlk.GetString(48507);
    Medium = lang->ReadString("MainForm", "Medium", "");
    if (Medium == "")
        Medium = tlk.GetString(48508);
    High = lang->ReadString("MainForm", "High", "");
    if (High == "")
        High = tlk.GetString(48509);
    None = lang->ReadString("MainForm", "None", "");
    if (None == "")
        None = tlk.GetString(48504);
    TwoTimes = lang->ReadString("MainForm", "TwoTimes", "");
    if (TwoTimes == "")
        TwoTimes = tlk.GetString(48505);
    FourTimes = lang->ReadString("MainForm", "FourTimes", "");
    if (FourTimes == "")
        FourTimes = tlk.GetString(48506);
    bool ok = LocalizeTree(this, Name, lang, &tlk);
    delete lang;
    if (ErrorText == "" || ErrorCaption == "" || FourTimes == ""
 ||        TwoTimes == "" || None == "" || High == "" || Medium == ""
 ||        Low == "" || !ok)
        FatalError = true;
}

void __fastcall TTFrmMain::SetBusy(bool Enable)
{
    SetControlEnabled(this, Enable);
}

void __fastcall TTFrmMain::SetControlEnabled(Controls::TControl* Control, bool Enable)
{
    Control->Enabled = Enable;
    Controls::TWinControl* win =
 dynamic_cast<Controls::TWinControl*>(Control);
    if (win)
        for (int i = 0; i < win->ControlCount; ++i)
        {
            Controls::TControl* child = win->Controls[i];
            if (child)
                SetControlEnabled(child, Enable);
        }
}

void __fastcall TTFrmMain::RunScan()
{
    TfrmProgress = new TTfrmProgress(this);
    TSysScan Scan(TfrmProgress->CGauge1);
    TfrmProgress->Show();
    HardwareScanned = ::RunScan(&Scan, &Data);
    TfrmProgress->Close();
    delete TfrmProgress;
    TfrmScanResults* Results =
        new TfrmScanResults(this, HardwareScanned, &Data);
    Results->ShowModal();
    delete Results;
}

void __fastcall TTFrmMain::ClearFirstRunFlag()
{
    AnsiString dir;
    GetGameInstallDir(dir);
    if (dir == "")
        dir = Sysutils::ExtractFilePath(Application->ExeName);
    AnsiString path = dir + "swkotor.ini";
    if (Sysutils::FileExists(path))
        SetFileAttributesA(path.c_str(), FILE_ATTRIBUTE_NORMAL);
    else
    {
        Classes::TFileStream* stream =
            new Classes::TFileStream(path, Classes::fmCreate);
        delete stream;
    }
    Inifiles::TIniFile* ini = new Inifiles::TIniFile(path);
    ini->DeleteKey("config", "firstrun");
    ini->UpdateFile();
    delete ini;
}

void __fastcall TTFrmMain::CheckSoftShadows()
{
    TSysScan Scan(NULL);
    if (::RunScan(&Scan, &Data))
    {
        TVersion driver;
        ParseVersionDriver(&driver, Data.NVidiaDriver);
        if (Data.GpuVendor == 1 && driver.Part[2] >= 4063)
        {
            AnsiString dir;
            GetGameInstallDir(dir);
            if (dir == "")
                dir = Sysutils::ExtractFilePath(Application->ExeName);
            AnsiString path = dir + "swkotor.ini";
            if (Sysutils::FileExists(path))
                SetFileAttributesA(path.c_str(), FILE_ATTRIBUTE_NORMAL);
            else
            {
                Classes::TFileStream* stream =
                    new Classes::TFileStream(path, Classes::fmCreate);
                delete stream;
            }
            Inifiles::TIniFile* ini = new Inifiles::TIniFile(path);
            ini->DeleteKey("Graphics Options", "AllowSoftShadows");
            ini->UpdateFile();
            delete ini;
        }
    }
}

void __fastcall TTFrmMain::LayoutForm()
{
    int mGraphics[4], mTrouble[4], mMisc[4];
    MeasureBox(gbGraphics, mGraphics);
    MeasureBox(gbTrouble, mTrouble);
    MeasureBox(gbMisc, mMisc);
    if (mMisc[3] <= mTrouble[3])
        mMisc[3] = mTrouble[3];
    else
        mTrouble[3] = mMisc[3];
    int w = mGraphics[0];
    if (mGraphics[0] < mTrouble[0])
        w = mTrouble[0];
    if (w < mMisc[0])
        w = mMisc[0];
    int boxWidth = w + 8;
    int box2Left = w + 28;
    int client = boxWidth + box2Left + 12;
    Width += client - ClientWidth;
    panelTop->Width = client;
    LayoutBox(gbGraphics, mGraphics, 12, boxWidth);
    LayoutBox(gbTrouble, mTrouble, box2Left, boxWidth);
    LayoutBox(gbMisc, mMisc, box2Left, boxWidth);
    panelBottom->Width = client;
    bnRecommendSettings->Left = 12;
    bnScanHardware->Left =
        bnRecommendSettings->Left + bnRecommendSettings->Width + 8;
    int Shift =
 bnCancel->Left + bnCancel->Width - (box2Left + boxWidth);
    bnCancel->Left -= Shift;
    bnSaveSettings->Left -= Shift;
}

static AnsiString __fastcall ClassName2Str(const System::ShortString& ClassName);

void __fastcall TTFrmMain::MeasureBox(Controls::TWinControl* Box, int M[4])
{
    M[1] = 0;
    M[2] = 0;
    M[3] = 0;
    for (int i = 0; i < Box->ControlCount; ++i)
    {
        Controls::TControl* Control = Box->Controls[i];
        AnsiString cls = ClassName2Str(Control->ClassName());
        if (cls == "TLabel")
        {
            if (Control->Width > M[1])
                M[1] = Control->Width;
        }
        else if (cls == "TComboBox")
        {
            Stdctrls::TComboBox* combo = (Stdctrls::TComboBox*)Control;
            HDC dc = GetDC(combo->Handle);
            HGDIOBJ oldFont =
 SelectObject(dc, combo->Font->Handle);
            int cx = 0;
            for (int j = 0; j < combo->Items->Count; ++j)
            {
                AnsiString s = combo->Items->Strings[j];
                SIZE size;
                GetTextExtentPoint32A(dc, s.c_str(), s.Length(), &size);
                if (size.cx > cx)
                    cx = size.cx;
            }
            SelectObject(dc, oldFont);
            ReleaseDC(combo->Handle, dc);
            cx += GetSystemMetrics(SM_CXVSCROLL)
 +                  8 * GetSystemMetrics(SM_CXBORDER);
            if (M[2] < cx)
                M[2] = cx;
        }
        else if (cls == "TCheckBox")
        {
            Stdctrls::TCheckBox* check = (Stdctrls::TCheckBox*)Control;
            HDC dc = GetDC(check->Handle);
            HGDIOBJ oldFont = SelectObject(dc, check->Font->Handle);
            AnsiString s = check->Caption;
            SIZE size;
            GetTextExtentPoint32A(dc, s.c_str(), s.Length(), &size);
            int w = size.cx;
            SelectObject(dc, oldFont);
            ReleaseDC(check->Handle, dc);
            w = 5 * GetSystemMetrics(SM_CXBORDER) + 20 + w;
            if (w > M[3])
                M[3] = w;
        }
    }
    M[0] = M[2] + M[1] + 36;
    if (M[3] + 48 >= M[0])
        M[0] = M[3] + 48;
}

static AnsiString __fastcall ClassName2Str(const System::ShortString& ClassName)
{
    return AnsiString(ClassName);
}

void __fastcall TTFrmMain::LayoutBox(Controls::TControl* Box, int M[4], int Left,
                          int Width)
{
    Controls::TWinControl* win = (Controls::TWinControl*)Box;
    Box->Left = Left;
    Box->Width = Width;
    int clientWidth = Box->ClientWidth;
    int labelX = (clientWidth - (M[2] + M[1] + 12)) / 2;
    int checkX = (clientWidth - M[3]) / 2;
    for (int i = 0; i < win->ControlCount; ++i)
    {
        Controls::TControl* Control = win->Controls[i];
        AnsiString cls = ClassName2Str(Control->ClassName());
        if (cls == "TLabel")
            Control->Left = labelX + M[1] - Control->Width;
        else if (cls == "TComboBox")
        {
            Control->Left = labelX + M[1] + 12;
            Control->Width = M[2];

            ((Stdctrls::TComboBox*)Control)->SelStart = 0;
            ((Stdctrls::TComboBox*)Control)->SelLength = 0;
        }
        else if (cls == "TCheckBox")
        {
            Control->Left = checkX;
            Control->Width = M[3];
        }
    }
}

TSysScanData::TSysScanData()
{
    GpuVendor = -1;
    DiskNames = new Classes::TStringList;
    Errors = new Classes::TStringList;
    Warnings = new Classes::TStringList;
    CDROMLetters = new Classes::TStringList;
    CDROMDrives = new Classes::TStringList;
    SwapFiles = new Classes::TStringList;
}

TSysScanData::~TSysScanData()
{
    for (int i = 0; i < DiskNames->Count; ++i)
        delete (void*)DiskNames->Objects[i];
    delete CDROMLetters;
    delete CDROMDrives;
    delete DiskNames;
    delete SwapFiles;
    delete Errors;
    delete Warnings;
}

TRequirements::TRequirements()
{
    RequiredDriverNvidia.Part[0] = 0;
    RequiredDriverNvidia.Part[1] = 0;
    RequiredDriverNvidia.Part[2] = 0;
    RequiredDriverNvidia.Part[3] = 0;
    RequiredDriverAti.Part[0] = 0;
    RequiredDriverAti.Part[1] = 0;
    RequiredDriverAti.Part[2] = 0;
    RequiredDriverAti.Part[3] = 0;
    GLRequired.Part[0] = 0;
    GLRequired.Part[1] = 0;
    GLRequired.Part[2] = 0;
    GLRequired.Part[3] = 0;
    GLRecommended.Part[0] = 0;
    GLRecommended.Part[1] = 0;
    GLRecommended.Part[2] = 0;
    GLRecommended.Part[3] = 0;
    GLRequiredNvidia.Part[0] = 0;
    GLRequiredNvidia.Part[1] = 0;
    GLRequiredNvidia.Part[2] = 0;
    GLRequiredNvidia.Part[3] = 0;
    GLRecommendedNvidia.Part[0] = 0;
    GLRecommendedNvidia.Part[1] = 0;
    GLRecommendedNvidia.Part[2] = 0;
    GLRecommendedNvidia.Part[3] = 0;
    GLRequiredAti.Part[0] = 0;
    GLRequiredAti.Part[1] = 0;
    GLRequiredAti.Part[2] = 0;
    GLRequiredAti.Part[3] = 0;
    GLRecommendedAti.Part[0] = 0;
    GLRecommendedAti.Part[1] = 0;
    GLRecommendedAti.Part[2] = 0;
    GLRecommendedAti.Part[3] = 0;
    DXRequired.Part[0] = 0;
    DXRequired.Part[1] = 0;
    DXRequired.Part[2] = 0;
    DXRequired.Part[3] = 0;
    DXRecommended.Part[0] = 0;
    DXRecommended.Part[1] = 0;
    DXRecommended.Part[2] = 0;
    DXRecommended.Part[3] = 0;
    OSRequired = new Classes::TStringList;
    OSRecommended = new Classes::TStringList;
    RequiredCards = new Classes::TStringList;
    RecommendedCards = new Classes::TStringList;
    RequiredCardsNvidia = new Classes::TStringList;
    RecommendedCardsNvidia = new Classes::TStringList;
    RequiredCardsAti = new Classes::TStringList;
    RecommendedCardsAti = new Classes::TStringList;
}

TRequirements::~TRequirements()
{
    delete OSRequired;
    delete OSRecommended;
    delete RequiredCards;
    delete RecommendedCards;
    delete RequiredCardsNvidia;
    delete RecommendedCardsNvidia;
    delete RequiredCardsAti;
    delete RecommendedCardsAti;
}

