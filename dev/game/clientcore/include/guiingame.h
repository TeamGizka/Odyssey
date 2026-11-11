#ifndef __GUIINGAME_H__
#define __GUIINGAME_H__

#define GUI_DIALOG_CAMERA_LOOK_AHEAD_DISTANCE 5.0f

#define GUI_COMPUTER_CAMERA_DEFAULT_FOV 55.0f

#define SAVEGAME_UNNUMBERED ((unsigned long)-1)

const unsigned char GUI_STATUS_JOURNAL = 0;
const unsigned char GUI_STATUS_CREDITS = 1;
const unsigned char GUI_STATUS_XP = 2;
const unsigned char GUI_STATUS_STEALTH_XP = 3;
const unsigned char GUI_STATUS_DARK_SHIFT = 4;
const unsigned char GUI_STATUS_LIGHT_SHIFT = 5;
const unsigned char GUI_STATUS_NET_SHIFT = 6;
const unsigned char GUI_STATUS_ITEM_RECEIVED = 7;
const unsigned char GUI_STATUS_ITEM_LOST = 8;
const unsigned char GUI_STATUS_COUNT = 13;

const unsigned char GUI_STATUS_LEVEL_UP = 9;
const unsigned char GUI_STATUS_NEW_QUEST = 10;
const unsigned char GUI_STATUS_QUEST_COMPLETE = 11;

class CExoLinkedListInternal;
class CExoStreamingSoundSource;
class CResRef;
class CSWGuiBarkBubble;
class CSWGuiDialog;
class CSWGuiDialogComputer;
class CSWGuiDialogLetterbox;
class CSWGuiFade;
class CSWGuiControllerLossBox;
class CSWGuiCreateMedicalItem;
class CSWGuiDeathDisplay;
class CSWDemoScreen;
class CSWGuiCreateItemMenu;
class CSWGuiCreateItemSubMenu;
class CSWGuiContainer;
class CSWGuiExamine;
class CSWGuiLoadModuleDebugMenu;
class CSWGuiPowersFeatsSkillsDebugMenu;
class CSWGuiExtent;
class CSWGuiInGameCharacter;
class CSWGuiInGameAreaTransition;
class CSWGuiInGameMenu;
class CSWGuiInGameMessages;
class CSWGuiInGamePause;
class CSWGuiInGameSoloModeQuery;
class CSWGuiManager;
class CSWGuiMessageBox;
class CSWGuiPanel;
class CSWGuiPartySelection;
class CSWGuiSkillInfoBox;
class CSWGuiSwoopUpgrade;
class CSWGuiStore;
class CSWGuiStatusSummary;
class CSWGuiTutorialBox;
class CSWGuiUpgradeSelection;
class CStatusSummary;
class Quaternion;
class Vector;
class CSWGuiMainInterface;
#include "math3d.h"
#include "exobase.h"
#include "exoresman.h"

class CGuiInGameMessage
{
public:
    enum { COLOR_RED = 1 };
    CExoString m_sMessage;
    unsigned long m_nType;
    unsigned char m_nColor;

    CGuiInGameMessage& operator=(const CGuiInGameMessage& message)
    {
        m_sMessage = message.m_sMessage;
        m_nType = message.m_nType;
        m_nColor = message.m_nColor;
        return *this;
    }
};

class SFeedbackMessage
{
public:
    CExoString m_sMessage;
    unsigned long m_nType;
    unsigned char m_nColor;
};

class SDialogMessage
{
public:
    CExoString m_sMessage;
    CExoString m_sSpeaker;
};

class CGuiInGameDialogMessage
{
public:
    CExoString m_sMessage;
    CExoString m_sSpeaker;

    CGuiInGameDialogMessage& operator=(const CGuiInGameDialogMessage& message)
    {
        m_sMessage = message.m_sMessage;
        m_sSpeaker = message.m_sSpeaker;
        return *this;
    }
};

class CPlaceableCamera
{
public:
    int m_nPlaceableID;
    Vector m_vPosition;
    Quaternion m_qOrientation;

    float m_fPitch;
    float m_fHeight;
    float m_fFieldOfView;
    float m_fMicRange;
};

class CGuiInGame
{
public:
    CGuiInGame();
    ~CGuiInGame();
    void DoQuickLoad();
    void ClearMapDisplays();

    enum { MESSAGE_BUFFER_CAPACITY = 64 };

