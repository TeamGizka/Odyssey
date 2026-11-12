#ifndef __NWCCREATUREAPPEARANCE_H__
#define __NWCCREATUREAPPEARANCE_H__

class CCAppearanceInfo
{
public:

    enum { APPEARANCE_SAFE_PROJECTILE_DUMMY = 201,
           BODY_VARIATION_B = 2, BODY_VARIATION_I = 9, BODY_VARIATION_L = 12, BODY_VARIATION_N = 14, HANDMAIDEN_BODY_VARIATION_ROBED = 2, HANDMAIDEN_ALIGNMENT_DARK = 40, APPEARANCE_HANDMAIDEN_ROBED_DARK = 669, APPEARANCE_HANDMAIDEN_ROBED_LIGHT = 454, APPEARANCE_HANDMAIDEN_DEFAULT = 638, APPEARANCE_GRAN_01 = 565 };

    enum Gender
    {
        GENDER_MALE = 0,
        GENDER_FEMALE = 1
    };
    static const unsigned char HEAD_OVERRIDE_DEFAULT = 255;
#ifndef _XBOX
#endif

    unsigned char m_nBodyVariation;
    unsigned char m_nTextureVar;

    unsigned short m_anArmorModelPart[6];
    unsigned char m_bUnknown0E;
    unsigned char m_bUnknown0F;
    unsigned long m_nUnmapped10;

    unsigned long m_bUseBodyVariation;
    unsigned short m_nAppearance;
    unsigned char m_nHead;
    unsigned char m_nGender;
    unsigned char m_nHeadVariation;
    unsigned char m_nUnknown1D;
    unsigned char m_nUnknown1E;
    unsigned char m_nUnknown1F;
    unsigned char m_nUnknown20;
    unsigned char m_nTail;
    unsigned char m_nWing;
    unsigned char m_bUnknown23;
    float m_fCameraHeight;
    float m_fDefaultHeight;
    short m_nAlignment;
    unsigned long m_nUnknown30;
    unsigned char m_nUnknown34;

    int m_bGroundTilt;
};

class CCAppearanceInfo;
class CAurObject;
class CExoString;
class CResRef;
class CSWCCreature;

class CSWCCreatureAppearance : public CCAppearanceInfo
{
public:
    CSWCCreatureAppearance(CSWCCreature* pCreature);

    ~CSWCCreatureAppearance();

    int CreateHead(unsigned char nType, CCAppearanceInfo* pAppearance);
    char GetGenderChar();
    unsigned char GetAnimType(const CExoString& sModelType);
    int RequestSimple(CCAppearanceInfo* pAppearance);
    int RequestHeadAppearance(CCAppearanceInfo* pAppearance);
    int RequestTailAppearance(CCAppearanceInfo* pAppearance);
    int RequestWingAppearance(CCAppearanceInfo* pAppearance);
    float GetHeight();
    CAurObject* GetCameraFollowPart();
    int GetFollowingBody();
    int GetArmourClass();
    float GetHeadHeightOffset();
    float GetCameraOffset();
    float GetRunLooksAwfulRate();
    void GetBTypeModelAndTextureResRef(CCAppearanceInfo* pAppearance,
                                       CResRef* pModel, CResRef* pTexture);
    void ForceAnimate();
    int CreateHeadAppearance(CCAppearanceInfo* pAppearance);
    int CreateTailAppearance(CCAppearanceInfo* pAppearance);
    int CreateWingAppearance(CCAppearanceInfo* pAppearance);
    int RequestBType(CCAppearanceInfo* pAppearance);
    int CreateBTypeBody(unsigned char nType, CCAppearanceInfo* pAppearance);
    void SetAppearanceAlignment(short nAlignment);
    int CreateSimple(unsigned char nType, CCAppearanceInfo* pAppearance);
    int RequestBodyAppearance(CCAppearanceInfo* pAppearance);
    int CreateBodyAppearance(CCAppearanceInfo* pAppearance);

    CSWCCreature* m_pCreature;
    unsigned long m_nUnknown40;
    unsigned long m_nUnknown44;
    unsigned long m_nUnknown48;
    unsigned long m_nUnknown4C;
    unsigned long m_nUnknown50;
    unsigned long m_nUnknown54;

    float m_fDriveAcceleration;
    float m_fDriveMaxSpeed;
    float m_fDriveAnimWalk;
    float m_fDriveAnimRun;
    unsigned long m_bForceAnimate;
};

#endif
