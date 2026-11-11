#ifndef __LINUXSPECIFIC_H__
#define __LINUXSPECIFIC_H__

#include <stddef.h>
#include <string.h>
#include <unistd.h>

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef int BOOL;
typedef long LONG;
typedef char CHAR;
typedef unsigned char UCHAR;

typedef void* HANDLE;
typedef void* HINSTANCE;
typedef void* HWND;
typedef void* HMODULE;
typedef void* HICON;
typedef void* HCURSOR;
typedef void* HBRUSH;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

inline int stricmp(const char* s1, const char* s2)
{
    return strcasecmp(s1, s2);
}

inline int strnicmp(const char* s1, const char* s2, size_t n)
{
    return strncasecmp(s1, s2, n);
}

#endif
