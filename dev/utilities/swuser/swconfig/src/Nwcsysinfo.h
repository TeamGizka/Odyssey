#ifndef NwcsysinfoH
#define NwcsysinfoH

#include <System.hpp>
#include <Sysutils.hpp>
#include <Classes.hpp>

void __fastcall GetOSVersion(System::AnsiString& Version, System::AnsiString& Name);

bool GetOSServicePack(System::AnsiString& ServicePack);

void GetSwapFileList(Classes::TStrings* List);

System::AnsiString __fastcall GetFileVersionString(System::AnsiString FileName,
                                                   System::AnsiString Key);

int GetCPUCount();

int GetCPURegistryMHz();

void GetCPUVendor(System::AnsiString& Vendor);

unsigned int GetCPUInfo(unsigned int Index);

void GetCPUDescription(System::AnsiString& Description);

int GetTotalMemory(System::AnsiString& Error);

void ScanHardDrives(Classes::TStrings* List);

bool GetCDROMLetters(Classes::TStrings* List);

void GetCDROMDriveNames(Classes::TStrings* List);

int GetGLDeviceCount();

bool GetGLDeviceInfo(int Index, System::AnsiString& Name, int* Error);

bool GetGLStrings(int Index, System::AnsiString& Vendor,
                  System::AnsiString& Renderer, int* Error);

void GetVideoCardName(System::AnsiString& Name);

int GetVideoMemory();
int GetVideoMemoryFallback();

bool GetDriverVersion(int Vendor, System::AnsiString& Version);

bool GetDisplayMode(int& Width, int& Height, int& BitsPerPixel);

bool GetRefreshRate(int& RefreshRate);

bool GetSoundCardName(System::AnsiString& Name);
int GetSoundCardNameAlt(System::AnsiString& Name);

void GetDirectXVersion(System::AnsiString& Version,
                       System::AnsiString& Friendly);

#endif
