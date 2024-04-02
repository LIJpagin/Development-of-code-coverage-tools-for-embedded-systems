#ifndef ENUMS_H
#define ENUMS_H

#include "Configuration.h"

#if defined(USING_LOGGING)
namespace Logging {



/*!
	\brief Класс перечисления уровней логирования.
	Уровни логирования работают по типу флагов и имеют приоритет.
	Устанавливая определенный уровень для логгера, все сообщения,
	уровень которых ниже установленного, не будут выводиться.
 */
enum class Level : unsigned
{
	Off		= 0,		///< Сообщения не выводятся. Наиболее низкий приоритет.
	Debug	= (1 << 4),	///< Информация, которая пригодится для отладки программы.
	Info	= (1 << 3),	///< Обычные и стандартные сообщения.
	Warn	= (1 << 2),	///< Не фатальные предупреждения.
	Error	= (1 << 1),	///< Записи ошибок.
	Fatal	= (1 << 0),	///< Фатальная ошибка. Наиболее высокий приоритет.
};
/// Перегрузка оператора битового ИЛИ для удобной работы с закрытыми перечислениями Level.
extern unsigned operator|(const Level lhs, const Level rhs);
/// Перегрузка оператора битового ИЛИ для удобной работы с закрытыми перечислениями Level.
extern unsigned operator|(const unsigned  lhs, const Level rhs);
/// Перегрузка оператора битового И для удобной работы с закрытыми перечислениями Level.
extern unsigned operator&(const unsigned  lhs, const Level rhs);
/// Перевод перечисления в строку
extern const char* LevelToString(const Level e);


/*!
	\brief Класс перечисления элементов, которые будут вставлены логером в записи.
 */
enum class Item : unsigned
{
	EntryLevel		= (1 << 1),	///< Уровень логирования.
	AbsoluteTime	= (1 << 2),	///< Время вызова лога.
	RelativeTime	= (1 << 3),	///< Время вызова лога.
	NameFile		= (1 << 4),	///< Имя файла, в котором вызван лог.
	NameFunction	= (1 << 5),	///< Имя функции, в которой вызван лог.
	NameTag			= (1 << 6),	///< Тег лога, используется для фильтрации.
	NumberLine		= (1 << 7),	///< Номер строки, на которой расположен вызов лога.
};
/// Перегрузка оператора битового ИЛИ для удобной работы с закрытыми перечислениями Item.
extern unsigned operator|(const Item lhs, const Item rhs);
/// Перегрузка оператора битового ИЛИ для удобной работы с закрытыми перечислениями Item.
extern unsigned operator|(const unsigned lhs, const Item rhs);
/// Перегрузка оператора битового И для удобной работы с закрытыми перечислениями Item.
extern unsigned operator&(const unsigned lhs, const Item rhs);



/*!
	\brief Класс перечисления категорий для фильтрации по категориям.
 */
enum class Tag : unsigned
{
	None		= 0,		///<
	Arinc429	= (1 << 1),	///< 
	Arinc708	= (1 << 2),	///< 
	Rs232		= (1 << 3),	///< 
	SVV			= (1 << 4),	///< 
	Camera		= (1 << 5),	///< 
	Radar		= (1 << 6),	///< 
	TAWS		= (1 << 7),	///< 
	TCAS		= (1 << 8),	///<
	// И так далее, максимум 32 флага
};
/// Перегрузка оператора битового ИЛИ для удобной работы с закрытыми перечислениями Tag.
extern unsigned operator|(const Tag lhs, const Tag rhs);
/// Перегрузка оператора битового ИЛИ для удобной работы с закрытыми перечислениями Tag.
extern unsigned operator|(const unsigned  lhs, const Tag rhs);
/// Перегрузка оператора битового И для удобной работы с закрытыми перечислениями Tag.
extern unsigned operator&(const unsigned  lhs, const Tag rhs);
/// Перевод перечисления в строку
extern const char* TagToString(const Tag e);



} // namespace Logging
#endif // defined(USING_LOGGING)
#endif // ENUMS_H