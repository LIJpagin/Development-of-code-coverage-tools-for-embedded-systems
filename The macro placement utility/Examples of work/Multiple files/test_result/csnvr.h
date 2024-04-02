/*
 * cvideoreg.h
 *
 *  Created on: 17 мая 2019 г.
 *      Author: Konstantinov_V
 */

#include "../../types.h"

#ifdef _SNVR

#ifndef CSNVR_H_
#define CSNVR_H_

#include "../../drivers/cvideo.h"
#include "../../drivers/carinc429out.h"
#include "../../drivers/carinc429in.h"
#include "../../interfaces/iusingtimers.h"
#include "../../interfaces/iusingirq.h"
#include "../../utils/setupmenu/camerarptrim.h"
#include "../camera/ccamera_base.h"

class CGUI;
class CClock;
class CI2C;
class CRS232;

class CSNVR : public CCameraBase, public IUsingIrq, public IUsingTimers
{
public:
	CSNVR(CVideo* videoDriver,CRS232 *rs232,CClock *pMfd_clock);

	static const char* Name[];
	static const MODULE_ID id = idCameraSNVR;
	virtual MODULE_ID GetID()	{	return id;	}

	virtual void Init();

	virtual void PrepareSystemSetting();

	virtual void PrepareUserSetting();
	virtual void UserDefault();

	virtual void InitSetupMenu();
	virtual void SetupMenuClick(int id, SETUP_ACTION action);

	virtual void LoadLanguage(unsigned int lang);

	virtual void TurnOn();
	virtual	void TurnOff();

	void DrawPlumbLine();

	virtual void DrawForeground();

	virtual DISPATCH_MESSAGE_RESULT MFD_DispatchMessage(MESSAGES message, DWORD lParam = 0, DWORD wParam = 0);

	virtual void SetPosInfo(){}
	virtual bool GetWarning(const char* &buff, WARNING_PRIORITY &prior, COLOR &color);
	virtual bool GetInvalid(const char* &buff, COLOR &color);

	virtual void Open(MODULE_STATE state);
	virtual void Close();

	virtual void OnIrq(DWORD number){}

	int GetPlumbLineX() { return _plln_x; }
	int GetPlumbLineY() { return _plln_y; }
	//void SetPlumbLineX(int x) { _plln_x = x; }
	//void SetPlumbLineY(int y) { _plln_y = y; }
	void SetPlumbLine(int x,int y) { _plln_x = x; _plln_y = y; }
	void ModifyPlumbLine(int dx,int dy) { _plln_x += dx; _plln_y += dy; }
	void ApplyTrim();
	void SetTrim(bool trim) { _trim = trim; }

	bool IsRollValid() { return paramState[RollFail] == mxNormal; }
	bool IsPitchValid() { return paramState[PitchFail] == mxNormal; }

	enum CAM_TRIM_STATE
	{
		ctsValid,
		ctsInvalid,
		ctsNotConn,
		ctsNotChosen,
		ctsNoDataSNVR
	};

	CAM_TRIM_STATE GetCameraTrimState();

protected:
	virtual void CheckError(){}

private:

	virtual void InitLanguageVariables();
	void ShowInterface();
	void LostReceiving();
	bool ReadIncomingInfo();

	CRS232 *_rs232;
	CClock *mfd_clock;

	enum CAM_STATE
	{
		camValid,
		camInvalid,
		camNotConn,
		camReserv
	};

	bool _noGPS;								// Флаг испарвности Навигации
	bool _trim;									// Флаг режима корректировки крена и тангажа
	static const int maxNumOfCameras = 8;		// Максимально возможное кол-во камер в СНВР
	int _numOfCameras;
	int selectedCamera;							// Камера, на которую указывает курсор
	bool activeCameras[maxNumOfCameras];		// Отображаемая камера, принятая от СНВР
	int selActiveCamera;						// Камера, выбранная для отображения. Используется для выдачи на СНВР
	bool recordingCameras[maxNumOfCameras];		// Пишущие камеры. Принимается от СНВР
	int selRecordingCamera[maxNumOfCameras];	// Пишущие камеры. Выдается на СНВР
	CAM_STATE _camStates[maxNumOfCameras];		// Исправности камер
	bool _identValid;							// Флаг корректности принятого идентификатора СНВР
	bool showCameras;							// Флаг отображения пиктограмм камер
	bool invCamAppered;
	bool _camModesEqual;						// Флаг совпадения заданного и принятого режима работы камер

