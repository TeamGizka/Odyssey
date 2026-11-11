#ifndef __SWGUILEGALSCREEN_H__
#define __SWGUILEGALSCREEN_H__

#include "swguiabilitieschargen.h"

class CSWGuiLegalScreen : public CSWGuiPanel
{
public:
    CSWGuiLegalScreen(CSWGuiManager* pManager, unsigned long nMinShowTime);
    virtual ~CSWGuiLegalScreen();

    void SetImage(const CResRef& cImage);

    unsigned long m_nShowTime;
    unsigned long m_nMinShowTime;
};

#endif
