#ifndef frmmainH
#define frmmainH

#include <System.hpp>
#include <Sysutils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <Forms.hpp>
#include <Comctrls.hpp>
#include <Dialogs.hpp>
#include <CGAUGES.H>

struct TVersion;

#include "Getprocessorspeed.h"

struct TSysScanData
{
    System::AnsiString OSName;
    System::AnsiString OSVersion;
    System::AnsiString OSServicePack;
    Classes::TStringList* SwapFiles;
    int CPUCount;
    int CPUSpeed;
    int CPUFamily;
    int CPUModel;
    int CPUStepping;
    System::AnsiString CPUVendor;
    System::AnsiString CPUName;
    int RAM;
    Classes::TStringList* DiskNames;
    Classes::TStringList* CDROMLetters;
    Classes::TStringList* CDROMDrives;
    System::AnsiString VideoCardName;
    int VideoMemory;
    System::AnsiString NVidiaDriver;
    int DesktopWidth;
    int DesktopHeight;
    int DesktopBitsPerPixel;
    int RefreshRate;
    int GpuVendor;
    int Unused5C;

    System::AnsiString SoundCardName;
    bool OpenGLInstalled;
    System::AnsiString OpenGLVersion;
    System::AnsiString OpenGLVendor;
    System::AnsiString OpenGLRenderer;
    bool DirectXInstalled;
    System::AnsiString DirectXVersion;
    System::AnsiString DirectXFriendly;
    Classes::TStringList* Errors;
    Classes::TStringList* Warnings;

    TSysScanData();
    ~TSysScanData();
};

struct TRequirements
{

    Classes::TStringList* OSRequired;
    Classes::TStringList* OSRecommended;
    System::AnsiString OSFailedMessage;
    System::AnsiString OSWarningMessage;

    unsigned int CPURequiredSpeed;
    unsigned int CPURecommendedSpeed;
    System::AnsiString CPUFailedMessage;
    System::AnsiString CPUWarningMessage;

    unsigned int RAMRequiredWin4;
    unsigned int RAMRecommendedWin4;
    System::AnsiString RAMFailedMessageWin4;
    System::AnsiString RAMWarningMessageWin4;
    unsigned int RAMRequiredWin5;
    unsigned int RAMRecommendedWin5;
    System::AnsiString RAMFailedMessageWin5;
    System::AnsiString RAMWarningMessageWin5;

    System::AnsiString RAMFailedMessageSelected;
    System::AnsiString RAMWarningMessageSelected;

    int HDRequiredMB;
    int HDRecommendedMB;
    System::AnsiString HDFailedMessage;
    System::AnsiString HDWarningMessage;

    unsigned int VRAMRequiredMB;
    unsigned int VRAMRecommendedMB;
    Classes::TStringList* RequiredCards;
    Classes::TStringList* RecommendedCards;
    Classes::TStringList* RequiredCardsNvidia;
    Classes::TStringList* RecommendedCardsNvidia;
    Classes::TStringList* RequiredCardsAti;
    Classes::TStringList* RecommendedCardsAti;
    System::AnsiString VRAMFailedMessage;
    System::AnsiString VRAMWarningMessage;
    System::AnsiString VRAMFailedMessage2;
    System::AnsiString VRAMWarningMessage2;
    System::AnsiString DriverWebSiteAti;
    System::AnsiString DriverWebSiteNvidia;
    System::AnsiString VRAMFailedMessageDrv;
    System::AnsiString VRAMFailedMessageDrvNvidia;
    System::AnsiString VRAMFailedMessageDrvAti;
    TVersion RequiredDriverNvidia;
    TVersion RequiredDriverAti;

    System::AnsiString AudioWarningMessage;
    System::AnsiString AudioFailedMessage;

    TVersion GLRequired;
    TVersion GLRecommended;
    System::AnsiString GLFailedMessage;
    System::AnsiString GLWarningMessage;
    System::AnsiString GLBadRenderer;

