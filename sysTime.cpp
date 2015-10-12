//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

//---------------------------------------------------------------------------

#ifdef __BORLANDC__
	#pragma package(smart_init)
#endif

namespace sysTime {
	const double DAY2MSEC = 86400000.0;
	const double DAY2SEC = 86400.0;
	const double DAY2MIN = 1440;
	const double MSEC2DAY = 1.1574074074074074074074074074074e-8;
	const double MSEC2SEC = 0.001;
	const double SEC2DAY = 1.1574074074074074074074074074074e-5;
	const double MIN2DAY = 6.9444444444444444444444444444444e-4;
	const double HOUR2DAY = 0.04166666666666666666666666666667;
	const ULONGLONG SEC2NANOSEC = 1000000000;//1e9
	const ULONGLONG MSEC2NANOSEC = 1000000;//1e6;
	const double NANOSEC2MSEC = 1e-6;
	const double NANOSEC2SEC = 1e-9;
	const ULONGLONG SEC2MSEC = 1000;


#ifdef __BORLANDC__
	TDateTime SystemTimeToDateTime(SYSTEMTIME *sysTime) {
	  return TDateTime(
		sysTime->wYear,
		sysTime->wMonth,
		sysTime->wDay,
		sysTime->wHour,
		sysTime->wMinute,
		sysTime->wSecond,
		sysTime->wMilliseconds);
	}

	TDateTime NowUTC() {
	  SYSTEMTIME sysTime;
	  ::GetSystemTime(&sysTime);

	  return sysTime::SystemTimeToDateTime(&sysTime);
	}

	SYSTEMTIME DateTimeToSystemTime(const TDateTime &dt) {
		SYSTEMTIME st;
        unsigned short year, month, day;
        unsigned short hour, min, sec, msec;

        dt.DecodeDate(&year, &month, &day);
        dt.DecodeTime(&hour, &min, &sec, &msec);

        st.wYear = year;
        st.wMonth = month;
        st.wDay = day;
        st.wHour = hour;
		st.wMinute = min;
        st.wSecond = sec;
        st.wMilliseconds = msec;

        return st;
    }

	int GetDateAsInt(const TDateTime &dt) {
		WORD year, mount, day;
		dt.DecodeDate(&year, &mount, &day);
		return year * 10000 + mount * 100 + day;
	}

	char * GetDate(const int &year, const int &mount, const int &day){
		char *date = new char[9];
		sprintf(date, "%4d%02d%02d", year, mount, day);
		return date;
	}

	char * GetDate(TDateTime *dt){
		unsigned short year, mount, day;
		dt->DecodeDate(&year, &mount, &day);
		return GetDate(year, mount, day);
	}

	wchar_t * GetDateW(SYSTEMTIME &st, wchar_t *wFormat) {
		wchar_t *wBuffer = new wchar_t[255];
		memset(wBuffer, 0, sizeof(wchar_t) * 255);
		swprintf(wBuffer, wFormat, st.wYear, st.wMonth, st.wDay);
		return wBuffer;
	}

