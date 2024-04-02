#include "WinLogger.h"

#if defined(USING_LOGGING) && defined(EMULATION)


#include <cstdarg>
#include <cstdio>

namespace Logging {


void WinLogger::Log(LogEntry& entry)
{
	if (_filter->Check(entry))
	{
		char buffer[LOGGING_SIZE_OF_BUFFERS];
		_formatter->Build(buffer, entry);
		switch (entry.level)
		{
			case Level::Debug:
				printf("\x1B[36m%s\033[0m\n", buffer);
				break;
			case Level::Info:
				printf("\x1B[37m%s\033[0m\n", buffer);
				break;
			case Level::Warn:
				printf("\x1B[33m%s\033[0m\n", buffer);
				break;
			case Level::Error:
				printf("\x1B[31m%s\033[0m\n", buffer);
				break;
			case Level::Fatal:
				printf("\x1B[41m%s\033[0m\n", buffer);
				break;
			default:;
		}
	}
}

void WinLogger::Log(const Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	LogEntry entry(level, format, args);
	Log(entry);
	va_end(args);
}

WinLogger::WinLogger()
{
	_filter = new DefaultFilter();
	_filter->SetLevel(Level::Debug);
	_formatter = new DefaultFormatter();
	_formatter->SetFlagsItems(Item::RelativeTime | Item::EntryLevel | Item::NameFile | Item::NumberLine);
}

WinLogger::~WinLogger()
{
	delete _filter;
	delete _formatter;
}


} // namespace Logging
#endif // defined(USING_LOGGING) && defined(EMULATION)