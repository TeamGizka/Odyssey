#include <vcl.h>
#pragma hdrstop

#include <IniFiles.hpp>

#include "Frmsysteminfo.h"
#include "Swutil.h"

#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SysScan.h"

#pragma package(smart_init)
TfrmSysInfo* frmSysInfo;

__fastcall TfrmSysInfo::TfrmSysInfo(Classes::TComponent* Owner,
                                    TSysScanData* Data, unsigned char* Results,
                                    TRequirements* Requirements, bool Details)
    : Forms::TForm(Owner)
{
    this->Data = Data;
    this->Requirements = Requirements;
    this->Results = Results;
    this->Details = Details;
    DriverPageShown = false;
    FatalError = false;

    LoadLocalization();
    if (!FatalError)
        FillData();
    LayoutForm();
}

void __fastcall TfrmSysInfo::bnOKClick(System::TObject* Sender)
{
    Close();
}

void __fastcall TfrmSysInfo::bnSaveClick(System::TObject* Sender)
{
    bool installed = IsGameInstalled();
    AnsiString oldDir = Sysutils::GetCurrentDir();

    AnsiString filter;
    filter.sprintf("%s (*.txt)|*.TXT", TextFiles.c_str());
    dlgSave->Filter = filter;
    dlgSave->FileName = "swinfo.txt";

    HMODULE shell = LoadLibraryA("shell32.dll");
    if (shell != NULL)
    {
        BOOL(__stdcall * GetSpecialFolderPath)(HWND, LPSTR, int, BOOL) =
            (BOOL(__stdcall*)(HWND, LPSTR, int, BOOL))
GetProcAddress(                shell, "SHGetSpecialFolderPathA");
        char buf[260];
        if (GetSpecialFolderPath != NULL &&
            GetSpecialFolderPath(NULL, buf, 0, false))
            dlgSave->InitialDir = buf;
        FreeLibrary(shell);
    }

    if (dlgSave->Execute())
    {
        if (Sysutils::FileExists(dlgSave->FileName))
        {
            SetFileAttributesA(dlgSave->FileName.c_str(),
                               FILE_ATTRIBUTE_NORMAL);
            DeleteFileA(dlgSave->FileName.c_str());
        }
        FILE* f = fopen(dlgSave->FileName.c_str(), "w");
        if (f != NULL)
        {
            TSysScan Scan(NULL);
            Scan.WriteReport(Data, f, Results, installed, NULL);
            fclose(f);
        }
        else if (ShowLocalizedMessageBox("Messages", "SaveReportText",
                                         "SaveReportCaption", 48523, 48522,
                                         MB_ICONERROR) == -1)
        {
            MessageBoxA(WindowHandle, ErrorText.c_str(), ErrorCaption.c_str(),
                        MB_ICONERROR);
        }
    }
    Sysutils::SetCurrentDir(oldDir);
}

void __fastcall TfrmSysInfo::FormPaint(System::TObject* Sender)
{
    if (FatalError)
    {
        Hide();
        MessageBoxA(WindowHandle, ErrorText.c_str(), ErrorCaption.c_str(),
                    MB_ICONERROR);
        Close();
    }
    if (Details && !DriverPageShown)
    {
        if (Data->GpuVendor == 0)
        {
            ShellExecuteA(NULL, NULL, Requirements->DriverWebSiteNvidia.c_str(),
                          "", "", SW_SHOWNORMAL);
            DriverPageShown = true;
        }
        else if (Data->GpuVendor == 1)
        {
            ShellExecuteA(NULL, NULL, Requirements->DriverWebSiteAti.c_str(),
                          "", "", SW_SHOWNORMAL);
            DriverPageShown = true;
        }
    }
}

