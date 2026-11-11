#include "clientaimaster.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "nwsobject.h"
#include "nwcmodule.h"

static unsigned __int64 g_starttime;
static unsigned __int64 g_endtime;
float g_AIModuleTime2 = 0;
float g_AIObjectTime = 0;
// PC 0x006047A0  Xbox 0x00034780
int CClientAIList::AddObject(CSWCObject* pObject)
{
    CExoLinkedListNode* pPos = m_lstObjects.GetHeadPos();

    while (pPos != NULL)
    {
        if ((unsigned long)m_lstObjects.GetAtPos(pPos) == pObject->m_nId)
        {
            return 0;
        }
        m_lstObjects.GetNext(pPos);
    }

    m_nCount++;
    pObject->m_pAIListPos =
        m_lstObjects.AddTail((unsigned long*)pObject->m_nId);
    if (m_pCurrent == NULL)
    {
        m_pCurrent = pObject->m_pAIListPos;
    }
    return 1;
}
// PC 0x00604810  Xbox 0x000347F0
int CClientAIList::RemoveObject(CSWCObject* pObject)
{
    CExoLinkedListNode* pPos = pObject->m_pAIListPos;

    if (pPos == NULL)
    {
        return 1;
    }

    if (m_pCurrent == pPos)
    {
        m_lstObjects.GetNext(m_pCurrent);
        if (m_pCurrent == NULL)
        {
            m_pCurrent = m_lstObjects.GetHeadPos();
            if (m_pCurrent == pPos)
            {
                m_pCurrent = NULL;
            }
        }
    }

    m_nCount--;
    m_lstObjects.Remove(pPos);
    pObject->m_pAIListPos = NULL;
    return 0;
}
// PC 0x00604870  Xbox 0x00034850
CSWCObject* CClientAIList::GetNextObject()
{
    if (m_lstObjects.GetHeadPos() == NULL)
    {
        return NULL;
    }

    m_lstObjects.GetNext(m_pCurrent);
    if (m_pCurrent == NULL)
    {
        m_pCurrent = m_lstObjects.GetHeadPos();
    }

    unsigned long nId = (unsigned long)m_lstObjects.GetAtPos(m_pCurrent);
    CClientExoAppInternal* pInternal =
        g_pAppManager->m_pClientExoApp->m_pcExoAppInternal;
    CGameObject* pGameObject = pInternal->GetGameObject(nId);
    CSWCObject* pSWCObject =
        pGameObject != NULL ? pGameObject->AsSWCObject() : NULL;
    return pSWCObject;
}
// PC 0x006048C0  Xbox 0x000348A0
int CClientAIMaster::UpdateState()
{
    CSWCObject* pObject = m_aiList.GetNextObject();
    int nCount = m_aiList.m_nCount;

    g_starttime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    if (pObject != NULL && nCount > 0)
    {
        do
        {
            if (pObject != NULL)
            {
                pObject->AIUpdate();
            }
            pObject = m_aiList.GetNextObject();
            nCount--;
        } while (nCount != 0);
    }
    g_endtime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    g_AIObjectTime = (float)(int)(g_endtime - g_starttime) * 0.001f;

    g_starttime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    __int64 nSnapshotDifference =
        pClientExoApp->m_pcExoAppInternal->m_pPauseTimer
            ->GetSnapshotTimeDifference();
    pClientExoApp->GetModule()->Update((float)nSnapshotDifference / 1000000.0f);
    g_endtime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    g_AIModuleTime2 =
        (float)(int)(g_endtime - g_starttime) * 0.001f;
    return g_endtime - g_starttime;
}
// PC 0x00604A20  Xbox 0x00034A00
int CClientAIMaster::AddObject(CSWCObject* pObject)
{
    return m_aiList.AddObject(pObject);
}
// PC 0x00604A30  Xbox 0x00034A10
int CClientAIMaster::RemoveObject(CSWCObject* pObject)
{
    return m_aiList.RemoveObject(pObject);
}
// PC 0x00604A40  Xbox 0x00034A20
CClientAIList::~CClientAIList()
{
    while (m_lstObjects.GetHeadPos() != NULL)
    {
        unsigned long nId = (unsigned long)m_lstObjects.GetHead();
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CClientExoAppInternal* pInternal = pClientExoApp->m_pcExoAppInternal;
        CGameObject* pGameObject = pInternal->GetGameObject(nId);
        RemoveObject(pGameObject->AsSWCObject());
    }
}
// PC 0x00604B20  Xbox 0x00034B00
CClientAIMaster::CClientAIMaster()
{
}
// PC 0x00604B40  Xbox 0x00034B20
CClientAIMaster::~CClientAIMaster()
{
}
// PC 0x00604790
void CClientAIMaster::SetExoAppInternal(CClientExoAppInternal* pInternal)
{
    m_pExoAppInternal = pInternal;
}
