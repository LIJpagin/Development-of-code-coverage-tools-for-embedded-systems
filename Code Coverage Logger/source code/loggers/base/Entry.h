#ifndef ENTRY_H
#define ENTRY_H

#include "Configuration.h"

#if defined(USING_LOGGING)
#define START_ACCURATE_CLOCK() Logging::BaseEntry::StartAccurateClock()


#include <ctime>
#include "Enums.h"
#include "../../TimeStamp.h"
#include "../../SharedPtr.h"

class AccurateClock;

namespace Logging {


/// Базовый класс записей логеров.
class BaseEntry
{
protected:
	/// Статическое поле для выдачи id записям лога.
	static unsigned _counterEntry;

public:
	/// Идентификатор записи.
	unsigned id { 0 };

	/// Конструктор не присваивает id созданным через него записям.
	BaseEntry() = default;
	virtual ~BaseEntry() = default;
	BaseEntry(const BaseEntry&) = default;
	BaseEntry(BaseEntry&& moved) = default;
	BaseEntry& operator=(const BaseEntry&) = default;
	BaseEntry& operator=(BaseEntry&& moved) = default;
};

/// Класс неформатированных записей логеров, хранящих минимальную информацию. 
class MinLogEntry : public BaseEntry
{
protected:
	/// Точные часы для фиксации времени.
	static AccurateClock _clock;

public:
	/// Время фиксации записи в формате Unix и количество тактов от времени фиксации для увеличения точности времени.
	TimeStamp timeStamp {};
	/// Уровень важности записи.
	Level level { Level::Off };
	/// Сообщение записи.
	SharedPtr<char> message;

	static void StartAccurateClock();

	/// Конструктор не присваивает id созданным через него записям.
	MinLogEntry() = default;
	MinLogEntry(const Level level, const char* format, ...);
	~MinLogEntry() = default;
	MinLogEntry(const MinLogEntry&) = default;
	MinLogEntry(MinLogEntry&& moved) = default;
	MinLogEntry& operator=(const MinLogEntry&) = default;
	MinLogEntry& operator=(MinLogEntry&& moved) = default;
};

/// Класс неформатированных записей логеров, хранящих подробную информацию. 
class LogEntry : public MinLogEntry
{
public:
	/// Имя файла, в котором вызвано создание записи, будет установлено в конструкторе макросом __FILE__.
	const char* nameFile { nullptr };
	/// Имя функции, будет установлено в конструкторе макросом __func__.
	const char* nameFunction { nullptr };
	/// Имя номер строки, будет установлен в конструкторе макросом __LINE__.
	unsigned numberLine { 0 };
	/// Категории лога для фильтрации.
	unsigned tags { 0 };

	/// Конструктор не присваивает id созданным через него записям.
	LogEntry() = default;
	LogEntry(const Level level, const char* format, ...);
	LogEntry(const Level level, const char* nameFile,
		const char* nameFunction, const unsigned numberLine,
		const char* format, ...);
	~LogEntry() = default;
	LogEntry(const LogEntry&) = default;
	LogEntry(LogEntry&& moved) = default;
	LogEntry& operator=(const LogEntry&) = default;
	LogEntry& operator=(LogEntry&& moved) = default;
};

#if defined(USING_LOGGING_COVERAGE)

/// Перечисление со структурами кода, которые анализирует покрытие.
enum class Statement : u8
{
	None,
	If,
	Else,
	For,
	While,
	DoWhile,
	SwitchCase,
	Function,
	Lambda
};

#pragma pack(push, 1)
/// Класс записей покрытия кода.
class CovEntry : public BaseEntry
{
public:
	/// Структура кода, которая встретилась логеру покрытия кода.
	Statement statement { Statement::None };
	/// Номер имени файла, в котором вызвано создание записи.
	u16 numberNameFile { 0 };
	/// Имя номер строки, будет установлен в конструкторе макросом __LINE__.
	u16 numberLine { 0 };

	/// Конструктор не присваивает id созданным через него записям.
	CovEntry() = default;
	CovEntry(const CovEntry&) = default;
	CovEntry(CovEntry&& moved) = default;
	CovEntry(Statement statement, const unsigned numberNameFile,
		const unsigned numberLine);
	CovEntry& operator=(const CovEntry&) = default;
	CovEntry& operator=(CovEntry&& moved) = default;
	virtual ~CovEntry() = default;
};
#pragma pack(pop)

#endif // defined(USING_LOGGING_COVERAGE)


} // namespace Logging

#else
#define START_ACCURATE_CLOCK()
#endif // defined(USING_LOGGING)
#endif // ENTRY_H
