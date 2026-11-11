#ifndef __SWGUIEXAMINE_H__
#define __SWGUIEXAMINE_H__

#include "swgui.h"
class CSWGuiManager;

class CSWGuiExamine : public CSWGuiMessageBox
{
public:

    enum { STRREF_ACCEPT_BUTTON = 0x62E };

    int m_nReturnInputMode;
    CSWGuiExamine(CSWGuiManager*);
    virtual void HandleInputEvent(int, int);
};

#endif