void __fastcall TfrmSysInfo::FillData()
{
    lOSString->Caption = Data->OSVersion;
    if (Results[0] == 0)
    {
        lPassFailOS->Caption = Passed;
        OSShape->Brush->Color = clGreen;
        lReasonOS->Caption = "";
    }
    else if (Results[0] == 1)
    {
        lPassFailOS->Caption = Failed;
        OSShape->Brush->Color = clRed;
        lReasonOS->Caption = ResolveText(Requirements->OSFailedMessage);
    }
    else if (Results[0] == 2)
    {
        lPassFailOS->Caption = Warning;
        OSShape->Brush->Color = clYellow;
        lReasonOS->Caption = ResolveText(Requirements->OSWarningMessage);
    }

    AnsiString text;
    AnsiString suffix = GetCpuSpeedSuffix();
    if (Data->CPUCount <= 1)
        text.sprintf("%s%s", Data->CPUName.c_str(), suffix.c_str());
    else
        text.sprintf("%u x %s%s", Data->CPUCount, Data->CPUName.c_str(),
                     suffix.c_str());
    lCPUString->Caption = text;
    if (Results[1] == 0)
    {
        lPassFailCPU->Caption = Passed;
        CPUShape->Brush->Color = clGreen;
        lReasonCPU->Caption = "";
    }
    else if (Results[1] == 1)
    {
        lPassFailCPU->Caption = Failed;
        CPUShape->Brush->Color = clRed;
        lReasonCPU->Caption = ResolveText(Requirements->CPUFailedMessage);
    }
    else if (Results[1] == 2)
    {
        lPassFailCPU->Caption = Warning;
        CPUShape->Brush->Color = clYellow;
        lReasonCPU->Caption = ResolveText(Requirements->CPUWarningMessage);
    }

    AnsiString ram;
    ram.sprintf("%u MB", Data->RAM);
    lRAMString->Caption = ram;
    if (Results[2] == 0)
    {
        lPassFailRAM->Caption = Passed;
        RAMShape->Brush->Color = clGreen;
        lReasonRAM->Caption = "";
    }
    else if (Results[2] == 1)
    {
        lPassFailRAM->Caption = Failed;
        RAMShape->Brush->Color = clRed;
        lReasonRAM->Caption =            ResolveText(Requirements->RAMFailedMessageSelected);
    }
    else if (Results[2] == 2)
    {
        lPassFailRAM->Caption = Warning;
        RAMShape->Brush->Color = clYellow;
        lReasonRAM->Caption =
            ResolveText(Requirements->RAMWarningMessageSelected);
    }

    AnsiString name;
    int i;
    for (i = 0; i < Data->DiskNames->Count; ++i)
    {
        name = Data->DiskNames->Strings[i];
        unsigned __int64 kb =
            *(unsigned __int64*)Data->DiskNames->Objects[i] / 0x400;
        AnsiString size;
        if (kb > 0x100000)
            size.sprintf(" %.2lf GB %s", kb / 1048576.0, Free.c_str());
        else
            size.sprintf(" %.0lf MB %s", kb / 1024.0, Free.c_str());
        Stdctrls::TLabel* label = new Stdctrls::TLabel(this);
        label->Parent = sbHDs;
        label->Top = 14 * i;
        label->Left = 8;
        label->Caption = name + size;
        label->Visible = true;
        label->Show();
    }

    Stdctrls::TLabel* reason;
    if (i <= 3)
    {
        Stdctrls::TLabel* label = new Stdctrls::TLabel(this);
        label->Parent = sbHDs;
        label->Top = 14 * i;
        label->Left = 8;
        label->Visible = true;
        label->Show();
        label->Font->Color = clNavy;
        reason = label;
    }
    else
        reason = lReasonHD;
    lReasonHD->Caption = "";
    if (Results[3] == 0)
    {
        lPassFailHD->Caption = Passed;
        HDShape->Brush->Color = clGreen;
        reason->Caption = "";
    }
    else if (Results[3] == 1)
    {
        lPassFailHD->Caption = Failed;
        HDShape->Brush->Color = clRed;
        reason->Caption = ResolveText(Requirements->HDFailedMessage);
    }
    else if (Results[3] == 2)
    {
        lPassFailHD->Caption = Warning;
        HDShape->Brush->Color = clYellow;
        reason->Caption = ResolveText(Requirements->HDWarningMessage);
    }

    AnsiString video;
    if (Data->VideoMemory != 0)
        video.sprintf("%s %uMB", Data->VideoCardName.c_str(),
                      Data->VideoMemory);
    else
        video.sprintf("%s", Data->VideoCardName.c_str());
    lVideoStirng->Caption = video;
    if (Results[4] == 0)
    {
        lPassFailVID->Caption = Passed;
        VidShape->Brush->Color = clGreen;
        lReasonVID1->Caption = "";
        lReasonVID2->Caption = "";
    }
    else if (Results[4] == 1)
    {
        lPassFailVID->Caption = Failed;
        VidShape->Brush->Color = clRed;
        lReasonVID1->Caption = ResolveText(Requirements->VRAMFailedMessage);
        lReasonVID2->Caption = ResolveText(Requirements->VRAMFailedMessage2);
    }
    else if (Results[4] == 2)
    {
        lPassFailVID->Caption = Warning;
        VidShape->Brush->Color = clYellow;
        if (Data->VideoMemory != 0)
        {
            lReasonVID1->Caption =
                ResolveText(Requirements->VRAMWarningMessage);
            lReasonVID2->Caption =
                ResolveText(Requirements->VRAMWarningMessage2);
        }
        else
        {
            lReasonVID1->Caption = VRAMNotDetected;
            lReasonVID2->Caption = "";
        }
    }

    if (Results[4] != 1 && Results[8] == 1)
    {
        lPassFailVID->Caption = Failed;
        VidShape->Brush->Color = clRed;
        lReasonVID1->Caption =
 ResolveText(Requirements->VRAMFailedMessageDrv);
        if (Data->GpuVendor == 0)
            lReasonVID2->Caption =
                ResolveText(Requirements->VRAMFailedMessageDrvNvidia);
        else if (Data->GpuVendor == 1)
            lReasonVID2->Caption =
                ResolveText(Requirements->VRAMFailedMessageDrvAti);
        else
            lReasonVID2->Caption = "";
    }

    if (Data->SoundCardName == "")
        lAudioString->Caption = NotInstalled;
    else
        lAudioString->Caption = Data->SoundCardName;
    if (Results[5] == 0)
    {
        lPassFailAUD->Caption = Passed;
        AUDShape->Brush->Color = clGreen;
        lReasonAUD->Caption = "";
    }
    else if (Results[5] == 1)
    {
        lPassFailAUD->Caption = Failed;
        AUDShape->Brush->Color = clRed;

        lReasonAUD->Caption = NotInstalled;
    }
    else if (Results[5] == 2)
    {
        lPassFailAUD->Caption = Warning;
        AUDShape->Brush->Color = clYellow;
        lReasonAUD->Caption = ResolveText(Requirements->AudioWarningMessage);
    }

    AnsiString gl;
    if (Data->OpenGLInstalled)
        gl = Data->OpenGLVendor + " - " + Data->OpenGLVersion;
    else
        gl = NotInstalled;
    lOGLString->Caption = gl;
    if (Results[6] == 0)
    {
        lPassFailGL->Caption = Passed;
        GLShape->Brush->Color = clGreen;
        lReasonOGL->Caption = "";
    }
    else if (Results[6] == 1)
    {
        lPassFailGL->Caption = Failed;
        GLShape->Brush->Color = clRed;
        lReasonOGL->Caption = ResolveText(Requirements->GLFailedMessage);
    }
    else if (Results[6] == 2)
    {
        lPassFailGL->Caption = Warning;
        GLShape->Brush->Color = clYellow;
        lReasonOGL->Caption = ResolveText(Requirements->GLWarningMessage);
    }

    AnsiString dx;
    if (Data->DirectXInstalled)
        dx = Data->DirectXVersion;
    else
        dx = NotInstalled;
    lDXString->Caption = dx;
    if (Results[7] == 0)
    {
        lPassFailDX->Caption = Passed;
        DXShape->Brush->Color = clGreen;
        lReasonDX->Caption = "";
    }
    else if (Results[7] == 1)
    {
        lPassFailDX->Caption = Failed;
        DXShape->Brush->Color = clRed;
        lReasonDX->Caption = ResolveText(Requirements->DXFailedMessage);
    }
    else if (Results[7] == 2)
    {
        lPassFailDX->Caption = Warning;
        DXShape->Brush->Color = clYellow;
        lReasonDX->Caption = ResolveText(Requirements->DXWarningMessage);
    }
}

