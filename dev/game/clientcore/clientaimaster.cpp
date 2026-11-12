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
// PC 0x00650780
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
// PC 0x006507F0
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
// PC 0x00650850
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

    CGameObject* pGameObject =
        g_pAppManager->m_pClientExoApp->m_pcExoAppInternal->GetGameObject(
            (unsigned long)m_lstObjects.GetAtPos(m_pCurrent));
    CSWCObject* pObject = pGameObject != NULL ? pGameObject->AsSWCObject() : NULL;
    return pObject;
}
// PC 0x006508C0
int CClientAIMaster::UpdateState()
{
    CSWCObject* pObject = m_aiList.GetNextObject();
    int nCount = m_aiList.m_nCount;

    g_starttime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    if (pObject != NULL)
    {
        while (nCount > 0)
        {
            if (pObject != NULL)
            {
                pObject->AIUpdate();
            }
            pObject = m_aiList.GetNextObject();
            nCount--;
        }
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
// PC 0x00650A30
int CClientAIMaster::AddObject(CSWCObject* pObject)
{
    return m_aiList.AddObject(pObject);
}
// PC 0x00650A40
int CClientAIMaster::RemoveObject(CSWCObject* pObject)
{
    return m_aiList.RemoveObject(pObject);
}
// PC 0x00650A50
CClientAIList::~CClientAIList()
{
    while (!m_lstObjects.IsEmpty())
    {
        unsigned long nId = (unsigned long)m_lstObjects.GetHead();
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CClientExoAppInternal* pInternal = pClientExoApp->m_pcExoAppInternal;
        CGameObject* pGameObject = pInternal->GetGameObject(nId);
        RemoveObject(pGameObject->AsSWCObject());
    }
}
// PC 0x00650B50
CClientAIMaster::CClientAIMaster()
{
}
// PC 0x00650B70
CClientAIMaster::~CClientAIMaster()
{
}
// PC 0x00650770
void CClientAIMaster::SetExoAppInternal(CClientExoAppInternal* pInternal)
{
    m_pExoAppInternal = pInternal;
}
