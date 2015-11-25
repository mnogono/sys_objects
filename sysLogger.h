//---------------------------------------------------------------------------

#ifndef sysLoggerH
#define sysLoggerH
//---------------------------------------------------------------------------

namespace sysLogger {

extern const char LOG_LEVEL_PROFILE;
extern const char LOG_LEVEL_TRACE;
extern const char LOG_LEVEL_DEBUG;
extern const char LOG_LEVEL_WARNING;
extern const char LOG_LEVEL_ERROR;
extern const char LOG_LEVEL_DISABLE;

extern char CURRENT_LOG_LEVEL;

extern CRITICAL_SECTION CS_WRITE_LOG;

extern char LOG_FILE_A[255];

bool CheckLogLevel(char logLevel);

void LogA(const char *msg, char logLevel, const char *file, const char *function, int line, int newLine);
void LogA(int code, char logLevel, const char *file, const char *function, int line, int newLine);

void LogW(const wchar_t *msg, char logLevel, const char *file, const char *function, int line, int newLine);
void LogW_(int code, char logLevel, const char *file, const char *function, int line, int newLine);

void Init(const char *logFilePath, char logLevel);

void Init(const wchar_t *logFilePath, char logLevel);

void SaveProfile(const wchar_t *profilePath);

#define LOG_A(msg, logLevel) LogA(msg, logLevel, __FILE__, __FUNCTION__, __LINE__, 1)
#define LOG_A_(msg, logLevel) LogA(msg, logLevel, __FILE__, __FUNCTION__, __LINE__, 0)

#define ERR_A(msg) LogA(msg, sysLogger::LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, 1)
#define ERR_A_(msg) LogA(msg, sysLogger::LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, 0)

#define WARN_A(msg) LogA(msg, sysLogger::LOG_LEVEL_WARNING, __FILE__, __FUNCTION__, __LINE__, 1)
#define WARN_A_(msg) LogA(msg, sysLogger::LOG_LEVEL_WARNING, __FILE__, __FUNCTION__, __LINE__, 0)

#define DEBUG_A(msg) LogA(msg, sysLogger::LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, 1)
#define DEBUG_A_(msg) LogA(msg, sysLogger::LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, 0)

#define TRACE_A(msg) LogA(msg, sysLogger::LOG_LEVEL_TRACE, __FILE__, __FUNCTION__, __LINE__, 1)
#define TRACE_A_(msg) LogA(msg, sysLogger::LOG_LEVEL_TRACE, __FILE__, __FUNCTION__, __LINE__, 0)

#define PROFILE_A(msg) LogA(msg, sysLogger::LOG_LEVEL_PROFILE, __FILE__, __FUNCTION__, __LINE__, 1)
#define PROFILE_A_(msg) LogA(msg, sysLogger::LOG_LEVEL_PROFILE, __FILE__, __FUNCTION__, __LINE__, 0)

#define LOG_W(msg, logLevel) LogW(msg, logLevel, __FILE__, __FUNCTION__, __LINE__, 1)
#define LOG_W_(msg, logLevel) LogW(msg, logLevel, __FILE__, __FUNCTION__, __LINE__, 0)

#define ERR_W(msg) LogW(msg, sysLogger::LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, 1)
#define ERR_W_(msg) LogW(msg, sysLogger::LOG_LEVEL_ERROR, __FILE__, __FUNCTION__, __LINE__, 0)

#define WARN_W(msg) LogW(msg, sysLogger::LOG_LEVEL_WARNING, __FILE__, __FUNCTION__, __LINE__, 1)
#define WARN_W_(msg) LogW(msg, sysLogger::LOG_LEVEL_WARNING, __FILE__, __FUNCTION__, __LINE__, 0)

#define DEBUG_W(msg) LogW(msg, sysLogger::LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, 1)
#define DEBUG_W_(msg) LogW(msg, sysLogger::LOG_LEVEL_DEBUG, __FILE__, __FUNCTION__, __LINE__, 0)

#define TRACE_W(msg) LogW(msg, sysLogger::LOG_LEVEL_TRACE, __FILE__, __FUNCTION__, __LINE__, 1)
#define TRACE_W_(msg) LogW(msg, sysLogger::LOG_LEVEL_TRACE, __FILE__, __FUNCTION__, __LINE__, 0)

}

#endif