    TVersion GLRequiredNvidia;
    TVersion GLRecommendedNvidia;
    System::AnsiString GLFailedMessageNvidia;
    System::AnsiString GLWarningMessageNvidia;
    System::AnsiString GLBadRendererNvidia;

    TVersion GLRequiredAti;
    TVersion GLRecommendedAti;
    System::AnsiString GLFailedMessageAti;
    System::AnsiString GLWarningMessageAti;
    System::AnsiString GLBadRendererAti;

    TVersion DXRequired;
    TVersion DXRecommended;
    System::AnsiString DXFailedMessage;
    System::AnsiString DXWarningMessage;

    bool __fastcall ReadRequirements();

    unsigned char* __fastcall CheckRequirements(TSysScanData* Data,
                                     unsigned char* Results);

    TRequirements();
    ~TRequirements();

private:

    void __fastcall SplitList(const System::AnsiString& Value, Classes::TStrings* List);

    TVersion __fastcall ParseVersion(const System::AnsiString& Value);

    bool __fastcall CardListContains(Classes::TStrings* List, TSysScanData* Data);
};

class TTFrmMain : public TForm
{
    __published
        :

          TPanel* panelBottom;
    TPanel* panelTop;
    TButton* bnScanHardware;
    TButton* bnRecommendSettings;
    TButton* bnSaveSettings;
    TButton* bnCancel;
    TGroupBox* gbGraphics;
    TLabel* Label1;
    TComboBox* cbResolution;
    TLabel* Label2;
    TComboBox* cbRefreshRate;
    TLabel* Label4;
    TComboBox* cbTexturePack;
    TCheckBox* chkEnableVSync;
    TGroupBox* gbMisc;
    TGroupBox* gbTrouble;
    TCheckBox* chkDisableMovies;
    TCheckBox* chkDisableSound;
    TCheckBox* chkEnableHardwareMouse;
    TCheckBox* chkForceSoftwareSound;

    void __fastcall bnCancelClick(System::TObject* Sender);
    void __fastcall bnRecommendSettingsClick(System::TObject* Sender);
    void __fastcall bnSaveSettingsClick(System::TObject* Sender);
    void __fastcall bnScanHardwareClick(System::TObject* Sender);
    void __fastcall ActItemChange(System::TObject* Sender);
    void __fastcall FormPaint(System::TObject* Sender);
    void __fastcall FormCloseQuery(System::TObject* Sender, bool& CanClose);

private:
    TSysScanData Data;

public:
    __fastcall TTFrmMain(Classes::TComponent* Owner);

    int Settings[17];

    int Unused;

private:
    bool Recommend;
    bool HardwareScanned;
    bool FatalError;
    bool InhibitChanges;
    System::AnsiString ErrorCaption;
    System::AnsiString ErrorText;
    System::AnsiString Low;
    System::AnsiString Medium;
    System::AnsiString High;
    System::AnsiString None;
    System::AnsiString TwoTimes;
    System::AnsiString FourTimes;

    void __fastcall FillResolutions();
    void __fastcall UpdateRefreshRates();
    void __fastcall FillTexturePacks();
    void __fastcall ApplyToUI();
    void __fastcall ReadSettingsFromUI();
    void __fastcall LoadLocalization();
    void __fastcall LoadSettings();
    bool __fastcall SaveSettings();
    void __fastcall RunScan();
    void __fastcall SetBusy(bool Enable);
    void __fastcall SetControlEnabled(Controls::TControl* Control,
                           bool Enable);
    void __fastcall LayoutForm();
    void __fastcall MeasureBox(Controls::TWinControl* Box, int M[4]);
    void __fastcall LayoutBox(Controls::TControl* Box, int M[4], int Left,
                   int Width);
    void __fastcall ClearFirstRunFlag();
    void __fastcall CheckSoftShadows();
};

extern PACKAGE TTFrmMain* TFrmMain;

#endif
