#include "pch.h"

#ifdef __BORLANDC__

#pragma hdrstop

#pragma package(smart_init)

#endif

namespace sysLogger {

const char LOG_LEVEL_TRACE = 0;
const char LOG_LEVEL_DEBUG = 1;
const char LOG_LEVEL_WARNING = 2;
const char LOG_LEVEL_ERROR = 3;
const char LOG_LEVEL_PROFILE = 4;
const char LOG_LEVEL_DISABLE = 5;

char CURRENT_LOG_LEVEL = LOG_LEVEL_DISABLE;

CRITICAL_SECTION CS_WRITE_LOG;

char LOG_FILE_A[255];

struct TProfile {
	__int64 msec;
	std::string msg;
};

std::list<TProfile> profile;

//---------------------------------------------------------------------------
void Init(const char *logFile, char logLevel) {
	strcpy(LOG_FILE_A, logFile);
	CURRENT_LOG_LEVEL = logLevel;
	InitializeCriticalSection(&CS_WRITE_LOG);
}

//---------------------------------------------------------------------------
void Init(const wchar_t *logFile, char logLevel) {
	sysStr::WToMB(logFile, &LOG_FILE_A[0], 255);
	CURRENT_LOG_LEVEL = logLevel;
	InitializeCriticalSection(&CS_WRITE_LOG);
}

//---------------------------------------------------------------------------
bool CheckLogLevel(char logLevel) {
	return CURRENT_LOG_LEVEL <= logLevel;
}

//---------------------------------------------------------------------------
void LogA(int code, char logLevel, const char *file, const char *function, int line, int newLine) {
	char msg[255];
	sprintf(msg, "%d", code);
	LogA(msg, logLevel, file, function, line, newLine);
}

//---------------------------------------------------------------------------
void LogA(const char *msg, char logLevel, const char *file, const char *function, int line, int newLine) {
	if (!CheckLogLevel(logLevel))  return;

	if (logLevel == LOG_LEVEL_PROFILE) {
		SYSTEMTIME t;
		GetSystemTime(&t);
		TProfile p;
		p.msec = sysTime::SystemTimeToMSec(t);
		p.msg = std::string(msg);
		profile.push_back(p);
		return;
	}

	/*
	if (newLine == 0) {
		char logFile[255];
		char *sDate = sysTime::GetCurrentDateGMT();
		sprintf(logFile, "%s_%s.log", LOG_FILE_A, sDate);
		delete []sDate;

		size_t msgSize = strlen(msg);

		EnterCriticalSection(&CS_WRITE_LOG);

		FILE *f = fopen(logFile, "ab");
		if (f != NULL) {
			fwrite(msg, sizeof(char), msgSize, f);
			fwrite("\r\n", sizeof(char), 2, f);
			fclose(f);
		}

		LeaveCriticalSection(&CS_WRITE_LOG);
	} else {
	*/
		char sLogLevel[20];

		if (logLevel == LOG_LEVEL_TRACE) {
			sprintf(sLogLevel, "[TRACE  ]\t");

		} else if (logLevel == LOG_LEVEL_DEBUG) {
			sprintf(sLogLevel, "[DEBUG  ]\t");

		} else if (logLevel == LOG_LEVEL_WARNING) {
			sprintf(sLogLevel, "[WARNING]\t");

		} else if (logLevel == LOG_LEVEL_ERROR) {
			sprintf(sLogLevel, "[ERROR  ]\t");

		} else if (logLevel == LOG_LEVEL_PROFILE) {
			sprintf(sLogLevel, "[PROFILE]\t");
		}

		DWORD threadId = GetCurrentThreadId();
		char sThreadId[20];
		sprintf(sThreadId, "thread: [%d] ", threadId);

		char sSourceInfo[255];
		sprintf(sSourceInfo, "%s[%s]:%d\t", file, function, line);

		SYSTEMTIME systemTime;
		GetLocalTime(&systemTime);

		char *sDateTime = sysTime::GetDateTime(systemTime, "%.4d/%.2d/%.2d %.2d:%.2d:%.2d ");

		EnterCriticalSection(&CS_WRITE_LOG);

		size_t msgSize = strlen(msg);

		if (msgSize > 0) {

			char *sDate = sysTime::GetCurrentDateGMT();

			char logFile[255];
			sprintf(logFile, "%s_%s.log", LOG_FILE_A, sDate);

			delete []sDate;

			FILE *f = fopen(logFile, "ab");
			if (f != NULL) {
				fwrite(sLogLevel, sizeof(char), strlen(sLogLevel), f);
				fwrite(sDateTime, sizeof(char), strlen(sDateTime), f);
				fwrite(sThreadId, sizeof(char), strlen(sThreadId), f);
				fwrite(sSourceInfo, sizeof(char), strlen(sSourceInfo), f);
				fwrite(msg, sizeof(char), msgSize, f);
				if (msg[msgSize - 1] != '\n' && newLine == 1) {
					static char *__rn = "\r\n";
					fwrite(__rn, sizeof(char), 2, f);
				}
				fclose(f);
			}
		}

		LeaveCriticalSection(&CS_WRITE_LOG);

		delete []sDateTime;
	//}
}

void LogW(int code, char logLevel, const char *file, const char *function, int line, int newLine) {
	wchar_t msg[255];
	swprintf(msg, L"%d", code);
	LogW(msg, logLevel, file, function, line, newLine);
}

void LogW(const wchar_t *msg, char logLevel, const char *file, const char *function, int line, int newLine) {
	if (!CheckLogLevel(logLevel))  return;

	/*
	if (newLine == 0) {
		EnterCriticalSection(&CS_WRITE_LOG);

		char *mbMsg;
		int mbMsgSize;
		if (sysStr::WideCharToUTF8(msg, -1, mbMsg, mbMsgSize)) {
			if (mbMsgSize > 0) {
				char logFile[255];

				char *sDate = sysTime::GetCurrentDateGMT();
				sprintf(logFile, "%s_%s.log", LOG_FILE_A, sDate);

				delete []sDate;

				FILE *f = fopen(logFile, "ab");
				if (f != NULL) {
					fwrite(mbMsg, sizeof(char), mbMsgSize - 1, f);
					fwrite("\r\n", sizeof(char), 2, f);
					fclose(f);
				}
			}
		}

		LeaveCriticalSection(&CS_WRITE_LOG);
	} else {
	*/
		char sLogLevel[20];

		if (logLevel == LOG_LEVEL_TRACE) {
			sprintf(sLogLevel, "[TRACE  ]\t");

		} else if (logLevel == LOG_LEVEL_DEBUG) {
			sprintf(sLogLevel, "[DEBUG  ]\t");

		} else if (logLevel == LOG_LEVEL_WARNING) {
			sprintf(sLogLevel, "[WARNING]\t");

		} else if (logLevel == LOG_LEVEL_ERROR) {
			sprintf(sLogLevel, "[ERROR  ]\t");

		} else if (logLevel == LOG_LEVEL_PROFILE) {
			sprintf(sLogLevel, "[PROFILE]\t");
		}

		DWORD threadId = GetCurrentThreadId();
		char sThreadId[20];
		sprintf(sThreadId, "thread: [%d] ", threadId);

		char sSourceInfo[255];
		sprintf(sSourceInfo, "%s[%s]:%d\t", file, function, line);

		SYSTEMTIME systemTime;
		GetLocalTime(&systemTime);

		char *sDateTime = sysTime::GetDateTime(systemTime, "%.4d/%.2d/%.2d %.2d:%.2d:%.2d ");

		EnterCriticalSection(&CS_WRITE_LOG);

		char *mbMsg;
		int mbMsgSize;
		if (sysStr::WideCharToUTF8(msg, -1, mbMsg, mbMsgSize)) {
			if (mbMsgSize > 0) {
				char logFile[255];

				char *sDate = sysTime::GetCurrentDateGMT();
				sprintf(logFile, "%s_%s.log", LOG_FILE_A, sDate);

				delete []sDate;

				FILE *f = fopen(logFile, "ab");
				if (f != NULL) {
					fwrite(sLogLevel, sizeof(char), strlen(sLogLevel), f);
					fwrite(sDateTime, sizeof(char), strlen(sDateTime), f);
					fwrite(sThreadId, sizeof(char), strlen(sThreadId), f);
					fwrite(sSourceInfo, sizeof(char), strlen(sSourceInfo), f);
					fwrite(mbMsg, sizeof(char), mbMsgSize - 1, f);
					if (mbMsg[mbMsgSize - 2] != '\n' && newLine == 1) {
						static char *__rn = "\r\n";
						fwrite(__rn, sizeof(char), 2, f);
					}
					fclose(f);
				}
			}
		}

		LeaveCriticalSection(&CS_WRITE_LOG);

		delete []sDateTime;
	//}
}

void SaveProfile(const wchar_t *profilePath) {
	std::ofstream out(profilePath, std::ios_base::out | std::ios_base::app);

    out << "--------------------------------------------------------------\n";

	__int64 prevTime = 0;
	for (std::list<TProfile>::iterator i = profile.begin(), iEnd = profile.end(); i != iEnd; ++i) {
		if (prevTime == 0) {
			prevTime = (*i).msec;
			continue;
		}

		__int64 delay = (*i).msec - prevTime;
		out << delay << " ms : " << (*i).msg << "\n";

		prevTime = (*i).msec;
	}

	out.close();
}

}
