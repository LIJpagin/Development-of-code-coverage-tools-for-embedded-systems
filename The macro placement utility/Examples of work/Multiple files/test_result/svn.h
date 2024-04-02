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

//! �����, ����������� ������ ������� ��������������� ��������� (���)
class SVN : public CCameraBase, public IUsingTimers
{
private:
	static const UINT ADDRESS_OF_SENDING_TO_SVN		= 0300; //< ����� �������� ������ ���������� �� ���
	static const UINT ADDRESS_OF_DATA_RECEPTION_SVN = 0303; //< ����� ������ ������ �� ���
	static const UINT ADDRESS_OF_BUK_SW_VERSION		= 0304; //< ����� ������ ������ �� ���-1 ��
	static const UINT ADDRESS_OF_TK_2A_SW_VERSION	= 0307; //< ����� ������ ������ �� ��-2�
	static const UINT ADDRESS_OF_TK_3A_SW_VERSION	= 0310; //< ����� ������ ������ �� ��-3�
	static const UINT ADDRESS_OF_TK_4A_SW_VERSION	= 0311; //< ����� ������ ������ �� ��-4�
	//! �������� ������� ��������/������ ����������� ������
	static const ARINC429_RATES SEND_RECEIVE_SPEED = ARINC429_RATES::R100;
	//! ������� ��������/������ ����������� ������
	static const int DATA_UPDATE_TIME = 100;
	//! ����� ������ ����������
	static const int FAILURE_CONTROL_TIME = 1000;
	//! ������������ ���������� ��������� ������� ������ �� ���
	static const int MAX_LOST_RECEIVING = 4;
	//! �����, ������� ����� ������������ ������ �����
	static const int CAMERAS_ICONS_SHOW_TIME = 5000;
	//! ������ ������ �����
	static const int CAMERA_ICON_WIDTH = 80;
	//! ������������ ���������� ���������
	static const int MAX_NUMBER_OF_DEVICE = 4;


