#include "nwssysadmin.h"
// PC 0x005802F0
CSWSSysAdmin::CSWSSysAdmin(unsigned long nPlayerId)
    : CSWSClient(nPlayerId)
{
}
// PC 0x00580310
CSWSSysAdmin::~CSWSSysAdmin()
{
}
// PC 0x00641DB0
inline CSWSSysAdmin* CSWSSysAdmin::AsSWSSysAdmin()
{
    return this;
}
