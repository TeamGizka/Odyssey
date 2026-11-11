#ifndef __SWGUISKILLFLOWCHART_H__
#define __SWGUISKILLFLOWCHART_H__

#include "swguicontrol.h"
#include "swguidrawable.h"
#include "swgui.h"
class CSWCCreatureStats;
class CSWGuiExtent;
class CSWGuiManager;
class CSWSCreatureStats;

class CSWGuiInGameSkillEntry : public CSWGuiLabelHilight
{
public:
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void Initialize(const CSWGuiExtent& cExtent,  // PC 0x006049D0
                                           CSWGuiLabelHilight* pProto)
    {
    }
    virtual void Initialize(const CSWGuiExtent& cExtent,  // PC 0x00693D50
                            const CSWGuiTextParams& cText,
                            const CSWGuiBorderParams& cBorder,
                            const CSWGuiBorderParams& cHilite)
    {
    }
    virtual void Initialize(CSWGuiText& cText, CSWGuiBorder& cBorder,
                            CSWGuiBorder& cHilite, int nSkill);

    void SetSkill(unsigned long nSkill, unsigned char nState);

    enum AbilityType { ABILITY_SKILL = 0, ABILITY_FEAT = 1, ABILITY_POWER = 2 };

    CSWGuiBorder m_cNormal;
    CSWGuiBorder m_cHilite;
    CSWGuiBorder m_cSelection;
};

class CSWGuiSkillInfoBox : public CSWGuiPanel
{
public:
    CSWGuiSkillInfoBox(CSWGuiManager* pManager);
    virtual ~CSWGuiSkillInfoBox();
    virtual void HandleInputEvent(int nEvent, int nState);
    void SetSkillList(const CExoArrayList<unsigned long>& lSkills,
                      unsigned char nState);
    void SetMessage(unsigned long nStrRef)
    {
        m_lblTitle.m_cText.m_cParams.SetStrRef(nStrRef);
    }

    CSWGuiListBox m_lbSkills;
    CSWGuiLabel m_lblTitle;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblOk;
#else
    CSWGuiButton m_btnExit;
#endif

    enum { MAX_SKILL_ENTRIES = 10 };
    CSWGuiInGameSkillEntry m_aSkillEntries[MAX_SKILL_ENTRIES];
};

class CSWGuiFlowSkillStruct
{
public:
    CSWGuiBorder m_cBorder;
    CSWGuiImage m_cLeftImage;
    CSWGuiImage m_cRightImage;
    unsigned long m_nSkill;
    unsigned char m_nStatus;
    unsigned char m_aUnmapped121[3];
    unsigned long m_bSelected : 1;
    unsigned long m_nUnmapped124 : 31;
    // PC 0x0073ABF0
    CSWGuiFlowSkillStruct()
    {
    }
    ~CSWGuiFlowSkillStruct()  // PC 0x0073AC40
    {
    }
};

struct CSWGuiSkillFlowCell
{
    unsigned char m_aUnmapped00[0x50];
    int m_nSkill;
    unsigned char m_aUnmapped54[0xD4];
};

class CSWGuiSkillFlow : public CSWGuiControl
{
public:

    static const unsigned long SKILL_INVALID = -1;

    CSWGuiSkillFlow();
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    SWGUI_PCVIRTUAL CSWGuiSkillFlow* AsSkillFlow()  // PC 0x00698BD0
    {
        return this;
    }

    void SetSkillStatus(unsigned char nSkill, unsigned char nStatus);

    enum SkillStatus
    {
        STATUS_AVAILABLE = 0,
        STATUS_GRANTED = 1,
        STATUS_GRANTED_ALTERNATE = 2,
        STATUS_UNAVAILABLE = 3,
        STATUS_CHOSEN = 4,
        STATUS_YELLOW_DIM_ARROW = 5,
        STATUS_YELLOW = 6
    };
    unsigned int SkillHitCheckMouse(int nX, int nY);
    void SetSkillSelected(unsigned char nSkill, int bSelected);

    void SetSkillInfo(unsigned char nIndex, unsigned long nSkill,
                      const CResRef& resIcon)
    {
        m_aSkills[nIndex].m_nSkill = nSkill;
        m_aSkills[nIndex].m_cLeftImage.m_cParams.SetImage(resIcon, 0);
    }

    CSWGuiFlowSkillStruct m_aSkills[3];
    CSWGuiImage m_aArrows[2];
};

class CSWGuiSkillFlowChart
{
public:
    CSWGuiSkillFlowChart();
    ~CSWGuiSkillFlowChart();
    int ClearChart();
    void SetSkillStatus(unsigned long nSkill, unsigned char nStatus);
    int SetSkillStatusAll(unsigned char nStatus);
    int SetTopLeft();
    int HandleInput(int nEvent);
    void CreatePowerChart(CSWSCreatureStats* pStats, int bAllPowers);
    void CreateFeatChart(CSWSCreatureStats* pStats);
    void CreateFeatChart(CSWCCreatureStats* pStats, int bBonusFeats);
    void BuildSortedFeatList(CExoArrayList<unsigned short>& cFeats);
    void BuildSortedPowerList(CExoArrayList<unsigned long>& lstPowers);

    int GetSelected();
    void SetSelectedSkill(unsigned long nSkill);
    void SetNumColumns(unsigned char nColumns);
    void Add(unsigned long nSkill, unsigned char nState);
    void AddPowerSet(unsigned long nPower1, unsigned long nPower2,
                     unsigned long nPower3);
    void AddFeatSet(unsigned long nFeat1, unsigned long nFeat2,
                    unsigned long nFeat3);
    unsigned long GetFirst();

    CExoArrayList<CSWGuiSkillFlow*> m_cFlows;
    unsigned char m_nSelectedCol;
    unsigned char m_nSelectedRow;
    unsigned char m_aUnmapped0E[2];
};

#endif
