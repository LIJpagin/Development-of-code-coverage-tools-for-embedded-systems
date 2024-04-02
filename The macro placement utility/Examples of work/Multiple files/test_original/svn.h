/*
*	svn.h
*	Created on: 18.07.2023
*      Author: Shpagin_D
*/

#include "../../types.h"

#ifdef _SVN
#ifndef SVN_H
#define SVN_H

#include "../../drivers/cvideo.h"
#include "../../drivers/carinc429in.h"
#include "../../drivers/carinc429out.h"
#include "../../interfaces/iusingtimers.h"
#include "../../utils/cmfdpoint.h"
#include "../camera/ccamera_base.h"
#include "../../utils/setupmenu/camerasettings.h"
#include "../../utils/pilotsmenu/function.h"

//! Класс, реализуюший работу системы видеонаблюдения вертолета (СВВ)
class SVN : public CCameraBase, public IUsingTimers
{
private:
	static const UINT ADDRESS_OF_SENDING_TO_SVN		= 0300; //< Адрес отправки данных управления на СВВ
	static const UINT ADDRESS_OF_DATA_RECEPTION_SVN = 0303; //< Адрес приема данных от СВВ
	static const UINT ADDRESS_OF_BUK_SW_VERSION		= 0304; //< Адрес приема версии ПО БУК-1 АБ
	static const UINT ADDRESS_OF_TK_2A_SW_VERSION	= 0307; //< Адрес приема версии ПО ТК-2А
	static const UINT ADDRESS_OF_TK_3A_SW_VERSION	= 0310; //< Адрес приема версии ПО ТК-3А
	static const UINT ADDRESS_OF_TK_4A_SW_VERSION	= 0311; //< Адрес приема версии ПО ТК-4А
	//! Скорость каналов отправки/приема управляющих данных
	static const ARINC429_RATES SEND_RECEIVE_SPEED = ARINC429_RATES::R100;
	//! Частота отправки/приема управляющих данных
	static const int DATA_UPDATE_TIME = 100;
	//! Время отказа управления
	static const int FAILURE_CONTROL_TIME = 1000;
	//! Максимальное количество неудачных попыток приема от СВВ
	static const int MAX_LOST_RECEIVING = 4;
	//! Время, которое будут показываться иконки камер
	static const int CAMERAS_ICONS_SHOW_TIME = 5000;
	//! Ширина иконок камер
	static const int CAMERA_ICON_WIDTH = 80;
	//! Максимальное количество устройств
	static const int MAX_NUMBER_OF_DEVICE = 4;


	//! Телевизионные камеры 
	enum class Camera
	{
		None = 0,	//<	
		TK2A,		//<	ТК-2А – обзор груза на внешней подвеске
		TK3A,		//<	ТК-3А – обзор закабинного пространства задней полусферы
		TK4Adeg0,	//<	ТК-4А (а) – обзор задней части фюзеляжа и груза на внешней подвеске
		TK4Adeg10,	//<	ТК-4А (б) – видеоизображение сдвинуто по вертикали на 10? вниз относительно видеоизображения ТК-4А (а)
		TK4Adeg20,	//<	ТК-4А (в) – видеоизображение сдвинуто по вертикали на 10? вниз относительно видеоизображения ТК-4А (в)
		TK4Adeg30,	//<	ТК-4А (г) – видеоизображение сдвинуто по вертикали на 10? вниз относительно видеоизображения ТК-4А (в)
		TK4Adeg40,	//< ТК-4А (д) – видеоизображение сдвинуто по вертикали на 10? вниз относительно видеоизображения ТК-4А (г)
	};
	//! Устройства в системе
	enum Device
	{
		BUK1AB,
		TK2A,
		TK3A,
		TK4A
	};
	//! Состояния устройств
	enum class State 
	{
		Denial	= 0b0,	//< Отказ
		Normal  = 0b1	//< Норма
	};
	//! Иконки камер
	struct Bmp {
		enum {
			Camera = 251,
			Border = 252,
		};
	};
	//! Матрица состояния слова
	enum class MatrixState
	{
		Normal,		//< Нормальная работа
		Unreliable,	//< Данные недостоверны
		NotUsed,	//< Не используется
		Failure		//< Отказ
	};
	//! Меню отображения информации о системе 
	class InformationAboutSystem : public SetupMenu::Item
	{
	public:
		InformationAboutSystem(IUsingSetupMenu* context, const char*& name,
			SetupMenu::ACCESS_LEVEL AccessLevel, int id, UINT funcType);
		void LoadLanguage(unsigned int lang) override;
		void Enter() override;
		void Exit() override;
		void DrawForeground() override;
		DISPATCH_MESSAGE_RESULT MFD_DispatchMessage(MESSAGES message, DWORD lParam, DWORD wParam) override;
		void  InitLanguageVariables() override;

