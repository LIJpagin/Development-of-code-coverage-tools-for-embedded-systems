#ifndef MFI_LOGGER_H
#define MFI_LOGGER_H

#include "base/Configuration.h"

#if defined(USING_LOGGING) && defined(USING_LOGGING_MFD)

#define GET_MFD_LOGGER() Logging::MfdLogger::GetInstance()
#define MFD_LOG(level, format, ...)	I_LOG(Logging::MfdLogger::GetInstance(), level, format, ##__VA_ARGS__)
#define MFD_LOG_TAG(level, tags, format, ...) I_LOG_TAG(Logging::MfdLogger::GetInstance(), level, tags, format, ...)
#define MFD_LOG_DEBUG(format, ...)	MFD_LOG(Logging::Level::Debug, format, ##__VA_ARGS__)
#define MFD_LOG_INFO(format, ...)	MFD_LOG(Logging::Level::Info, format, ##__VA_ARGS__)
#define MFD_LOG_WARN(format, ...)	MFD_LOG(Logging::Level::Warn, format, ##__VA_ARGS__)
#define MFD_LOG_ERROR(format, ...)	MFD_LOG(Logging::Level::Error, format, ##__VA_ARGS__)
#define MFD_LOG_FATAL(format, ...)	MFD_LOG(Logging::Level::Fatal, format, ##__VA_ARGS__)


#include "../cmfdpoint.h"
#include "../../interfaces/iarchiveclient.h"
#include "../../interfaces/idrawable.h"
#include "../../utils/fonts.h"
#include "../setupmenu/iusingsetupmenu.h"
#include "../../interfaces/iusingmessages.h"
#include "base/Interfaces.h"


namespace Logging {


/// \brief Класс, реализующий логгер, выводящий информацию отладки на экран МФИ поверх всего остального 
class MfdLogger final :
	public ILogger,
	public IUsingSink<LogEntry, SIZE_OF_ENTRIES_IN_MFD_SINK>,
	public Singleton<MfdLogger>,
	public IArchiveClient,
	public IDrawable,
	public IUsingSetupMenu,
	public IUsingMessages
{
	friend class Singleton;

public:

	static const FONT SCREEN_FONT;

	// Константы консоли
	static constexpr int NUMBER_OF_ROWS		= 49;	//< Количество столбцов
	static constexpr int NUMBER_OF_LINES	= 21;	//< Количество строк
	static constexpr int FONT_WIDTH			= 10;	//< Ширина шрифта
	static constexpr int FONT_HEIGHT		= 20;	//< Высота шрифта
	static constexpr int PADDING_LEFT		= 50;	//< Отступ слева
	static constexpr int PADDING_TOP		= 28;	//< Отступ сверху

	bool IsControlOutput { false };		//< Управлять выводом диагностической информации
	bool IsPaused { false };			//< Пауза обновления информации

	MfdLogger(const MfdLogger&) = delete;
	MfdLogger(MfdLogger&& moved) = delete;
	const MfdLogger& operator=(const MfdLogger&) = delete;
	MfdLogger& operator=(MfdLogger&& moved) = delete;

	MODULE_ID GetID() override;

	void PrepareSystemSetting() override;
	void PrepareUserSetting() override { }
	void UserDefault() override { }

	void InitSetupMenu() override;
	void SetupMenuClick(int id, SETUP_ACTION action) override;
	/// Отрисовка консоли
	void DrawForeground() override;
	void LoadLanguage(unsigned int lang) override;
	DISPATCH_MESSAGE_RESULT MFD_DispatchMessage(MESSAGES message, DWORD lParam = 0, DWORD wParam = 0) override;

	/// Очистка консоли
	void Clear();
	/// Метод создания простой записи лога, выводящейся на экран МФИ
	void Log(LogEntry& entry) override;
	void Log(const Level level, const char* format, ...) override;
	void Dump() override { }

	/// Отрисовка удаленного управления
	///// \warning Выводит только в случае включения отладки и удаленного управления
	static void TraceRemoteControl(const char* format, ...);
	static WORD SystemTextOut(const char* str, const int x, const int y, COLOR pen = clWhite, const WORD count = 0);
	/// Вывод форматированного текста системным шрифтом (как sprintf)
	static WORD SystemFormat(const int x, const int y, COLOR pen, COLOR br, const char* format, ...);
	/// Вывод форматированного текста системным шрифтом (как sprintf)
	static WORD SystemFormat(const CMFDPoint point, COLOR pen, COLOR br, const char* format, ...);
	/// Вывод форматированного текста системным шрифтом (как sprintf)
	static WORD SystemFormat(const int x, const int y, const char* format, ...);
	/// Вывод форматированного текста системным шрифтом (как sprintf)
	static WORD SystemFormat(const CMFDPoint point, const char* format, ...);

	bool IsEnableOutputControl() const;

protected:
	MfdLogger();
	~MfdLogger() override;

	void InitLanguageVariables() override;

private:
	static COLOR _colors[];
	
	int _textColor;
	int _remoteControl;
	int _level { 0 }; //< Уровень логирования
 	int _enableOutputControl { 0 }; //< Разрешение на управление диагностической информацией
	int _offsetOutputLine { 0 }; 	//< Сдвиг строк диагностической информации при управлении
	int _offsetOutputChar { 0 }; 	//< Сдвиг символов диагностической информации при управлении
	bool _isUseBackgraund { false };//< Использовать задний фон при отобращениии информации

	const char* lang_CONSOLE;
	const char* lang_TRASS;
	const char* lang_LIST;
	const char* lang_DEFOPTIONSET;
	const char* lang_COLOR;
	const char* lang_COLOR_LIST;
	const char* lang_REM_CONTROL;
	const char* lang_LEVEL;
	const char* lang_LEVEL_LIST;
	const char* lang_CLEAR;
	const char* lang_ENABLE_OUTPUT_CONTROL;
};


} /// namespace Logging

#else
// Пустые макросы в случае отсутствия логирования логирования на экран МФИ
#define GET_MFD_LOGGER() nullptr
#define MFD_LOG(...) 
#define MFD_LOG_TAG(...)
#define MFD_LOG_DEBUG(...) 
#define MFD_LOG_INFO(...)
#define MFD_LOG_WARN(...)
#define MFD_LOG_ERROR(...) 
#define MFD_LOG_FATAL(...) 

#endif // defined(USING_LOGGING) && defined(USING_MFD_LOGGING)
#endif // MFI_LOGGER_H
