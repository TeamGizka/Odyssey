#ifndef FrmscanresultH
#define FrmscanresultH

#include <System.hpp>
#include <Sysutils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>

#include "Frmmain.h"

class TfrmScanResults : public Forms::TForm
{
    __published
        :
          Stdctrls::TButton*
              bnDetails;
    Stdctrls::TLabel* lLine1;
    Stdctrls::TLabel* lDetailedReport;
    Stdctrls::TButton* bnClose;
    Extctrls::TShape* Shape1;
    Stdctrls::TLabel* lGlobalPassFail;
    Stdctrls::TLabel* lLine2;
    void __fastcall FormPaint(System::TObject* Sender);
    void __fastcall bnCloseClick(System::TObject* Sender);
    void __fastcall bnDetailsClick(System::TObject* Sender);

private:
    TSysScanData* Data;
    bool FatalError;
    bool HardwareScanned;
    TRequirements Requirements;

    unsigned char Results[9];

    System::AnsiString ScanFailed;
    System::AnsiString RequirementsMet;
    System::AnsiString RequirementsNotMet;
    System::AnsiString ScanFailedText;
    System::AnsiString RequirementsMetText;
    System::AnsiString
        RequirementsNotMetText;
    System::AnsiString
        RecommendationMetText;
    System::AnsiString
        RecommendationNotMetText;
    System::AnsiString ErrorText;
    System::AnsiString ErrorCaption;

    void __fastcall LoadRequirements();
    void __fastcall LoadLocalization();
    void __fastcall UpdateResults();
    void __fastcall LayoutForm();
    void __fastcall AutoWidth(Controls::TWinControl* Root);
    void __fastcall CenterButtons();

public:
    __fastcall TfrmScanResults(Classes::TComponent* Owner, bool HardwareScanned,
                               TSysScanData* Data);
    __fastcall ~TfrmScanResults();
};

extern PACKAGE TfrmScanResults* frmScanResults;

#endif
