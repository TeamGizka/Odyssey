#ifndef __NWRACE_H__
#define __NWRACE_H__

#include "exobase.h"

class CSWRace
{
public:

    enum { FAVORED_CLASS_HIGHEST = 0xFF };
    static const unsigned short CRACE_UNKNOWN;
    static const unsigned short CRACE_ELF;
    static const unsigned short CRACE_GNOME;
    static const unsigned short CRACE_HALFELF;
    static const unsigned short CRACE_HALFLING;
    static const unsigned short CRACE_DROID;
    static const unsigned short CRACE_HUMAN;

    static const unsigned short CRACE_ALL_RACES;
    static const unsigned short CRACE_INVALID_RACE;
    static const unsigned short CRACE_UNKNOWN_MASK;
    static const unsigned short CRACE_ELF_MASK;
    static const unsigned short CRACE_GNOME_MASK;
    static const unsigned short CRACE_HALFELF_MASK;
    static const unsigned short CRACE_HALFLING_MASK;
    static const unsigned short CRACE_HUMAN_MASK;
    static const unsigned short CRACE_DROID_MASK;
    static const unsigned short CRACE_PLACEABLE_MASK;
    static const unsigned short CRACE_ALL_RACES_MASK;
    static const unsigned short CRACE_INVALID_RACE_MASK;

    CSWRace();
    ~CSWRace();

    CExoString GetNameText();
    CExoString GetConverNameText();
    CExoString GetConverNameLowerText();
    void LoadFeatsTable(const CExoString& sTable);
    int IsFirstLevelGrantedFeat(unsigned short nFeat);

    void SetNameStrRef(unsigned long nStrRef)
    {
        m_nNameStrRef = nStrRef;
    }

    void SetConverNameStrRef(unsigned long nStrRef)
    {
        m_nConverNameStrRef = nStrRef;
    }

    void SetConverNameLowerStrRef(unsigned long nStrRef)
    {
        m_nConverNameLowerStrRef = nStrRef;
    }

    void SetNamePluralStrRef(int nStrRef)
    {
        m_nNamePluralStrRef = nStrRef;
    }

    void SetDescriptionStrRef(int nStrRef)
    {
        m_nDescriptionStrRef = nStrRef;
    }

    void SetBiographyStrRef(int nStrRef)
    {
        m_nBiographyStrRef = nStrRef;
    }

    void SetFavoredClass(unsigned char nClass)
    {
        m_nFavoredClass = nClass;
    }

    void SetPlayerRace(int bPlayerRace)
    {
        m_bPlayerRace = bPlayerRace;
    }

    void SetStrengthAdjust(unsigned char nAdjust)
    {
        m_nStrengthAdjust = nAdjust;
    }

    void SetDexterityAdjust(unsigned char nAdjust)
    {
        m_nDexterityAdjust = nAdjust;
    }

    void SetIntelligenceAdjust(unsigned char nAdjust)
    {
        m_nIntelligenceAdjust = nAdjust;
    }

    void SetCharismaAdjust(unsigned char nAdjust)
    {
        m_nCharismaAdjust = nAdjust;
    }

    void SetWisdomAdjust(unsigned char nAdjust)
    {
        m_nWisdomAdjust = nAdjust;
    }

    void SetConstitutionAdjust(unsigned char nAdjust)
    {
        m_nConstitutionAdjust = nAdjust;
    }

    void SetEnduranceAdjust(unsigned char nAdjust)
    {
        m_nEnduranceAdjust = nAdjust;
    }

    void SetAge(int nAge)
    {
        m_nAge = nAge;
    }

    void SetAppearance(unsigned short nAppearance)
    {
        m_nAppearance = nAppearance;
    }

    unsigned long m_nNameStrRef;
    unsigned long m_nConverNameStrRef;
    unsigned long m_nConverNameLowerStrRef;
    int m_nNamePluralStrRef;
    int m_nDescriptionStrRef;
    int m_nBiographyStrRef;
    unsigned char m_nStrengthAdjust;
    unsigned char m_nDexterityAdjust;
    unsigned char m_nIntelligenceAdjust;
    unsigned char m_nCharismaAdjust;
    unsigned char m_nWisdomAdjust;
    unsigned char m_nConstitutionAdjust;
    unsigned char m_nEnduranceAdjust;
    unsigned char m_nFavoredClass;
    int m_bPlayerRace;
    unsigned short m_nFirstLevelGrantedFeatCount;
    unsigned char m_aUnmapped26[2];
    unsigned short* m_pFirstLevelGrantedFeats;
    int m_nAge;

    unsigned char GetStrengthAdjust() { return m_nStrengthAdjust; }
    unsigned char GetDexterityAdjust() { return m_nDexterityAdjust; }
    unsigned char GetConstitutionAdjust() { return m_nConstitutionAdjust; }
    unsigned char GetIntelligenceAdjust() { return m_nIntelligenceAdjust; }
    unsigned char GetWisdomAdjust() { return m_nWisdomAdjust; }
    unsigned char GetCharismaAdjust() { return m_nCharismaAdjust; }

    unsigned short m_nAppearance;
    unsigned char m_aUnmapped32[2];
};

#endif
