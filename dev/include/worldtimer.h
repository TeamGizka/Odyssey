#ifndef __WORLDTIMER_H__
#define __WORLDTIMER_H__

#define WORLD_TIMER_SNAPSHOT_TICKS_PER_MILLISECOND 1000

#ifndef _XBOX
#pragma pack(push, 1)
#endif

class CWorldTimer
{
public:
    CWorldTimer();
    ~CWorldTimer();

    static const unsigned long MIN_YEAR;
    static const unsigned long DEFAULT_YEAR;
    static const unsigned long MAX_YEAR;
    static const unsigned long MAX_YEAR_LIMIT;
    static const unsigned long MIN_MONTH;
    static const unsigned long DEFAULT_MONTH;
    static const unsigned long MAX_MONTH;
    static const unsigned long MIN_DAY;
    static const unsigned long DEFAULT_DAY;
    static const unsigned long MAX_DAY;
    static const unsigned long MIN_HOUR;
    static const unsigned long DEFAULT_HOUR;
    static const unsigned long MAX_HOUR;
    static const unsigned long MIN_MINUTE;
    static const unsigned long DEFAULT_MINUTE;
    static const unsigned long MAX_MINUTE;
    static const unsigned long MIN_SECOND;
    static const unsigned long DEFAULT_SECOND;
    static const unsigned long MAX_SECOND;
    static const unsigned long MIN_MILLISECOND;
    static const unsigned long DEFAULT_MILLISECOND;
    static const unsigned long MAX_MILLISECOND;
    static const unsigned long MONTHS_IN_YEAR;
    static const unsigned long DAYS_IN_MONTH;
    static const unsigned long DAYS_IN_YEAR;
    static const unsigned long HOURS_IN_DAY;
    static const unsigned long SECONDS_IN_MINUTE;
    static const unsigned long MILLISECONDS_IN_SECOND;
    static const unsigned long MILLISECONDS_IN_MINUTE;
    static const unsigned long MICROSECONDS_IN_MILLISECOND;
    static const unsigned long WORLD_TIME_INVALID;

    void SetMinutesPerHour(unsigned char nMinutesPerHour);
    void SetSnapshotTime();
    unsigned __int64 GetSnapshotTime();
    unsigned __int64 GetSnapshotTimeDifference();
    unsigned long ConvertToCalendarDay(unsigned long nYear,
                                       unsigned long nMonth,
                                       unsigned long nDay);
    unsigned long ConvertToTimeOfDay(unsigned long nHour, unsigned long nMinute,
                                     unsigned long nSecond,
                                     unsigned long nMillisecond);
    unsigned long ConvertFromCalendarDay(unsigned long nCalendarDay,
                                         unsigned long* pnYear,
                                         unsigned long* pnMonth,
                                         unsigned long* pnDay);
    unsigned long ConvertFromTimeOfDay(unsigned long nTimeOfDay,
                                       unsigned long* pnHour,
                                       unsigned long* pnMinute,
                                       unsigned long* pnSecond,
                                       unsigned long* pnMillisecond);
    void SetWorldTime(unsigned long nCalendarDay, unsigned long nTimeOfDay,
                      int bUpdateSnapshot);
    void GetWorldTime(unsigned long* pnCalendarDay, unsigned long* pnTimeOfDay);
    void ResetTimer(CWorldTimer* pTimer);
    int AddWorldTimes(unsigned long nDay1, unsigned long nTime1,
                      unsigned long nDay2, unsigned long nTime2,
                      unsigned long* pnDay, unsigned long* pnTime);
    int CompareWorldTimes(unsigned long nDay1, unsigned long nTime1,
                          unsigned long nDay2, unsigned long nTime2);
    void PauseWorldTimer();
    void UnpauseWorldTimer();
    unsigned long GetWorldTimeHour();
    unsigned long GetWorldTimeMinute();
    unsigned long GetWorldTimeSecond();
    unsigned long GetWorldTimeMillisecond();
    void SetFixedSnapshotRate(int nFixed, int nRate);
    void SetSnapshotScale(float fScale);
    unsigned long GetCalendarDayFromSeconds(float fSeconds);
    unsigned long GetTimeOfDayFromSeconds(float fSeconds);
    void AdvanceToTime(int nHour, int nMinute, int nSecond, int nMillisecond);
    void SetWorldTime(unsigned long nYear, unsigned long nMonth,
                      unsigned long nDay, unsigned long nHour,
                      unsigned long nMinute, unsigned long nSecond,
                      unsigned long nMillisecond, int bUpdateSnapshot);
    int SubtractWorldTimes(unsigned long nDay1, unsigned long nTime1,
                           unsigned long nDay2, unsigned long nTime2,
                           unsigned long* pnDay, unsigned long* pnTime);
    unsigned long GetTimeDifferenceFromWorldTime(unsigned long nDay,
                                                 unsigned long nTime);

    int m_bFixedSnapshotRate;
    int m_nFixedSnapshotRate;
    int m_nSnapshotScale;
    unsigned __int64 m_nSnapshotTime;
    unsigned __int64 m_nPreviousSnapshotTime;
    unsigned __int64 m_nLastTimer;
    int m_bPaused;
    unsigned long m_nPausedCalendarDay;
    unsigned long m_nPausedTimeOfDay;
    unsigned long m_nCalendarDayOffset;
    unsigned long m_nTimeOfDayOffset;
    unsigned char m_nMinutesPerHour;
    unsigned char m_aUnmapped39[3];
    unsigned long m_nMillisecondsPerDay;
    unsigned long m_nSecondsPerDay;
};

#ifndef _XBOX
#pragma pack(pop)
#endif

#endif
