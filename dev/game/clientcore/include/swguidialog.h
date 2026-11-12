#ifndef __SWGUIDIALOG_H__
#define __SWGUIDIALOG_H__

#include "swguiabilitieschargen.h"
#include "swguiingamecharacter.h"

class CExoSoundSource;

class CSWGuiDialog : public CSWGuiPanel
{
public:

    enum DialogInputEvent
    {
        GUIEVENT_REPLY_1 = 254,
        GUIEVENT_REPLY_2 = 255,
        GUIEVENT_REPLY_3 = 256,
        GUIEVENT_REPLY_4 = 257,
        GUIEVENT_REPLY_5 = 258,
        GUIEVENT_REPLY_6 = 259,
        GUIEVENT_REPLY_7 = 260,
        GUIEVENT_REPLY_8 = 261,
        GUIEVENT_REPLY_9 = 262,
        GUIEVENT_DIALOG_SKIP = 505
    };

    enum { MAX_REPLIES = 30, NUMBERED_REPLIES = 9 };
    CSWGuiDialog(CSWGuiManager* pManager);

    virtual void HandleInputEvent(int nEvent, int nState);
    virtual int SelectReply(int nReply);
    virtual void Reset();
    virtual void SetReplies(int nCount, CExoString* pReplies,
                            unsigned long* pReplyIndices, int nStartIndex,
                            int bUnknown);
    virtual unsigned long SetOwner(unsigned long nOwner)  // PC 0x0070E380
    {
        m_nOwner = nOwner;
        return nOwner;
    }
    virtual void SetDialogMessage(const CExoString& sMessage);
    virtual void ShowDialogMessage();
    virtual int HandleSelectReply(CSWGuiControl* pControl);

    void EndDialog();
    void SetReplyActive(CSWGuiControl* pControl);
    void SetReplyInactive(CSWGuiControl* pControl);
    int GetReplyIndex(int nReply);

    unsigned long m_nReplyContext;
    int m_nSelectedReply;
    int m_nReplyCount;
#ifdef _XBOX
    int m_bDialogActive;
    int m_bDialogWaiting;
#endif
    CSWGuiLabel m_lblReply[MAX_REPLIES];
    unsigned long m_aReplyIndex[MAX_REPLIES];
    unsigned long m_nOwner;
    CSWGuiListBox m_lbReplies;
    CSWGuiLabel m_lblMessage;
    Vector m_vReplyColor;
    Vector m_vReplyHighlightColor;
    CExoString m_sMessage;
#ifdef _XBOX
    int m_bDialogEnded;
#else
    unsigned long m_bDialogActive : 1;
    unsigned long m_bDialogWaiting : 1;
    unsigned long m_bDialogEnded : 1;
    unsigned long m_nDialogFlagsUnused : 29;
#endif
};

struct CSWGuiZeroExtent : public CSWGuiExtent
{
    CSWGuiZeroExtent()
    {
        m_nHeight = 0;
        m_nWidth = 0;
        m_nTop = 0;
        m_nLeft = 0;
    }
};

class CSWGuiDialogLetterbox : public CSWGuiPanel
{
public:

    enum Position
    {
        POSITION_TOP = 1,
        POSITION_BOTTOM = 2,
        POSITION_MIDDLE = 3
    };

    CSWGuiDialogLetterbox(CSWGuiManager* pManager);

    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);

    int IsFadeDone();
    void SetExtentSpecial(CSWGuiExtent cExtent);
    void SetTop();
    void SetBottom();
    CSWGuiManager* SetMiddle();
    void ResetFade();
    void SetFade(int bFadeIn, float fDelay, float fLength,
                 const Vector& vColor);

    CSWGuiZeroExtent m_cTarget;
    CSWGuiZeroExtent m_cStart;
    int m_nPosition;
    int m_bDone;
    CExoString m_sUnused;
    float m_fFadeDelay;
    float m_fFadeLength;
    float m_fFadeTime;
    int m_bFadeIn;
};

class CSWGuiDialogTop : public CSWGuiPanel
{
public:
    CSWGuiDialogTop(CSWGuiManager* pManager, const CResRef& cFont,
                    const Vector& vColor);

    void SetReply(const CExoString& sReply);

    CSWGuiLabel m_lblReply;
};

class CSWGuiDialogCinematic : public CSWGuiDialog
{
public:
    CSWGuiDialogCinematic(CSWGuiManager* pManager);
    virtual ~CSWGuiDialogCinematic();

    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual int SelectReply(int nReply);
    virtual void Reset();
    virtual void SetReplies(int nCount, CExoString* pReplies,
                            unsigned long* pReplyIndices, int nStartIndex,
                            int bUnknown);
    virtual void ShowDialogMessage();

    CSWGuiDialogTop* m_pTop;
};

class CSWGuiDialogComputerInfo
{
public:
    CSWGuiLabel m_lblName;
    CSWGuiLabel m_lblValue;
};

class CSWGuiDialogComputer : public CSWGuiDialog
{
public:

    enum InfoIndex
    {
        INFO_COMP_SKILL = 0,
        INFO_REP_SKILL = 1,
        INFO_COMP_SPIKES = 2,
        INFO_REP_UNITS = 3
    };

    CSWGuiDialogComputer(CSWGuiManager* pManager);
    virtual ~CSWGuiDialogComputer();

    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();
    virtual void SetReplies(int nCount, CExoString* pReplies,
                            unsigned long* pReplyIndices, int nStartIndex,
                            int bUnknown);
    virtual void SetDialogMessage(const CExoString& sMessage);
    SWGUI_PCVIRTUAL CExoString GetFullScreenBG();

    void UpdateSkills();
    void SetType(unsigned char nType);

    CSWGuiDialogComputerInfo m_aInfo[4];
    CSWGuiListBox m_lbMessage;
    CSWGuiLabel m_lblBar[10];
    CExoSoundSource* m_pSound;
};

class CSWGuiDialogComputerCamera : public CSWGuiDialog
{
public:
    CSWGuiDialogComputerCamera(CSWGuiManager* pManager);
    virtual ~CSWGuiDialogComputerCamera();
    void SetLiveFeedMessageVisible(int bVisible);

    CSWGuiLabel m_lblCameraTitle;
    CSWGuiBlackenedLabel m_lblReturn;
    CSWGuiDialogTop* m_pTop;
};

#endif