	private:
		const char* lang_DEVICE;			//<	"Устройство"
		const char* lang_SOFTWARE_VERSION;	//<	"Версия ПО" 
		const char* lang_STATE;				//<	"Состояние"
		const char* lang_DEVICE_NAME[MAX_NUMBER_OF_DEVICE]; //<	"БУК-1АБ", "ТК-2А","ТК-3А","ТК-4А"
	};
	#pragma pack(push, 1)
	//! Объединение-слово, передаваемое по каналу Arinc 429
	union ArincWord
	{
		//! Структура для представления данных в пришедшем слове (Слово 303)
		struct ReceptionToSvn
		{

			UINT address				: 8;
			UINT extention				: 2;	//< Расширение адреса
			UINT videoSourceCode		: 3;	//< Код источника видео для отображения на МФИ
			UINT isSvnServiceability	: 1;	//< Исправность БУК-1АБ: 1 – исправность, 0 – отказ
			UINT isVideoFromTK2A		: 1;	//< Наличие видео от ТК-2А: 1 – наличие, 0 – отсутствие
			UINT isVideoFromTK3A		: 1;	//< Наличие видео от ТК-3А: 1 – наличие, 0 – отсутствие
			UINT isVideoFromTK4A		: 1;	//< Наличие видео от ТК-4А: 1 – наличие, 0 – отсутствие
			UINT reserve1				: 1;	//< Резерв
			UINT isChange				: 1;	//< Обновление слова состояния: Инверсия при изменении
			UINT reserve2				: 10;	//< Резерв
			UINT matrix					: 2;
			UINT parity					: 1;

			//! Получить номер отображаемой камеру по коду
			Camera GetDisplayedCamera() const;
			//! Получить состояние устройств из пришедшего слова по номеру камеры
			State GetDeviceState(const Device device) const;
		} receiveData;

		//! Структура для представления данных слова на отправку для СВВ (Слово 300)
		struct SendingToSvn
		{
			UINT address			: 8;
			UINT extention			: 2;	//< Расширение адреса
			UINT videoSourceCode	: 3;	//< Код источника видео для отображения на МФИ
			UINT reserve1			: 5;	//< Резерв
			UINT isChange			: 1;	//< Бит обновления команды управления: Инверсия при изменении
			UINT reserve2			: 10;	//< Резерв
			UINT matrix				: 2;
			UINT parity				: 1;

			// Установить код выбранной камеру по номеру
			void SetDisplayedCamera(const Camera camera);
		} sendData;

		//! Структура для представления данных слова Версии ПО (Слова 304, 307, 310, 311)
		struct SoftwareVersion
		{
			UINT address	: 8;
			UINT symbol1	: 4;
			UINT symbol2	: 4;
			UINT symbol3	: 4;
			UINT symbol4	: 4;
			UINT reserve	: 5;
			UINT matrix		: 2;
			UINT parity		: 1;

			//! Получить версию ПО в виде числа
			UINT GetSoftwareVersion() const;
		} versionData;
		DWORD word;

		ArincWord() : word(0) { }
		ArincWord(DWORD data) : word(data) { }
	};
	#pragma pack(pop)


	//! Состояния устройств
	static State _deviceStates[MAX_NUMBER_OF_DEVICE];
	//! Версии ПО БУК и ТК
	static int _softwareVersionDevice[MAX_NUMBER_OF_DEVICE];
	enum
	{
		WordDataReceptionSvn,
		WordBukSwVersion,
		WordTk2aSwVersion,
		WordTk3aSwVersion,
		WordTk4aSwVersion,
		numberOfWords
	};
	//! Количество неудачных попыток приема после последнего принятого
	static UINT _numberOfFailedReceive[numberOfWords];
	//! Состояние приятых от СВВ слов 
	static PARAM_STATE _wordState[numberOfWords];
	//! Обрабатывать слова с версией ПО устройств
	static bool _isReceiveSoftwareVersion;


	//! Указатель на канал приема данных от СВВ
	CArinc429In* _arinc429in;
	//! Указатель на канал отправки данных на СВВ
	CArinc429Out* _arinc429out;