    enum { MAX_PLACEABLE_CAMERAS = 70 };

    enum { IN_GAME_SCREEN_COUNT = 8 };

    enum { MAX_DIALOG_SPANNING_TEXT = 8 };

    enum
    {
        SCREEN_NONE = -1,
        SCREEN_EQUIP = 0,
        SCREEN_INVENTORY = 1,
        SCREEN_CHARACTER = 2,
        SCREEN_ABILITIES = 3,
        SCREEN_MESSAGES = 4,
        SCREEN_JOURNAL = 5,
        SCREEN_MAP = 6,
        SCREEN_OPTIONS = 7
    };

    enum
    {
        GUISTATUS_SHOW_MAIN_INTERFACE = 1,
        GUISTATUS_DIALOG = 2,
        GUISTATUS_HIDE_MAIN_INTERFACE = 3,
        GUISTATUS_RESTORE_MAIN_INTERFACE = 4
    };

    enum { CAMERA_VIDEO_EFFECT_DEFAULT = -1, CAMERA_VIDEO_EFFECT_UNCHANGED = -2 };

    enum
    {
        GUISTATE_MAIN_INTERFACE = 1,
        GUISTATE_DIALOG = 2,
        GUISTATE_HIDDEN = 3
    };

    enum DialogSelection
    {
        DIALOG_SELECTION_CLOSE = -2
    };

    enum { DIALOG_REPLY_INDEX_NONE = -1 };

    enum DialogFade
    {
        DIALOG_FADE_NONE = 0,
        DIALOG_FADE_OUT = 1,
        DIALOG_FADE_OUT_ALTERNATE = 4
    };

    enum DialogShot
    {
        DIALOG_SHOT_CLOSEUP = 1,
        DIALOG_SHOT_KEEP = 5,
        DIALOG_SHOT_COMPUTER = 6
    };
    static Vector ENABLED_ICON_COLOR;
    static Vector COLOR_BLUE;
    static Vector COLOR_YELLOW;
    static Vector COLOR_CYAN;
    static Vector COLOR_RED;
    static Vector COLOR_GREEN;
    static Vector DISABLED_ICON_COLOR;
    static Vector COLOR_BLACK;
    static Vector COLOR_WHITE;
    static Vector COLOR_ORANGE;
    static Vector COLOR_PURPLE;
    static Vector DISABLED_TEXT;
    static Vector STANDARD_BLUE;
    static Vector COLOR_TRANSPARENT;

