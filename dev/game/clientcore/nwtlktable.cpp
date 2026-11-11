#include "nwtlktable.h"
#include "exobase.h"
#include "exotlktable.h"
#include "nwsobject.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "clientoptions.h"
#include "nwcmodule.h"
#include "nwrules.h"
#include "nwrace.h"
#include "nwclass.h"
#include "nwscreaturestats.h"
#include "exoinput.h"
#include "exoinputinternal.h"
#include "swguiinOPTKeyMapping.h"
// PC 0x00674B40  Xbox 0x0006B920
unsigned char CSWTlkTable::GetObjectGender()
{
    if (m_pObject != 0 && m_pObject->AsSWCObject() != 0 &&
        m_pObject->AsSWCCreature() != 0 &&
        m_pObject->AsSWCCreature()->m_pStats != 0)
    {
        return m_pObject->AsSWCCreature()->m_pStats->m_nGender;
    }
    return CTlkTable::GENDER_MALE;
}
// PC 0x00674B90  Xbox 0x0006B970
void CSWTlkTable::GetTokenValue(const CTlkTableToken& token, CExoString& sValue)
{
    STR_RES strRes;
    unsigned long nHour;
    switch (token.m_nType)
    {
    case CTlkTableToken::GUI_STRING:
        sValue = g_pAppManager->GetClientExoApp()->GetGUIString(token.m_nStringRefs[0]);
        break;
    case CTlkTableToken::GENDER_STRING:
        sValue = g_pAppManager->GetClientExoApp()->GetGUIString(token.m_nStringRefs[(unsigned char)m_nCurGender]);
        break;
    case CTlkTableToken::FULL_NAME:
    case CTlkTableToken::FIRST_NAME:
    case CTlkTableToken::LAST_NAME:
    case CTlkTableToken::RACE_NAME:
    case CTlkTableToken::RACE_NAME_LOWER:
    case CTlkTableToken::SUBRACE_NAME:
    case CTlkTableToken::CLASS_NAME:
    case CTlkTableToken::CLASS_NAME_LOWER:
    case CTlkTableToken::CLASS_LEVEL:
    case CTlkTableToken::ALIGNMENT_NAME:
    case CTlkTableToken::ALIGNMENT_NAME_LOWER:
    case CTlkTableToken::ALIGNMENT_STRING:
        if (m_pObject == 0)
        {
            sValue = g_pAppManager->GetClientExoApp()->GetGUIString(token.m_nTokenValue);
        }
        else if (m_pObject->AsSWCObject() != 0)
        {
            if (m_pObject->AsSWCCreature() != 0)
            {
                if (m_pObject->AsSWCCreature()->GetStats() == 0)
                {
                    sValue = "NO STATS";
                }
                else
                {
                    unsigned char nGender = m_nCurGender;
                    switch (token.m_nType)
                    {
                    case CTlkTableToken::FULL_NAME:
                        sValue = m_pObject->AsSWCCreature()->GetStats()->GetFullName();
                        break;
                    case CTlkTableToken::FIRST_NAME:
                        sValue = m_pObject->AsSWCCreature()->GetStats()->m_sFirstName;
                        break;
                    case CTlkTableToken::LAST_NAME:
                        sValue = m_pObject->AsSWCCreature()->GetStats()->m_sLastName;
                        break;
                    case CTlkTableToken::RACE_NAME:
                        {
                            unsigned short nRace = m_pObject->AsSWCCreature()->GetStats()->m_nRace;
                            sValue = g_pRules->m_pRaces[nRace].GetConverNameText();
                        }
                        break;
                    case CTlkTableToken::RACE_NAME_LOWER:
                        {
                            unsigned short nRace = m_pObject->AsSWCCreature()->GetStats()->m_nRace;
                            sValue = g_pRules->m_pRaces[nRace].GetConverNameLowerText();
                        }
                        break;
                    case CTlkTableToken::SUBRACE_NAME:
                        sValue = m_pObject->AsSWCCreature()->GetStats()->GetRaceString();
                        break;
                    case CTlkTableToken::CLASS_NAME:
                        {
                            unsigned char nClass = m_pObject->AsSWCCreature()->GetStats()->GetClass(0);
                            sValue = g_pRules->m_pClasses[nClass].GetNameText();
                        }
                        break;
                    case CTlkTableToken::CLASS_NAME_LOWER:
                        {
                            unsigned char nClass = m_pObject->AsSWCCreature()->GetStats()->GetClass(0);
                            sValue = g_pRules->m_pClasses[nClass].GetNameLowerText();
                        }
                        break;
                    case CTlkTableToken::CLASS_LEVEL:
                        sValue = m_pObject->AsSWCCreature()->GetStats()->GetClassLevel(0);
                        break;
                    case CTlkTableToken::ALIGNMENT_NAME:
                        sValue = m_pObject->AsSWCCreature()->GetStats()->GetAlignmentString(0);
                        break;
                    case CTlkTableToken::ALIGNMENT_NAME_LOWER:
                        sValue = m_pObject->AsSWCCreature()->GetStats()->GetAlignmentString(1);
                        break;
                    case CTlkTableToken::ALIGNMENT_STRING:
                        {
                            unsigned char nIndex;
                            switch (m_pObject->AsSWCCreature()->GetStats()->GetSimpleAlignmentGoodEvil())
                            {
                            case CSWSCreatureStats::ALIGNMENT_GOOD: nIndex = 0; break;
                            case CSWSCreatureStats::ALIGNMENT_EVIL: nIndex = 2; break;
                            default: nIndex = 1; break;
                            }
                            sValue = g_pAppManager->GetClientExoApp()->GetGUIString(token.m_nStringRefs[nIndex]);
                        }
                        break;
                    }
                    m_nCurGender = nGender;
                }
            }
            else
                sValue = g_pAppManager->GetClientExoApp()->GetGUIString(token.m_nTokenValue);
        }
        else
            sValue = "NO OBJECT";
        break;
    case CTlkTableToken::YEAR:
        {
            unsigned long nYear = g_pAppManager->GetClientExoApp()->GetModule()->m_nTimeA4;
            sValue = nYear;
        }
        break;
    case CTlkTableToken::MONTH:
        {
            unsigned long nMonth = g_pAppManager->GetClientExoApp()->GetModule()->m_nTimeA8;
            sValue = nMonth;
        }
        break;
    case CTlkTableToken::HOUR:
        {
            nHour = g_pAppManager->GetClientExoApp()->GetModule()->m_nTimeB0;
            sValue = (nHour + CWorldTimer::HOURS_IN_DAY / 2 - 1) % (CWorldTimer::HOURS_IN_DAY / 2) + 1;
        }
        break;
    case CTlkTableToken::TIME_OF_DAY:
        {
            unsigned char nDawn = g_pAppManager->GetClientExoApp()->GetModule()->GetDawnHour();
            unsigned char nDusk = g_pAppManager->GetClientExoApp()->GetModule()->GetDuskHour();
            nHour = g_pAppManager->GetClientExoApp()->GetModule()->m_nTimeB0;
            unsigned char nIndex;
            if (nHour < nDawn || nHour > nDusk)
                nIndex = 0;
            else
                nIndex = 3 * (nHour - nDawn) / (nDusk - nDawn + 1) + 1;
            sValue = g_pAppManager->GetClientExoApp()->GetGUIString(token.m_nStringRefs[nIndex]);
        }
        break;
    case CTlkTableToken::PARSE_BEGIN:
        m_bParsing = m_bFlag;
        sValue = "";
        break;
    case CTlkTableToken::PARSE_SKIP:
        m_bParsing = m_bFlag;
    case CTlkTableToken::PARSE_EMPTY:
        sValue = "";
        break;
    case CTlkTableToken::PARSE_END:
        sValue = "";
        m_bParsing = 0;
        break;
    case CTlkTableToken::PLAYER_NAME:
        {
            CSWCPlayer* pPlayer = g_pAppManager->GetClientExoApp()->GetPlayerByPlayerID(m_nPlayerId);
            if (pPlayer == 0)
                sValue = g_pAppManager->GetClientExoApp()->GetGUIString(token.m_nTokenValue);
            else
                sValue = pPlayer->m_sPlayerName;
        }
        break;
    case CTlkTableToken::CHARACTER:
        sValue.Format("%c", token.m_nTokenValue);
        break;
    case CTlkTableToken::INPUT_CONTROL:
        {
            unsigned long nEvent = token.m_nTokenValue;
            int nType = token.m_nStringRefs[0];
            sValue = g_pTlkTable->GetSimpleString(0);
            CExoInputEventDesc* pEvent = g_pExoInput->GetEvent(nEvent, CExoInput::EVENTCLASS_ANY);
            if (pEvent != 0)
            {
                if (pEvent->Is2Button() && nType > CSWGuiKeyMapButton::CONTROL_PRIMARY)
                {
                    int nKey = ((CExoInputeventDesc2ButtonAxis*)pEvent)->m_nControl2;
                    if (nKey != 0)
                        sValue = g_pAppManager->GetClientExoApp()->GetDisplayCharacterForKey(nKey);
                }
                else
                {
                    int nKey = pEvent->m_nControl;
                    if (nKey != 0)
                        sValue = g_pAppManager->GetClientExoApp()->GetDisplayCharacterForKey(nKey);
                }
            }
        }
        break;
    case CTlkTableToken::MOUSE_BUTTON:
        {
            int nButton = token.m_nTokenValue;
            unsigned long nStrRef = 0;
            if (!g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bReverseMouseButtons)
            {
                switch (nButton)
                {
                case MOUSE_RIGHT: nStrRef = STRREF_MOUSE_RIGHT; break;
                case MOUSE_LEFT: nStrRef = STRREF_MOUSE_LEFT; break;
                case MOUSE_RIGHT_LOWER: nStrRef = STRREF_MOUSE_RIGHT_LOWER; break;
                case MOUSE_LEFT_LOWER: nStrRef = STRREF_MOUSE_LEFT_LOWER; break;
                }
            }
            else
            {
                switch (nButton)
                {
                case MOUSE_LEFT: nStrRef = STRREF_MOUSE_RIGHT; break;
                case MOUSE_RIGHT: nStrRef = STRREF_MOUSE_LEFT; break;
                case MOUSE_LEFT_LOWER: nStrRef = STRREF_MOUSE_RIGHT_LOWER; break;
                case MOUSE_RIGHT_LOWER: nStrRef = STRREF_MOUSE_LEFT_LOWER; break;
                }
            }
            sValue = g_pTlkTable->GetSimpleString(nStrRef);
        }
        break;
    }
}
