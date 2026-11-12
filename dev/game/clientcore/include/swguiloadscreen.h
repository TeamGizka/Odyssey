#ifndef __SWGUILOADSCREEN_H__
#define __SWGUILOADSCREEN_H__

#include "swguiabilitieschargen.h"

class CSWGuiLoadScreen : public CSWGuiPanel
{
public:
    CSWGuiLoadScreen(CSWGuiManager* pManager);

#ifndef _XBOX
    virtual void Draw(float fElapsed);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();
#endif

    void SetImage(const CResRef& cImage);

    enum
    {
        STRREF_LOADING = 42493,
        STRREF_SAVING = 42528
    };

    void SetSaving(int bSaving)
    {
        m_lblLoading.m_cText.m_cParams.SetStrRef(
            bSaving ? STRREF_SAVING : STRREF_LOADING);
    }

    void SetHint(unsigned long nStrRef);
    void AddHintToMessageBuffer();

    CSWGuiProgressBar m_pbProgress;
    CSWGuiLabel m_lblHint;
    CSWGuiLabel m_lblLogo;
    CSWGuiLabel m_lblLoading;
    unsigned long m_nHintStrRef;
#ifndef _XBOX
    CResRef m_res5A0;
    CSWGuiLabel m_lblBackground;
#endif
};

#endif