    int GetCanClick();
    void AddFloatyText(unsigned long oidObject, const CExoString& sText,
                      const Vector& vColor, float fDuration);
    int CopyQuickSaveGameToFutureGame(unsigned long nSaveGame,
                                    const CExoString& sSaveName);
    int UnpackQuickSaveGame(unsigned long nSaveGame,
                           const CExoString& sSaveName);
    int GetCanOrientInDialog(unsigned long oidObject);
    int GetCanHeadFollowInDialog(unsigned long oidObject);
    void ClearDialogAnimationData();
    void ClearLastDialogAnimationData();
    int CloseDialog();
#ifdef _XBOX
    void ClearDialogReplyText(); void SetGlobalUnpauseableState(int bUnpauseable); void PostDialogConversationText(const CExoString& sConversation); void PostDialogSoundText(const CExoString& sSound);
#endif
    int GetMiniMapVisible();
    int IsOnDialogNonDependantList(const CExoString& sName);
    void AddToDialogNonDependantList(const CExoString& sName);
    void ClearDialogNonDependantList();
    unsigned char GetInterfaceOffset();
    void HidePowersFeatsSkillsDebugMenu();
    int Initialize();
    void InitializeGlobal();
    int InitializeShotCamera();
    Vector GetDialogCameraPosition(int bLookAhead);
    unsigned long GetDialogCamera();
    int GetHoldWorldFadeInForDialog()
    {
        return m_bHoldWorldFadeInForDialog;
    }
    int SetAnimatedCamera(unsigned short nAnimation, float fFieldOfView);
    void ShutDownGlobal();
    void ShutDown();
    int TutorialReasonWillShowWindow(unsigned char nReason);

#ifdef _XBOX
    enum { TUTORIAL_WINDOW_COUNT = 42 };
#else
    enum { TUTORIAL_WINDOW_COUNT = 43 };
#endif
    int ShowTutorialWindow(unsigned char nReason, unsigned long oidInstigator,
                           unsigned long oidTarget, unsigned long nActionParam);
    int SetTutorialReason(unsigned char nReason);
    void SetMainInterfaceTarget(unsigned long oidTarget, int bFlag);
    void StartGlobalFade(int nFade, float fA, float fB, const Vector& vColor);
    void ShowBarkBubble(unsigned long oidObject, const CExoString& sText,
                        const CResRef& cSound, int nFlags);
    void RePopulateMainInterface();
#ifdef _XBOX
    void ChangeCharacter();
#else
    int ChangeCharacter();
#endif
    CExoString GetCameraAnimationName(unsigned short nAnimation);
    int IsLoopingDialogStuntAnimation(unsigned short nAnimation);
    int IsFireForgetDialogAnimation(unsigned short nAnimation);
    int IsLoopingDialogAnimation(unsigned short nAnimation);
    int IsOverlayDialogAnimation(unsigned short nAnimation);
    void SetDeferredFOVCallForAnimatedCutscenes(float fFieldOfView);
    int GetBarkBubbleVisible(int& nTop);
    int GetFadeOverridePanelVisible();
    void ShowPartySelection(const CExoString& sExitScript, int bAllowCancel,
                            int nForceNPC1, int nForceNPC2);
    void MarkNoClickEvent(unsigned long nMilliseconds);
    void SuppressStatusSummary(int nNumEntries);
    int CanPlayDialogAnimations(unsigned long oidObject);
    void GetStatusSummary(CStatusSummary& cSummary);
    void SetStatusSummary(const CStatusSummary& cSummary);
    int GetPendingStatusSummary();
    void ShowStatusSummary();
    int HideSWInGameGui(int bHide);
#ifdef _XBOX
    int HideSWInGameGui();
#endif
    void NextSWInGameGui();
    void PrevSWInGameGui();
    void ResetInterfaceFont();
    void ResetInterfaceForSize();
    void InitializeMap();
    void HideExamineBox();
    void HideStoreGui();
    void ShowStoreGui(unsigned long oidStore, unsigned long oidCreature);
    void HideUpgradeScreen();
    void RemoveGUIPopUps();
    void HideSoloMode();
    void HideSubItemCreateMenu();
    void HideLoadModuleDebugMenu();
    void ShowDebugMenu();
    void ShowSoloModeQuery(int nQuery);
    int ShowSWInGameGui(int nMenu);
#ifdef _XBOX
    int ShowSWInGameGui();
#endif
    void ShowContainerGui(unsigned long oidContainer, int nGuiType);
    void ShowExamineBox(unsigned long nStrRef, int nReturnInputClass);
    int SetLevelUpMode(int bMode);
    int InitializeStuntModelData(unsigned long nModels);
    int LoadStuntModelsForDialog();
    void UnloadStuntModelsForDialog();
    void SetConversationType(unsigned long nType, unsigned char nComputerType);
    void ClearRepliesData();
    int InitializeRepliesData(unsigned long nReplies);

    int HandleDialogReplies(
        unsigned long oidPlayer, unsigned long oidSpeaker,
        unsigned char nComputerType, CExoString* psText,
        unsigned long* pnValue1, unsigned long* pnValue2,
        unsigned long* pnValue3, int* pnValue4, float* pfValue5,
        float* pfValue6, unsigned long* pnValue7, unsigned short* pnValue8,
        float* pfValue9, int* pnValue10, unsigned char* pnValue11,
        Vector* pvValue12, float* pfValue13, float* pfValue14,
        unsigned long nEntries, unsigned long nReplies,
        unsigned long nReplyContext, unsigned long* pnAnimations,
        unsigned long** ppnAnimationIDs, unsigned short** ppnAnimationFlags,
        int bUnknown);

