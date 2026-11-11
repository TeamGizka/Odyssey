#include <windows.h>
#include "exobase.h"
#include "exoinput.h"
#include "aurora.h"
#include "swgui.h"
#include "swminigame.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "guiingame.h"
#include "nwcobject.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "nwcitem.h"
#include "nwcarea.h"
#include "nwcmodule.h"
#include "nwcsoundobject.h"
#include "swcroom.h"
#include "swparty.h"
#include "swbehaviorcamera.h"
#include "swmain.h"

const int CExoSound::CEXOSOUND_MODE_GENERIC = 0;
const int CExoSound::CEXOSOUND_MODE_PAUSE_ALL = 3;
float DriveWalkThreshold = 0.6f;
int g_bDisableSound = 1;
int g_bEnableAurPostString = 0;
int g_bLeftMouseDown = 0;
int g_bLogConversations = 0;
int g_bMouseCameraRotateToggle = 0;
int g_bShiftActionState = 0;
int g_bShowIndicator = 0;
int g_nJournalSortType = 0;
int g_nSetMovieFrameRate = 0;
unsigned long LockRenderDev = 0;

void CClientExoApp::AddFloatyText(unsigned long, unsigned char, int)
{
}

int CClientExoApp::AddMoveToModuleMovie(CExoString const&)
{
    return 0;
}

int CClientExoApp::AddMovieToMovieQueue(CExoString const&, int)
{
    return 0;
}

void CClientExoApp::AttachPlayerGameObjectToArea()
{
}

void CClientExoApp::BarkString(unsigned long, CExoString const&, CResRef const&, int)
{
}

void CClientExoApp::ChangeCharacter(int, int)
{
}

int CClientExoApp::ChangeCharacterToNextLivingPartyMember(int, int, unsigned char)
{
    return 0;
}

void CClientExoApp::ClearEnemySighted()
{
}

void CClientExoApp::DisableInput()
{
}

void CClientExoApp::DisableVideoEffect()
{
}

void CClientExoApp::EnableVideoEffect(int)
{
}

CSWCArea* CClientExoApp::GetAreaByGameObjectID(unsigned long)
{
    return 0;
}

int CClientExoApp::GetAreaNotReadyToDisplay()
{
    return 0;
}

int CClientExoApp::GetAutoPaused()
{
    return 0;
}

int CClientExoApp::GetCanSendDialog(unsigned long, unsigned long, CSWSDialog*)
{
    return 0;
}

int CClientExoApp::GetClientLanguage()
{
    return 0;
}

int CClientExoApp::GetClientMeleeAnimation(unsigned long, unsigned long, unsigned short, int, int, int)
{
    return 0;
}

CClientOptions* CClientExoApp::GetClientOptions()
{
    static char s_null[sizeof(CClientOptions)];
    return (CClientOptions*)s_null;
}

int CClientExoApp::GetClientRangedAnimation(unsigned long, int, int, int)
{
    return 0;
}

CSWCCreature* CClientExoApp::GetCreatureByGameObjectID(unsigned long)
{
    return 0;
}

int CClientExoApp::GetCreditSequenceInProgress()
{
    return 0;
}

CExoString CClientExoApp::GetGUIString(unsigned long)
{
    return CExoString();
}

CSWCObject* CClientExoApp::GetGameObject(unsigned long)
{
    return 0;
}

CGameObjectArray* CClientExoApp::GetGameObjectArray()
{
    return 0;
}

int CClientExoApp::GetGameOver()
{
    return 0;
}

unsigned char CClientExoApp::GetGamePlayHint()
{
    return 0;
}

CGuiInGame* CClientExoApp::GetInGameGui()
{
    static char s_null[sizeof(CGuiInGame)];
    return (CGuiInGame*)s_null;
}

CSWCItem* CClientExoApp::GetItemByGameObjectID(unsigned long)
{
    return 0;
}

int CClientExoApp::GetLastTarget()
{
    return 0;
}

CExoString& CClientExoApp::GetLiveContentName(int)
{
    static CExoString s_null;
    return s_null;
}

CExoString CClientExoApp::GetLoadMusicByModuleName(CExoString const&)
{
    return CExoString();
}

unsigned char CClientExoApp::GetLoadStep(int)
{
    return 0;
}

int CClientExoApp::GetLoadingSaveGame()
{
    return 0;
}

CSWMiniGameObjectArray* CClientExoApp::GetMGOArray()
{
    return 0;
}

