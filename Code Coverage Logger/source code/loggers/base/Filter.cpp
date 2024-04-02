#include "Filter.h"

#if defined(USING_LOGGING)
namespace Logging {



void FilterStrategy::SetLevel(const Level level)
{
	if (level == Level::Off)
		_flagsLevels = 0;
	_flagsLevels = ((static_cast<unsigned>(level) - 1) << 1) | 1;
}

void FilterStrategy::SetFlagsLevels(const unsigned flags)
{
	this->_flagsLevels = flags;
}

unsigned FilterStrategy::GetFlagsLevels() const
{
	return _flagsLevels;
}

void FilterStrategy::SetFlagsTags(const unsigned flags)
{
	_flagsTags = flags;
}

unsigned FilterStrategy::GetFlagsTags() const
{
	return _flagsTags;
}

bool DefaultFilter::Check(const LogEntry& entry)
{
	if ((_flagsLevels & entry.level) != 0)
		return true;
	return false;
}



} // namespace Logging
#endif // defined(USING_LOGGING)