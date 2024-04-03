#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include <ctime>
#include "../types.h"


/*!
	\brief �����, ����������� �������� � ������ � ������ ��������, ���������� �� AccurateClock
 */
class TimeStamp
{
protected:
	// ��������������� �������� ������� � ������������
	uint64_t _nanoseconds { 0 };

public:
	TimeStamp() = default;
	/// ����������� ��������� ����� � �����������
	TimeStamp(const time_t posixDateTime);
	TimeStamp(const uint64_t nanoseconds);
	TimeStamp(const time_t posixDateTime, const unsigned nanoseconds);

	// �������� ���������� ������
	uint64_t GetSeconds() const;
	// �������� ���������� �����������
	uint64_t GetMilliSeconds() const;
	// �������� ���������� �����������
	uint64_t GetMicroSeconds() const;
	// �������� ���������� ����������
	uint64_t GetNanoSeconds() const;

	/// �������� ���� � �����, ����������� �� ������ ����, � ������� POSIX
	time_t GetPosixDateTime() const;
	/// �������� ����������� ���� � �����, ����������� �� ������ ����
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