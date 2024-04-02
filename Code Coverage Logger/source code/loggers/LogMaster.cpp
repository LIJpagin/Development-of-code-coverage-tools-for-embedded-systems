/*
 *  LogMaster.cpp
 *  Created on: 03 окт. 2023 г.
 *      Author: Shpagin_D
 */

#include "LogMaster.h"

#include <cstdarg>


#if defined(USING_LOGGING)
namespace  Logging {


bool LogMaster::AddLogger(ILogger* logger)
{
	for (auto& obj : _loggers) {
		if (obj == nullptr) {
			obj = logger;
			return true;
		}
	}
	return false;
}

bool LogMaster::RemoveLogger(const ILogger* logger)
{
	for (auto obj : _loggers) {
		if (obj == logger) {
			obj = nullptr;
			return true;
		}
	}
	return false;
}

void LogMaster::Log(const Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogEntry entry(level, format, args);
	for (const auto obj : _loggers) {
		if (obj != nullptr)
			obj->Log(entry);
	}
	va_end(args);
}

void LogMaster::Log(LogEntry& entry)
{
	for (const auto obj : _loggers) {
		if (obj != nullptr)
			obj->Log(entry);
	}
}


} // namespace  Logging
#endif // defined(USING_LOGGING)