#ifndef CCAMERABASE_H
#define CCAMERABASE_H

#include "../../types.h"
#include "../cmodule.h"
#include "../../drivers/ci2c.h"
#include "../../drivers/cvideo.h"
#include "../../interfaces/ieventsender.h"
#include <xparameters.h>

//class CVideo;

// Класс реализующий работу с Altera
class CCameraBase : public CModule, public virtual IEventSender
{
public:
	CCameraBase(CVideo* videoDriver=NULL) : _videoDriver(videoDriver),_i2cDriver(XPAR_I2C_MASTER_2_BASEADDR, 0x42),
	_manualConfig(palSecamConfig),	//загружается из архива
	_loadingConfigType(lcAuto)		//загружается из архива
	{};

	virtual bool GetWarning(const char* &buff, WARNING_PRIORITY &prior, COLOR &color) { return false;	}

	virtual void PrepareUserSetting() {}
	virtual void UserDefault() {}

	virtual void SetPosInfo() {} 
	virtual void CheckError()	{}
	virtual void DrawForeground()	{ }
	virtual void PrepareSystemSetting()	{}
	virtual void InitSetupMenu()	{}
	virtual void SetupMenuClick(int id, SETUP_ACTION action) {}

	virtual bool GetInvalid(const char* &buff, COLOR &color){return false;}
	virtual void Init(){};
	DISPATCH_MESSAGE_RESULT MFD_DispatchMessage(MESSAGES message, DWORD lParam, DWORD wParam){return dmrDispatched;}


	virtual void LoadLanguage(unsigned int lang) {}
	virtual void TurnOn(){}
	virtual	void TurnOff() {}
	virtual SetupMenu::Item* GetConfigurator() {return NULL; }
	virtual bool IsPilotMenuAvailable() const;

protected:
	virtual void InitLanguageVariables() {}

	CVideo* _videoDriver;
	CI2C _i2cDriver;

	struct VideoInputConfig
	{
		DWORD startV;
		DWORD endV;
		DWORD startH;
		DWORD endH;
		DWORD quant;
		DWORD reduce;
	};
	static const VideoInputConfig palSecamConfig;//статик для использования в качестве параметров по умолчанию в PrepareSystemSetting
	VideoInputConfig _manualConfig;

	enum LOADING_CONFIG
	{
		lcAuto = 0,
		lcManual = 1
	};
	int _loadingConfigType;
	void WriteIinitSequence();
	CVideo::STANDARD GetStandard();
	void ManualConfigSynchro();
	void LoadConfig();
	void RefreshConfig();
private:
};

#endif /* CCourse_H */
