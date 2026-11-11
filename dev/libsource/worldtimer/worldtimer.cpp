#include "exobase.h"
#include "worldtimer.h"

const unsigned long CWorldTimer::MIN_YEAR = 0;
const unsigned long CWorldTimer::DEFAULT_YEAR = 1340;
const unsigned long CWorldTimer::MAX_YEAR = 32000;
const unsigned long CWorldTimer::MAX_YEAR_LIMIT = 32767;
const unsigned long CWorldTimer::MIN_MONTH = 1;
const unsigned long CWorldTimer::DEFAULT_MONTH = 6;
const unsigned long CWorldTimer::MAX_MONTH = 12;
const unsigned long CWorldTimer::MIN_DAY = 1;
const unsigned long CWorldTimer::DEFAULT_DAY = 1;
const unsigned long CWorldTimer::MAX_DAY = 28;
const unsigned long CWorldTimer::MIN_HOUR = 0;
const unsigned long CWorldTimer::DEFAULT_HOUR = 23;
const unsigned long CWorldTimer::MAX_HOUR = 23;
const unsigned long CWorldTimer::MIN_MINUTE = 0;
const unsigned long CWorldTimer::DEFAULT_MINUTE = 59;
const unsigned long CWorldTimer::MAX_MINUTE = 59;
const unsigned long CWorldTimer::MIN_SECOND = 0;
const unsigned long CWorldTimer::DEFAULT_SECOND = 56;
const unsigned long CWorldTimer::MAX_SECOND = 59;
const unsigned long CWorldTimer::MIN_MILLISECOND = 0;
const unsigned long CWorldTimer::DEFAULT_MILLISECOND = 0;
const unsigned long CWorldTimer::MAX_MILLISECOND = 999;
const unsigned long CWorldTimer::MONTHS_IN_YEAR = 12;
const unsigned long CWorldTimer::DAYS_IN_MONTH = 28;
const unsigned long CWorldTimer::DAYS_IN_YEAR = 336;
const unsigned long CWorldTimer::HOURS_IN_DAY = 24;
const unsigned long CWorldTimer::SECONDS_IN_MINUTE = 60;
const unsigned long CWorldTimer::MILLISECONDS_IN_SECOND = 1000;
const unsigned long CWorldTimer::MILLISECONDS_IN_MINUTE = 60000;
const unsigned long CWorldTimer::MICROSECONDS_IN_MILLISECOND = 1000;
const unsigned long CWorldTimer::WORLD_TIME_INVALID = 0xfffffffeUL;
// PC 0x004015A0
CWorldTimer::~CWorldTimer()
{
}
// PC 0x004ADBA0  Xbox 0x00276A90
void CWorldTimer::SetMinutesPerHour(unsigned char nMinutesPerHour)
{
    m_nMinutesPerHour = nMinutesPerHour;
    if (nMinutesPerHour == 0)
    {
        m_nMinutesPerHour = 5;
    }
    m_nSecondsPerDay = 1440 * m_nMinutesPerHour;
    m_nMillisecondsPerDay = 1440000 * m_nMinutesPerHour;
}
// PC 0x004ADBD0  Xbox 0x00276AC0
void CWorldTimer::SetSnapshotTime()
{
    unsigned __int64 nSnapshotTime = m_nSnapshotTime;
    m_nPreviousSnapshotTime = nSnapshotTime;
    if (m_bFixedSnapshotRate == 0)
    {
        unsigned __int64 nTimer =
            g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        m_nSnapshotTime += (nTimer - m_nLastTimer) * m_nSnapshotScale / 100;
        m_nLastTimer = nTimer;
    }
    else
    {
        m_nSnapshotTime = nSnapshotTime + m_nSnapshotScale *
                                              (1000000 / m_nFixedSnapshotRate) /
                                              100;
    }
}
// PC 0x004ADC70  Xbox 0x00276B60
unsigned __int64 CWorldTimer::GetSnapshotTime()
{
    return m_nSnapshotTime;
}
// PC 0x004ADC80  Xbox 0x00276B70
unsigned __int64 CWorldTimer::GetSnapshotTimeDifference()
{
    if (m_bPaused != 0)
    {
        return 0;
    }
    return m_nSnapshotTime - m_nPreviousSnapshotTime;
}
// PC 0x004ADCA0  Xbox 0x00276B90
unsigned long CWorldTimer::ConvertToCalendarDay(unsigned long nYear,
                                                unsigned long nMonth,
                                                unsigned long nDay)
{
    unsigned long nCalendarDay;
    if (nYear <= MAX_YEAR)
    {
        nCalendarDay = DAYS_IN_YEAR * nYear;
    }
    else
    {
        nCalendarDay = DAYS_IN_YEAR * DEFAULT_YEAR;
    }
    if (nMonth >= MIN_MONTH && nMonth <= MAX_MONTH)
    {
        nCalendarDay += DAYS_IN_MONTH * (nMonth - 1);
    }
    else
    {
        nCalendarDay += DAYS_IN_MONTH * (DEFAULT_MONTH - 1);
    }
    if (nDay >= MIN_DAY && nDay <= MAX_DAY)
    {
        nCalendarDay += nDay - 1;
    }
    return nCalendarDay;
}
// PC 0x004ADCF0  Xbox 0x00276BE0
unsigned long CWorldTimer::ConvertToTimeOfDay(unsigned long nHour,
                                              unsigned long nMinute,
                                              unsigned long nSecond,
                                              unsigned long nMillisecond)
{
    unsigned long nTimeOfDay;
    if (nHour <= MAX_HOUR)
    {
        nTimeOfDay = MILLISECONDS_IN_MINUTE * nHour * m_nMinutesPerHour;
    }
    else
    {
        nTimeOfDay = MILLISECONDS_IN_MINUTE * DEFAULT_HOUR * m_nMinutesPerHour;
    }
    if (nMinute <= MAX_MINUTE)
    {
        nTimeOfDay += MILLISECONDS_IN_MINUTE * nMinute;
    }
    else
    {
        nTimeOfDay += MILLISECONDS_IN_MINUTE * DEFAULT_MINUTE;
    }
    if (nSecond <= MAX_SECOND)
    {
        nTimeOfDay += MILLISECONDS_IN_SECOND * nSecond;
    }
    else
    {
        nTimeOfDay += MILLISECONDS_IN_SECOND * DEFAULT_SECOND;
    }
    return nTimeOfDay + nMillisecond;
}
// PC 0x004ADD50  Xbox 0x00276C40
unsigned long CWorldTimer::ConvertFromCalendarDay(unsigned long nCalendarDay,
                                                  unsigned long* pnYear,
                                                  unsigned long* pnMonth,
                                                  unsigned long* pnDay)
{
    *pnDay = nCalendarDay % DAYS_IN_MONTH + 1;
    unsigned long nYear = nCalendarDay / DAYS_IN_MONTH / MONTHS_IN_YEAR;
    *pnMonth = nCalendarDay / DAYS_IN_MONTH % MONTHS_IN_YEAR + 1;
    *pnYear = nYear;
    if (nYear > MAX_YEAR_LIMIT)
    {
        *pnYear = DEFAULT_YEAR;
    }
    return nYear;
}
// PC 0x004ADD90  Xbox 0x00276C80
unsigned long CWorldTimer::ConvertFromTimeOfDay(unsigned long nTimeOfDay,
                                                unsigned long* pnHour,
                                                unsigned long* pnMinute,
                                                unsigned long* pnSecond,
                                                unsigned long* pnMillisecond)
{
    *pnMillisecond = nTimeOfDay % MILLISECONDS_IN_SECOND;
    *pnSecond = nTimeOfDay / MILLISECONDS_IN_SECOND % SECONDS_IN_MINUTE;
    *pnMinute = nTimeOfDay / MILLISECONDS_IN_SECOND / SECONDS_IN_MINUTE % m_nMinutesPerHour;
    unsigned long nHour = nTimeOfDay / MILLISECONDS_IN_SECOND / SECONDS_IN_MINUTE / m_nMinutesPerHour;
    *pnHour = nHour;
    return nHour;
}
// PC 0x004ADDE0  Xbox 0x00276CD0
void CWorldTimer::SetWorldTime(unsigned long nCalendarDay,
                               unsigned long nTimeOfDay, int bUpdateSnapshot)
{
    if (bUpdateSnapshot == 1)
    {
        SetSnapshotTime();
    }
    unsigned __int64 nMilliseconds = m_nSnapshotTime / MICROSECONDS_IN_MILLISECOND;
    m_nCalendarDayOffset = nCalendarDay - nMilliseconds / m_nMillisecondsPerDay;
    m_nTimeOfDayOffset = nTimeOfDay - nMilliseconds % m_nMillisecondsPerDay;
}
// PC 0x004ADE40  Xbox 0x00276D30
void CWorldTimer::GetWorldTime(unsigned long* pnCalendarDay,
                               unsigned long* pnTimeOfDay)
{
    unsigned __int64 nSnapshotTime = m_nSnapshotTime;
    if (m_bPaused == 1)
    {
        *pnCalendarDay = m_nPausedCalendarDay;
        *pnTimeOfDay = m_nPausedTimeOfDay;
    }
    else
    {
        unsigned __int64 nMilliseconds = nSnapshotTime / MICROSECONDS_IN_MILLISECOND;
        unsigned __int64 nMsPerDay = m_nMillisecondsPerDay;
        *pnCalendarDay = nMilliseconds / nMsPerDay + m_nCalendarDayOffset;
        *pnTimeOfDay = nMilliseconds % nMsPerDay + m_nTimeOfDayOffset;
    }
    while (*pnTimeOfDay > 0xFFFFFFFF - m_nMillisecondsPerDay)
    {
        --*pnCalendarDay;
        *pnTimeOfDay += m_nMillisecondsPerDay;
    }
    while (*pnTimeOfDay >= m_nMillisecondsPerDay)
    {
        ++*pnCalendarDay;
        *pnTimeOfDay -= m_nMillisecondsPerDay;
    }
}
// PC 0x004ADEF0  Xbox 0x00276DE0
void CWorldTimer::ResetTimer(CWorldTimer* pTimer)
{
    m_bFixedSnapshotRate = pTimer->m_bFixedSnapshotRate;
    m_nFixedSnapshotRate = pTimer->m_nFixedSnapshotRate;
    m_nSnapshotTime = pTimer->m_nSnapshotTime;
    m_nPreviousSnapshotTime = pTimer->m_nPreviousSnapshotTime;
    m_bPaused = pTimer->m_bPaused;
    m_nPausedCalendarDay = pTimer->m_nPausedCalendarDay;
    m_nPausedTimeOfDay = pTimer->m_nPausedTimeOfDay;
    m_nCalendarDayOffset = pTimer->m_nCalendarDayOffset;
    m_nTimeOfDayOffset = pTimer->m_nTimeOfDayOffset;
    m_nMinutesPerHour = pTimer->m_nMinutesPerHour;
    m_nMillisecondsPerDay = pTimer->m_nMillisecondsPerDay;
    m_nSecondsPerDay = pTimer->m_nSecondsPerDay;
    m_nSnapshotScale = 100;
}
// PC 0x004ADF50  Xbox 0x00276E40
int CWorldTimer::AddWorldTimes(unsigned long nDay1, unsigned long nTime1,
                               unsigned long nDay2, unsigned long nTime2,
                               unsigned long* pnDay, unsigned long* pnTime)
{
    if (nTime1 >= m_nMillisecondsPerDay || nTime2 >= m_nMillisecondsPerDay)
    {
        return -2;
    }
    *pnDay = nDay2 + nDay1;
    *pnTime = nTime1 + nTime2;
    if (nTime1 + nTime2 >= m_nMillisecondsPerDay)
    {
        ++*pnDay;
        *pnTime -= m_nMillisecondsPerDay;
    }
    return 0;
}
// PC 0x004ADFA0  Xbox 0x00276E90
int CWorldTimer::CompareWorldTimes(unsigned long nDay1, unsigned long nTime1,
                                   unsigned long nDay2, unsigned long nTime2)
{
    if (nTime1 >= m_nMillisecondsPerDay || nTime2 >= m_nMillisecondsPerDay)
    {
        return -2;
    }
    if (nDay1 == nDay2)
    {
        if (nTime1 > nTime2)
        {
            return 1;
        }
        if (nTime1 < nTime2)
        {
            return -1;
        }
        return 0;
    }
    if (nDay1 > nDay2)
    {
        return 1;
    }
    return -1;
}
// PC 0x004ADFF0  Xbox 0x00276EE0
void CWorldTimer::PauseWorldTimer()
{
    if (m_bPaused == 0)
    {
        unsigned long nCalendarDay;
        unsigned long nTimeOfDay;
        GetWorldTime(&nCalendarDay, &nTimeOfDay);
        m_nPausedCalendarDay = nCalendarDay;
        m_nPausedTimeOfDay = nTimeOfDay;
        m_bPaused = 1;
    }
}
// PC 0x004AE030  Xbox 0x00276F20
void CWorldTimer::UnpauseWorldTimer()
{
    if (m_bPaused == 1)
    {
        unsigned __int64 nMilliseconds = m_nSnapshotTime / MICROSECONDS_IN_MILLISECOND;
        m_nCalendarDayOffset =
            m_nPausedCalendarDay - nMilliseconds / m_nMillisecondsPerDay;
        m_bPaused = 0;
        m_nTimeOfDayOffset =
            m_nPausedTimeOfDay - nMilliseconds % m_nMillisecondsPerDay;
    }
}
// PC 0x004AE090  Xbox 0x00276F80
unsigned long CWorldTimer::GetWorldTimeHour()
{
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    GetWorldTime(&nCalendarDay, &nTimeOfDay);
    return nTimeOfDay / MILLISECONDS_IN_SECOND / SECONDS_IN_MINUTE / m_nMinutesPerHour;
}
// PC 0x004AE0D0  Xbox 0x00276FC0
unsigned long CWorldTimer::GetWorldTimeMinute()
{
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    GetWorldTime(&nCalendarDay, &nTimeOfDay);
    return nTimeOfDay / MILLISECONDS_IN_SECOND / SECONDS_IN_MINUTE % m_nMinutesPerHour;
}
// PC 0x004AE110  Xbox 0x00277000
unsigned long CWorldTimer::GetWorldTimeSecond()
{
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    GetWorldTime(&nCalendarDay, &nTimeOfDay);
    return nTimeOfDay / MILLISECONDS_IN_SECOND % SECONDS_IN_MINUTE;
}
// PC 0x004AE140  Xbox 0x00277030
unsigned long CWorldTimer::GetWorldTimeMillisecond()
{
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    GetWorldTime(&nCalendarDay, &nTimeOfDay);
    return nTimeOfDay % MILLISECONDS_IN_SECOND;
}
// PC 0x004AE170  Xbox 0x00277060
void CWorldTimer::SetFixedSnapshotRate(int nFixed, int nRate)
{
    m_nSnapshotTime = 0;
    m_nPreviousSnapshotTime = 0;
    m_bFixedSnapshotRate = nFixed;
    m_nFixedSnapshotRate = nRate;
}
// PC 0x004AE190  Xbox 0x00277080
void CWorldTimer::SetSnapshotScale(float fScale)
{
    m_nSnapshotScale = (int)(fScale * 100.0f);
}
// PC 0x004AE1B0  Xbox 0x002770A0
unsigned long CWorldTimer::GetCalendarDayFromSeconds(float fSeconds)
{
    float fSecondsPerDay = (float)m_nSecondsPerDay;
    if (fSeconds < fSecondsPerDay)
    {
        return 0;
    }
    unsigned long nCalendarDay = 0;
    float fTime = fSeconds;
    do
    {
        fTime -= fSecondsPerDay;
        ++nCalendarDay;
    } while (fTime >= fSecondsPerDay);
    return nCalendarDay;
}
// PC 0x004AE200  Xbox 0x002770F0
unsigned long CWorldTimer::GetTimeOfDayFromSeconds(float fSeconds)
{
    if (fSeconds < 0.0f)
    {
        return 0;
    }
    float fSecondsPerDay = (float)m_nSecondsPerDay;
    if (fSeconds < fSecondsPerDay)
    {
        return (int)(fSeconds * 1000.0f);
    }
    float fTime = fSeconds;
    do
    {
        fTime -= fSecondsPerDay;
    } while (fTime >= fSecondsPerDay);
    return (int)(fTime * 1000.0f);
}
// PC 0x004AE260  Xbox 0x00277160
void CWorldTimer::AdvanceToTime(int nHour, int nMinute, int nSecond,
                                int nMillisecond)
{
    if (nHour < 0 || nMinute < 0 || nSecond < 0 || nMillisecond < 0)
    {
        return;
    }

    nSecond += (unsigned long)nMillisecond / MILLISECONDS_IN_SECOND;
    nMillisecond %= 1000;
    nMinute += (unsigned long)nSecond / SECONDS_IN_MINUTE;
    nSecond %= 60;
    nHour += (unsigned long)nMinute / 60;
    nMinute %= 60;
    unsigned long nDays = (unsigned long)nHour / HOURS_IN_DAY;
    nHour %= 24;

    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    GetWorldTime(&nCalendarDay, &nTimeOfDay);
    unsigned long nYear;
    unsigned long nMonth;
    unsigned long nDay;
    ConvertFromCalendarDay(nCalendarDay, &nYear, &nMonth, &nDay);
    if (nDays == 0)
    {
        if (CompareWorldTimes(
                nCalendarDay, nTimeOfDay, nCalendarDay,
                ConvertToTimeOfDay(nHour, nMinute, nSecond, nMillisecond)) == 1)
        {
            nDays = 1;
        }
    }
    if (nDays > 0)
    {
        nDay += nDays;
        nMonth += nDay / 29;
        nDay %= 29;
        nYear += nMonth / 13;
        nMonth %= 13;
        if (nYear > MAX_YEAR)
        {
            return;
        }
    }
    SetWorldTime(ConvertToCalendarDay(nYear, nMonth, nDay),
                 ConvertToTimeOfDay(nHour, nMinute, nSecond, nMillisecond), 0);
}
// PC 0x004AE3E0  Xbox 0x002772E0
void CWorldTimer::SetWorldTime(unsigned long nYear, unsigned long nMonth,
                               unsigned long nDay, unsigned long nHour,
                               unsigned long nMinute, unsigned long nSecond,
                               unsigned long nMillisecond, int bUpdateSnapshot)
{
    unsigned long nCalendarDay = ConvertToCalendarDay(nYear, nMonth, nDay);
    unsigned long nTimeOfDay =
        ConvertToTimeOfDay(nHour, nMinute, nSecond, nMillisecond);
    SetWorldTime(nCalendarDay, nTimeOfDay, bUpdateSnapshot);
}
// PC 0x004AE460  Xbox 0x00277360
int CWorldTimer::SubtractWorldTimes(unsigned long nDay1, unsigned long nTime1,
                                    unsigned long nDay2, unsigned long nTime2,
                                    unsigned long* pnDay, unsigned long* pnTime)
{
    if (CompareWorldTimes(nDay1, nTime1, nDay2, nTime2) == -1)
    {
        return -2;
    }
    *pnDay = nDay1 - nDay2;
    *pnTime = nTime1 - nTime2;
    if (nTime1 - nTime2 >= m_nMillisecondsPerDay)
    {
        --*pnDay;
        *pnTime += m_nMillisecondsPerDay;
    }
    return 0;
}
// PC 0x004AE4C0  Xbox 0x002773C0
CWorldTimer::CWorldTimer()
{
    m_bFixedSnapshotRate = 0;
    m_nMinutesPerHour = 5;
    m_nSecondsPerDay = 7200;
    m_nMillisecondsPerDay = 7200000;
    m_nSnapshotScale = 100;
    m_nSnapshotTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    m_nLastTimer = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    m_nPreviousSnapshotTime = m_nSnapshotTime;
    SetWorldTime(DEFAULT_YEAR, DEFAULT_MONTH, DEFAULT_DAY, DEFAULT_HOUR,
                 DEFAULT_MINUTE, DEFAULT_SECOND, DEFAULT_MILLISECOND, 0);
    m_bPaused = 0;
}
// PC 0x004AE570  Xbox 0x00277470
unsigned long CWorldTimer::GetTimeDifferenceFromWorldTime(unsigned long nDay,
                                                          unsigned long nTime)
{
    unsigned long nCurrentDay;
    unsigned long nCurrentTime;
    GetWorldTime(&nCurrentDay, &nCurrentTime);
    if (CompareWorldTimes(nCurrentDay, nCurrentTime, nDay, nTime) != -1)
    {
        unsigned long nDifference = nCurrentTime - nTime;
        if (nDifference >= m_nMillisecondsPerDay)
        {
            nDifference += m_nMillisecondsPerDay;
        }
        return nDifference;
    }
    return 0;
}
