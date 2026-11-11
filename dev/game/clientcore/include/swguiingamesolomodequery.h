#ifndef __SWGUIINGAMESOLOMODEQUERY_H__
#define __SWGUIINGAMESOLOMODEQUERY_H__

#include "swgui.h"
class CSWGuiManager;

class CSWGuiInGameSoloModeQuery : public CSWGuiMessageBox
{
public:
    CSWGuiInGameSoloModeQuery(CSWGuiManager* pManager);
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    void SetQueryText(int nQuery);

    CSWGuiLabel m_lblLine1;
    CSWGuiLabel m_lblLine2;
    CSWGuiLabel m_lblLine3;
    CSWGuiLabel m_lblLine4;
    CSWGuiLabel m_lblLine5;
    int m_nQuery;
};

#endif
