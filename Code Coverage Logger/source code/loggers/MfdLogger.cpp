// Реализация класса MfiLogger
#include "MfdLogger.h"

#if defined(USING_LOGGING) && defined(USING_LOGGING_MFD)

#include "../cgui.h"
#include "../setupmenu/list.h"
#include "../setupmenu/submenu.h"
#include "../setupmenu/function.h"
#include "../../scrcoord.h"

#include <cstdarg>


namespace Logging {


COLOR MfdLogger::_colors[] =
{
	clWhite,
	clBlack,
	clBlue,
	clGreen,
	clRed,
	clTransparent
};
const FONT MfdLogger::SCREEN_FONT = Fonts::FONT_M_MONOSPACED;

MfdLogger::MfdLogger() : _textColor(0)
{
	MfdLogger::InitLanguageVariables();
	MfdLogger::PrepareSystemSetting();
	MfdLogger::PrepareUserSetting();
	MfdLogger::InitSetupMenu();
	ILogger::Disable();
	Clear();
	_formatter = new DefaultFormatter();
	_formatter->SetFlagsItems(0);
	_filter = new DefaultFilter();
	_filter->SetLevel(Level::Debug);
}

MfdLogger::~MfdLogger()
{
	delete _formatter;
	delete _filter;
}

void MfdLogger::InitSetupMenu()
{
	// инициализация меню
	if (_setupMenu)
		return;
	_setupMenu = new SetupMenu::Submenu(this, lang_CONSOLE,		SetupMenu::aclSystem, SetupMenu::idNull, setNull);
	_setupMenu->Add(new SetupMenu::List(this, lang_TRASS,		SetupMenu::aclSystem, SetupMenu::idNull, setNull, reinterpret_cast<int*>(&_isEnable), lang_LIST));
	_setupMenu->Add(new SetupMenu::List(this, lang_COLOR,		SetupMenu::aclSystem, SetupMenu::idNull, setNull, &_textColor, lang_COLOR_LIST));
	_setupMenu->Add(new SetupMenu::List(this, lang_REM_CONTROL,	SetupMenu::aclSystem, SetupMenu::idNull, setNull, &_remoteControl, lang_LIST));
	_setupMenu->Add(new SetupMenu::List(this, lang_ENABLE_OUTPUT_CONTROL, SetupMenu::aclSystem, SetupMenu::idNull, setNull, &_enableOutputControl, lang_LIST));
	_setupMenu->Add(new SetupMenu::List(this, lang_LEVEL,		SetupMenu::aclSystem, SetupMenu::idNull, setNull, &_level, lang_LEVEL_LIST));
	_setupMenu->Add(new SetupMenu::Function(this, lang_CLEAR,	SetupMenu::aclSystem, 1, setAction));
}

void MfdLogger::SetupMenuClick(int id, SETUP_ACTION action)
{
	switch (action)
	{
		case setAction:
			Clear();
		default:
			break;
	}
}

void MfdLogger::InitLanguageVariables()
{
	lang_CONSOLE		= IUsingLanguage::error_lang;
	lang_TRASS			= IUsingLanguage::error_lang;
	lang_LIST			= IUsingLanguage::error_lang;
	lang_DEFOPTIONSET	= IUsingLanguage::error_lang;
	lang_COLOR			= IUsingLanguage::error_lang;
	lang_COLOR_LIST		= IUsingLanguage::error_lang;
	lang_REM_CONTROL	= IUsingLanguage::error_lang;
	lang_LEVEL			= IUsingLanguage::error_lang;
	lang_LEVEL_LIST		= IUsingLanguage::error_lang;
	lang_CLEAR 			= IUsingLanguage::error_lang;
	lang_ENABLE_OUTPUT_CONTROL = IUsingLanguage::error_lang;
}

void MfdLogger::LoadLanguage(const unsigned int lang)
{
	switch (lang)
	{
		case 0: // Русский
			lang_CONSOLE		= "Диагностическая информация";
			lang_TRASS			= "Отображение";
			lang_LIST			= "Выкл;Вкл;";
			lang_DEFOPTIONSET	= "Стандартные настройки установлены\n";
			lang_COLOR			= "Цвет текста";
			lang_COLOR_LIST		= "Белый;Черный;Синий;Зеленый;Красный;По уровню важности;";
			lang_REM_CONTROL	= "Удаленное управление";
			lang_LEVEL			= "Уровень важности записей";
			lang_LEVEL_LIST		= "Отключено;Отладка;Информация;Предупреждения;Ошибки;";
			lang_CLEAR			= "Очистить вывод";
			lang_ENABLE_OUTPUT_CONTROL = "Управление выводом на экран";
			break;
		case 1:
			lang_CONSOLE		= "Diagnostic information";
			lang_TRASS			= "Display";
			lang_LIST			= "Off;On;";
			lang_DEFOPTIONSET	= "Default options sets\n";
			lang_COLOR			= "Text color";
			lang_COLOR_LIST		= "White;Black;Blue;Green;Red;";
			lang_REM_CONTROL	= "Remote control";
			lang_LEVEL			= "Importance level of the records";
			lang_LEVEL_LIST		= "Off;Debug;Info;Warning;Error;By level of importance;";
			lang_CLEAR			= "Clear output";
			lang_ENABLE_OUTPUT_CONTROL = "Output control on screen";
			break;
		default:;
	}
}

DISPATCH_MESSAGE_RESULT MfdLogger::MFD_DispatchMessage(MESSAGES message, DWORD lParam, DWORD wParam)
{
	DISPATCH_MESSAGE_RESULT result = dmrDispatched;
	switch (message)
	{
		case msKey:
			if (wParam == 1)
			{
				switch (lParam)
				{
					case keyDsUp:
						_isUseBackgraund = !_isUseBackgraund;
						break;
					case keyDsDown:
						_textColor = (_textColor + 1) % ARRAY_LENGTH(_colors);
						break;
					case keyN0:
						Clear();
						break;
					case keyN1:
						IsPaused = !IsPaused;
						_offsetOutputLine = 0;
						_offsetOutputChar = 0;
						if (IsPaused) {
							MFD_LOG_WARN("Пауза вывода информации");
						}
						else {
							MFD_LOG_INFO("Старт вывода информации");
						}
						break;
					case keyN2:
						IsControlOutput = false;
						IsPaused = false;
						_offsetOutputLine = 0;
						_offsetOutputChar = 0;
						MFD_LOG_WARN("Отключено управления информацией");
						break;
					default:
						result = dmrNotDispatched;
				}
			}
			else
				result = dmrNotDispatched;
			break;
		case msEnRightOut:
			if (IsPaused)
				_offsetOutputChar += (int)lParam;
			break;
		case msEnRightIn:
			if (IsPaused)
				_offsetOutputLine += (int)lParam;
			break;
		default:
			result = dmrNotDispatched;
			break;
	}

	return result;
}

MODULE_ID MfdLogger::GetID()
{
	return idUtilsConsole;
}

void MfdLogger::PrepareSystemSetting()
{
	sysParams.Add(&_isEnable, false);
	sysParams.Add(&_textColor, 0);
	sysParams.Add(&_remoteControl, 0);
	sysParams.Add(&_enableOutputControl, 0);
	sysParams.Add(&_level, 0);
}

void MfdLogger::DrawForeground()
{
	if (!_isEnable)
		return;
	if (_isUseBackgraund) {
		gui->SetBrush(clBlack);
		gui->Rectangle(SCR_LEFT, SCR_TOP, SCR_RIGHT, SCR_BOTTOM);
	}
	gui->SetBrush(clNone);

	int entry = ((int)_sink.Size() - NUMBER_OF_LINES) < 0 ? 0 : (int)_sink.Size() - NUMBER_OF_LINES + _offsetOutputLine;
	int line = 0;
	while ((entry < (int)_sink.Size()) && (line != NUMBER_OF_LINES))
	{
		char buffer[LOGGING_SIZE_OF_BUFFERS];
		const auto count = _formatter->Build(buffer, _sink.GetValue(entry));
		if (_colors[_textColor] != clTransparent)
			gui->SetPen(_colors[_textColor]);
		else
			switch (_sink.GetValue(entry).level)
			{
				case Level::Debug:	gui->SetPen(clBlue);	break;
				case Level::Info:	gui->SetPen(clGreen);	break;
				case Level::Warn:	gui->SetPen(clYellow);	break;
				case Level::Error:	gui->SetPen(clRed);		break;
				case Level::Fatal:	gui->SetPen(clRed);		break;
				default:;
			}
		SystemTextOut(buffer,
			PADDING_LEFT + _offsetOutputChar * FONT_WIDTH,
			PADDING_TOP + line * FONT_HEIGHT,
			_colors[_textColor], (WORD)count);
		++line; ++entry;
	}

	if (IsControlOutput == true) {
		if (IsPaused)
			gui->SetPen(clYellow);
		else
			gui->SetPen(clBlinkYellow);
		gui->Border(SCR_LEFT, SCR_TOP, SCR_RIGHT, SCR_BOTTOM);
	}
}

void MfdLogger::TraceRemoteControl(const char* format, ...)
{
	if (!GetInstance()->IsEnable())
		return;

	if (!GetInstance()->_remoteControl)
		return;

	va_list list;
	va_start(list, format);
	auto entry = LogEntry(Level::Off, format, list);
	GetInstance()->Log(entry);
	va_end(list);
}

void MfdLogger::Clear()
{
	GetSink()->Clear();
}

void MfdLogger::Log(LogEntry& entry)
{
	switch (_level) {
		case 0:		_filter->SetLevel(Level::Off);		break;
		case 1:		_filter->SetLevel(Level::Debug);	break;
		case 2:		_filter->SetLevel(Level::Info);		break;
		case 3:		_filter->SetLevel(Level::Warn);		break;
		case 4:		_filter->SetLevel(Level::Error);	break;
		default:	_filter->SetLevel(Level::Off);		break;
	}
	if (_filter->Check(entry) && !IsPaused)
		_sink.AddValueBack(entry);
}

void MfdLogger::Log(const Level level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	auto entry = LogEntry(level, format, args);
	Log(entry);
	va_end(args);
}

WORD MfdLogger::SystemFormat(const CMFDPoint point, const char* format, ...)
{
	char buffer[LOGGING_SIZE_OF_BUFFERS];
	va_list list;
	va_start(list, format);
	const auto count = vsprintf(buffer, format, list);
	va_end(list);
	return gui->SystemTextOut(buffer, point, count);
}

WORD MfdLogger::SystemFormat(const int x, const int y, const char* format, ...)
{
	char buffer[LOGGING_SIZE_OF_BUFFERS];
	va_list list;
	va_start(list, format);
	const auto count = vsprintf(buffer, format, list);
	va_end(list);
	return SystemTextOut(buffer, x, y, (COLOR)0, (WORD)count);
}

WORD MfdLogger::SystemFormat(const CMFDPoint point, COLOR pen, COLOR br, const char* format, ...)
{
	char buffer[LOGGING_SIZE_OF_BUFFERS];
	va_list list;
	va_start(list, format);
	const auto count = vsprintf(buffer, format, list);
	va_end(list);
	return SystemTextOut(buffer, point.x, point.y, pen, (WORD)count);
}

WORD MfdLogger::SystemFormat(const int x, const int y, const COLOR pen, COLOR br, const char* format, ...)
{
	char buffer[LOGGING_SIZE_OF_BUFFERS];
	va_list list;
	va_start(list, format);
	const auto count = vsprintf(buffer, format, list);
	va_end(list);
	return SystemTextOut(buffer, x, y, pen, (WORD)count);
}

WORD MfdLogger::SystemTextOut(const char* str, const int x, const int y, const COLOR pen, const WORD count)
{
	return gui->SystemTextOut(str, x, y, pen, clTransparent, count);
}

bool MfdLogger::IsEnableOutputControl() const
{
	return static_cast<bool>(_enableOutputControl);
}


} // namespace Logging


#endif // defined(USING_LOGGING) && defined(USING_MFD_LOGGING)
