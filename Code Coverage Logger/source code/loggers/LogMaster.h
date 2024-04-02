/*
 *  LogMaster.h
 *  Created on: 03 окт. 2023 г.
 *      Author: Shpagin_D
 */

#ifndef LOG_MASTER_H
#define LOG_MASTER_H

#include "base/Interfaces.h"

#if defined(USING_LOGGING)

#define ADD_LOGGER(logger) Logging::LogMaster::GetInstance()->AddLogger(logger)
#define GET_LOG() Logging::LogMaster::GetInstance()
#define LOG(level, format, ...)													\
	{																			\
		Logging::LogEntry entry = Logging::LogEntry(level, LOGGING_FILE_NAME,	\
			LOGGING_FUNCTION_NAME, __LINE__, format, ##__VA_ARGS__);			\
		Logging::LogMaster::GetInstance()->Log(entry);							\
	}
#define LOG_TAG(level, tags, format, ...)										\
	{																			\
		Logging::LogEntry entry = Logging::LogEntry(level, LOGGING_FILE_NAME,	\
			LOGGING_FUNCTION_NAME, __LINE__, format, ##__VA_ARGS__);			\
		entry.tags = tags;														\
		Logging::LogMaster::GetInstance()->Log(entry);							\
	}
#define LOG_DEBUG(format, ...)	LOG(Logging::Level::Debug, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)	LOG(Logging::Level::Info, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)	LOG(Logging::Level::Warn, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)	LOG(Logging::Level::Error, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...)	LOG(Logging::Level::Fatal, format, ##__VA_ARGS__)


namespace  Logging {


/**
 * \brief Данный класс является пересылающим записи лога всем подключенным логгерам
 */
class LogMaster final :
	public IUsingLogging,
	public Singleton<LogMaster>
{
	friend class Singleton;

public:
	static constexpr unsigned MAX_NUMBER_OF_LOGGERS = 5;

	LogMaster(const LogMaster&) = delete;
	LogMaster(LogMaster&& moved) = delete;
	const LogMaster& operator=(const LogMaster&) = delete;
	LogMaster& operator=(LogMaster&& moved) = delete;
	~LogMaster() override = default;

	bool AddLogger(ILogger* logger);
	bool RemoveLogger(const ILogger* logger);

	void Log(const Level level, const char* format, ...) override;
	void Log(LogEntry& entry) override;

private:
	ILogger* _loggers[MAX_NUMBER_OF_LOGGERS] = { nullptr };

	LogMaster() = default;
};


} // namespace  Logging

#else

#define ADD_LOGGER(...)
#define GET_LOG()
#define LOG(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)

#endif // defined(USING_LOGGING)
#endif // LOG_MASTER_H