	//! ������������� ������ 
	enum class Camera
	{
		None = 0,	//<	
		TK2A,		//<	��-2� � ����� ����� �� ������� ��������
		TK3A,		//<	��-3� � ����� ����������� ������������ ������ ���������
		TK4Adeg0,	//<	��-4� (�) � ����� ������ ����� �������� � ����� �� ������� ��������
		TK4Adeg10,	//<	��-4� (�) � ���������������� �������� �� ��������� �� 10? ���� ������������ ���������������� ��-4� (�)
		TK4Adeg20,	//<	��-4� (�) � ���������������� �������� �� ��������� �� 10? ���� ������������ ���������������� ��-4� (�)
		TK4Adeg30,	//<	��-4� (�) � ���������������� �������� �� ��������� �� 10? ���� ������������ ���������������� ��-4� (�)
		TK4Adeg40,	//< ��-4� (�) � ���������������� �������� �� ��������� �� 10? ���� ������������ ���������������� ��-4� (�)
	};
	//! ���������� � �������
	enum Device
	{
		BUK1AB,
		TK2A,
		TK3A,
		TK4A
	};
	//! ��������� ���������
	enum class State 
	{
		Denial	= 0b0,	//< �����
		Normal  = 0b1	//< �����
	};
	//! ������ �����
	struct Bmp {
		enum {
			Camera = 251,
			Border = 252,
		};
	};
	//! ������� ��������� �����
	enum class MatrixState
	{
		Normal,		//< ���������� ������
		Unreliable,	//< ������ ������������
		NotUsed,	//< �� ������������
		Failure		//< �����
	};
	//! ���� ����������� ���������� � ������� 
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
		const char* lang_DEVICE;			//<	"����������"
		const char* lang_SOFTWARE_VERSION;	//<	"������ ��" 
		const char* lang_STATE;				//<	"���������"
		const char* lang_DEVICE_NAME[MAX_NUMBER_OF_DEVICE]; //<	"���-1��", "��-2�","��-3�","��-4�"
	};
	#pragma pack(push, 1)
	//! �����������-�����, ������������ �� ������ Arinc 429
	union ArincWord
	{
		//! ��������� ��� ������������� ������ � ��������� ����� (����� 303)
		struct ReceptionToSvn
		{

			UINT address				: 8;
			UINT extention				: 2;	//< ���������� ������
			UINT videoSourceCode		: 3;	//< ��� ��������� ����� ��� ����������� �� ���
			UINT isSvnServiceability	: 1;	//< ����������� ���-1��: 1 � �����������, 0 � �����
			UINT isVideoFromTK2A		: 1;	//< ������� ����� �� ��-2�: 1 � �������, 0 � ����������
			UINT isVideoFromTK3A		: 1;	//< ������� ����� �� ��-3�: 1 � �������, 0 � ����������
			UINT isVideoFromTK4A		: 1;	//< ������� ����� �� ��-4�: 1 � �������, 0 � ����������
			UINT reserve1				: 1;	//< ������
			UINT isChange				: 1;	//< ���������� ����� ���������: �������� ��� ���������
			UINT reserve2				: 10;	//< ������
			UINT matrix					: 2;
			UINT parity					: 1;

			//! �������� ����� ������������ ������ �� ����
			Camera GetDisplayedCamera() const;
			//! �������� ��������� ��������� �� ���������� ����� �� ������ ������
			State GetDeviceState(const Device device) const;
		} receiveData;

		//! ��������� ��� ������������� ������ ����� �� �������� ��� ��� (����� 300)
		struct SendingToSvn
		{
			UINT address			: 8;
			UINT extention			: 2;	//< ���������� ������
			UINT videoSourceCode	: 3;	//< ��� ��������� ����� ��� ����������� �� ���
			UINT reserve1			: 5;	//< ������
			UINT isChange			: 1;	//< ��� ���������� ������� ����������: �������� ��� ���������
			UINT reserve2			: 10;	//< ������
			UINT matrix				: 2;
			UINT parity				: 1;

			// ���������� ��� ��������� ������ �� ������
			void SetDisplayedCamera(const Camera camera);
		} sendData;

		//! ��������� ��� ������������� ������ ����� ������ �� (����� 304, 307, 310, 311)
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

			//! �������� ������ �� � ���� �����
			UINT GetSoftwareVersion() const;
		} versionData;
		DWORD word;

		ArincWord() : word(0) { }
		ArincWord(DWORD data) : word(data) { }
	};
	#pragma pack(pop)


	//! ��������� ���������
	static State _deviceStates[MAX_NUMBER_OF_DEVICE];
	//! ������ �� ��� � ��
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
	//! ���������� ��������� ������� ������ ����� ���������� ���������
	static UINT _numberOfFailedReceive[numberOfWords];
	//! ��������� ������� �� ��� ���� 
	static PARAM_STATE _wordState[numberOfWords];
	//! ������������ ����� � ������� �� ���������
	static bool _isReceiveSoftwareVersion;


	//! ��������� �� ����� ������ ������ �� ���
	CArinc429In* _arinc429in;
	//! ��������� �� ����� �������� ������ �� ���
	CArinc429Out* _arinc429out;

	//! ��������� ������� � ���� ������ 
	PilotsMenu::Function _systemState;
	//! ������������ ��������� ������� � ���� ������ 
	bool _isDrawSystemState { false };;
	//! �������� ���� �� ���� ����� ��������� ����� ������ ����� 
	bool _isStateWordReceived{ false };
	//! ���� ��������� ������ � �����������
	SetupMenu::CameraSettings _cameraSettings;
	//! ������������ ��� ������
	Camera _showCamera { Camera::None };
	//! ��������� ������������� ������ ��� ������ �� ������ ���
	Camera _selectedCamera { Camera::TK2A };
	//! ��������� ���� ������� ��� ��-4� (��� ���������� ��������� ����������� ������)
	Camera _selectedDegreesTK4A { Camera::TK4Adeg0 };
	//! ���������� ��� ��� ������ ���
	int _switchToSvnAtMfiStart { false };
	//! ���������� �� ��������� ������� ������������� (����� ��� ���� ���������� ������� ����������)
	bool _hasStateOfSystemChanged { false };
	//! ���������� ������ ������ ����������
	bool _isTimerFailureControlEnable { false };
	//! ������� �� ���� ������
	bool _isOpenPilotMenu { false };


	//! ��������� ������ ����� �� ������ ���
	void DrawCamerasIcons() const;
	//! ��������� ���� ������ ���������� � �����������
	void DrawFailure() const;
	//! ��������� ��������� ������� � ���� ������
	void DrawSystemState() const;
	//! �������� � ��������� ������ �� ���
	void ReceiveAndProcessData();
	//! �������� � ����� � �������� ������ ��� ���
	void PackAndSendData();
	//! �������� ������ ������� ��������� �� ����� ������
	MatrixState GetMatrixState(const UINT matrix) const;
	//! �������� ������ �������
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
	const char* lang_SWITCH_TO_VSS_AT_MFI_START;	//<	"������� � ��� ��� ������ ���"
	const char* lang_NO_YES;						//<	"���������� � �������"
	const char* lang_SYSTEM_STATE;					//< "��������� �������"
	const char* lang_SYSTEM_STATE_UPPER;			//< "��������� �������"
	const char* lang_SYSTEM_INFORMATION;			//<	"���;��;"
	const char* lang_NO_DATA_FROM_SVN;				//<	"��� ������ �� ���"
	const char* lang_SVN_FAILURE;					//<	"����� ���"
	const char* lang_SVN_CAMERA_SETTINGS;			//<	"��������� ����� ���"
	const char* lang_CAMERA_PROFILE;				//<	"������� �����"
	const char* lang_CAMERA_PROFILE_LIST;			//<	"��������������;����������������;"
	const char* lang_FAILURE;						//<	"�����"
	const char* lang_BUK_1AB;						//<	"���-1��"
	const char* lang_FAILURE_CONTROLE;				//<	"���."
	const char* lang_DEVICE;						//<	"����������"
	const char* lang_STATE;							//<	"���������"
	const char* lang_NO_DATA;						//<	"��� ������"
	const char* lang_SERVICEABLE;					//<	"��������"
	const char* lang_EXIT;							//< "�����"
	const char* lang_DEVICE_NAME[MAX_NUMBER_OF_DEVICE]; //<	"���-1��", "��-2�","��-3�","��-4�"

	void InitLanguageVariables() override;
};


#endif // SVN_H
#endif // _SVN
