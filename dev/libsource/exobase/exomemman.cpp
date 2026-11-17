#include <string.h>

const char* g_pLastFile = NULL;
int g_iComment = 0;
int g_iLastLine = 0;
char g_szLastComment[1024];

// Dead code: not in the PC executable
int SetFileLine(const char* sFile, int nLine)
{
    g_pLastFile = sFile;
    g_iLastLine = nLine;
    return 0;
}

// Dead code: not in the PC executable
int SetComment(const char* sComment)
{
    strcpy(g_szLastComment, sComment);
    g_iComment = 1;
    return 0;
}
