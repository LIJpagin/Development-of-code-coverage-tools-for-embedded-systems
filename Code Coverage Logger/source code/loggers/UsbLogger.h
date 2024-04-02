/*!
	\brief Файл, содержащий логгер, работающий с ПК по USB
	\author Шпагин Д.
	\date 18 окт. 2023 г.
	\Warning В данном файле присутствуют макросы, которые обязательны к применению, т.к. в релизе некоторые логгеры могут отсутствовать
*/
#ifndef USB_LOGGER_H
#define USB_LOGGER_H

#include "base/Interfaces.h"

#if defined(USING_LOGGING) && defined(USING_LOGGING_USB)

#define GET_USB_LOGGER() Logging::UsbLogger::GetInstance()
#define USB_LOG(level, format, ...)	I_LOG(Logging::UsbLogger::GetInstance(), level, format, ##__VA_ARGS__)
#define USB_LOG_TAG(level, tags, format, ...) I_LOG_TAG(Logging::UsbLogger::GetInstance(), level, tags, format, ...)
#define USB_LOG_DEBUG(format, ...)	USB_LOG(Logging::Level::Debug, format, ##__VA_ARGS__)
#define USB_LOG_INFO(format, ...)	USB_LOG(Logging::Level::Info, format, ##__VA_ARGS__)
#define USB_LOG_WARN(format, ...)	USB_LOG(Logging::Level::Warn, format, ##__VA_ARGS__)
#define USB_LOG_ERROR(format, ...)	USB_LOG(Logging::Level::Error, format, ##__VA_ARGS__)
#define USB_LOG_FATAL(format, ...)	USB_LOG(Logging::Level::Fatal, format, ##__VA_ARGS__)


namespace Logging {



/// Класс логгера с отправкой по usb согласно протоколу обмена данными по usb
class UsbLogger final :
	public ILogger,
	public IUsingSink<LogEntry, SIZE_OF_ENTRIES_IN_USB_SINK>,
	public Singleton<UsbLogger>
{
	friend class Singleton;

	unsigned _numberOfLostEntries { 0 };

public:
	UsbLogger(const UsbLogger&) = delete;
	UsbLogger(UsbLogger&& moved) = delete;
	const UsbLogger& operator=(const UsbLogger&) = delete;
	UsbLogger& operator=(UsbLogger&& moved) = delete;

	void Log(LogEntry& entry) override;
	void Log(const Level level, const char* format, ...) override;
	/// Отправить весь накопленный буфер в канал логирования.
	void Dump() override;

	/// Получить количество потерянных записей
	unsigned GetNumberOfLostEntries() const;

protected:
	UsbLogger();
	~UsbLogger() override;
};



} // namespace Logging

#else

#define GET_USB_LOGGER() nullptr
#define USB_LOG(...)
#define USB_LOG_TAG(...)
#define USB_LOG_DEBUG(...) 
#define USB_LOG_INFO(...)
#define USB_LOG_WARN(...)
#define USB_LOG_ERROR(...)
#define USB_LOG_FATAL(...)

#endif // defined(USING_LOGGING) && defined(USING_LOGGING_USB)
#endif // USB_LOGGER_H
