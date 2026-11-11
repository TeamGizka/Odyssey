#ifndef FrmsysteminfoH
#define FrmsysteminfoH

#include <System.hpp>
#include <Sysutils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>

#include "Frmmain.h"
#include "Swsettings.h"
#include "Swutil.h"

class TfrmSysInfo : public Forms::TForm
{
    typedef Forms::TForm inherited;

    __published :
                  Stdctrls::TGroupBox* GroupBox1;
    Stdctrls::TLabel* lAudioString;
    Stdctrls::TLabel* lVideoStirng;
    Stdctrls::TLabel* lRAMString;
    Stdctrls::TLabel* lCPUString;
    Stdctrls::TLabel* lOSString;
    Stdctrls::TLabel* lOGLString;
    Stdctrls::TLabel* lDXString;
    Stdctrls::TLabel* DX;
    Stdctrls::TLabel* GL;
    Stdctrls::TLabel* AUD;
    Stdctrls::TLabel* Vid;
    Stdctrls::TLabel* HD;
    Stdctrls::TLabel* RAM;
    Stdctrls::TLabel* CPU;
    Stdctrls::TLabel* OS;
    Stdctrls::TButton* bnSave;
    Stdctrls::TButton* bnOK;
    Stdctrls::TLabel* lPassFailOS;
    Stdctrls::TLabel* lPassFailCPU;
    Stdctrls::TLabel* lPassFailRAM;
    Stdctrls::TLabel* lPassFailHD;
    Stdctrls::TLabel* lPassFailVID;
    Stdctrls::TLabel* lPassFailGL;
    Stdctrls::TLabel* lPassFailDX;
    Extctrls::TShape* OSShape;
    Extctrls::TShape* CPUShape;
    Extctrls::TShape* RAMShape;
    Extctrls::TShape* HDShape;
    Extctrls::TShape* VidShape;
    Extctrls::TShape* GLShape;
    Extctrls::TShape* DXShape;
    Extctrls::TShape* AUDShape;
    Stdctrls::TLabel* lPassFailAUD;
    Extctrls::TShape* Shape1;
    Stdctrls::TLabel* Label10;
    Extctrls::TShape* Shape2;
    Stdctrls::TLabel* Label11;
    Extctrls::TShape* Shape3;
    Stdctrls::TLabel* Label13;
    Stdctrls::TLabel* lReasonOS;
    Stdctrls::TLabel* lReasonCPU;
    Stdctrls::TLabel* lReasonRAM;
    Stdctrls::TLabel* lReasonHD;
    Stdctrls::TLabel* lReasonVID1;
    Stdctrls::TLabel* lReasonVID2;
    Stdctrls::TLabel* lReasonAUD;
    Stdctrls::TLabel* lReasonOGL;
    Stdctrls::TLabel* lReasonDX;
    Forms::TScrollBox* sbHDs;
    Dialogs::TSaveDialog* dlgSave;
    void __fastcall bnOKClick(System::TObject* Sender);
    void __fastcall bnSaveClick(System::TObject* Sender);
    void __fastcall FormPaint(System::TObject* Sender);

private:
    TSysScanData* Data;
    bool FatalError;
    bool Details;
    bool DriverPageShown;
    TRequirements* Requirements;
    unsigned char* Results;

    System::AnsiString TextFiles;
    System::AnsiString Passed;
    System::AnsiString Failed;
    System::AnsiString Warning;
    System::AnsiString Free;
    System::AnsiString NotInstalled;
    System::AnsiString ErrorText;
    System::AnsiString ErrorCaption;
    System::AnsiString Recommended;
    System::AnsiString Required;
    System::AnsiString RecommendedAudio;
    System::AnsiString VRAMNotDetected;

    void __fastcall FillData();
    System::AnsiString __fastcall ResolveText(System::AnsiString Value);
    System::AnsiString __fastcall GetCpuSpeedSuffix();
    void __fastcall LayoutForm();
    void __fastcall LayoutColumns();
    void __fastcall AutoWidth(Controls::TWinControl* Root);
    void __fastcall CenterButtons();
    void __fastcall LoadLocalization();

public:
    __fastcall TfrmSysInfo(Classes::TComponent* Owner, TSysScanData* Data,
                           unsigned char* Results, TRequirements* Requirements,
                           bool Details);
};

extern PACKAGE TfrmSysInfo* frmSysInfo;

#endif
