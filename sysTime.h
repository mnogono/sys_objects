//---------------------------------------------------------------------------

#ifndef sysTimeH
#define sysTimeH

//---------------------------------------------------------------------------

#ifndef __BORLANDC__
	typedef unsigned long long ULONGLONG;
#endif

namespace sysTime {
	extern const double DAY2MSEC;
	extern const double DAY2SEC;
	extern const double DAY2MIN;
	extern const double MSEC2DAY;
	extern const double MSEC2SEC;
	extern const double SEC2DAY;
	extern const double MIN2DAY;
	extern const double HOUR2DAY;
	extern const ULONGLONG SEC2NANOSEC;
	extern const ULONGLONG MSEC2NANOSEC;
	extern const double NANOSEC2MSEC;
	extern const double NANOSEC2SEC;
	extern const ULONGLONG SEC2MSEC;

#ifdef __BORLANDC__
	TDateTime NowUTC();

	TDateTime SystemTimeToDateTime(SYSTEMTIME *sysTime);

    SYSTEMTIME DateTimeToSystemTime(const TDateTime &dt);

    /*
    conver TDateTime to double seconds since 1 jan 1970 + frac part of msec in local time
    */
    double GetLocalTime(TDateTime *dt);

    /*
    return c-string like "YYYMMDD"
    do not forget to delete result memmory
    */
	char * GetDate(TDateTime *dt);

	/*
	return int number like YYYYMMDD
	*/
	int GetDateAsInt(const TDateTime &dt);
#endif

	/*
	wFormat like L"%d-%d-%d"
	do not forget to delete result memmory
	*/
	wchar_t * GetDateW(SYSTEMTIME &st, const wchar_t *wFormat);

	/*
	do not forget to delete result memmory
	//sFormat like "%d-%d-%d %d:%d:%d"
	*/
	char * GetDateTime(const SYSTEMTIME &st, const char *sFormat);

	//wFormat like L"%d-%d-%d %d:%d:%d"
	wchar_t *GetDateTime(const SYSTEMTIME &st, const wchar_t *wFormat);

#ifdef __BORLANDC__
    /*
    oonvert local time dt to seconds since 1 jan 1970;
    */
    long GetSecondsLocalTime(TDateTime *dt);
#endif

    /*
	return c-string like "YYYYMMDD"
    do not forget to delete result memmory
    */
    char * GetCurrentDateGMT();

#ifdef __BORLANDC__
    /*
    wBuffer must be allocated before execute function
    */
    void GetCurrentLocalDateTimeW(wchar_t *wBuffer, const String &sFormat);
#endif

	/**
    return msec since January 1, 1601 (UTC)
	*/
	ULONGLONG SystemTimeToMSec(const SYSTEMTIME &sysTime);

	/**
	example of getting system time
	SYSTEMTIME st;
	GetSystemTime(&st);

	return msec between sysTime1 and sysTime2
	result always >= 0
	*/
	ULONGLONG DiffSystemTimeMSec(const SYSTEMTIME &sysTime1, const SYSTEMTIME &sysTime2);

    /*
    return msec diff between fileTime1 - fileTime2
    */
	ULONGLONG DiffFileTimeMSec(const FILETIME &fileTime1, const FILETIME &fileTime2);

	/*
	Check is year leap
	*/
	bool IsLeapYear(int year);

	/*
	return days in month
	*/
	unsigned int GetMonthDays(unsigned int year, unsigned int month);

	/*
	extract year, month and day from string sDate format like 'yyyymmdd'
	*/
	//void ParseDate(const wchar_t *sDate, int &year, int &month, int &day);
	void ParseDate(const wchar_t *sDate, int &year, int &month, int &day);

	/*
	convert to days - VCL Builder format
	@param timeGMT - grinvitch midnight time, seconds
	*/
	double ConvertToDays(const __int64 &timeGMT);

	/*
	convert to days - VCL Builder format
	@param timeGMT - grinvitch midnight time, seconds
	*/
	double ConvertToDaysLocalTime(const __int64 &secGMT);

	/*
	Local time bias in seconds
	*/
	long GetLocalTimeBias();

    /*
	convert gmt date time (days) to local time date time
    */
    double GetLocalTime(double dtGMT);

    /*
    convert date time local time to gmt
    */
    double GetGMTTime(double dtLocalTime);

	/*
    return c-string like "YYYMMDD"
    do not forget to delete result memmory
    */
    char * GetDate(const int &year, const int &mount, const int &day);

    /*
    	return 1 if st1 > st2
        return 0 if st1 = st2
        return -1 if st1 < st2
    */
    char CompareSystemTime(const SYSTEMTIME &st1, const SYSTEMTIME &st2);

	//SYSTEMTIME ConvertGMTSystemTimeToLocalSystemTime(const SYSTEMTIME &st);

	//concept of sysTime library for working with FILETIME, SYSTEMTIME

#ifdef __BORLANDC__
	//convert borland date time into unix time (count of seconds since 1970)
	__int64 ConvertToUnixTime(const TDateTime &dt);

	//convert borland date time into unix time (count of milliseconds since 1970)
	__int64 ConvertToUnixTimeMilliseconds(const TDateTime &dt);
#endif
}

#endif


