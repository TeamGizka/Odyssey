#include "nwcanimbase.h"
// PC 0x00690EF0  Xbox 0x000B7060
CSWBehaviorAttachSound::CSWBehaviorAttachSound(CAurObject* pObject,
                                               CAurObject* pParent,
                                               CAurPart* pPart,
                                               const CResRef& cResRef)
    : CAurBehaviorAttach(pObject, pParent, pPart)
{
    m_resSound = cResRef;
    m_pSoundSource = new CExoStreamingSoundSource;
    m_pSoundSource->SetLooping(1);
    m_pSoundSource->Set3D(1);
    m_pSoundSource->SetPriorityGroup(25);
    m_pSoundSource->SetDistance(20.0f, 150.0f);
}
// PC 0x00690FC0  Xbox 0x000B71A0
int CSWBehaviorAttachSound::Control(float fTimeStep)
{
    int nResult = CAurBehaviorAttach::Control(fTimeStep);
    Vector vPosition = ((Gob*)m_pObject)->GetPosition();
    m_pSoundSource->SetPosition(vPosition);
    return nResult;
}
// PC 0x00691000  Xbox 0x000B71E0
int CSWBehaviorAttachSound::PlaySoundA()
{
    int nResult = m_pSoundSource->IsPlaying();
    if (nResult == 0)
    {
        return m_pSoundSource->Play(m_resSound, RESTYPE_WAV, 0, 0, 0);
    }
    return nResult;
}
// PC 0x00691030  Xbox 0x000B7140
CSWBehaviorAttachSound::~CSWBehaviorAttachSound()
{
    if (m_pSoundSource != 0)
    {
        m_pSoundSource->Stop();
        delete m_pSoundSource;
        m_pSoundSource = 0;
    }
}
