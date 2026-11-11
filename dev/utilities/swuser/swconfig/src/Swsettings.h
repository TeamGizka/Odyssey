#ifndef SwsettingsH
#define SwsettingsH

#include <System.hpp>
#include <Sysutils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <IniFiles.hpp>

#include "Getprocessorspeed.h"
#include "frmmain.h"
#include "Swutil.h"

bool __fastcall LocalizeTree(Controls::TWinControl* Root,
                  const System::AnsiString& Section, Inifiles::TIniFile* Ini,
                  CTalkTable* Tlk);

bool __fastcall LocalizeControl(Controls::TControl* Control,
                     const System::AnsiString& Section, Inifiles::TIniFile* Ini,
                     CTalkTable* Tlk);

int __fastcall ShowLocalizedMessageBox(const System::AnsiString Section,
                            const System::AnsiString TextIdent,
                            const System::AnsiString CaptionIdent,
                            const int& TextStrRef, const int& CaptionStrRef,
                            const unsigned& Flags);

void ParseVersionDriver(TVersion* Result, const System::AnsiString& Value);

bool __fastcall ReadSettings(int* Settings);

bool __fastcall WriteSettings(int* Settings);

#endif