    int HandleDialogEntry(unsigned long nPlayerId, unsigned long oidTarget,
                          unsigned long oidSpeaker, unsigned long oidListener,
                          unsigned long oidCamera, unsigned char nGender,
                          const CExoString& sText, unsigned long nAnimations,
                          unsigned long* pAnimationObjects,
                          unsigned short* pAnimations, unsigned long nCameraAngle,
                          int nCameraId, float fCameraHeight, float fTargetHeight,
                          unsigned long oidCameraTarget, unsigned short nCameraAnimation,
                          float fFieldOfView, int nVideoEffect, unsigned char nFadeType,
                          const Vector& vFadeColor, float fFadeDelay,
                          float fFadeLength, int bSoundActive);
    void SetReplyData(unsigned long nReply, const CExoString& sText,
                      unsigned long nValue1, unsigned long nValue2,
                      unsigned long nValue3, int nValue4, float fValue5,
                      float fValue6, unsigned long nValue7,
                      unsigned short nValue8, float fValue9, int nValue10,
                      unsigned char nValue11, const Vector& vValue12,
                      float fValue13, float fValue14, unsigned long nAnimations,
                      unsigned long* pnAnimationIDs,
                      unsigned short* pnAnimationFlags);
    void SetDialogAnimationData(unsigned long nIndex, unsigned long nAnimation,
                                unsigned short nFlags);
    int StoreLastDialogAnimationData();
    int ResetDialogAnimations();
    void ResetTalkAnimations();
    int InitializeDialogAnimationData(unsigned long nAnimations);
    void HideContainerGui();
    void HideGalaxyMapGui();
    void ShowItemCreateMenu();
    void SetupPauseGuiExtent(CSWGuiExtent* pExtent);
    int IsPauseGUIVisible();

    enum { PAUSE_REASON_ACTION_SELECTED = 10 };
    void SetPaused(int bPaused, unsigned char nReason);
    void SetJournalDone(int bDone);

    int GetJournalDone()
    {
        return m_bJournalDone;
    }

    CSWGuiMessageBox* GetMessageBox()
    {
        return m_pMessageBox;
    }
    CSWGuiMainInterface* GetMainInterface();
#ifndef _XBOX
    int SetGlobalUnpauseableState(int bUnpauseable);
#endif
    int GetPartyAccessPanelUp(int bAny); int GetCurrentEquipScreenNPC();
    void UpdateStatus(unsigned char nMessage, int nUnknown); SFeedbackMessage* GetMessageBuffer();
    int GetMessageBufferSize();

    enum ChatMessageMask
    {
        MESSAGE_MASK_CHAT_TALK = 1,
        MESSAGE_MASK_CHAT_TELL = 2,
        MESSAGE_MASK_CHAT_SHOUT = 4,
        MESSAGE_MASK_CHAT_WHISPER = 8,
        MESSAGE_MASK_CHAT_PARTY = 64
    };

    static const unsigned long MESSAGE_MASK_HS_MODE = 0x80;

    static const unsigned long MESSAGE_MASK_DEBUG_INFO = 0x10000000;
    void AppendToMsgBuffer(const CExoString& sMessage, unsigned long nType,
                           unsigned char nColor);
    void AppendToDialogBuffer(const CExoString& sSpeaker,
                              const CExoString& sMessage);
    void AppendToDialogBuffer(const CExoString& sMessage,
                              unsigned long oidSpeaker);
    void SetCombatMessage(unsigned long nMessage);
    CSWGuiPanel* GetGuiInGameScreenPanel(unsigned char nScreen);
    void HideItemCreateMenu();
    void DestroyDeathMenuGUI();
    void ShowSubItemCreateMenu(unsigned long nSubType);
    void DialogGuiDoneScroll();
    CExoString DialogNodeSpanningTextParse(const CExoString& sText);
    void StopGlobalFade();
    int IsGlobalFading();
    int IsGlobalFadeObscuring();
    int IsCameraDialog();
    void ShowDialogMessage();
    void ShowDialogReplies(unsigned long nStartIndex, unsigned long nReplyContext);
    void UnlockDialogParticipantOrientations();
    void UpdateShotCamera(unsigned long nShot, int nCamera, float fPitch,
                          float fHeight, unsigned long oidTarget, int nVideoEffect);
    int IsInDialog(unsigned long oidObject);
    void EndDialogCamera();
#ifdef _XBOX

    void UpdateAreaTransitionGUI(unsigned long oidArea, const Vector& vPosition);
    unsigned long GetAreaTransitionObject();

#else
    void SetAreaTransition(unsigned long oidArea, const Vector& vPosition);
    void UpdateAreaTransitionGUI();
#endif
    void HideAreaTransition();
    Vector GetAreaTranstionPosition();
    int SetDialogSelection(int nReply, int nReplyCount, int bSelected);
    int InitializePlaceableCameras(int nCameras);
    void SetPlaceableCamera(int nCamera, int nPlaceableID,
                            const Vector& vPosition,
                            const Quaternion& qOrientation, float fPitch,
                            float fHeight, float fFieldOfView, float fMicRange);
    CPlaceableCamera* GetPlaceableCamera(int nCamera);

