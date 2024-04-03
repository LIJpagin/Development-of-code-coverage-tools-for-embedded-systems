#ifndef ACCURATE_CLOCK_H
#define ACCURATE_CLOCK_H

#include <ctime>
#include "TimeStamp.h"
#include "../interfaces/iusingtimers.h"

/*!
	\brief Класс, предоставляющий интерфейс для создания класса часов, работающих на тактовом счетчике.
	При переносе на другую платформу будет необходимо реализовать данный интерфейс с классами счетчика и часов на этой платформе.
	Если класс точных часов уже будет реализован, необходимо создать класс адаптер.
 */
class IAccurateClock
{
protected:
	/// Время старта блока для расчета времени относительно начала работы.
	static time_t _startTime;
	/// Такты, прошедшие с начала работы блока.
	static uint64_t _cycles;

public:
	/// Установить время старта блока.
	static void SetStartTime(const time_t startTime);
	/// Получить время старта блока.
	static time_t GetStartTime();
	/// Получить время старта блока в формате TimeStamp.
	static TimeStamp GetStartTimeStamp();

	IAccurateClock() = default;
	virtual ~IAccurateClock() = default;

	/// Запуск таймера и счетчика.
	virtual void Start() { }
	/// Остановка таймера и счетчика.
	virtual void Stop() { }
	/// Временную метку от начала работы блока.
	virtual TimeStamp GetTimeStamp() const = 0;
};



class Counter;
/*!
	\brief Класс, реализующий работу со временем на блоке МФИ.
 */
class AccurateClock : public IAccurateClock, public IUsingTimers
{
protected:
	/// Интервал срабатывания таймера для обновления значения времени.
	static constexpr unsigned DELAY_TIME_VALUE_UPDATE { 1000 };
	/// Счетчик тактов
	static Counter _cyclesCounter;
	/// Время старта блока для расчета времени относительно начала работы.
	static time_t _startTime;

public:
	/// Запуск таймера и счетчика.
	void Start() override;
	/// Остановка таймера и счетчика.
	void Stop() override;
	/// Временную метку от начала работы блока.
	TimeStamp GetTimeStamp() const;

	/// Обработчик сообщений. В данной перегрузке обрабатываются сообщения таймера обновлении времени.
	DISPATCH_MESSAGE_RESULT MFD_DispatchMessage(MESSAGES message, DWORD lParam = 0, DWORD wParam = 0) override;
};

#endif // ACCURATE_CLOCK_H