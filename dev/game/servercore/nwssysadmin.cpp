#include "nwssysadmin.h"
// PC 0x005E1CB0 GOG 0x005E1CE0 Xbox 0x0028DBD0
CSWSSysAdmin::CSWSSysAdmin(unsigned long nPlayerId)
    : CSWSClient(nPlayerId)
{
}
// PC 0x005E1CD0 GOG 0x005E1D00
CSWSSysAdmin::~CSWSSysAdmin()
{
}
// PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
inline CSWSSysAdmin* CSWSSysAdmin::AsSWSSysAdmin()
{
    return this;
}