CSWMiniGame* CClientExoApp::GetMiniGame()
{
    return 0;
}

CSWCModule* CClientExoApp::GetModule()
{
    return 0;
}

int CClientExoApp::GetModuleResourceLoaded(unsigned char)
{
    return 0;
}

CExoArrayList<CExoString>* CClientExoApp::GetMoveToModuleMovies()
{
    return 0;
}

int CClientExoApp::GetNextLoadScreenHintSTRREF(CExoString const&, unsigned char)
{
    return 0;
}

int CClientExoApp::GetPausedByCombat()
{
    return 0;
}

int CClientExoApp::GetPazaakResults(int*, int*)
{
    return 0;
}

unsigned long CClientExoApp::GetPlayerCreatureId()
{
    return 0;
}

CSWGuiManager* CClientExoApp::GetSWGuiManager()
{
    return 0;
}

CSWParty* CClientExoApp::GetSWParty()
{
    static char s_null[sizeof(CSWParty)];
    return (CSWParty*)s_null;
}

CSWCSoundObject* CClientExoApp::GetSoundObjectByGameObjectID(unsigned long)
{
    return 0;
}

int CClientExoApp::GetSoundPausedByCombat()
{
    return 0;
}

unsigned char CClientExoApp::GetStoryHint()
{
    return 0;
}

CWorldTimer* CClientExoApp::GetWorldTimer()
{
    return 0;
}

void CClientExoApp::HideLoadScreen(int, int)
{
}

int CClientExoApp::IsMoviePlaying()
{
    return 0;
}

int CClientExoApp::IsSoundPlayingInDialog()
{
    return 0;
}

int CClientExoApp::PlayLoadModuleMovies()
{
    return 0;
}

int CClientExoApp::PlayMovie(CExoString const&, int&, int, int)
{
    return 0;
}

int CClientExoApp::PlayMovieQueue(int)
{
    return 0;
}

int CClientExoApp::PlayRumblePattern(int)
{
    return 0;
}

void CClientExoApp::ResetDriveAcceleration()
{
}

void CClientExoApp::ResetModuleResourcesLoaded()
{
}

void CClientExoApp::RunDeathSequence()
{
}

unsigned long CClientExoApp::ServerToClientObjectId(unsigned long)
{
    return 0;
}

void CClientExoApp::SetAutoPaused(int, unsigned char)
{
}

void CClientExoApp::SetCanSendDialog(int)
{
}

void CClientExoApp::SetDialogModelsSent(int)
{
}

void CClientExoApp::SetDialogSent(int)
{
}

void CClientExoApp::SetEndGamePending(int, float, int)
{
}

int CClientExoApp::SetGameOver(int)
{
    return 0;
}

void CClientExoApp::SetInputActive(int, int)
{
}

void CClientExoApp::SetInputClass(int, int)
{
}

void CClientExoApp::SetLastPartyMemberTempKilled(unsigned long)
{
}

void CClientExoApp::SetLastTarget(unsigned long, int)
{
}

void CClientExoApp::SetLoadBarProgress(int, int)
{
}

void CClientExoApp::SetLoadScreenByModuleName(CExoString const&)
{
}

void CClientExoApp::SetLoadScreenHint(unsigned long)
{
}

void CClientExoApp::SetLoadScreenHintUpdateTimer(unsigned long)
{
}

void CClientExoApp::SetLoadStep(int, unsigned char)
{
}

void CClientExoApp::SetModuleResourceLoaded(unsigned char)
{
}

void CClientExoApp::SetPauseState(unsigned char, int, CExoLinkedList<unsigned long>*)
{
}

void CClientExoApp::SetPausedByCombat(int, unsigned char, int)
{
}

int CClientExoApp::SetPlayerCreature(unsigned long)
{
    return 0;
}

void CClientExoApp::ShowLoadScreen(int, int, int)
{
}

int CClientExoApp::ShowTutorialWindow(unsigned char, unsigned long, unsigned long, unsigned long)
{
    return 0;
}

void CClientExoApp::StartCreditSequence(Vector const&, CExoString const&)
{
}

void CClientExoApp::StartPazaakGame(int, CExoString const&, int, int, unsigned long)
{
}

int CClientExoApp::StopRumblePattern(int)
{
    return 0;
}

void CClientExoApp::StopSoundPlayingInDialog()
{
}

void CClientExoAppInternal::UpdateRumble(float)
{
}

void CExoInput::ClearEvents()
{
}

void CExoInput::EndAttemptedBind()
{
}