	static const int videoRegTime = 100;
	static const int showCamerasTime = 10000;
	static const int blockRecordTime = 3000;
	static const int coordDisagrTime = 3000;
	int _changeCamStateScore;
	static const int changeCamStateScoreLimit = blockRecordTime / videoRegTime;
	bool _controlBlocked;						// Флаг блокировки управления на время переключения режимов работы камер

	int blockCamRecordOff;						// Номер камеры, отключение записи с которой заблокировано, для выдачи сообщения
	bool _changeCamState;

	// Параметры, принятые от СНВР
	DWORD _time;								// Время от СНВР
	float _lat;									// Широта от СНВР
	float _long;								// Долгота от СНВР
	int _recorderState;							// Состояние накопителя

	int _lostReceiving;							// Кол-во неудачных попыток приема пакета от СНВР после последнего принятого
	static const int maxLostReceiving = 15;		// Макс. кол-во неудачных попыток приема от СНВР до
	bool _prevReceiving;

	int _unixt;									// Время от Навигации, или собственное время МФИ если нет Навигации
	float _latNav;								// Широта от Навигации
	float _longNav;								// Долгота от Навигации
	bool _isCoordDisagrTimerOn;					// Флаг установки таймера рассогласования координат и времени СНВР и Навигации

	int _plln_ptr;								// Указатель отвесной линии
	int _plln_x;
	int _plln_y;
	float _d_pitch;
	float _d_roll;

	int _cameraIdentificator;					// Идентификатор СНВР
	int _recCamTurnOffs[maxNumOfCameras];		// Флаги запрета отключения записи с камер

	const DWORD _bytesCountToRecieve;			// Длина пакера от СНВР в байтах
	BYTE camerasMode;							// Заданные режимы работы камер (для сравнения с принятыми)

	void DrawCameras();
	void DrawAllCams();
	void DrawRecAtten();
	void DrawGPSAtten();
	void DrawCamAtten();

	void UpdateFails();

	UINT ToUnixTime(UINT day,UINT month,UINT year,UINT hour=0,UINT min=0,UINT sec=0);

	union Converter
	{
#pragma pack(push, 1)
		struct
		{
			UINT marker : 16;
			UINT identificator : 8;
			UINT dateTime : 32;
			float latitude;
			float longtitude;
			UINT reserv : 8;
			UINT stateStorage : 8;
			UINT camerasMode : 8;
			UINT stateCamera : 8;
			UINT reserv2 : 32;
			UINT end : 8;

		};
#pragma pack(pop)
		BYTE data[26];
	};
	enum SETUP_ID
	{
		idNull = SetupMenu::idNull,
		idVideoConfig
	};
	enum
	{
		bmpCamera = 245,
		bmpBorder = 246,
		bmpCanvas = 247,
		bmpRecorder = 248,
		bmpGPS = 249,
		bmpCanvasYellow = 250,
	};

	const static int CAM_WIDTH = 80;
	const static int CAM_HEIGHT = 40;

	const static int CAM_RECT_LEFTTOP_X = SCR_RIGHT - CAM_WIDTH - CAM_WIDTH/2;
	const static int CAM_RECT_RIGHTTOP_X = SCR_RIGHT - CAM_WIDTH/2;
	const static int CAM_HIGHEST_TOP_Y = SCR_TOP + 2*CAM_HEIGHT;
	const static int CAM_VERT_SPACE = CAM_HEIGHT + 18;

	const char* lang_NAME;
	const char* lang_NODATASNVR;

	const char* lang_INVAL_CORR_ROLL_PITCH;
	const char* lang_NO_ROLL;
	const char* lang_NO_PITCH;
	const char* lang_NO_CAMERA;
	const char* lang_APPLY;
	const char* lang_EXIT;
	const char* lang_NO_SNVR_CONTROL;

	const char* lang_ID;
	const char* lang_NUM_OF_CAMERAS;
	const char* lang_PLUMB_LINE_PTR;
	const char* lang_PLUMB_LINE_CAM;
	const char* lang_ROLL_PITCH_CORR;
	const char* lang_VIDEO_REC_OFF;
	const char* lang_VIDEO_REC_OFF_LIST;
	const char* lang_VIDEO_REC_OFF_1;
	const char* lang_VIDEO_REC_OFF_2;
	const char* lang_VIDEO_REC_OFF_3;
	const char* lang_VIDEO_REC_OFF_4;
	const char* lang_VIDEO_REC_OFF1;
	const char* lang_VIDEO_REC_OFF2;
	const char* lang_VIDEO_REC_OFF3;
};

#endif /* CSNVR_H_ */

#endif