    enum { DIALOG_REPLY_ESCAPE = -3 };
    void UpdateDialog();
    void SetDialogAnimations();

    void HandleDialogSelection(int nReply, int bSelected);
    void UpdateDialogLipSync();
    void FadeAndStopDialogAmbientTrack();
    void LoadAmbientTrackForDialog(const CResRef& cAmbientTrack);
    void SwitchToSWInGameGui(int nMenu);
    void UpdateMessageGUI();
    void StopBarkBubble(int bFade);
    void UpdateCreatedInGameGUI(int nOldScreen, int nNewScreen);
    void ShowFlashingStatus(unsigned char nStatus);
    void EndControllerPauseState();
    void EnterControllerPauseState();
    void UpdateControllerPauseState();
    void SetGlobalDialogState(int nState);
    void SetDialogSound(unsigned long nSoundId, const CResRef& cSound);
    int HandleDialogSound(unsigned long oidSpeaker, const CResRef& cSound);
    void LoadCameraModelForDialog(const CResRef& cCameraModel);
    void SetStuntModelData(unsigned long nModel, unsigned long oidObject,
                           const CResRef& cModel);
    int HandleDialogModels(unsigned long oidPlayer, unsigned long nModels,
                           unsigned long* pObjectIds, CResRef* pModels,
                           const CResRef& cCameraModel, const CResRef& cAmbientTrack);
    void ShowDialogEntry(const CExoString& sMessage, unsigned long oidOwner,
                         unsigned long nFlags);
    void ResetComputerCameraShot();
    void CleanUpEntryNode();
    void CleanUpReplyNode();
    int HandleDialogReplyChosen(unsigned long oidSpeaker, unsigned long nReply,
                                unsigned long nReplyCount, const CExoString& sText,
                                unsigned long nSound, unsigned char nFlags,
                                int bSelected);
    void SetSWGuiStatus(int nStatus, int bActive);

    int m_bInitialized;
    unsigned char m_aUnknown04[4];

    CSWGuiInGameMenu* m_pInGameMenu;
    CSWGuiPanel* m_apScreenPanels[IN_GAME_SCREEN_COUNT];
    int m_nLastGuiPanel;

    int m_pPartySelection;

    int m_bShowPauseGui;
    CSWGuiManager* m_pGuiManager;
    CSWGuiDialog* m_pDialog;
    CSWGuiDialog* m_pCameraDialog;
    CSWGuiDialogComputer* m_pComputerDialog;

    CSWGuiDialog* m_pComputerScreen;
    CSWGuiBarkBubble* m_pBarkBubble;
    CSWGuiExamine* m_pExamineBox;
    CSWGuiContainer* m_pContainer;
    CSWGuiCreateItemMenu* m_pItemCreateMenu;
    CSWGuiCreateItemSubMenu* m_pSubItemCreateMenu;
    CSWGuiDialogLetterbox* m_pDialogTop;
    CSWGuiDialogLetterbox* m_pDialogLetterbox;
    CSWGuiDialogLetterbox* m_pDialogFade;
    CSWGuiFade* m_pGlobalFade;
    CSWGuiLoadModuleDebugMenu* m_pLoadModuleDebugMenu;
    CSWGuiPowersFeatsSkillsDebugMenu* m_pPowersFeatsSkillsDebugMenu;

    CSWGuiPartySelection* m_pPartySelectionPanel;
    CSWGuiInGamePause* m_pPausePanel;

    CSWGuiPanel* m_pGalaxyMap;

    CSWGuiStore* m_pStorePanel;

