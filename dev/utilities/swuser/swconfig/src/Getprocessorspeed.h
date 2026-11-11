#ifndef GetprocessorspeedH
#define GetprocessorspeedH

#include <System.hpp>

struct TVersion
{
    int Part[4];

    TVersion() {}

    TVersion(const int* p)
    {
        Part[0] = p[0];
        Part[1] = p[1];
        Part[2] = p[2];
        Part[3] = p[3];
    }

    TVersion(int a, int b, int c, int d) throw();

    TVersion& operator=(const TVersion& rhs);
    bool operator<(const TVersion& rhs) const;
    bool operator>(const TVersion& rhs) const;
    bool operator>=(const TVersion& rhs) const;
};

int* GetVersionQuad(int Quad[4], AnsiString FileName);

int GetProcessorSpeed();

#endif