void CExoInput::PauseRumble()
{
}

void CExoInput::SetActive(int)
{
}

void CExoInput::UnpauseRumble()
{
}

int CExoSound::Play3DOneShotSound(CResRef const&, Vector, float, unsigned char, unsigned long, unsigned char, float, float)
{
    return 0;
}

int CExoSound::PlayOneShotSound(CResRef const&, unsigned char, unsigned long, unsigned char, float, float)
{
    return 0;
}

void CExoSound::SetMusicVolume(float, int)
{
}

void CExoSound::SetSoundMode(int)
{
}

void CExoSound::StopAllOneShots()
{
}

void CGuiInGame::AddToDialogNonDependantList(CExoString const&)
{
}

void CGuiInGame::AppendToDialogBuffer(CExoString const&, CExoString const&)
{
}

void CGuiInGame::AppendToMsgBuffer(CExoString const&, unsigned long, unsigned char)
{
}

int CGuiInGame::CloseDialog()
{
    return 0;
}

int CGuiInGame::GetCurrentEquipScreenNPC()
{
    return 0;
}

SFeedbackMessage* CGuiInGame::GetMessageBuffer()
{
    return 0;
}

int CGuiInGame::GetMessageBufferSize()
{
    return 0;
}

int CGuiInGame::GetPartyAccessPanelUp(int)
{
    return 0;
}

CPlaceableCamera* CGuiInGame::GetPlaceableCamera(int)
{
    return 0;
}

void CGuiInGame::GetStatusSummary(CStatusSummary&)
{
}

int CGuiInGame::HandleDialogEntry(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned char, CExoString const&, unsigned long, unsigned long*, unsigned short*, unsigned long, int, float, float, unsigned long, unsigned short, float, int, unsigned char, Vector const&, float, float, int)
{
    return 0;
}

int CGuiInGame::HandleDialogModels(unsigned long, unsigned long, unsigned long*, CResRef*, CResRef const&, CResRef const&)
{
    return 0;
}

int CGuiInGame::HandleDialogReplies(unsigned long, unsigned long, unsigned char, CExoString*, unsigned long*, unsigned long*, unsigned long*, int*, float*, float*, unsigned long*, unsigned short*, float*, int*, unsigned char*, Vector*, float*, float*, unsigned long, unsigned long, unsigned long, unsigned long*, unsigned long**, unsigned short**, int)
{
    return 0;
}

int CGuiInGame::HandleDialogReplyChosen(unsigned long, unsigned long, unsigned long, CExoString const&, unsigned long, unsigned char, int)
{
    return 0;
}

int CGuiInGame::HandleDialogSound(unsigned long, CResRef const&)
{
    return 0;
}

void CGuiInGame::HideContainerGui()
{
}

int CGuiInGame::InitializePlaceableCameras(int)
{
    return 0;
}

int CGuiInGame::IsAnimationPlayingInDialog()
{
    return 0;
}

int CGuiInGame::IsCameraDialog()
{
    return 0;
}

int CGuiInGame::IsFadingInDialog()
{
    return 0;
}

int CGuiInGame::IsGlobalFading()
{
    return 0;
}

int CGuiInGame::IsOnDialogNonDependantList(CExoString const&)
{
    return 0;
}

void CGuiInGame::MarkNoClickEvent(unsigned long)
{
}

void CGuiInGame::RemoveGUIPopUps()
{
}

void CGuiInGame::SetCombatMessage(unsigned long)
{
}

void CGuiInGame::SetConversationType(unsigned long, unsigned char)
{
}

void CGuiInGame::SetGlobalDialogState(int)
{
}

int CGuiInGame::SetGlobalUnpauseableState(int)
{
    return 0;
}

void CGuiInGame::SetLockHeadFollowInDialog(unsigned long, int)
{
}

void CGuiInGame::SetLockObjectOrientationInDialog(unsigned long, int)
{
}

void CGuiInGame::SetPlaceableCamera(int, int, Vector const&, Quaternion const&, float, float, float, float)
{
}

void CGuiInGame::SetShotFromScript(int, int)
{
}

void CGuiInGame::ShowBarkBubble(unsigned long, CExoString const&, CResRef const&, int)
{
}

void CGuiInGame::ShowDialogMessage()
{
}

void CGuiInGame::ShowGalaxyMapGui(int)
{
}

int CGuiInGame::ShowLevelUpGUI(int)
{
    return 0;
}

