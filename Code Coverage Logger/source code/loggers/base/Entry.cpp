#include "Entry.h"

#if defined(USING_LOGGING)

#include <cstdarg>
#include <cstdio>
#include "../../AccurateClock.h"

namespace Logging {



unsigned BaseEntry::_counterEntry { 0 };

AccurateClock MinLogEntry::_clock;

void MinLogEntry::StartAccurateClock()
{
	_clock.Start();
}

MinLogEntry::MinLogEntry(const Level level, const char* format, ...)
{
	message = SharedPtr<char>(new char[SIZE_OF_BUFFERS]);
	this->level = level;
	id = ++_counterEntry;
	va_list args;
	va_start(args, format);
	vsprintf(message.Get(), format, args);
	va_end(args);
	timeStamp = _clock.GetTimeStamp();
}

LogEntry::LogEntry(const Level level, const char* format, ...)
{
	message = SharedPtr<char>(new char[SIZE_OF_BUFFERS]);
	this->level = level;
	id = ++_counterEntry;
	va_list args;
	va_start(args, format);
	vsprintf(message.Get(), format, args);
	va_end(args);
	timeStamp = _clock.GetTimeStamp();
}

LogEntry::LogEntry(const Level level, const char* nameFile,
	const char* nameFunction, const unsigned numberLine, const char* format, ...)
	: nameFile(nameFile), nameFunction(nameFunction), numberLine(numberLine)
{
	message = SharedPtr<char>(new char[SIZE_OF_BUFFERS]);
	this->level = level;
	id = ++_counterEntry;
	va_list args;
	va_start(args, format);
	vsprintf(message.Get(), format, args);
	va_end(args);
	timeStamp = _clock.GetTimeStamp();
}

#if defined(USING_LOGGING_COVERAGE)
CovEntry::CovEntry(Statement statement, const unsigned numberNameFile, const unsigned numberLine)
	: statement(statement), numberNameFile(numberNameFile), numberLine(numberLine)
{
	id = ++_counterEntry;
}
#endif // defined(USING_LOGGING_COVERAGE)

} // namespace Logging
#endif // defined(USING_LOGGING)
