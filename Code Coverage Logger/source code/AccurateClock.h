#ifndef ACCURATE_CLOCK_H
#define ACCURATE_CLOCK_H

#include <ctime>
#include "TimeStamp.h"
#include "../interfaces/iusingtimers.h"

/*!
	\brief �����, ��������������� ��������� ��� �������� ������ �����, ���������� �� �������� ��������.
	��� �������� �� ������ ��������� ����� ���������� ����������� ������ ��������� � �������� �������� � ����� �� ���� ���������.
	���� ����� ������ ����� ��� ����� ����������, ���������� ������� ����� �������.
 */
class IAccurateClock
{
protected:
	/// ����� ������ ����� ��� ������� ������� ������������ ������ ������.
	static time_t _startTime;
	/// �����, ��������� � ������ ������ �����.
	static uint64_t _cycles;

public:
	/// ���������� ����� ������ �����.
	static void SetStartTime(const time_t startTime);
	/// �������� ����� ������ �����.
	static time_t GetStartTime();
	/// �������� ����� ������ ����� � ������� TimeStamp.
	static TimeStamp GetStartTimeStamp();

	IAccurateClock() = default;
	virtual ~IAccurateClock() = default;

	/// ������ ������� � ��������.
	virtual void Start() { }
	/// ��������� ������� � ��������.
	virtual void Stop() { }
	/// ��������� ����� �� ������ ������ �����.
	virtual TimeStamp GetTimeStamp() const = 0;
};



class Counter;
/*!
	\brief �����, ����������� ������ �� �������� �� ����� ���.
 */
class AccurateClock : public IAccurateClock, public IUsingTimers
{
protected:
	/// �������� ������������ ������� ��� ���������� �������� �������.
	static constexpr unsigned DELAY_TIME_VALUE_UPDATE { 1000 };
	/// ������� ������
	static Counter _cyclesCounter;
	/// ����� ������ ����� ��� ������� ������� ������������ ������ ������.
	static time_t _startTime;

public:
	/// ������ ������� � ��������.
	void Start() override;
	/// ��������� ������� � ��������.
	void Stop() override;
	/// ��������� ����� �� ������ ������ �����.
	TimeStamp GetTimeStamp() const;

	/// ���������� ���������. � ������ ���������� �������������� ��������� ������� ���������� �������.
	DISPATCH_MESSAGE_RESULT MFD_DispatchMessage(MESSAGES message, DWORD lParam = 0, DWORD wParam = 0) override;
};

#endif // ACCURATE_CLOCK_H