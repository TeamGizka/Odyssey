#ifndef __NWSDIALOG_H__
#define __NWSDIALOG_H__

class CExoLinkedListInternal;
class CExoLocString;
class CResGFF;
class CResStruct;
#include "exobase.h"
#include "gameobjectarray.h"
#include "exoresman.h"
#include "math3d.h"

class CSWSObject;
class CSWSDialogPlayer
{
public:

    unsigned long m_nPlayerId;
    unsigned long m_oidPlayer;
    unsigned char m_nGender;
};
class CSWSDialogEntry;
class CSWSDialogReply;

class CSWSDialogAnimation
{
public:
    CSWSDialogAnimation();

    CExoString m_sParticipant;
    unsigned short m_nAnimation;
};

class CSWSDialogActiveEntry
{
public:
    CSWSDialogActiveEntry() : m_nIndex(0) {}

    CResRef m_cActive;
    unsigned long m_nIndex;
};

class CSWSDialogLinkEntry
{
public:
    CSWSDialogLinkEntry();

    enum { LINK_INDEX_NONE = 0 };

    CResRef m_cActive;
    unsigned long m_nIndex;
    int m_bDisplayInactive;
};

class CSWSDialogBase
{
public:

    enum { PLOT_INDEX_INVALID = -1 };

    enum WaitFlags { WAIT_FOR_SOUND = 2, WAIT_EXPLICIT_DELAY = 0x10 };
    enum { SOUND_EXISTS_UNCHECKED = 0x80 };
    enum FadeType { FADE_TYPE_NONE = 0, FADE_TYPE_IN = 1, FADE_TYPE_OUT = 2 };
    enum { DELAY_DEFAULT = -1 };

    CSWSDialogBase();
    ~CSWSDialogBase();
    CSWSDialogBase& operator=(const CSWSDialogBase& dialogBase);

    enum SoundExistsFlags { SOUND_EXISTS_PRESENT = 1, SOUND_EXISTS_SKIP = 2 };

    CExoString m_sSpeaker;
    CExoLocString m_lsText;
    CResRef m_cScript;
    int m_nActionParam1;
    int m_nActionParam2;
    int m_nActionParam3;
    int m_nActionParam4;
    int m_nActionParam5;
    CResRef m_cScript2;
    int m_nActionParam1b;
    int m_nActionParam2b;
    int m_nActionParam3b;
    int m_nActionParam4b;
    int m_nActionParam5b;
    CExoString m_sActionParamStrA;
    CExoString m_sActionParamStrB;
    int m_bNodeUnskippable;
    unsigned long m_nWaitFlags;
    int m_nDelay;
    int m_nSoundDelay;
    int m_bSoundActive;
    int m_nSoundIndex;
    CResRef m_cSoundResRef;
    CExoString m_sAnimation;
    CExoString m_sQuest;
    unsigned long m_nQuestEntry;
    int m_nPlotIndex;
    float m_fPlotXPPercentage;
    int m_nAnimationCount;
    CSWSDialogAnimation* m_pAnimations;
    unsigned char m_nFadeType;
    char m_aUnmapped69[3];
    Vector m_vFadeColor;
    float m_fFadeLength;
    float m_fFadeDelay;
    unsigned char m_bSoundExists;
    int m_nAlienRaceNode;
    int m_nEmotion;
    int m_nFacialAnim;
};

class CSWSDialogCamera
{
public:

    enum { CAMERA_ANIMATION_NONE = 0x7FFF };

    CSWSDialogCamera()
        : m_nCameraAngle(0),
          m_nCameraId(0),
          m_fCameraHeightOffset(0.0f),
          m_fTargetHeightOffset(0.0f),
          m_nCameraAnimation(CAMERA_ANIMATION_NONE),
          m_fCameraFieldOfView(-1.0f),
          m_nCameraVideoEffect(-1)
    {
    }

    CExoString m_sListener;
    int m_nCameraAngle;
    int m_nCameraId;
    float m_fCameraHeightOffset;
    float m_fTargetHeightOffset;
    short m_nCameraAnimation;
    float m_fCameraFieldOfView;
    int m_nCameraVideoEffect;
    int m_n24;
};

class CSWSDialogNode : public CSWSDialogBase, public CSWSDialogCamera
{
public:
    CSWSDialogNode();
    ~CSWSDialogNode();
    CSWSDialogNode& operator=(const CSWSDialogNode& dialogNode);

};

class CSWSDialogSpeaker
{
public:
    CSWSDialogSpeaker();

    unsigned long m_oidSpeaker;
    CExoString m_sSpeaker;
};

class CSWSDialogStuntModel
{
public:
    CSWSDialogStuntModel();

    CExoString m_sParticipant;
    CResRef m_cStuntModel;
};

class CSWSDialog
{
public:

    enum { PSEUDO_RANDOM_CAMERA_COUNT = 41 };
    enum DialogCameraAngle
    {
        CAMERA_ANGLE_FIRST = 1,
        CAMERA_ANGLE_SECOND = 2,
        CAMERA_ANGLE_THIRD = 3
    };
    enum { DIALOG_INDEX_INVALID = -1 };

    enum { DIALOG_RESULT_READY = 0, DIALOG_RESULT_WAIT = 10 };
    enum { ANIMATION_RETRY_DELAY = 1500 };

    CSWSDialog();
    virtual ~CSWSDialog();