	char * GetDateTime(const SYSTEMTIME &st, const char *sFormat) {
		char *sBuffer = new char[255];
		memset(sBuffer, 0, sizeof(char) * 255);
		sprintf(sBuffer, sFormat, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		return sBuffer;
	}

	wchar_t *GetDateTime(const SYSTEMTIME &st, const wchar_t *wFormat) {
		wchar_t *wBuffer = new wchar_t[255];
		memset(wBuffer, 0, sizeof(wchar_t) * 255);
		swprintf(wBuffer, wFormat, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		return wBuffer;
	}

    long GetSecondsLocalTime(TDateTime *dt){
        return (dt->Val - 25569.0) * DAY2SEC;
    }

	/*
	Local time bias in seconds
	*/
	long GetLocalTimeBias(){
		TIME_ZONE_INFORMATION timeZoneInformation;
		GetTimeZoneInformation(&timeZoneInformation);
		return (timeZoneInformation.Bias/* + timeZoneInformation.DaylightBias*/) * 60;
	}

	char * GetCurrentDateGMT(){
		static double fLocalTimeBiasInDays = GetLocalTimeBias() * SEC2DAY; // / double(SEC_IN_DAY);
        TDateTime dtGMT = Now() + fLocalTimeBiasInDays;
        return GetDate(&dtGMT);
    }

	void GetCurrentLocalDateTimeW(wchar_t *wBuffer, String &sFormat){
		String _sFormat = sysBaseTypes::EmptyTo(sFormat, "yyyy-mm-dd hh:mm:nn");
        TDateTime dt = Now();
		String sDateTime = dt.FormatString(_sFormat);
        wcscpy(wBuffer, sDateTime.c_str());
        wBuffer[sDateTime.Length()] = 0;
    }
#endif

	ULONGLONG SystemTimeToMSec(const SYSTEMTIME &systemTime) {
		FILETIME fileTime;
		SystemTimeToFileTime(&systemTime, &fileTime);
		ULONGLONG nanoSec100Count = (((ULONGLONG) fileTime.dwHighDateTime) << 32) + fileTime.dwLowDateTime;
		return (nanoSec100Count * NANOSEC2MSEC * 100);
	}

	ULONGLONG DiffSystemTimeMSec(const SYSTEMTIME &sysTime1, const SYSTEMTIME &sysTime2) {
		ULONGLONG mSec1 = SystemTimeToMSec(sysTime1);
		ULONGLONG mSec2 = SystemTimeToMSec(sysTime2);
		if (mSec1 > mSec2) return mSec1 - mSec2;
		return mSec2 - mSec1;
	}

   	ULONGLONG DiffFileTimeMSec(const FILETIME &fileTime1, const FILETIME &fileTime2){
		SYSTEMTIME sysTime1;
        SYSTEMTIME sysTime2;
        FileTimeToSystemTime(&fileTime1, &sysTime1);
        FileTimeToSystemTime(&fileTime2, &sysTime2);

        return DiffSystemTimeMSec(sysTime1, sysTime2);
    }



	bool IsLeapYear(unsigned int year) {
		return !(year % 4) && ((year % 100) || !(year % 400));
	}

	unsigned int GetMonthDays(unsigned int year, unsigned int month) {
		static const int monthDays[2][13] = {
			{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
			{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
		};

		return monthDays[(IsLeapYear(year))?1:0][month];
	}

	/*
	void ParseDate(const wchar_t *sDate, int &year, int &month, int &day) {
		int date;
		swscanf(sDate, L"%d", &date);

		year = 0.0001 * date;
		month = 0.01 * (date - year * 10000);
		day = date - year * 10000 - month * 100;
	}
	*/
	void ParseDate(const wchar_t *sDate, int &year, int &month, int &day) {
		swscanf(sDate, L"%4d%2d%2d", &year, &month, &day);
	}

	double ConvertToDays(const __int64 &timeGMT){
		return 25569.0 + timeGMT * SEC2DAY;// / 86400.0;
	}

	double ConvertToDaysLocalTime(const __int64 &secGMT){
		double gmtDays = ConvertToDays(secGMT);
		long timeZoneShiftInSeconds = GetLocalTimeBias();
		return gmtDays - timeZoneShiftInSeconds * SEC2DAY;// / 86400.0;
	}

    /*
	convert gmt date time (days) to local time date time
    */
    double GetLocalTime(double dtGMT){
        static double fLocalTimeBiasInDays = GetLocalTimeBias() * SEC2DAY;// / double(SEC_IN_DAY);
        return dtGMT - fLocalTimeBiasInDays;
    }

	#ifdef __BORLANDC__

    double GetLocalTime(TDateTime *dt){
        WORD hours, minutes, seconds, milliseconds;
        dt->DecodeTime(&hours, &minutes, &seconds, &milliseconds);
        return GetSecondsLocalTime(dt) + milliseconds * MSEC2SEC;
    }

	#endif

    /*
    convert date time local time to gmt
    */
    double GetGMTTime(double dtLocalTime){
        static double fLocalTimeBiasInDays = GetLocalTimeBias() * SEC2DAY; // / double(SEC_IN_DAY);
        return dtLocalTime + fLocalTimeBiasInDays;
    }

    char CompareSystemTime(const SYSTEMTIME &st1, const SYSTEMTIME &st2) {
    	ULONGLONG diff = SystemTimeToMSec(st1) - SystemTimeToMSec(st2);
        if (diff > 0) {
			return 1;
        } else if (diff < 0) {
			return -1;
		}

        return 0;
    }

	/*
	SYSTEMTIME ConvertGMTSystemTimeToLocalSystemTime(const SYSTEMTIME &st) {
		long bias = GetLocalTimeBias()
		ULONGLONG msecTime = SystemTimeToMSec(st);
		return
	}
	*/

	__int64 ConvertToUnixTime(const TDateTime &dt){
		return (dt.Val - 25569.0) * 86400;
	}

	//convert borland date time into unix time (count of milliseconds since 1970)
	__int64 ConvertToUnixTimeMilliseconds(const TDateTime &dt){
		return ConvertToUnixTime(dt) * (__int64)SEC2MSEC + (__int64)MilliSecondOf(dt);
	}
}
