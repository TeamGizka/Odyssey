#ifndef __SWBEHAVIORCAMERADIALOG_H__
#define __SWBEHAVIORCAMERADIALOG_H__

#include "aurbehavior.h"
#include <aurpointer.h>
#include "math3d.h"
#include "swbehaviorcamera.h"

class CAurObject;
class CAurTexture;

struct CSWCameraLineOfAction
{

    static const unsigned long EMPTY_OBJECT_ID = -1;

    unsigned long m_oidFirst;
    unsigned long m_oidSecond;
    int m_nSide;
};

class CSWBehaviorCameraDialog : public CAurBehavior
{
public:

    static const int INITIAL_CAMERA_STATE = 1;

    static const int FIXED_CAMERA_SHOT = 6;

    enum
    {
        CLOSE_UP_CAMERA_SHOT = 1,
        OVER_SHOULDER_CAMERA_SHOT = 2,
        TWO_PERSON_CAMERA_SHOT = 3
    };

    static const int DEFAULT_DIALOG_VIEW_ANGLE = 55;

    float m_fFirstHeight;
    float m_fSecondHeight;
    float m_fFirstOffset;
    float m_fSecondOffset;
    float GetSecondOffset() { return m_fSecondOffset; }
    float m_fCameraYaw;
    int m_nShot;
    float m_fYawOffset;
    float m_fPitchOffset;
    int m_nActiveShot;
    int m_nDialogType;
    int m_nCameraState;
    Vector m_vCameraPosition;
    Quaternion m_qCameraOrientation;
    unsigned long m_oidFirst;
    unsigned long m_oidSecond;
    SafePointer<CAurObject> m_pFirstObject;
    SafePointer<CAurObject> m_pSecondObject;
    CSWCameraLineOfAction m_aLineOfAction[4];

    virtual ~CSWBehaviorCameraDialog() throw();
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // PC 0x006BB740  Xbox 0x000F8080
    {
        return AUR_BEHAVIOR_CAMERA_DIALOG;
    }

    CSWBehaviorCameraDialog(CAurObject* pObject, int nDialogType);
    void SetShot(int nShot, Vector vPosition, Quaternion qOrientation,
                 float fCameraYaw);
    int GetLineOfActionSide(unsigned long oidFirst, unsigned long oidSecond);
    void GetCameraParameters(unsigned long nShot, unsigned long nSide,
                            Vector vFirst, Vector vSecond, Vector& vPosition,
                            Quaternion& qOrientation);
    int GetSideUnobstructed();
    int AddCalculatedLineOfAction(unsigned long oidFirst,
                                  unsigned long oidSecond);
    void SetShot(unsigned long oidFirst, unsigned long oidSecond,
                int const& nShot, float const& fFirstHeight,
                float const& fSecondHeight, float fCameraYaw);
};

#endif