	//! Состояние системы в меню пилота 
	PilotsMenu::Function _systemState;
	//! Отрисовывать состояние системы в меню пилота 
	bool _isDrawSystemState { false };;
	//! Получено хотя бы одно слово состояние после старта блока 
	bool _isStateWordReceived{ false };
	//! Меню настройки камеры в диагностике
	SetupMenu::CameraSettings _cameraSettings;
	//! Отображаемая СВВ камера
	Camera _showCamera { Camera::None };
	//! Выбранная пользователем камера для показа на экране МФИ
	Camera _selectedCamera { Camera::TK2A };
	//! Выбранный угол наклона для ТК-4А (для сохранения состояния предыдущего выбора)
	Camera _selectedDegreesTK4A { Camera::TK4Adeg0 };
	//! Отображать СВВ при старте МФИ
	int _switchToSvnAtMfiStart { false };
	//! Изменялось ли состояние системы пользователем (нужно для бита обновления команды управления)
	bool _hasStateOfSystemChanged { false };
	//! Установлен таймер отказа управления
	bool _isTimerFailureControlEnable { false };
	//! Открыто ли меню пилота
	bool _isOpenPilotMenu { false };


	//! Отрисовка иконок камер на экране МФИ
	void DrawCamerasIcons() const;
	//! Отрисовка поля вывода информации о неисправных
	void DrawFailure() const;
	//! Отрисовка состояния системы в меню пилота
	void DrawSystemState() const;
	//! Принятие и обработка данных от СВВ
	void ReceiveAndProcessData();
	//! Упаковка в слово и отправка данных для СВВ
	void PackAndSendData();
	//! Получить статус матрицы состояния из сырых данных
	MatrixState GetMatrixState(const UINT matrix) const;
	//! Обновить ошибки системы
	void UpdateSystemFail();


public:
	static const MODULE_ID id = idCameraSVN;
	static const char* Name[];

	SVN(CVideo* videoDriver, CArinc429In* arinc429in, CArinc429Out* arinc429out);
	void Init() override;
	void InitSetupMenu() override;
	void LoadLanguage(unsigned int lang) override;

	void UserDefault() override;
	void PrepareUserSetting() override;
	void PrepareSystemSetting() override;

	void TurnOn() override;
	void TurnOff() override;
	void Open(MODULE_STATE state) override;
	void Close() override;
	
	void SetupMenuClick(int id, SETUP_ACTION action) override;
	DISPATCH_MESSAGE_RESULT MFD_DispatchMessage(MESSAGES message, DWORD lParam, DWORD wParam) override;

	MODULE_ID GetID() override;
	bool GetWarning(const char*& buff, WARNING_PRIORITY& prior, COLOR& color) override;
	bool GetInvalid(const char*& buff, COLOR& color) override;
	int SwitchToSvnAtMfiStart() const;
	
	void DrawForeground() override;
	SetupMenu::Item* GetConfigurator() override;

	void AddToMenu(PilotsMenu::Menu& menu) override;
	void OnPilotsMenu(FuncId id) override;
	bool IsPilotMenuAvailable() const override;

private:
	const char* lang_NAME;
	const char* lang_SWITCH_TO_VSS_AT_MFI_START;	//<	"Переход в СВН при старте МФИ"
	const char* lang_NO_YES;						//<	"Информация о системе"
	const char* lang_SYSTEM_STATE;					//< "Состояние системы"
	const char* lang_SYSTEM_STATE_UPPER;			//< "СОСТОЯНИЕ СИСТЕМЫ"
	const char* lang_SYSTEM_INFORMATION;			//<	"Нет;Да;"
	const char* lang_NO_DATA_FROM_SVN;				//<	"НЕТ ДАННЫХ ОТ СВН"
	const char* lang_SVN_FAILURE;					//<	"ОТКАЗ СВН"
	const char* lang_SVN_CAMERA_SETTINGS;			//<	"Настройки камер СВН"
	const char* lang_CAMERA_PROFILE;				//<	"Профиль камер"
	const char* lang_CAMERA_PROFILE_LIST;			//<	"Автоматический;Пользовательский;"
	const char* lang_FAILURE;						//<	"Отказ"
	const char* lang_BUK_1AB;						//<	"БУК-1АБ"
	const char* lang_FAILURE_CONTROLE;				//<	"УПР."
	const char* lang_DEVICE;						//<	"Устройство"
	const char* lang_STATE;							//<	"Состояние"
	const char* lang_NO_DATA;						//<	"Нет данных"
	const char* lang_SERVICEABLE;					//<	"Исправен"
	const char* lang_EXIT;							//< "ВЫХОД"
	const char* lang_DEVICE_NAME[MAX_NUMBER_OF_DEVICE]; //<	"БУК-1АБ", "ТК-2А","ТК-3А","ТК-4А"

	void InitLanguageVariables() override;
};


#endif // SVN_H
#endif // _SVN
