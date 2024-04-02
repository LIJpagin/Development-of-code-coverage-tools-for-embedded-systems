#ifndef FILTER_H
#define FILTER_H

#include "Configuration.h"

#if defined(USING_LOGGING)

#include "Entry.h"
#include "Enums.h"


/// Пространство имен со всеми деталями логирования.
namespace Logging {



class FilterStrategy
{
protected:
	/// Флаги уровней логирования.
	unsigned _flagsLevels = Level::Warn | Level::Error | Level::Fatal | Level::Info;
	/// Флаги категорий 
	unsigned _flagsTags = static_cast<unsigned>(Tag::None);

public:
	virtual ~FilterStrategy() = default;
	/// Метод проверки записи на то, нужно ли ее логировать.
	virtual bool Check(const LogEntry& entry) = 0;
	/*!
		Метод позволяет установить текущий уровень логирования.
		Уровни выше приоритетом также будут выводиться.
	 */
	void SetLevel(const Level level);
	/// Метод позволяет установить нужные уровни логирования по типу флагов.
	void SetFlagsLevels(const unsigned flags);
	/// Метод для получения выставленных флагов логирования.
	unsigned GetFlagsLevels() const;
	/// Метод позволяет установить нужные категории лога по типу флагов для фильтрации.
	void SetFlagsTags(const unsigned flags);
	/// Метод для получения выставленных флагов категорий.
	unsigned GetFlagsTags() const;
};


class DefaultFilter : public FilterStrategy
{
public:
	~DefaultFilter() override = default;
	bool Check(const LogEntry& entry) override;
};



} // namespace Logging
#endif // !defined(USING_LOGGING)
#endif // FILTER_H