System::AnsiString __fastcall TfrmSysInfo::ResolveText(System::AnsiString Value)
{
    AnsiString result = Value;
    int len = Value.Length();
    if (len > 0)
    {
        int i;
        for (i = 1; i <= len; ++i)
        {
            if (Value[i] < '0')
                break;
            if (Value[i] > '9')
                break;
        }
        if (len >= i)
            result = Value;
        else
        {
            int strref = Value.ToInt();
            CTalkTable tlk;
            AnsiString dir = Sysutils::ExtractFilePath(Application->ExeName);
            if (dir[dir.Length()] != '\\')
                dir += "\\";
            if (tlk.Open((dir + "Dialog.tlk").c_str()))
                result = tlk.GetString(strref);
            else
                result = "";
        }
    }
    return result;
}

System::AnsiString __fastcall TfrmSysInfo::GetCpuSpeedSuffix()
{
    AnsiString Result;
    if (Data->CPUName.Pos("+") != 0)
    {
        Result.sprintf(" (%uMHz)", Data->CPUSpeed);
        return Result;
    }

    AnsiString name = Sysutils::LowerCase(Data->CPUName);
    int hz = name.Pos("hz");
    if (hz > 2)
    {
        name.SetLength(hz - 1);
        bool ghz = name[hz - 1] == 'g';
        int end = hz - 2;
        while (end > 0)
        {
            name.SetLength(end);
            if (name[end] != ' ')
                break;
            --end;
        }
        if (end > 0)
        {
            int run;
            for (run = 0; end > 0; ++run)
            {
                name.SetLength(end);
                if (name[end] != '.' && name[end] != ',' &&
 name[end] >= '0' &&                    name[end] <= '9')
                    --end;
                else
                    break;
            }
            AnsiString number = name.SubString(end + 1, run);
            while (number.Pos(",") > 0)
                number.Delete(number.Pos(","), 1);
            double reported = atof(number.c_str());
            if (ghz)
                reported *= 1000.0;
            if (fabs(reported - Data->CPUSpeed) / reported * 100.0 > 2.0)
                Result.sprintf(" (@%uMHz)", Data->CPUSpeed);
        }
    }
    return Result;
}

