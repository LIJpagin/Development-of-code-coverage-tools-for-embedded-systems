#include "TimeStamp.h"

TimeStamp::TimeStamp(const uint64_t nanoseconds)
	: _nanoseconds(nanoseconds)
{ }

TimeStamp::TimeStamp(const time_t posixDateTime)
	: _nanoseconds(static_cast<uint64_t>(posixDateTime)
	* static_cast<uint64_t>(1000000000))
{ }

TimeStamp::TimeStamp(const time_t posixDateTime, const unsigned nanoseconds)
	: _nanoseconds(static_cast<uint64_t>(posixDateTime)
	* static_cast<uint64_t>(1000000000)
	+ static_cast<uint64_t>(nanoseconds))
{ }

uint64_t TimeStamp::GetSeconds() const
{
	return _nanoseconds / static_cast<uint64_t>(1000000000);
}

uint64_t TimeStamp::GetMilliSeconds() const
{
	return _nanoseconds / static_cast<uint64_t>(1000000);
}

uint64_t TimeStamp::GetMicroSeconds() const
{
	return _nanoseconds / static_cast<uint64_t>(1000);
}

uint64_t TimeStamp::GetNanoSeconds() const
{
	return _nanoseconds;
}

time_t TimeStamp::GetPosixDateTime() const
{
	return static_cast<time_t>(_nanoseconds) / static_cast<uint64_t>(1000000000);
}

tm* TimeStamp::GetCalendarDateTime() const
{
	const time_t tmp = GetPosixDateTime();
	return localtime(&tmp);
}

TimeStamp TimeStamp::operator+(const TimeStamp& other) const
{
	return TimeStamp(this->_nanoseconds + other._nanoseconds);
}

TimeStamp TimeStamp::operator-(const TimeStamp& other) const
{
	return TimeStamp(this->_nanoseconds - other._nanoseconds);
}

bool TimeStamp::operator==(const TimeStamp& other) const
{
	return this->_nanoseconds == other._nanoseconds;
}

bool TimeStamp::operator!=(const TimeStamp& other) const
{
	return this->_nanoseconds != other._nanoseconds;
}

bool TimeStamp::operator<(const TimeStamp& other) const
{
	return this->_nanoseconds < other._nanoseconds;
}

bool TimeStamp::operator>(const TimeStamp& other) const
{
	return this->_nanoseconds > other._nanoseconds;
}

bool TimeStamp::operator<=(const TimeStamp& other) const
{
	return this->_nanoseconds <= other._nanoseconds;
}

bool TimeStamp::operator>=(const TimeStamp& other) const
{
	return this->_nanoseconds >= other._nanoseconds;
}

inline TimeStamp::operator uint64_t() const {
	return _nanoseconds;
}