    CSWGuiUpgradeSelection* m_pUpgradeScreen;
    CSWGuiCreateMedicalItem* m_pChemicalUpgradeScreen;
    CSWGuiSwoopUpgrade* m_pSwoopUpgradeScreen;
    CSWGuiInGameSoloModeQuery* m_pSoloModeQuery;
    CSWGuiMainInterface* m_pMainInterface;
    CSWGuiInGameAreaTransition* m_pAreaTransition;
    CSWGuiMessageBox* m_pMessageBox;
    CSWGuiMessageBox* m_pA4;
    CSWGuiDeathDisplay* m_pDeathDisplay;
    CSWGuiSkillInfoBox* m_pSkillInfoBox;
    CSWGuiTutorialBox* m_pTutorialBox;
    CSWGuiControllerLossBox* m_pControllerLossBox;
    CSWGuiStatusSummary* m_pStatusSummary;
    int m_nBC;
    CSWDemoScreen* m_pDemoScreen;
    CStatusSummary* m_pPendingStatusSummary;
    int m_bDialogEntryShown;
    int m_nGlobalDialogState;
    unsigned char m_aUnknownB8[0xC0 - 0xB8];
    int m_nC0;
    int m_nDialogCameraType;
    int m_nDialogReply;
    int m_nDialogReplyCount;
    int m_bDialogReplySelected;
    int m_nDialogReplySelection;
    unsigned long m_nGlobalUnpauseableState;
    int m_bControllerPauseActive;
    int m_bPausedByCombatBeforeControllerPause;

    int m_bComputerCameraShot;
    Vector m_vComputerCameraShot;
    float m_fComputerCameraMicRange;
    CGuiInGameMessage* m_pMessageBuffer;
    CGuiInGameDialogMessage* m_pDialogBuffer;
    CGuiInGameMessage* m_pCombatBuffer;
    unsigned long m_MessageBufferSize;
    unsigned long m_nDialogBufferSize;
    unsigned long m_nCombatBufferSize;
    int m_bItemCreateMenuAvailable;
    int m_LevelUpMode;

    int m_bReplyPending;
    int m_nReplies;
    CExoString* m_asReplyText;
    unsigned long* m_pnReplyValue1;
    unsigned long* m_pnReplyValue2;
    unsigned long* m_pnReplyValue3;
    int* m_pnReplyValue4;
    float* m_pfReplyValue5;
    float* m_pfReplyValue6;
    unsigned long* m_pnReplyValue7;
    unsigned short* m_pnReplyValue8;
    float* m_pfReplyValue9;
    int* m_pnReplyValue10;
    unsigned char* m_pnReplyValue11;
    Vector* m_pvReplyValue12;
    float* m_pfReplyValue13;
    float* m_pfReplyValue14;

    int* m_pnReplyAnimations;
    unsigned long** m_ppnReplyAnimationIDs;
    unsigned short** m_ppnReplyAnimationFlags;

    unsigned long m_oidComputerCameraTarget;
    unsigned short m_nDialogCameraAnimation;
    unsigned char m_aUnknown166[2];

    int m_bDialogCameraAnimationActive;

    int m_bDialogSoundQueried;
    unsigned long m_oidDialogObject1;
    unsigned long m_oidDialogObject2;
    unsigned long m_oidPreviousDialogObject1;
    unsigned long m_oidPreviousDialogObject2;

    unsigned long m_oidDialogSoundObject;
    unsigned long m_oidDialogCamera;

    unsigned long m_oidCameraAnimationObject;
    unsigned long m_nStuntModels;
    unsigned long* m_pnStuntModelIDs;
    CResRef* m_pStuntModelResRefs;
    int m_nDialogAnimations;
    int m_nLastDialogAnimations;
    unsigned long* m_pDialogAnimations;
    unsigned short* m_pDialogAnimationFlags;
    unsigned long* m_pLastDialogAnimations;
    unsigned short* m_pLastDialogAnimationFlags;
    int m_nPlaceableCameras;
    CPlaceableCamera m_aPlaceableCameras[MAX_PLACEABLE_CAMERAS];

    int m_bPendingDialogBark;
    CExoString m_sPendingDialogBark;
    int m_bShotCameraInitialized;
    int m_bAnimatedCameraActive;
    int m_bAnimatedCameraLoaded;
    float m_fAnimatedCameraTime;
    unsigned char m_aUnknownB30[0xB34 - 0xB30];
    int m_nDialogCameraState;
    int m_bPaused;

#ifndef _XBOX
    int m_nPauseState;
#endif
    CExoStreamingSoundSource* m_pDialogSound;
    CExoLinkedList<unsigned long> m_lDialogNoOrientList;
    CExoLinkedList<unsigned long> m_lDialogNoHeadFollowList;
    CExoLinkedList<CExoString> m_lDialogNonDependantList;