void __fastcall TfrmSysInfo::LayoutForm()
{
    LayoutColumns();
    AutoWidth(this);
    CenterButtons();
}

void __fastcall TfrmSysInfo::LayoutColumns()
{
    int w1 = max(OS->Width, CPU->Width);
    int w2 = max(RAM->Width, HD->Width);
    int w3 = max(Vid->Width, AUD->Width);
    int w4 = max(GL->Width, DX->Width);
    int headerW = max(max(w3, w4), max(w1, w2));

    int pf1 = max(lPassFailOS->Width, lPassFailCPU->Width);
    int pf2 = max(lPassFailRAM->Width, lPassFailHD->Width);
    int pf3 = max(lPassFailVID->Width, lPassFailAUD->Width);
    int pf4 = max(lPassFailGL->Width, lPassFailDX->Width);
    int pfW = max(max(pf3, pf4), max(pf1, pf2));

    int shapesLeft = headerW + OS->Left + 6;
    OSShape->Left = shapesLeft;
    CPUShape->Left = shapesLeft;
    RAMShape->Left = shapesLeft;
    HDShape->Left = shapesLeft;
    VidShape->Left = shapesLeft;
    AUDShape->Left = shapesLeft;
    GLShape->Left = shapesLeft;
    DXShape->Left = shapesLeft;

    int pfLeft = OSShape->Width + shapesLeft + 6;
    lPassFailOS->Left = pfLeft;
    lPassFailCPU->Left = pfLeft;
    lPassFailRAM->Left = pfLeft;
    lPassFailHD->Left = pfLeft;
    lPassFailVID->Left = pfLeft;
    lPassFailAUD->Left = pfLeft;
    lPassFailGL->Left = pfLeft;
    lPassFailDX->Left = pfLeft;

    int valueLeft = pfW + pfLeft + 30;
    lOSString->Left = valueLeft;
    lReasonOS->Left = valueLeft;
    lCPUString->Left = valueLeft;
    lReasonCPU->Left = valueLeft;
    lRAMString->Left = valueLeft;
    lReasonRAM->Left = valueLeft;
    lReasonHD->Left = valueLeft;
    lVideoStirng->Left = valueLeft;
    lReasonVID1->Left = valueLeft;
    lReasonVID2->Left = valueLeft;
    lAudioString->Left = valueLeft;
    lReasonAUD->Left = valueLeft;
    lOGLString->Left = valueLeft;
    lReasonOGL->Left = valueLeft;
    lDXString->Left = valueLeft;
    lReasonDX->Left = valueLeft;
    sbHDs->Left = valueLeft - 8;
}

