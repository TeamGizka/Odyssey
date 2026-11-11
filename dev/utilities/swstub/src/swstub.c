#include <windows.h>
#include <process.h>
#include <string.h>

int nSpawnMode = _P_OVERLAY;
// PC 0x004041F0
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    HKEY hKey;
    DWORD dwType;
    DWORD dwDataSize;
    char szPath[MAX_PATH];
    WIN32_FIND_DATAA FindFileData;
    HANDLE hFindFile;

    if (strcmp(lpCmdLine, "") == 0)
        return 0;

    strcpy(szPath, "");
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\BioWare\\SW\\Kotor", 0,
                 KEY_READ, &hKey);
    dwType = REG_SZ;
    dwDataSize = MAX_PATH;
    if (RegQueryValueEx(hKey, "Path", NULL, &dwType, szPath, &dwDataSize) == 0)
        RegCloseKey(hKey);
    if (strcmp(szPath, "") != 0)
        strcat(szPath, "\\");
    strcat(szPath, "swupdate.new");

    Sleep(1000);
    hFindFile = FindFirstFile(szPath, &FindFileData);
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFindFile);

        SetFileAttributes("swupdate.bak", FILE_ATTRIBUTE_NORMAL);
        SetFileAttributes("swupdate.new", FILE_ATTRIBUTE_NORMAL);
        SetFileAttributes("swupdate.exe", FILE_ATTRIBUTE_NORMAL);
        DeleteFile("swupdate.bak");
        CopyFile("swupdate.exe", "swupdate.bak", FALSE);
        if (!CopyFile("swupdate.new", "swupdate.exe", FALSE))
        {
            int bRetry = 1;

            while (bRetry)
            {
                Sleep(1000);
                if (CopyFile("swupdate.new", "swupdate.exe", FALSE))
                    bRetry = 0;
            }
        }
        DeleteFile("swupdate.bak");
        DeleteFile("swupdate.new");
        _spawnl(nSpawnMode, "swupdate.exe", "swupdate.exe", "success",
                lpCmdLine, NULL);
    }
    _spawnl(nSpawnMode, "swupdate.exe", "swupdate.exe", "failure1", lpCmdLine,
            NULL);
    return 1;
}
