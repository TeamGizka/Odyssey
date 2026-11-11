#ifndef AURTEXCONTROL_H
#define AURTEXCONTROL_H

#include <aurlist.h>

class CAurTexture;

#define DISTORT_ANGLE_SCALE 0.024543693f

class TextureController
{
public:
    TextureController(CAurTexture* pTexture);
    virtual ~TextureController();
    virtual void Control() = 0;
    virtual void Init() = 0;
    virtual void ParseField(char* sField);
#ifdef _XBOX

    virtual bool IsLinear();
#else
    virtual char IsLinear();
#endif
    virtual unsigned char* Distort(unsigned char* pImage);
    virtual void SetImage();

    enum
    {
        DISTORT_HEIGHT_BIAS = 127,
        DISTORT_HEIGHT_RANGE = 128,
        DISTORT_ANGLE_RANGE = 256
    };

    enum
    {
        CHANNEL_BYTE_COUNT = 4,
        DEFAULT_CHANNEL_COUNT = 4,
        CHANNEL_BYTE_MAX = 255,
        CHANNEL_NIBBLE_BITS = 4,
        CHANNEL_NIBBLE_MAX = 15,
        CHANNEL_LOW_MASK = 0x0f,
        CHANNEL_HIGH_MASK = 0xf0
    };

    unsigned char* Height2Channel(unsigned char* pHeight,
                                  unsigned char* pImage);

    CAurTexture* m_pTexture;
    int m_nLastUpdate;
    int m_bDistort;
    int m_nDistortAngle;
    float m_fDistortAmplitude;
    float m_fDistortSpeed;
    List<float> m_lstChannelScale;
    List<float> m_lstChannelTranslate;
    unsigned char* m_pImage;
    static unsigned char newimage[262144];
};

class LifeTextureController : public TextureController
{
public:
    LifeTextureController(CAurTexture* pTexture);
    virtual ~LifeTextureController();
    virtual void Control();
    virtual void Init();

    enum
    {
        LIFE_UPDATE_FRAMES = 5,
        LIFE_COLOR_STEP = 4,
        LIFE_DEATH_COLOR = 128,
        LIFE_SURVIVE_NEIGHBORS = 2,
        LIFE_BIRTH_NEIGHBORS = 3
    };

    unsigned char* m_pState;
};

class PerlinTextureController : public TextureController
{
public:
    PerlinTextureController(CAurTexture* pTexture);
    virtual ~PerlinTextureController();
    virtual void Control();
    virtual void Init();
    // PC 0x00463310
    virtual void ParseField(char* sField)
    {
        TextureController::ParseField(sField);
    }
    virtual void BuildPerlinImage(unsigned char* pImage, int nSize);

    unsigned char* m_pPerlinImage;
    unsigned char* m_pWorkImage;
    float m_fElapsed;
};

class RandomTextureController : public TextureController
{
public:
    RandomTextureController(CAurTexture* pTexture);
    virtual ~RandomTextureController();
    virtual void Control();
    virtual void Init();

    unsigned char* m_pRandomImage;
    unsigned char* m_pChannelImage;
};

class RingDistortTextureController : public TextureController
{
public:
    RingDistortTextureController(CAurTexture* pTexture);
    virtual ~RingDistortTextureController();
    virtual void Control();
    virtual void Init();
#ifdef _XBOX  // PC 0x005A4CF0  Xbox 0x0027E2B0
    virtual bool IsLinear()
#else
    virtual char IsLinear()
#endif
    {
        return 1;
    }

    unsigned char* m_pRingImage;
    float m_fAngle;
};

#ifdef _XBOX

class HeatDistortTextureController : public TextureController
{
public:
    HeatDistortTextureController(CAurTexture* pTexture);
    virtual ~HeatDistortTextureController();
    virtual void Control();
    virtual void Init();
    virtual bool IsLinear()
    {
        return 1;
    }

    unsigned char* m_pHeatImage;
    float m_fAngle;
};
#endif

class WaterTextureController : public TextureController
{
public:
    WaterTextureController(CAurTexture* pTexture);
    virtual ~WaterTextureController()  // PC 0x00463120  Xbox 0x002D0310
    {
        delete[] m_pfHeight;
        delete[] m_pfVelocity;
        delete[] m_pfNextHeight;
        delete[] m_pWaterImage;
        delete[] m_pDistortImage;
    }
    virtual void Control();
    virtual void Init();
    virtual void ParseField(char* sField);

    int m_nWaterWidth;
    int m_nWaterHeight;
    float* m_pfHeight;
    float* m_pfNextHeight;
    float* m_pfVelocity;

    float m_fForceAngle;
    float m_fWaveAngle;
    float m_fForceCycleSpeed;
    float m_fAngleCycleSpeed;
    unsigned char* m_pWaterImage;
    unsigned char* m_pDistortImage;
    int m_nUnknown64;
    int m_nWaveType;
};

class WaveTextureController : public TextureController
{
public:
    WaveTextureController(CAurTexture* pTexture);
    virtual ~WaveTextureController()  // PC 0x004632B0  Xbox 0x002D04A0
    {
        delete[] m_pfWaveImage;
        delete[] m_pfOffsetImage;
    }
    virtual void Control();
    virtual void Init();
    virtual void ParseField(char* sField);

    float* m_pfWaveImage;
    float* m_pfOffsetImage;
};

#define ARTURO_SPEED_SCALE 1.5f

class ArturoTextureController : public TextureController
{
public:
    ArturoTextureController(CAurTexture* pTexture);
    virtual ~ArturoTextureController()  // PC 0x00463380  Xbox 0x002D0570
    {
        delete[] m_pfArturoCurrent;
        delete[] m_pfArturoPrevious;
        delete[] m_pfArturoNext;
        delete[] m_pArturoIndex;
    }
    virtual void Control();
    virtual void Init();
    virtual void ParseField(char* sField);
    virtual void BuildArturoImage();

    enum
    {
        ARTURO_FIRST_HALF = 0,
        ARTURO_SECOND_HALF = 1,
        ARTURO_UPLOAD = 2,
        ARTURO_STAGE_COUNT = 3,
        ARTURO_HEIGHT_BIAS = 127,

        ARTURO_IMPULSE_RANGE = 256,
        ARTURO_IMPULSE_CENTER = 128,
        ARTURO_CHANNEL_MAX = 255
    };

    int m_nArturoWidth;
    int m_nArturoHeight;
    float* m_pfArturoNext;
    float* m_pfArturoCurrent;
    float* m_pfArturoPrevious;
    float m_fArturoFrame;
    unsigned char* m_pArturoIndex;
};

class CycleTIDTextureController : public TextureController
{
public:
    CycleTIDTextureController(CAurTexture* pTexture);
    virtual ~CycleTIDTextureController()  // PC 0x004634C0  Xbox 0x002D06C0
    {
    }
    virtual void Control();
    virtual void Init();
#ifdef _XBOX  // PC 0x0041F250
    virtual bool IsLinear()
#else
    virtual char IsLinear()
#endif
    {
        return 0;
    }
    virtual void ParseField(char* sField);

    float m_fCycleRate;
    int m_nLastTID;
    float m_fElapsed;
};

#endif