void __fastcall TfrmSysInfo::AutoWidth(Controls::TWinControl* Root)
{
    int w = 0;
    for (int i = 0; i < Root->ControlCount; ++i)
    {
        Controls::TControl* Control = Root->Controls[i];
        if (Control != NULL)
        {
            Controls::TWinControl* win =
                dynamic_cast<Controls::TWinControl*>(Control);
            if (win != NULL)
                AutoWidth(win);
            if (w < Control->Left + Control->Width)
                w = Control->Left + Control->Width;
        }
    }
    if (w + 16 > Root->Width)
        Root->Width = w + 16;
}

void __fastcall TfrmSysInfo::CenterButtons()
{
    int shift = Width - bnOK->Left - bnOK->Width;
    int saveLeft = (bnSave->Left + shift) / 2;
    int okShift = saveLeft - bnSave->Left;
    bnSave->Left = saveLeft;
    bnOK->Left = okShift + bnOK->Left;
}

void __fastcall TfrmSysInfo::LoadLocalization()
{
    CTalkTable tlk;
    AnsiString dir;
    if (!GetGameInstallDir(dir))
        dir = Sysutils::ExtractFilePath(Application->ExeName);
    if (dir[dir.Length()] != '\\')
        dir += "\\";

    Inifiles::TIniFile* Ini;
    if (Sysutils::FileExists(dir + "Language.ini"))
        Ini = new Inifiles::TIniFile(dir + "Language.ini");
    else
        Ini = new Inifiles::TIniFile(dir + "Utils\\" + "Language.ini");

    tlk.Open((dir + "Dialog.tlk").c_str());

    ErrorText = Ini->ReadString("Global", "ErrorText", "");
    ErrorCaption = Ini->ReadString("Global", "ErrorCaption", "");

    TextFiles = Ini->ReadString("SysInfo", "TextFiles", "");
    if (TextFiles == "")
        TextFiles = tlk.GetString(48259);
    Passed = Ini->ReadString("SysInfo", "Passed", "");
    if (Passed == "")
        Passed = tlk.GetString(48260);
    Failed = Ini->ReadString("SysInfo", "Failed", "");
    if (Failed == "")
        Failed = tlk.GetString(48261);
    Warning = Ini->ReadString("SysInfo", "Warning", "");
    if (Warning == "")
        Warning = tlk.GetString(48262);
    Recommended = Ini->ReadString("SysInfo", "Recommended", "");
    if (Recommended == "")
        Recommended = tlk.GetString(49110);
    Required = Ini->ReadString("SysInfo", "Required", "");
    if (Required == "")
        Required = tlk.GetString(49111);
    Free = Ini->ReadString("SysInfo", "Free", "");
    if (Free == "")
        Free = tlk.GetString(48264);
    NotInstalled = Ini->ReadString("SysInfo", "NotInstalled", "");
    if (NotInstalled == "")
        NotInstalled = tlk.GetString(48263);
    RecommendedAudio = Ini->ReadString("SysInfo", "RecommendedAudio", "");
    if (RecommendedAudio == "")
        RecommendedAudio = tlk.GetString(49112);
    VRAMNotDetected = Ini->ReadString("SysInfo", "VRAMNotDetected", "");
    if (VRAMNotDetected == "")
        VRAMNotDetected = tlk.GetString(48414);

    LocalizeTree(this, Name, Ini, NULL);
    bool treeOk = LocalizeTree(this, Name, Ini, &tlk);

    if (ErrorText == "" || ErrorCaption == "" || VRAMNotDetected == "" ||
        RecommendedAudio == "" || NotInstalled == "" || Free == "" ||
        Required == "" || Recommended == "" || Warning == "" || Failed == "" ||
        Passed == "" || TextFiles == "" || !treeOk)
        FatalError = true;
}

