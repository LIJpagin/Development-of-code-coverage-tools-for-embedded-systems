#ifndef WIN_LOGGER_H
#define WIN_LOGGER_H

#include "base/Interfaces.h"

#if defined(USING_LOGGING) && defined(EMULATION)

#define GET_WIN_LOGGER() Logging::WinLogger::GetInstance()
#define WIN_LOG(level, format, ...)	I_LOG(Logging::WinLogger::GetInstance(), level, format, ##__VA_ARGS__)
#define WIN_LOG_DEBUG(format, ...)	WIN_LOG(Logging::Level::Debug, format, ##__VA_ARGS__)
#define WIN_LOG_INFO(format, ...)	WIN_LOG(Logging::Level::Info, format, ##__VA_ARGS__)
#define WIN_LOG_WARN(format, ...)	WIN_LOG(Logging::Level::Warn, format, ##__VA_ARGS__)
#define WIN_LOG_ERROR(format, ...)	WIN_LOG(Logging::Level::Error, format, ##__VA_ARGS__)
#define WIN_LOG_FATAL(format, ...)	WIN_LOG(Logging::Level::Fatal, format, ##__VA_ARGS__)


namespace Logging {



/// Класс логгера выводящий лог в консоль или другой поток
class WinLogger final :
	public ILogger,
	public Singleton<WinLogger>
{
	friend class Singleton;

public:
	WinLogger(const WinLogger&) = delete;
	WinLogger(WinLogger&& moved) = delete;
	const WinLogger& operator=(const WinLogger&) = delete;
	WinLogger& operator=(WinLogger&& moved) = delete;

	void Log(LogEntry& entry) override;
	void Log(const Level level, const char* format, ...) override;

protected:
	WinLogger();
	~WinLogger() override;
};



} // namespace Logging

#else

// Пустые макросы в случае отсутствия логирования в эмуляции
#define GET_WIN_LOGGER() nullptr
#define WIN_LOG(...)
#define WIN_LOG_DEBUG(...) 
#define WIN_LOG_INFO(...)
#define WIN_LOG_WARN(...)
#define WIN_LOG_ERROR(...)
#define WIN_LOG_FATAL(...)

#endif // defined(USING_LOGGING) && defined(EMULATION)
#endif // WIN_LOGGER_H