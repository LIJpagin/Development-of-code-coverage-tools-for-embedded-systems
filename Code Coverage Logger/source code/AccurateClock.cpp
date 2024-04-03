#include "AccurateClock.h"
#include "../drivers/counter.h"
#if defined(EMULATION)
#include "../pld/xparameters.h"
#endif


/// Количество секунд за один такт процессора.
constexpr unsigned NANOSECONDS_PER_CYCLES { 1000000000 / XPAR_CPU_CORE_CLOCK_FREQ_HZ };

uint64_t IAccurateClock::_cycles { 0 };

time_t IAccurateClock::_startTime { 0 };

void IAccurateClock::SetStartTime(const time_t startTime)
{
	_startTime = startTime;
}

time_t IAccurateClock::GetStartTime()
{
	return _startTime;
}

TimeStamp IAccurateClock::GetStartTimeStamp()
{
	return TimeStamp(_startTime, 0);
}





Counter AccurateClock::_cyclesCounter { Counter::ctMeasure };

void AccurateClock::Start()
{
	SetTimer(DELAY_TIME_VALUE_UPDATE, msTimeValueUpdate);
	_cyclesCounter.ResetAndStart();
}

void AccurateClock::Stop()
{
	KillTimer(msTimeValueUpdate);
	_cyclesCounter.ResetAndStop();
}

TimeStamp AccurateClock::GetTimeStamp() const
{
	return TimeStamp((_cycles + static_cast<uint64_t>(_cyclesCounter.GetCount()))
		* static_cast<uint64_t>(NANOSECONDS_PER_CYCLES));
}

DISPATCH_MESSAGE_RESULT AccurateClock::MFD_DispatchMessage(MESSAGES message, DWORD lParam, DWORD wParam)
{
	DISPATCH_MESSAGE_RESULT result(dmrNotDispatched);

	switch (message)
	{
		case msTimeValueUpdate:
			_cycles += static_cast<uint64_t>(_cyclesCounter.GetCount());
			_cyclesCounter.ResetAndStart();
			break;
		default:
			result = dmrNotDispatched;
			break;
	}
	return result;
}