void CGuiInGame::ShowPartySelection(CExoString const&, int, int, int)
{
}

void CGuiInGame::ShowStoreGui(unsigned long, unsigned long)
{
}

void CGuiInGame::ShowUpgradeScreen(unsigned long)
{
}

void CGuiInGame::StartGlobalFade(int, float, float, Vector const&)
{
}

void CGuiInGame::StopGlobalFade()
{
}

void CGuiInGame::SuppressStatusSummary(int)
{
}

void CGuiInGame::UpdateDialogLipSync()
{
}

void CGuiInGame::UpdateStatus(unsigned char, int)
{
}

void CSWCArea::AttachmentFromServerArea(CSWSArea*)
{
}

void CSWCArea::DetachFromServerArea()
{
}

CSWCRoom* CSWCArea::GetRoom(CExoString const&)
{
    return 0;
}

int CSWCArea::SetFogColor(Vector const&)
{
    return 0;
}

void CSWCArea::StartSounds()
{
}

int CSWCCreature::GetForceShield()
{
    return 0;
}

float CSWCCreature::GetHeadTurnHorizontal()
{
    return 0;
}

int CSWCCreature::GetPauseReadyAnimation()
{
    return 0;
}

CSWSCreature* CSWCCreature::GetServerCreature()
{
    return 0;
}

void CSWCCreature::HideEquippedHeadItem()
{
}

void CSWCCreature::HideEquippedItems(CResRef const&, CResRef const&)
{
}

int CSWCCreature::LookAt(unsigned long, float)
{
    return 0;
}

void CSWCCreature::PlaySoundSetSound(unsigned short, int)
{
}

void CSWCCreature::ResetCameraToFollow()
{
}

void CSWCCreature::SetCombatMode(int)
{
}

void CSWCCreature::SetCombatState(int)
{
}

int CSWCCreature::SetInParty(int)
{
    return 0;
}

void CSWCCreature::SetLockOrientationToObject(unsigned long, int)
{
}

void CSWCCreature::SetOverrideLightsaberState(int, int, int)
{
}

void CSWCCreature::ShowEquippedHeadItem()
{
}

void CSWCCreature::ShowEquippedItems()
{
}

CExoString CSWCCreatureStats::GetFullName()
{
    return CExoString();
}

void CSWCItem::PowerItem(int, int, int, int)
{
}

int CSWCModule::IsCameraAnimated()
{
    return 0;
}

int CSWCModule::RestoreCamera()
{
    return 0;
}

int CSWCObject::AnimationFireAndForget(unsigned short)
{
    return 0;
}

void CSWCObject::AttachmentFromServerObject(CSWSObject*)
{
}

void CSWCObject::ClearAllActions()
{
}

void CSWCObject::DetachFromServerObject()
{
}

float CSWCObject::GetAnimationLength(unsigned short)
{
    return 0;
}

void CSWCObject::SetDesiredFadeState(unsigned char, int, unsigned long, unsigned long)
{
}

void CSWCObject::SetFadeState(unsigned char)
{
}

void CSWCObject::SetFeedbackInfo(CExoString const&)
{
}

void CSWCRoom::PlayAnimation(unsigned short)
{
}

void CSWCSoundObject::ChangeFixedVariance(float)
{
}

void CSWCSoundObject::ChangePitchVariation(float)
{
}

void CSWCSoundObject::ChangeVolume(int)
{
}

void CSWCSoundObject::FadeAndStop(unsigned long)
{
}

void CSWCameraOnAStick::SetCameraForScreenShot()
{
}

void CSWGuiInterfaceAction::Clear()
{
}

void CSWGuiManager::Draw(float)
{
}

float CSWMGGunBank::SetBulletLifeSpan(float)
{
    return 0;
}

float CSWMGGunBank::SetBulletSpeed(float)
{
    return 0;
}

int CSWMGGunBank::SetBulletTargetType(int)
{
    return 0;
}

void CSWMGGunBank::SetGunModel(CResRef const&)
{
}

float CSWMGGunBank::SetRateOfFire(float)
{
    return 0;
}

float CSWMGTargettingGunBank::SetHorizontalSpread(float)
{
    return 0;
}

float CSWMGTargettingGunBank::SetInaccuracy(float)
{
    return 0;
}

float CSWMGTargettingGunBank::SetSensingRadius(float)
{
    return 0;
}

float CSWMGTargettingGunBank::SetVerticalSpread(float)
{
    return 0;
}

void CSWMiniGame::GetClipDist(float*, float*)
{
}

