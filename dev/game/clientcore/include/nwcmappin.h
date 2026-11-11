#ifndef __NWCMAPPIN_H__
#define __NWCMAPPIN_H__

#include "nwcobject.h"

class CSWCMapPin : public CSWCObject
{
public:

    enum PinType
    {
        PIN_TYPE_OBJECT_ID = 0,
        PIN_TYPE_REFERENCE_NUMBER = 1
    };

    CSWCMapPin();
    virtual ~CSWCMapPin();
    // PC 0x004015A0
    virtual void AIUpdate()
    {
    }
    // PC 0x0066B360
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags)
    {
        return 0;
    }
    // PC 0x0060E760
    virtual void UnloadModel(unsigned char nModel)
    {
    }
    // PC 0x004015A0
    virtual void UnloadModel()
    {
    }

    int m_bEnabled;
    CExoString m_sText;
    unsigned long m_nReference;
    int m_nType;
};

#endif
