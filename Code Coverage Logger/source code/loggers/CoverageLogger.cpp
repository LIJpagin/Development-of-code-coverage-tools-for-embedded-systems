/*
 *  CoverageLogger.h
 *  Created on: 18 окт. 2023 г.
 *      Author: Shpagin_D
 */

#include "CoverageLogger.h"

#if defined(USING_LOGGING) && defined(USING_LOGGING_COVERAGE)
namespace Logging {


void CoverageLogger::Log(const Level level, const char* format, ...)
{ }

void CoverageLogger::Log(LogEntry& entry)
{ }

unsigned CoverageLogger::GetNumberOfLostEntries() const
{
	return _numberOfLostEntries;
}

void CoverageLogger::Log(CovEntry& entry)
{
	if (_sink.IsFull())
		++_numberOfLostEntries;
	_sink.AddValueBack(entry);
}

void CoverageLogger::Dump()
{ }


} // namespace Logging
#endif // defined(USING_LOGGING) && defined(USING_LOGGING_COVERAGE)
