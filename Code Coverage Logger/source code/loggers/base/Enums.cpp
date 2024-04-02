#include "Enums.h"

#if defined(USING_LOGGING)
namespace Logging {


unsigned operator|(const Level lhs, const Level rhs)
{
	return static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs);
}

unsigned operator|(const unsigned lhs, const Level rhs)
{
	return lhs | static_cast<unsigned>(rhs);
}

unsigned operator&(const unsigned lhs, const Level rhs)
{
	return lhs & static_cast<unsigned>(rhs);
}

const char* LevelToString(const Level e)
{
	switch (e)
	{
		case Level::Off:	return " Off ";
		case Level::Debug:	return "Debug";
		case Level::Info:	return " Info";
		case Level::Warn:	return " Warn";
		case Level::Error:	return "Error";
		case Level::Fatal:	return "Fatal";
		default:			return "-----";
	}
}

unsigned operator|(const Item lhs, const Item rhs)
{
	return static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs);
}

unsigned operator|(const unsigned lhs, const Item rhs)
{
	return lhs | static_cast<unsigned>(rhs);
}

unsigned operator&(const unsigned lhs, const Item rhs)
{
	return lhs & static_cast<unsigned>(rhs);
}

unsigned operator|(const Tag lhs, const Tag rhs)
{
	return static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs);
}

unsigned operator|(const unsigned lhs, const Tag rhs)
{
	return lhs | static_cast<unsigned>(rhs);
}

unsigned operator&(const unsigned lhs, const Tag rhs)
{
	return lhs & static_cast<unsigned>(rhs);
}

const char* TagToString(const Tag e)
{
	switch (e)
	{
		case Tag::None:		return "None";
		case Tag::Arinc429: return "Arinc429";
		case Tag::Arinc708: return "Arinc708";
		case Tag::Rs232:	return "Rs232";
		case Tag::SVV:		return "SVV";
		case Tag::Camera:	return "Camera";
		case Tag::Radar:	return "Radar";
		case Tag::TAWS:		return "TAWS";
		case Tag::TCAS:		return "TCAS";
		default:			return "unknown";
	}
}



} // namespace Logging
#endif // defined(USING_LOGGING)