#ifndef __NWREENTRANTSERVER_H__
#define __NWREENTRANTSERVER_H__

#include "exobase.h"

class CSWReentrantServerStats
{
public:

    enum Operation
    {
        OPERATION_LOAD_MODULE = 1,
        OPERATION_SAVE_GAME = 2,
        OPERATION_LOAD_GAME = 3
    };
    // PC 0x004E9ED0 GOG 0x004E9B00
    void StartStallEvent(unsigned long nEvent)
    {
        m_bOperationActive = 1;
        m_nOperation = nEvent;
        m_bOperationStartPending = 1;
        m_nUnknown56 = 0;
    }

    CSWReentrantServerStats();

    ~CSWReentrantServerStats();

    int m_bOperationActive;
    int m_nOperation;
    int m_nProgress;
    int m_nProgressTotal;
    int m_bOperationStartPending;
    int m_bOperationEndPending;
    CExoString m_sUnknown24;
    CExoString m_sUnknown32;
    CExoString m_sUnknown40;

    unsigned long m_nResourceType;
    unsigned char m_aUnmapped[4];
    int m_nUnknown56;
};

#endif
