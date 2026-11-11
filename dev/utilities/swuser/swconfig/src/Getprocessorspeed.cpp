#include <vcl.h>
#pragma hdrstop

#include "Getprocessorspeed.h"
#include "Nwcsysinfo.h"
#include <Sysutils.hpp>

#pragma package(smart_init)

TVersion::TVersion(int a, int b, int c, int d) throw()
{
    Part[0] = a;
    Part[1] = b;
    Part[2] = c;
    Part[3] = d;
}

int* GetVersionQuad(int Quad[4], AnsiString FileName)
{
    AnsiString ext = ExtractFileExt(FileName).Delete(1, 1);

    if (ext == "exe" || ext == "dll")
    {
        AnsiString version = GetFileVersionString(FileName, "FileVersion");
        bool missing = false;
        int dot = version.AnsiPos(".");
        if (dot == 0)
            missing = true;
        if (!missing)
        {
            try
            {
                Quad[0] = version.SubString(1, dot - 1).ToInt();
            }
            catch (Sysutils::EConvertError&)
            {
                missing = true;
            }
        }
        version = version.SubString(dot + 1, version.Length() - dot);
        dot = version.AnsiPos(".");
        if (dot == 0)
            missing = true;
        if (!missing)
        {
            try
            {
                Quad[1] = version.SubString(1, dot - 1).ToInt();
            }
            catch (Sysutils::EConvertError&)
            {
                missing = true;
            }
        }
        version = version.SubString(dot + 1, version.Length() - dot);
        dot = version.AnsiPos(".");
        if (dot == 0)
            missing = true;
        if (!missing)
        {
            try
            {
                Quad[2] = version.SubString(1, dot - 1).ToInt();
            }
            catch (Sysutils::EConvertError&)
            {
                missing = true;
            }
        }
        version = version.SubString(dot + 1, version.Length() - dot);
        if (version.Length() == 0)
            missing = true;
        if (!missing)
        {
            try
            {
                Quad[3] = version.ToInt();
            }
            catch (Sysutils::EConvertError&)
            {
                missing = true;
            }
        }
        if (missing)
        {
            TDateTime stamp = FileDateToDateTime(FileAge(FileName));
            AnsiString time = stamp.FormatString("hh") + stamp.FormatString("nn") +
                              stamp.FormatString("ss");
            Quad[0] = stamp.FormatString("yyyy").ToInt();
            Quad[1] = stamp.FormatString("m").ToInt();
            Quad[2] = stamp.FormatString("d").ToInt();
            Quad[3] = time.ToInt();
        }
    }
    else
    {
        TDateTime stamp = FileDateToDateTime(FileAge(FileName));
        AnsiString time = stamp.FormatString("hh") + stamp.FormatString("nn") +
                          stamp.FormatString("ss");
        Quad[0] = stamp.FormatString("yyyy").ToInt();
        Quad[1] = stamp.FormatString("m").ToInt();
        Quad[2] = stamp.FormatString("d").ToInt();
        Quad[3] = time.ToInt();
    }
    return Quad;
}

TVersion& TVersion::operator=(const TVersion& rhs)
{
    Part[0] = rhs.Part[0];
    Part[1] = rhs.Part[1];
    Part[2] = rhs.Part[2];
    Part[3] = rhs.Part[3];
    return *this;
}

bool TVersion::operator<(const TVersion& rhs) const
{
    if (Part[0] < rhs.Part[0])
        return true;
    if (Part[0] > rhs.Part[0])
        return false;
    if (Part[1] < rhs.Part[1])
        return true;
    if (Part[1] > rhs.Part[1])
        return false;
    if (Part[2] < rhs.Part[2])
        return true;
    if (Part[2] > rhs.Part[2])
        return false;
    if (Part[3] < rhs.Part[3])
        return true;
    if (Part[3] > rhs.Part[3])
        return false;
    return false;
}

bool TVersion::operator>(const TVersion& rhs) const
{
    if (Part[0] > rhs.Part[0])
        return true;
    if (Part[0] < rhs.Part[0])
        return false;
    if (Part[1] > rhs.Part[1])
        return true;
    if (Part[1] < rhs.Part[1])
        return false;
    if (Part[2] > rhs.Part[2])
        return true;
    if (Part[2] < rhs.Part[2])
        return false;
    if (Part[3] > rhs.Part[3])
        return true;
    if (Part[3] < rhs.Part[3])
        return false;
    return false;
}

bool TVersion::operator>=(const TVersion& rhs) const
{
    if (Part[0] > rhs.Part[0])
        return true;
    if (Part[0] < rhs.Part[0])
        return false;
    if (Part[1] > rhs.Part[1])
        return true;
    if (Part[1] < rhs.Part[1])
        return false;
    if (Part[2] > rhs.Part[2])
        return true;
    if (Part[2] < rhs.Part[2])
        return false;
    if (Part[3] > rhs.Part[3])
        return true;
    if (Part[3] < rhs.Part[3])
        return false;
    return true;
}

static __int64 MeasureRDTSC(void(__cdecl* fn)(unsigned int), unsigned int arg)
{
    unsigned int startHi, startLo, endHi, endLo;
    try
    {
        asm {
            push    arg
            mov     ebx, fn
            db      0Fh, 31h
            mov     esi, eax
            mov     edi, edx
            call    ebx
            db      0Fh, 31h
            pop     ebx
            mov     endHi, edx
            mov     endLo, eax
            mov     startHi, edi
            mov     startLo, esi
        }
    }
    catch (...)
    {
        return -1;
    }
    return (((__int64)endHi << 32) + endLo)
         - (((__int64)startHi << 32) + startLo);
}

static void __cdecl DelayQPC_atLeast(unsigned int ms)
{
    LARGE_INTEGER freq, start, now;
    __int64 target;
    if (!QueryPerformanceFrequency(&freq))
        return;
    target = freq.QuadPart / 1000 * ms;
    QueryPerformanceCounter(&start);
    do
    {
        QueryPerformanceCounter(&now);
    } while (now.QuadPart - start.QuadPart < target);
}

static void __cdecl DelayQPC_exact(unsigned int ms)
{
    LARGE_INTEGER freq, start, now;
    __int64 target;
    if (!QueryPerformanceFrequency(&freq))
        return;
    target = freq.QuadPart / 1000 * ms;
    QueryPerformanceCounter(&start);
    do
    {
        QueryPerformanceCounter(&now);
    } while (now.QuadPart - start.QuadPart == target);
}

int GetProcessorSpeed()
{
    unsigned int n = 1;
    unsigned int ms = 50;
    __int64 busy = 0;
    __int64 idle = 0;
    for (unsigned int i = 0; i < n; ++i)
    {
        busy += MeasureRDTSC(DelayQPC_atLeast, ms);
        idle += MeasureRDTSC(DelayQPC_exact, ms);
    }
    busy -= idle;
    busy /= n;
    busy /= ms;
    busy /= 1000;
    return busy;
}

static void AsmTail()
{
    asm {
        nop
    }
}
