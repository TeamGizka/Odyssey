#ifndef __NWCSPELLVISUAL_H__
#define __NWCSPELLVISUAL_H__

#include "exoresman.h"

class Gob;
class CSWCCreature;

#define SPELL_VISUAL_CONJURE_DURATION 5000

class CSWCSpellCastVisual
{
public:
    CSWCSpellCastVisual();
    ~CSWCSpellCastVisual();

    void UnloadConjureVisual();
    void UnloadCastVisual();
    void UpdateTimer(unsigned long nTime);
    int LoadConjureVisual(CResRef cHeadVisual, CResRef cHandVisual,
                          CResRef cGroundVisual, CResRef cSound,
                          CResRef cVocalization, CSWCCreature* pCreature);
    int LoadCastVisual(CResRef cHeadVisual, CResRef cHandVisual,
                       CResRef cGroundVisual, CSWCCreature* pCreature);

    Gob* m_pHeadConjureVisual;
    Gob* m_pHandConjureVisual;
    Gob* m_pGroundConjureVisual;
    Gob* m_pHeadCastVisual;
    Gob* m_pHandCastVisual;
    Gob* m_pGroundCastVisual;
    unsigned long m_nConjureTime;
    unsigned long m_nCastTime;
    int m_bTimerRunning;
    int m_bHeadConjureFaded;
    int m_bHandConjureFaded;
    int m_bGroundConjureFaded;
};

#endif
