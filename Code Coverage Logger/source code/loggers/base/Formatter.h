#ifndef FORMATTER_H
#define FORMATTER_H

#include "Configuration.h"

#if defined(USING_LOGGING)

#include "Entry.h"
#include "Enums.h"


/// Пространство имен со всеми деталями логирования.
namespace Logging {


class FormatterStrategy
{
protected:
	/// Флаги элементов лога.
	unsigned _flagsItems = Item::EntryLevel | Item::AbsoluteTime | Item::NameFile | Item::NameFunction | Item::NumberLine;

public:
	virtual ~FormatterStrategy() = default;
	/// Метод сборки форматером строки из записи.
	virtual int Build(char* buffer, const LogEntry& entry) = 0;
	/// Метод позволяет установить нужные элементы лога по типу флагов.
	void SetFlagsItems(const unsigned flags);
	/// Метод для получения выставленных флагов элементов лога.
	unsigned GetFlagsItems() const;
};


class DefaultFormatter : public FormatterStrategy
{
public:
	int Build(char* buffer, const LogEntry& entry) override;
};


class JsonFormatter : public FormatterStrategy
{
public:
	int Build(char* buffer, const LogEntry& entry) override;
};


} // namespace Logging
#endif // !defined(USING_LOGGING)
#endif // FORMATTER_H