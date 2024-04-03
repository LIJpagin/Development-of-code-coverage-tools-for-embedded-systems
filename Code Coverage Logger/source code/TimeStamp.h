#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include <ctime>
#include "../types.h"


/*!
	\brief  ласс, реализующий хранение и работу с точным временем, получаемым от AccurateClock
 */
class TimeStamp
{
protected:
	// «афиксированное значение времени в наносекундах
	uint64_t _nanoseconds { 0 };

public:
	TimeStamp() = default;
	///  онструктор переведет врем€ в наносекунды
	TimeStamp(const time_t posixDateTime);
	TimeStamp(const uint64_t nanoseconds);
	TimeStamp(const time_t posixDateTime, const unsigned nanoseconds);

	// ѕолучить количество секунд
	uint64_t GetSeconds() const;
	// ѕолучить количество миллисекунд
	uint64_t GetMilliSeconds() const;
	// ѕолучить количество микросекунд
	uint64_t GetMicroSeconds() const;
	// ѕолучить количество наносекунд
	uint64_t GetNanoSeconds() const;

	/// ѕолучить дату и врем€, округленные до секунд вниз, в формате POSIX
	time_t GetPosixDateTime() const;
	/// ѕолучить календарную дату и врем€, округленные до секунд вниз
	tm* GetCalendarDateTime() const;

	TimeStamp operator+(const TimeStamp& other) const;
	TimeStamp operator-(const TimeStamp& other) const;
	bool operator==(const TimeStamp& other) const;
	bool operator!=(const TimeStamp& other) const;
	bool operator<(const TimeStamp& other) const;
	bool operator>(const TimeStamp& other) const;
	bool operator<=(const TimeStamp& other) const;
	bool operator>=(const TimeStamp& other) const;

	operator uint64_t() const;
};


#endif // TIME_STAMP_H