    int HandleReply(unsigned long nEntry, CSWSObject* pOwner,
                    unsigned long nReply, int bEscape, unsigned long oidSpeaker,
                    int bSkip);
    int SendDialogReplies(CSWSObject* pOwner, unsigned long nPlayerId);
    int SendDialogEntry(CSWSObject* pOwner, unsigned long nReply,
                        unsigned long nEntry, unsigned long& nResult);
    void ClearDialogOwnerInObject(unsigned long oidObject);
    void AddJournalEntry(const CExoString& sPlotID, unsigned long nState,
                         unsigned long nPlayerId);
    int RemovePlayer(unsigned long oidPlayer);

    void AddPlayer(CSWSDialogPlayer* pPlayer);
    void RunEndConversationScript(CSWSObject* pOwner, int bAborted);
    void Cleanup();
    int SetDialogOwner(CSWSObject* pOwner);
    unsigned long GetCurrentPlayerId();
    unsigned long GetDialogModel(int nStunt, int& bNonDependant);

    int GetStartEntry(CSWSObject* pOwner);
    CSWSObject* GetSpeaker(CSWSObject* pOwner, const CExoString& sSpeaker);
    void BarkOneLine(int nEntry);
    int CheckScript(CSWSObject* pObject, const CResRef& cScript);
    int GetStartEntryOneLiner(CSWSObject* pOwner, CExoLocString& lsText,
                             CResRef& cSound, CResRef& cScript);
    void GiveDialogPlotXP(int nPlotIndex, float fPercent);
    void LoadDialogBase(CSWSDialogBase& dialogBase, CResGFF* pGFF,
                       CResStruct& resStruct, unsigned long& nDelay,
                       int& bSuccess);
    void LoadDialogCamera(CSWSDialogCamera& camera, CResGFF* pGFF,
                          CResStruct& resStruct, int& bSuccess);
    void LoadDialogLinkedNode(CSWSDialogLinkEntry& link, CResGFF* pGFF,
                              CResStruct& resStruct, int& bSuccess,
                              unsigned long nReplies);
    int LoadDialog(CResGFF* pGFF, int bLoadReplies);
    void ClearAllPlayerHostileActions();
    void RunScript(CSWSObject* pObject, const CResRef& cScript);
    void InitializePsuedoRandomCameraAngles(CResGFF* pResGFF);
    int SendDialogModels(CSWSObject* pOwner, unsigned long oidPlayer);
    int SendDialogEntryNode(unsigned long nPlayerId, unsigned long oidTarget,
                            unsigned long oidSpeaker, unsigned long oidListener,
                            unsigned long oidCamera, unsigned char nGender,
                            CSWSObject* pOwner, CSWSDialogEntry& entry,
                            unsigned long& nResult);
    CExoString GetExoStringFromLocString(CExoLocString& locString,
                                         unsigned char nGender);
    int SendDialogReplyNode(unsigned long oidPlayer, unsigned long oidTarget,
                            unsigned char nGender, CSWSDialogReply* pReplies,
                            unsigned long nReplyCount,
                            unsigned long nReplyOffset, unsigned long nEntry,
                            int bShowAll);
    void ReEquipHItemForDialog();
    void ReEquipItemsForDialog();
    void UnequipItemsForDialog(unsigned long oidObject);
    void UnequipHItemForDialog(unsigned long oidObject);
    float SetDialogDelay(CSWSObject* pObject, CExoLocString& lsText,
                         unsigned long nDelay, int bSound, unsigned long nFlag);

    int m_nEntries;
    CSWSDialogEntry* m_pEntries;
    int m_nReplies;
    CSWSDialogReply* m_pReplies;
    int m_nStartingEntries;
    CSWSDialogActiveEntry* m_pStartingEntries;
    CResRef m_cEndConversationScript;
    CResRef m_cEndConversationAbortedScript;
    int m_n3C;
    int m_n40;
    int m_n44;
    int m_nStunts;
    CSWSDialogStuntModel* m_pStunts;
    CResRef m_cCameraModel;
    int m_n60;
    int m_n64;
    CExoLinkedList<CSWSDialogPlayer> m_lstPlayers;

    int m_bRepliesSent;
    int m_bEndOfDialog;

    int m_nCurrentEntry;
    int m_nSelectedReply;
    char m_nCurrentPlayerGender;
    char m_aUnmapped79[0x3];
    unsigned long m_oidCurrentPlayer;
    unsigned long m_oidDialogOwner;
    CSWSDialogSpeaker* m_pSpeakers;
    unsigned long m_nSpeakers;

    int m_bMultiPCDialogViewOnly;
    int m_n90;

    unsigned long m_oidLastEntrySpeaker;
    int m_n98;
    int m_nPseudoRandomCameraAngle;
    int m_nPseudoRandomCameraStep;
    int m_bPseudoRandomCameraDirection;
    int m_nPseudoRandomCameraAngles[PSEUDO_RANDOM_CAMERA_COUNT];
    int m_nF4;
    int m_nF8;
    CResRef m_cAmbientTrack;
    int m_n10C;
    int m_n110;
    int m_n170;
    int m_n174;
    int m_n178;
    CExoLinkedList<unsigned long> m_lstUnequippedItems;
    CExoLinkedList<unsigned long> m_lstUnequippedHeadItems;
    int m_nOwnerAILevel;

    unsigned long m_nAnimationRetryDelay;
    unsigned long m_nLastEntryDay;
    unsigned long m_nLastEntryTime;
};

class CSWSDialogEntry : public CSWSDialogNode
{
public:
    CSWSDialogEntry();

    int m_nReplies;
    CSWSDialogLinkEntry* m_pReplies;
};

class CSWSDialogReply : public CSWSDialogNode
{
public:
    CSWSDialogReply();

    int m_nEntries;
    CSWSDialogActiveEntry* m_pEntries;
    int m_nReplyIndex;
};

#endif
