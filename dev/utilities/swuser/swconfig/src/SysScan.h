#ifndef SysScanH
#define SysScanH

#include <System.hpp>
#include <Sysutils.hpp>
#include <Classes.hpp>
#include <Comctrls.hpp>
#include <stdio.h>
#include <CGAUGES.H>

#include "frmmain.h"

class TSysScan
{
public:
    bool ScanCPU;
    bool ScanOS;
    bool ScanRAM;
    bool ScanHD;
    bool ScanVideo;
    bool ScanSound;
    bool ScanDirectX;
    bool ScanOpenGL;
    bool ScanCDROM;
    Comctrls::TProgressBar* ProgressBar;
    TCGauge* Gauge;
    System::AnsiString GaugeClassName;

    TSysScan(Classes::TComponent* ProgressControl);
    ~TSysScan();

    void Begin();
    void Step();

    bool __fastcall WriteReport(TSysScanData* Data, FILE* Stream,
                                const unsigned char* Results, bool GameExists,
                                Classes::TStrings* List);
};

bool __fastcall RunScan(TSysScan* Scan, TSysScanData* Data);

bool __fastcall GetRecommendedSettings(TSysScan* Scan, TSysScanData* Data,
                                       int Settings[17]);

bool GetRecommendedDefaults(TSysScanData* Data, int Settings[17]);

#endif
