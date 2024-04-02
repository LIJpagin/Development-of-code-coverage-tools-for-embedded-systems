/*!
	\brief Файл, содержащий логгер, работающий с ПК по USB
	\author Шпагин Д.
	\date 18 окт. 2023 г.
*/
#include "UsbLogger.h"

#if defined(USING_LOGGING) && defined(USING_LOGGING_USB)


#include <cstdarg>

namespace Logging {


void UsbLogger::Log(LogEntry& entry)
{
	if (_filter->Check(entry)) {
		if (_sink.IsFull())
			++_numberOfLostEntries;
		_sink.AddValueBack(entry);
	}
}

void UsbLogger::Log(const Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	auto entry = LogEntry(level, format, args);
	Log(entry);
	va_end(args);
}

void UsbLogger::Dump()
{

}

unsigned UsbLogger::GetNumberOfLostEntries() const
{
	return _numberOfLostEntries;
}


UsbLogger::UsbLogger()
{
	_filter = new DefaultFilter();
	_formatter = new DefaultFormatter();
	_formatter->SetFlagsItems(Item::EntryLevel | Item::RelativeTime | Item::NameFile | Item::NameFunction | Item::NumberLine);
}

UsbLogger::~UsbLogger()
{
	delete _filter;
	delete _formatter;
}


} // namespace Logging


#endif // defined(USING_LOGGING) && defined(USING_LOGGING_USB)