int CSWMiniGame::GetEnemy(int)
{
    return 0;
}

CSWMGObstacle* CSWMiniGame::GetObstacle(int)
{
    return 0;
}

void CSWMiniGame::SetClipDist(float, float)
{
}

void* CSWMiniGameObject::GetLastBulletHitShooter()
{
    return 0;
}

void* CSWMiniGameObject::GetLastFollowerHit()
{
    return 0;
}

void CSWMiniGameObject::OnHitBullet(int)
{
}

void* CSWMiniGameObjectArray::GetMiniGameObject(unsigned long, unsigned char)
{
    return 0;
}

CSWMiniGameObject* CSWMiniGameObjectArray::GetMiniGameObjectByName(CExoString const&)
{
    return 0;
}

float CSWMiniPlayer::SetAPS(float)
{
    return 0;
}

float CSWMiniPlayer::SetMaxSpeed(float)
{
    return 0;
}

float CSWMiniPlayer::SetMinSpeed(float)
{
    return 0;
}

Vector CSWMiniPlayer::SetOffset(Vector const&)
{
    return Vector();
}

Vector CSWMiniPlayer::SetOrigin(Vector const&)
{
    return Vector();
}

void CSWParty::ClearParty()
{
}

void CSWParty::FollowStateChanged()
{
}

CSWCCreature* CSWParty::GetCharacter(int)
{
    return 0;
}

float CSWParty::GetFollowRange()
{
    return 0;
}

int CSWParty::GetIndex(unsigned long)
{
    return 0;
}

CSWCCreature* CSWParty::GetPlayerCharacter()
{
    return 0;
}

int CSWParty::IsPartyMember(unsigned long)
{
    return 0;
}

int CSWParty::IsPartyTogether()
{
    return 0;
}

void CSWParty::RecaulateFollowPoint()
{
}

int CSWParty::RemoveCharacter(unsigned long)
{
    return 0;
}

void CSWParty::ResetAllFollowData(Vector const&, float, int)
{
}

void CSWParty::SetLeader(int)
{
}

float CSWPartyFollowDataArray::AdvancePointOnPath(float, int, Vector const&, int, Vector const&, int, Vector&, int&)
{
    return 0;
}

CSWPartyFollowData* CSWPartyFollowDataArray::GetPathData(int)
{
    return 0;
}

int CSWPartyFollowDataArray::GetPointBackFromStart(float, Vector&, int&, int)
{
    return 0;
}

void CSWPartyFollowDataArray::ProcessPoint(Vector const&, float, float)
{
}

void CSWPartyMemberData::SetCombatMessage(unsigned long)
{
}

void CSWTrackFollower::AdjustHitPoints(int)
{
}

CSWMGTargettingGunBank* CSWTrackFollower::GetGunBank(int)
{
    return 0;
}

int CSWTrackFollower::GetLastObstacleHit()
{
    return 0;
}

Vector CSWTrackFollower::GetPosition()
{
    return Vector();
}

int CSWTrackFollower::GetSoundFrequency(int)
{
    return 0;
}

int CSWTrackFollower::GetSoundFrequencyIsRandom(int)
{
    return 0;
}

int CSWTrackFollower::GetSoundVolume(int)
{
    return 0;
}

void CSWTrackFollower::OnDamage(int)
{
}

void CSWTrackFollower::OnDeath(int)
{
}

void CSWTrackFollower::OnHitObstacle(int)
{
}

int CSWTrackFollower::SetHitPoints(int)
{
    return 0;
}

float CSWTrackFollower::SetInvincibility(float)
{
    return 0;
}

int CSWTrackFollower::SetMaxHitPoints(int)
{
    return 0;
}

void CSWTrackFollower::SetSoundFrequency(int, int)
{
}

void CSWTrackFollower::SetSoundFrequencyIsRandom(int, int)
{
}

void CSWTrackFollower::SetSoundVolume(int, int)
{
}

float CSWTrackFollower::SetSpeed(float)
{
    return 0;
}

float CSWTrackFollower::SetSphereRadius(float)
{
    return 0;
}

int AurAddDrawableSquare(float*, unsigned long)
{
    return 0;
}

void AurDisableSpeedBlur()
{
}

void AurEnableSpeedBlur()
{
}

void AurRemoveDrawableSquare(unsigned long)
{
}

void AurSetSpeedBlurRatio(float)
{
}

char* SetFramesPerSecond(char*)
{
    return 0;
}
