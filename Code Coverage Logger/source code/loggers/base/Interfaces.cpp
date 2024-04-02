/*!
	\brief Файл, порождающий логгеры и описывающий их поведение
	\author Шпагин Д.
	\date 18 окт. 2023 г.
*/


#include "Interfaces.h"


#if defined(USING_LOGGING)

#include <cstdarg>

namespace Logging {



void IUsingLogging::Debug(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(Level::Debug, format, args);
	va_end(args);
}

void IUsingLogging::Info(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(Level::Info, format, args);
	va_end(args);
}

void IUsingLogging::Warn(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(Level::Warn, format, args);
	va_end(args);
}

void IUsingLogging::Error(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(Level::Error, format, args);
	va_end(args);
}

void IUsingLogging::Fatal(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Log(Level::Fatal, format, args);
	va_end(args);
}




void ILogger::Enable()
{
	_isEnable = true;
}

void ILogger::Disable()
{
	_isEnable = false;
}

bool ILogger::IsEnable() const
{
	return _isEnable;
}

void ILogger::SetFilter(FilterStrategy* filter)
{
	if (filter != nullptr)
		_filter = filter;
}

FilterStrategy* ILogger::GetFilter()
{
	return _filter;
}

void ILogger::SetFormatter(FormatterStrategy* formatter)
{
	if (formatter != nullptr)
		_formatter = formatter;
}

FormatterStrategy* ILogger::GetFormatter()
{
	return _formatter;
}



} // namespace Logging
#endif // defined(USING_LOGGING)