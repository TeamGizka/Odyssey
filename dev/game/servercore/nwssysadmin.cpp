#include "nwssysadmin.h"
// PC 0x005E1CB0
CSWSSysAdmin::CSWSSysAdmin(unsigned long nPlayerId)
    : CSWSClient(nPlayerId)
{
}
// PC 0x005E1CD0
CSWSSysAdmin::~CSWSSysAdmin()
{
}
// PC 0x00698BD0
inline CSWSSysAdmin* CSWSSysAdmin::AsSWSSysAdmin()
{
    return this;
}
