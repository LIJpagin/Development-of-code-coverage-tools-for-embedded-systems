#include "Formatter.h"

#if defined(USING_LOGGING)

#include <cstdio>
#include <cstdlib>
#include "../../AccurateClock.h"

namespace Logging {


void FormatterStrategy::SetFlagsItems(const unsigned flags)
{
	_flagsItems = flags;
}

unsigned FormatterStrategy::GetFlagsItems() const
{
	return _flagsItems;
}

int DefaultFormatter::Build(char* buffer, const LogEntry& entry)
{
	char* current = buffer;

	// Уровень логирования
	if ((_flagsItems & Item::EntryLevel) != 0)
		current += sprintf(current, " [%s] ", LevelToString(entry.level));

	// Время вызова лога
	if ((_flagsItems & Item::AbsoluteTime) != 0)
	{
		const tm* time = (AccurateClock::GetStartTimeStamp() + entry.timeStamp).GetCalendarDateTime();
		current += sprintf(current, "[%02d.%02d.%d %02d:%02d:%02d.%09lld] ",
			time->tm_mday, time->tm_mon + 1, time->tm_year + 1900,
			time->tm_hour, time->tm_min, time->tm_sec, entry.timeStamp.GetNanoSeconds() % 1000000000);
	}
	else if ((_flagsItems & Item::RelativeTime) != 0)
	{
		const tm* time = entry.timeStamp.GetCalendarDateTime();
		current += sprintf(current, "[%02d:%02d:%02d.%09lld] ", time->tm_hour - 3, time->tm_min,
			time->tm_sec, entry.timeStamp.GetNanoSeconds() % 1000000000);
	}

	// Имя файла, в котором вызван лог
	if ((_flagsItems & Item::NameFile) != 0)
		current += sprintf(current, "%s ", entry.nameFile);

	// Имя функции, в которой вызван лог
	if ((_flagsItems & Item::NameFunction) != 0)
		current += sprintf(current, "%s ", entry.nameFunction);

	// Номер строки, на которой расположен вызов лога
	if ((_flagsItems & Item::NumberLine) != 0)
		current += sprintf(current, "(%d) ", entry.numberLine);

	// Сообщение лога
	current += sprintf(current, "%s", entry.message.Get());
	return abs(current - buffer);
}

int JsonFormatter::Build(char* buffer, const LogEntry& entry)
{
	char* current = buffer;

	*current = '{';
	++current;

	// Уровень логирования.
	if ((_flagsItems & Item::EntryLevel) != 0)
		current += sprintf(current, "Level : %s, ", LevelToString(entry.level));

	// Время вызова лога.
	if ((_flagsItems & Item::AbsoluteTime) != 0) {
		current += sprintf(current, "AbsoluteTime : %lld",
		   (AccurateClock::GetStartTimeStamp() + entry.timeStamp).GetNanoSeconds());
	}
	else if ((_flagsItems & Item::RelativeTime) != 0) {
		current += sprintf(current, "RelativeTime : %lld", entry.timeStamp.GetNanoSeconds());
	}

	// Имя файла, в котором вызван лог.
	if ((_flagsItems & Item::NameFile) != 0)
		current += sprintf(current, "NameFile : %s, ", entry.nameFile);

	// Имя функции, в которой вызван лог.
	if ((_flagsItems & Item::NameFunction) != 0)
		current += sprintf(current, "NameFunction : %s, ", entry.nameFunction);

	// Номер строки, на которой расположен вызов лога.
	if ((_flagsItems & Item::NumberLine) != 0)
		current += sprintf(current, "NumberLine : %d, ", entry.numberLine);

	current += sprintf(current, "Message : %s, ", entry.message.Get());

	current -= 2;
	*current = '}';
	return abs(current - buffer);
}


} // namespace Logging


#endif // defined(USING_LOGGING)
