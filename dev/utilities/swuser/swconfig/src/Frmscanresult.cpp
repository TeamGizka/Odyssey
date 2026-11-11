#include <vcl.h>
#pragma hdrstop

#include <Registry.hpp>
#include <stdio.h>
#include <string.h>
#include <IniFiles.hpp>

#include "Frmscanresult.h"
#include "Frmsysteminfo.h"
#include "Swsettings.h"
#include "Swutil.h"

#pragma package(smart_init)
TfrmScanResults* frmScanResults;

__fastcall TfrmScanResults::TfrmScanResults(Classes::TComponent* Owner,
                                            bool HardwareScanned,                                            TSysScanData* Data)
    : Forms::TForm(Owner)
{
    this->Data = Data;
    FatalError = false;
    this->HardwareScanned = HardwareScanned;
    LoadLocalization();
    if (!FatalError)
    {
        LoadRequirements();
        if (!FatalError)
        {
            Requirements.CheckRequirements(this->Data, Results);
            UpdateResults();
        }
    }
    LayoutForm();
}

__fastcall TfrmScanResults::~TfrmScanResults()
{
}

void __fastcall TfrmScanResults::bnCloseClick(System::TObject*  )
{
    Close();
}

void __fastcall TfrmScanResults::bnDetailsClick(System::TObject*  )
{
    Hide();
    bool Details = false;
    if (Results[8] == 1)
    {
        Details =            (ShowLocalizedMessageBox("Messages", "DriversOutOfDate",
                                     "ReadRequirementsCaption", 49144, 48245,
                                     MB_YESNO | MB_ICONWARNING) == IDYES);
    }
    TfrmSysInfo* SysInfoForm =        new TfrmSysInfo(this, Data, Results, &Requirements,
 Details);
    SysInfoForm->ShowModal();
    delete SysInfoForm;
    Show();
}

void __fastcall TfrmScanResults::FormPaint(System::TObject*  )
{
    if (FatalError)
    {
        Hide();
        MessageBoxA(WindowHandle, ErrorText.c_str(), ErrorCaption.c_str(),
                    MB_ICONERROR);
        Close();
    }
}

void __fastcall TfrmScanResults::LoadRequirements()
{
    if (!Requirements.ReadRequirements())
    {
        FatalError = true;
        if (ShowLocalizedMessageBox("Messages", "ReadRequirementsText",
                                    "ReadRequirementsCaption", 48258, 48245,
                                    MB_ICONERROR) == -1)
        {
            MessageBoxA(WindowHandle, ErrorText.c_str(), ErrorCaption.c_str(),
                        MB_ICONERROR);
        }
        bnDetails->Enabled = false;
    }
}

void __fastcall TfrmScanResults::LoadLocalization()
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
    ScanFailed = lang->ReadString("SysResult", "ScanFailed", "");
    if (ScanFailed == "")
        ScanFailed = tlk.GetString(48425);
    RequirementsMet = lang->ReadString("SysResult", "RequirementsMet", "");
    if (RequirementsMet == "")
        RequirementsMet = tlk.GetString(48426);
    RequirementsNotMet =        lang->ReadString("SysResult", "RequirementsNotMet", "");
    if (RequirementsNotMet == "")
        RequirementsNotMet = tlk.GetString(48427);
    ScanFailedText = lang->ReadString("SysResult", "ScanFailedText", "");
    if (ScanFailedText == "")
        ScanFailedText = tlk.GetString(48437);
    RequirementsMetText =        lang->ReadString("SysResult", "RequirementsMetText", "");
    if (RequirementsMetText == "")
        RequirementsMetText = tlk.GetString(48429);
    RequirementsNotMetText =        lang->ReadString("SysResult",
 "RequirementsNotMetText", "");
    if (RequirementsNotMetText == "")
        RequirementsNotMetText = tlk.GetString(48428);
    RecommendationMetText =        lang->ReadString("SysResult",
 "RecommendationMetText", "");
    if (RecommendationMetText == "")
        RecommendationMetText = tlk.GetString(48430);
    RecommendationNotMetText =        lang->ReadString("SysResult",
 "RecommendationNotMetText", "");
    if (RecommendationNotMetText == "")
        RecommendationNotMetText = tlk.GetString(48440);
    bool ok = LocalizeTree(this, Name, lang, &tlk);
    FatalError =        (ErrorText == "" || ErrorCaption == "" ||
 RequirementsMet == "" ||         RequirementsNotMet == "" ||
 RequirementsMetText == "" ||         RequirementsNotMetText == "" ||
 RecommendationMetText == "" ||         RecommendationNotMetText == "" ||
 ScanFailed == "" ||         ScanFailedText == "" || !ok);
}

void __fastcall TfrmScanResults::UpdateResults()
{
    if (HardwareScanned)
    {
        if (Results[0] == 1 || Results[1] == 1 || Results[2] == 1 ||
            Results[3] == 1 || Results[4] == 1 || Results[8] == 1 ||
            Results[5] == 1 || Results[6] == 1 || Results[7] == 1)
        {
            Shape1->Brush->Color = clRed;
            lGlobalPassFail->Caption = RequirementsNotMet;
            lLine1->Caption = RequirementsNotMetText;
            lLine2->Caption = "";
        }
        else if (Results[0] == 2 || Results[1] == 2 || Results[2] == 2 ||
                 Results[3] == 2 || Results[4] == 2 || Results[8] == 2 ||
                 Results[5] == 2 || Results[6] == 2 || Results[7] == 2)
        {
            Shape1->Brush->Color = clYellow;
            lGlobalPassFail->Caption = RequirementsMet;
            lLine1->Caption = RequirementsMetText;
            lLine2->Caption = RecommendationNotMetText;
        }
        else
        {
            Shape1->Brush->Color = clGreen;
            lGlobalPassFail->Caption = RequirementsMet;
            lLine1->Caption = RecommendationMetText;
            lLine2->Caption = "";
        }
    }
    else
    {
        Shape1->Brush->Color = clYellow;
        lGlobalPassFail->Caption = ScanFailed;
        lLine1->Caption = ScanFailedText;
        lLine2->Caption = "";
    }
}

void __fastcall TfrmScanResults::LayoutForm()
{
    AutoWidth(this);
    CenterButtons();
}

void __fastcall TfrmScanResults::AutoWidth(Controls::TWinControl* Root)
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

void __fastcall TfrmScanResults::CenterButtons()
{
    int shift = Width - bnClose->Left - bnClose->Width - 28;
    bnDetails->Left = bnDetails->Left + shift;
    bnClose->Left = bnClose->Left + shift;
}

