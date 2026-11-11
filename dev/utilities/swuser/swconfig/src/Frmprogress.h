#ifndef FrmprogressH
#define FrmprogressH

#include <System.hpp>
#include <Sysutils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
#include <CGAUGES.H>

class TTfrmProgress : public Forms::TForm
{
    __published :
                  Stdctrls::TLabel* lScanning;
    TCGauge* CGauge1;
    void __fastcall FormPaint(System::TObject* Sender);

private:

    System::AnsiString String1;
    System::AnsiString String2;
    bool FatalError;

    void __fastcall LoadLocalization();

public:
    __fastcall TTfrmProgress(Classes::TComponent* Owner);

};

extern PACKAGE TTfrmProgress* TfrmProgress;

#endif
