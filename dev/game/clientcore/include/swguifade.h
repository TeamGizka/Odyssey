#ifndef __SWGUIFADE_H__
#define __SWGUIFADE_H__

#include "swgui.h"
class CSWGuiManager;
class Vector;

class CSWGuiFade : public CSWGuiPanel
{
public:
    float m_fFadeStart;
    float m_fFadeLength;
    float m_fFadeElapsed;
    int m_bFadeIn;
    CSWGuiLabel m_lblMessage;
    unsigned long m_nFadeTime;
    int IsFadeDone();
    void SetFade(int, float, float, Vector const&);
    virtual void Draw(float);
#ifndef _XBOX
    virtual CSWGuiControl* HitCheckMouse(int, int);
#else
    int IsObscuring();
#endif
    void SetMsg(unsigned long);
    void ResetFade();
    CSWGuiFade(CSWGuiManager*);
};

#endif
