#ifndef __NWSSYSADMIN_H__
#define __NWSSYSADMIN_H__

#include "nwsclient.h"

class CSWSSysAdmin : public CSWSClient
{
public:
    virtual ~CSWSSysAdmin();
    CSWSSysAdmin(unsigned long nPlayerId);
    virtual CSWSSysAdmin* AsSWSSysAdmin();
};

#endif