    int m_nDialogSpanningTextRemaining;
    int m_nDialogSpanningTextCount;
    CExoString m_asDialogSpanningText[MAX_DIALOG_SPANNING_TEXT];
    int m_bHoldWorldFadeInForDialog;
#ifdef _XBOX

    int m_nXboxUnmappedB98;
    int m_nDialogReplyRow;
#else
    unsigned char m_aUnknownB9C[0xBA4 - 0xB9C];
#endif
    unsigned long m_oidPostDialogCharacterSwitch;
    unsigned char m_aTutorialWindowsShown[33];
    unsigned char m_aUnmappedFA9[7];
#ifdef _XBOX

    int m_bCreditSequenceInProgress;
    void StartCreditSequence(const Vector& vColor, const CExoString& sMusic);
#endif
    int m_nDialogScrollWait;
    int m_bDialogScrollFinished;

    float m_fDialogCameraPullBackRange;
    CExoStreamingSoundSource* m_pDialogAmbientTrack;
#ifdef _XBOX
    CExoStreamingSoundSource* m_pCreditsMusic;
#endif
    unsigned char m_nAbilityMode;
    unsigned char m_nInventoryFilter;
    unsigned char m_aUnknownBC2[2];
    unsigned long m_bJournalDone : 1;
    unsigned long m_nJournalFlagsUnused : 31;

    enum { MESSAGE_TYPE_DIALOG = 0, MESSAGE_TYPE_FEEDBACK = 1 };
    unsigned char m_nMessageType;
    unsigned char m_aUnknownBC9[0xBCC - 0xBC9];
    unsigned long m_nNoClickDay;
    unsigned long m_nNoClickTime;
    unsigned long m_nNoClickDuration;

    int m_bUsePartyCharacterForDialog;
    unsigned long m_nDialogSoundId;
    CResRef m_cDialogSound;

    unsigned long m_aDialogParticipants[3];
    int m_bDeferredFOVCall;
    float m_fDeferredFieldOfView;
    int m_bTransitionScriptRunning;
#ifdef _XBOX
    int m_nXboxUnmappedC0C;

    void SetXboxUnmappedC0C(int nValue) { m_nXboxUnmappedC0C = nValue; }
#endif
    int m_bInputActiveBeforeControllerPause;
#ifndef _XBOX
    Vector m_vAreaTransition;
    unsigned long m_oidAreaTransition;

    int m_bCameraRelativeAreaTransition;
    int m_bDanceDanceMalak;
    int m_n1024;
    int m_n1028;
    int m_n102C;
    CExoArrayList<CExoLocString> m_a1030;
    CExoArrayList<unsigned long> m_a103C;
    CExoArrayList<unsigned long> m_a1048;
#endif
    void ShowGalaxyMapGui(int nPlanet);
    int ShowLevelUpGUI(int nArg1);
    void ShowUpgradeScreen(unsigned long oidItem);
    void SetShotFromScript(int nCamera, int nVideoEffect);
    void SetComputerCameraShot(int nCamera, unsigned long oidTarget,
                               int nVideoEffect);
    void SetLockObjectOrientationInDialog(unsigned long oidObject, int bLock);
    void SetLockHeadFollowInDialog(unsigned long oidObject, int bLock);
    int IsAnimationPlayingInDialog();
    int IsFadingInDialog();
};

#include "swgui.h"
class CExoStreamingSoundSource;
class CResRef;
class CSWGuiExtent;
class CSWGuiManager;

class CSWGuiBarkBubble : public CSWGuiPanel
{
public:

    enum { BARK_DISTANCE_SQUARED = 36 };
    CSWGuiLabel m_lblBarkText;
    CSWGuiExtent m_cInitialExtent;
    float m_fTimeLeft;
    CExoStreamingSoundSource* m_pSoundSource;
    int m_nStartTime;
    unsigned long m_oidSpeaker;
    int m_bSoundStarted;
    unsigned int m_bForceVisible : 1;
    unsigned int : 31;
    CSWGuiBarkBubble(CSWGuiManager*);
    virtual ~CSWGuiBarkBubble();
    void SetBark(unsigned long, CExoString const&, CResRef const&, int);
    void Pause();
    void Resume();
    void StopBark(int);
    virtual void Draw(float);
    virtual void OnPanelAdded();
    virtual void SetExtent(const CSWGuiExtent&);
};

extern int g_bCreateAllGUIs;

#endif
