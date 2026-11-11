#include "exobase.h"
#include "serverexoapp.h"

static CExoArrayList<CExoString> s_lstBannedListIP;
static CExoArrayList<CExoString> s_lstBannedListCDKey;
static CExoArrayList<CExoString> s_lstBannedListPlayerName;
static int s_bReloadModuleWhenEmpty = 0;

static void AddToBannedList(CExoArrayList<CExoString>& lstBanned, const CExoString& sEntry)
{
    int i;

    for (i = 0; i < lstBanned.num; i++)
    {
        if (lstBanned[i] == sEntry)
        {
            return;
        }
    }
    lstBanned.Add(sEntry);
}

static void RemoveFromBannedList(CExoArrayList<CExoString>& lstBanned, const CExoString& sEntry)
{
    int i;

    for (i = 0; i < lstBanned.num; i++)
    {
        if (lstBanned[i] == sEntry)
        {
            lstBanned.DelIndex(i);
            return;
        }
    }
}

static void AppendBannedList(CExoString& sList, const char* szTitle,
                             CExoArrayList<CExoString>& lstBanned)
{
    int i;

    sList = sList + CExoString(szTitle) + CExoString("\n");
    for (i = 0; i < lstBanned.num; i++)
    {
        sList = sList + lstBanned[i] + CExoString("\n");
    }
}

void CServerExoApp::SetReloadModuleWhenEmpty(int bReload)
{
    s_bReloadModuleWhenEmpty = bReload;
}

CExoString CServerExoApp::GetBannedListString()
{
    CExoString sList;

    AppendBannedList(sList, "Banned IP Addresses:", s_lstBannedListIP);
    AppendBannedList(sList, "Banned Public CD Keys:", s_lstBannedListCDKey);
    AppendBannedList(sList, "Banned Player Names:", s_lstBannedListPlayerName);
    return sList;
}

void CServerExoApp::AddIPToBannedList(CExoString sIP)
{
    AddToBannedList(s_lstBannedListIP, sIP);
}

void CServerExoApp::AddPlayerNameToBannedList(CExoString sPlayerName)
{
    AddToBannedList(s_lstBannedListPlayerName, sPlayerName);
}

void CServerExoApp::AddCDKeyToBannedList(CExoString sCDKey)
{
    AddToBannedList(s_lstBannedListCDKey, sCDKey);
}

void CServerExoApp::RemoveIPFromBannedList(CExoString sIP)
{
    RemoveFromBannedList(s_lstBannedListIP, sIP);
}

void CServerExoApp::RemovePlayerNameFromBannedList(CExoString sPlayerName)
{
    RemoveFromBannedList(s_lstBannedListPlayerName, sPlayerName);
}

void CServerExoApp::RemoveCDKeyFromBannedList(CExoString sCDKey)
{
    RemoveFromBannedList(s_lstBannedListCDKey, sCDKey);
}
