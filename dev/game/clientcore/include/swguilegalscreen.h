#ifndef __SWGUILEGALSCREEN_H__
#define __SWGUILEGALSCREEN_H__

#include "swguiabilitieschargen.h"

class CSWGuiLegalScreen : public CSWGuiPanel
{
public:
    CSWGuiLegalScreen(CSWGuiManager* pManager, unsigned long nMinShowTime);

    void SetImage(const CResRef& cImage);

    unsigned long m_nShowTime;
    unsigned long m_nMinShowTime;
};

class CSWDemoScreen : public CSWGuiPanel
{
public:
    CSWDemoScreen(CSWGuiManager* pManager, unsigned long nMinShowTime, unsigned long nStrRef, unsigned long nLeft,
                  unsigned long nTop);

    virtual void Update(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);

    void SetImage(const CResRef& cImage);

    unsigned long m_nShowTime;
    unsigned long m_nMinShowTime;
    int m_n70;
    CSWGuiLabel m_lblDemoText;
    int m_bShowing;
};

#endif
