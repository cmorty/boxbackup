// --------------------------------------------------------------------------
//
// File
//		Name:    Logging.h
//		Purpose: Generic logging core routines declarations and macros
//		Created: 2006/12/16
//
// --------------------------------------------------------------------------

#ifndef LOGGING__H
#define LOGGING__H

#include <cstring>
#include <iomanip>
#include <sstream>
#include <vector>

/*
#define BOX_LOG(level, stuff) \
{ \
    if(Log::sMaxLoggingLevelForAnyOutput >= level) \
        std::ostringstream line; \
        line << stuff; \
        Log::Write(level, __FILE__, __LINE__, line.str()); \
    } \
}
*/

#define BOX_LOG(level, stuff) \
{ \
	std::ostringstream _box_log_line; \
	_box_log_line << stuff; \
	Logging::Log(level, __FILE__, __LINE__, _box_log_line.str()); \
}

#define BOX_SYSLOG(level, stuff) \
{ \
	std::ostringstream _box_log_line; \
	_box_log_line << stuff; \
	Logging::LogToSyslog(level, __FILE__, __LINE__, _box_log_line.str()); \
}

#define BOX_FATAL(stuff)   BOX_LOG(Log::FATAL,   stuff)
#define BOX_ERROR(stuff)   BOX_LOG(Log::ERROR,   stuff)
#define BOX_WARNING(stuff) BOX_LOG(Log::WARNING, stuff)
#define BOX_NOTICE(stuff)  BOX_LOG(Log::NOTICE,  stuff)
#define BOX_INFO(stuff)    BOX_LOG(Log::INFO,    stuff)
#define BOX_TRACE(stuff)   \
	if (Logging::IsEnabled(Log::TRACE)) \
	{ BOX_LOG(Log::TRACE, stuff) }

#define BOX_LOG_SYS_WARNING(stuff) \
	BOX_WARNING(stuff << ": " << std::strerror(errno) << " (" << errno << ")")
#define BOX_LOG_SYS_ERROR(stuff) \
	BOX_ERROR(stuff << ": " << std::strerror(errno) << " (" << errno << ")")
#define BOX_LOG_SYS_FATAL(stuff) \
	BOX_FATAL(stuff << ": " << std::strerror(errno) << " (" << errno << ")")

#ifdef WIN32
	#define BOX_LOG_WIN_ERROR(stuff) \
		BOX_ERROR(stuff << ": " << GetErrorMessage(GetLastError()))
	#define BOX_LOG_WIN_ERROR_NUMBER(stuff, number) \
		BOX_ERROR(stuff << ": " << GetErrorMessage(number))
#endif

#define BOX_FORMAT_HEX32(number) \
	std::hex << \
	std::showbase << \
	std::internal << \
	std::setw(10) << \
	std::setfill('0') << \
	(number) << \
	std::dec

#define BOX_FORMAT_ACCOUNT(accno) \
	BOX_FORMAT_HEX32(accno)

#define BOX_FORMAT_OBJECTID(objectid) \
	std::hex << \
	std::showbase << \
	(objectid) << \
	std::dec

#undef ERROR

namespace Log
{
	enum Level 
	{
		NOTHING = 1,
		FATAL,
		ERROR,
		WARNING,
		NOTICE,
		INFO,
		TRACE, 
		EVERYTHING,
		INVALID = -1
	};
}

// --------------------------------------------------------------------------
//
// Class
//		Name:    Logger
//		Purpose: Abstract base class for log targets
//		Created: 2006/12/16
//
// --------------------------------------------------------------------------

class Logger
{
	private:
	Log::Level mCurrentLevel;
	
	public:
	Logger();
	virtual ~Logger();
	
	virtual bool Log(Log::Level level, const std::string& rFile, 
		int line, std::string& rMessage) = 0;
	
	void Filter(Log::Level level)
	{
		mCurrentLevel = level;
	}

	virtual const char* GetType() = 0;
	Log::Level GetLevel() { return mCurrentLevel; }
	
	virtual void SetProgramName(const std::string& rProgramName) = 0;
};

// --------------------------------------------------------------------------
//
// Class
//		Name:    Console
//		Purpose: Console logging target
//		Created: 2006/12/16
//
// --------------------------------------------------------------------------

class Console : public Logger
{
	private:
	static bool sShowTime;
	static bool sShowTimeMicros;
	static bool sShowTag;
	static std::string sTag;
	#ifndef WIN32
	static bool sShowPID;
	#endif

	public:
	virtual bool Log(Log::Level level, const std::string& rFile, 
		int line, std::string& rMessage);
	virtual const char* GetType() { return "Console"; }
	virtual void SetProgramName(const std::string& rProgramName) { }

	static void SetTag(const std::string& rTag);
	static void SetShowTime(bool enabled);
	static void SetShowTimeMicros(bool enabled);
	#ifndef WIN32
	static void SetShowPID(bool enabled);
	#endif
};

// --------------------------------------------------------------------------
//
// Class
//		Name:    Syslog
//		Purpose: Syslog (or Windows Event Viewer) logging target
//		Created: 2006/12/16
//
// --------------------------------------------------------------------------

class Syslog : public Logger
{
	private:
	std::string mName;

	public:
	Syslog();
	virtual ~Syslog();
	
	virtual bool Log(Log::Level level, const std::string& rFile, 
		int line, std::string& rMessage);
	virtual const char* GetType() { return "Syslog"; }
	virtual void SetProgramName(const std::string& rProgramName);
};

// --------------------------------------------------------------------------
//
// Class
//		Name:    Logging
//		Purpose: Static logging helper, keeps track of enabled loggers
//			 and distributes log messages to them.
//		Created: 2006/12/16
//
// --------------------------------------------------------------------------

class Logging
{
	private:
	static std::vector<Logger*> sLoggers;
	static bool sLogToSyslog, sLogToConsole;
	static std::string sContext;
	static bool sContextSet;
	static Console* spConsole;
	static Syslog*  spSyslog;
	static Log::Level sGlobalLevel;
	static Logging    sGlobalLogging;
	
	public:
	Logging ();
	~Logging();
	static void ToSyslog  (bool enabled);
	static void ToConsole (bool enabled);
	static void FilterSyslog  (Log::Level level);
	static void FilterConsole (Log::Level level);
	static void Add    (Logger* pNewLogger);
	static void Remove (Logger* pOldLogger);
	static void Log(Log::Level level, const std::string& rFile, 
		int line, const std::string& rMessage);
	static void LogToSyslog(Log::Level level, const std::string& rFile, 
		int line, const std::string& rMessage);
	static void SetContext(std::string context);
	static void ClearContext();
	static void SetGlobalLevel(Log::Level level) { sGlobalLevel = level; }
	static Log::Level GetGlobalLevel() { return sGlobalLevel; }
	static Log::Level GetNamedLevel(const std::string& rName);
	static bool IsEnabled(Log::Level level)
	{
		return (int)sGlobalLevel >= (int)level;
	}
	static void SetProgramName(const std::string& rProgramName);

	class Guard
	{
		private:
		Log::Level mOldLevel;

		public:
		Guard(Log::Level newLevel)
		{
			mOldLevel = Logging::GetGlobalLevel();
			Logging::SetGlobalLevel(newLevel);
		}
		~Guard()
		{
			Logging::SetGlobalLevel(mOldLevel);
		}
	};
};

#endif // LOGGING__H
