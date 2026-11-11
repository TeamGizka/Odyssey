#ifndef __NWSCLIENT_H__
#define __NWSCLIENT_H__

class CSWSPlayer;

class CSWSClient
{
public:
    CSWSClient(unsigned long nPlayerId);
    virtual ~CSWSClient();
    // PC 0x0063E7F0
    virtual CSWSPlayer* AsSWSPlayer()
    {
        return 0;
    }

    unsigned long GetPlayerId()
    {
        return m_nPlayerId;
    }

    unsigned long m_nPlayerId;
    int m_nLanguage;

public:

    void SetLanguage(int nLanguage)
    {
        m_nLanguage = nLanguage;
    }
    int GetLanguage()
    {
        return m_nLanguage;
    }
};

#endif
