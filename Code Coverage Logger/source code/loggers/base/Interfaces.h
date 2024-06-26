/*!
	\brief Файл, порождающий логгеры и описывающий их поведение.
	\author Шпагин Д.
	\date 18 окт. 2023 г.
	\Warning В данном файле присутствуют макросы, которые обязательны к применению, т.к. в релизе некоторые логгеры могут отсутствовать.
*/
#ifndef LOG_INTERFACES_H
#define LOG_INTERFACES_H

#include "Configuration.h"

#if defined(USING_LOGGING)
#define I_LOG(logger, level, format, ...) { Logging::LogEntry entry(level, LOGGING_FILE_NAME, \
			LOGGING_FUNCTION_NAME, __LINE__, format, ##__VA_ARGS__); logger->Log(entry); }
#define I_LOG_TAG(logger, level, tags, format, ...) { Logging::LogEntry entry(level, LOGGING_FILE_NAME, \
			LOGGING_FUNCTION_NAME, __LINE__, format, ##__VA_ARGS__); entry.tags = tags; logger->Log(entry); }
#define I_LOG_DEBUG(logger, format, ...)  I_LOG(Logging::Level::Debug, format, ##__VA_ARGS__))
#define I_LOG_INFO(logger, format, ...)   I_LOG(Logging::Level::Info, format, ##__VA_ARGS__))
#define I_LOG_WARN(logger, format, ...)   I_LOG(Logging::Level::Warn, format, ##__VA_ARGS__))
#define I_LOG_ERROR(logger, format, ...)  I_LOG(Logging::Level::Error, format, ##__VA_ARGS__))
#define I_LOG_FATAL(logger, format, ...)  I_LOG(Logging::Level::Fatal, format, ##__VA_ARGS__))

#define LEVEL_OFF	Logging::Level::Off
#define LEVEL_DEBUG	Logging::Level::Debug
#define LEVEL_INFO	Logging::Level::Info
#define LEVEL_WARN	Logging::Level::Warn
#define LEVEL_FATAL	Logging::Level::Fatal

#define GET_NAME_VAR(variable) #variable

#if defined(USING_FULL_FILE_NAME)
	#define LOGGING_FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
	#define LOGGING_FILE_NAME __FILE__
#endif

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600))
	#define LOGGING_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
	#define LOGGING_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
	#define LOGGING_FUNCTION_NAME __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
	#define LOGGING_FUNCTION_NAME __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
	#define LOGGING_FUNCTION_NAME __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
	#define LOGGING_FUNCTION_NAME __func__
#else
	#define LOGGING_FUNCTION_NAME "(unknown)"
#endif


#include "Entry.h"
#include "Filter.h"
#include "Formatter.h"
#include "../../../utils/CircleBuffer.h"


/// Пространство имен со всеми деталями логирования.
namespace Logging {



/**
	\brief Абстрактный класс c общим интерфейсом для LogMaster и классов-наследников ILogger.
 */
class IUsingLogging
{
public:
	IUsingLogging() = default;
	IUsingLogging(const IUsingLogging&) = default;
	IUsingLogging(IUsingLogging&& moved) = default;
	virtual ~IUsingLogging() = default;
	IUsingLogging& operator=(const IUsingLogging&) = default;
	IUsingLogging& operator=(IUsingLogging&& moved) = default;

	/// Метод создания простой записи лога, необходимо реализовать в класса наследниках.
	virtual void Log(const Level level, const char* format, ...) = 0;
	/// Метод создания записи лога с помощью класса LogEntry.
	virtual void Log(LogEntry& entry) = 0;
	/// Метод-обертка метода Log, добавляющая уровень Debug.
	virtual void Debug(const char* format, ...);
	/// Метод-обертка метода Log, добавляющая уровень Info.
	virtual void Info(const char* format, ...);
	/// Метод-обертка метода Log, добавляющая уровень Warning.
	virtual void Warn(const char* format, ...);
	/// Метод-обертка метода Log, добавляющая уровень Error.
	virtual void Error(const char* format, ...);
	/// Метод-обертка метода Log, добавляющая уровень Fatal.
	virtual void Fatal(const char* format, ...);
};



/**
	\brief Абстрактный базовый класс логгеров.
	Под этим классом логгеры будут управляться в классе LogMaster согласно паттерну Наблюдатель (Observer).
	\Warning При наследовании необходима реализация Singleton логгера.
 */
class ILogger : public IUsingLogging
{
protected:
	/// Должен ли работать логер.
	bool _isEnable { true };

	FilterStrategy* _filter { nullptr };
	FormatterStrategy* _formatter { nullptr };

	/// Конструктор скрыт, чтобы исключить создание логгеров класса больше чем один (Singleton).
	ILogger() = default;
	~ILogger() override = default;

public:
	ILogger(const ILogger&) = delete;
	ILogger(ILogger&& moved) = delete;
	const ILogger& operator=(const ILogger&) = delete;
	ILogger& operator=(ILogger&& moved) = delete;

	/// Включить логирование.
	virtual void Enable();
	/// Выключить логирование.
	virtual void Disable();
	/// Возвращает true, если логер работает.
	bool IsEnable() const;
	/// Установить фильтр логирования.
	void SetFilter(FilterStrategy* filter);
	/// Получить фильтр
	FilterStrategy* GetFilter();
	/// Установить форматер логирования.
	void SetFormatter(FormatterStrategy* formatter);
	/// Получить форматер.
	FormatterStrategy* GetFormatter();
};



/**
	\brief Интерфейс для логгеров, которым необходим буфер.
 */
template<typename Entry, unsigned SizeSink>
class IUsingSink
{
public:
	/// Более короткое имя для буфера записей логгера.
	using Sink = CircleBuffer<Entry, SizeSink>;
	/// Размер буфера для записей логгера
	static constexpr unsigned SIZE_OF_SINK { SizeSink };
	/// Отправить весь накопленный буфер в канал логирования.
	virtual void Dump() = 0;
	/// Получить буфер логгера. Может быть использовано, если логеру нужно отдать данные на запись.
	Sink* GetSink() { return &_sink; }

protected:
	/// Буфер сбора записей логера.
	Sink _sink;

	~IUsingSink() = default;
};



/**
	\brief Singleton Мэйерса в виде шаблона.
	Использует отложенную инициализацию, т.е. инициализация объекта происходит только при первом вызове GetInstance().
	Пользуемся для создания классов наследников абстрактного класса ILogger, дабы сделать невозможным создание больше одного объекта класса.
	Наследуйте его, например, следующим образом:
	\code
	class UsbLogger : public Singleton<UsbLogger>
	{
	private:
		friend class Singleton;
	}
	\endcode
	После наследования появится метод GetInstance(), возвращающий указатель на статический объект, который будет инициализирован
	только при первом вызове этой функции.
 */
template <typename T>
class Singleton
{
protected:
	Singleton() = default;
	~Singleton() = default;

public:
	static T* GetInstance() {
		static T instance;
		return &instance;
	}
	Singleton(const Singleton<T>&) = delete;
	Singleton(Singleton&& moved) = delete;

	const Singleton<T>& operator=(const Singleton<T>&) = delete;
	Singleton& operator=(Singleton&& moved) = delete;
};


} // namespace Logging

#else

#define I_LOG(...)
#define I_LOG_TAG(...)
#define I_LOG_DEBUG(...)
#define I_LOG_INFO(...)
#define I_LOG_WARN(...)
#define I_LOG_ERROR(...)
#define I_LOG_FATAL(...)
#define GET_NAME_VAR(...) 

#endif // !defined(USING_LOGGING)
#endif // LOG_INTERFACES_H