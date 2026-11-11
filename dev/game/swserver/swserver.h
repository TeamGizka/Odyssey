#ifndef __SWSERVER_H__
#define __SWSERVER_H__

#include "exobase.h"

enum
{
    SWSERVER_INPUT_LINE_SIZE = 1024,
    SWSERVER_MODE_IDLE = 0,
    SWSERVER_MODE_LOADING = 1,
    SWSERVER_MODE_RUNNING = 2,
    SWSERVER_PROTOCOL_UDP = 1,
    SWSERVER_CHAT_SERVER_SHOUT = 5,
    SWSERVER_STRREF_BOOTED = 5928
};
const unsigned long SWSERVER_INVALID_OBJECT_ID = 0x7f000000;

unsigned int __stdcall Input(void* pData);
int GetInstanceNumber();
void ReleaseInstanceNumber();
void ProcessCommandLine(int argc, char** argv, int& bRunModule);
void ProcessKeyboardInput();
int AdminLoadModule(char* szModule);
int AdminLoadGame(int nSlot);
int AdminSaveGame(int nSlot, CExoString sName, int bForce);
void AdminStatus();
void AdminKick(CExoString sPlayer);
void AdminClientInfo(CExoString sPlayer);
void AdminSay(CExoString sMessage);
void SignalHandler(int nSignal);
void InitGameApp();
void PrintGameInfo();
void AdminSetPort(unsigned long nPort);

#endif
