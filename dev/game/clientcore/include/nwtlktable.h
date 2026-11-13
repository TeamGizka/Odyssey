#ifndef __NWTLKTABLE_H__
#define __NWTLKTABLE_H__

#include "exobase.h"
#include "exotlktable.h"
#include "appmanager.h"
#include "clientexoapp.h"

class CGameObject;

class CSWTlkTable : public CTlkTable
{
public:

    enum MouseButtonStringRef
    {
        STRREF_MOUSE_LEFT = 48725, STRREF_MOUSE_RIGHT = 48726,
        STRREF_MOUSE_LEFT_LOWER = 48727, STRREF_MOUSE_RIGHT_LOWER = 48728
    };

    enum MouseButtonToken
    {
        MOUSE_RIGHT = 0, MOUSE_LEFT = 1,
        MOUSE_RIGHT_LOWER = 2, MOUSE_LEFT_LOWER = 3
    };

    enum { CLOCK_HOURS = 12 };

    CSWTlkTable() { m_nPlayerId = 0; }
    CGameObject* m_pObject;
    unsigned int m_bFlag : 1;
    unsigned int : 31;

    unsigned long m_nPlayerId;
    unsigned char GetObjectGender();
    CExoString Fetch(unsigned long nStrRef, CGameObject* pObject, int nGender,
                     int nFlags)
    {
        m_pObject = pObject;
        m_nCurGender = GetObjectGender();
        m_bFlag = nFlags;

        return g_pAppManager->m_pClientExoApp->GetGUIString(nStrRef);
    }

    void ParseStr(CExoString& sText, CGameObject* pObject, int nGender,
                  int nFlags)
    {
        m_pObject = pObject;
        m_nCurGender = GetObjectGender();
        m_bFlag = nFlags;
        CTlkTable::ParseStr(sText);
    }
    virtual void GetTokenValue(const CTlkTableToken& token, CExoString& sValue);
};

extern CSWTlkTable* g_pSWTlkTable;

#endif
