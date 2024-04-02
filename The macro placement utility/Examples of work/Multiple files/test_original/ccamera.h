/*
 * ñamera.h
 *
 *  Created on: 15.05.2014
 *      Author: Plaksin_A
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "../../types.h"
#include "../../interfaces/iusingtimers.h"
#include "../../interfaces/iusingirq.h"
#include "../../drivers/ci2c.h"
#include "../../drivers/cvideo.h"
#include "../../utils/setupmenu/camerasettings.h"
#include "../../utils/setupmenu/list.h"
#include "../camera/ccamera_base.h"

class CGUI;

class CCamera : public CCameraBase
{
public:
	CCamera(CVideo* videoDriver);

	static const char* Name[];
	static const MODULE_ID id = idCamera;
	virtual MODULE_ID GetID()	{	return id;	}
	virtual void SetPosInfo() {}
	virtual void CheckError()	{}
	virtual void DrawForeground()	{}

	virtual void PrepareSystemSetting();

	virtual void PrepareUserSetting() {}
	virtual void UserDefault() {};
	virtual bool GetWarning(const char* &buff, WARNING_PRIORITY &prior, COLOR &color){return false;}
	virtual void InitSetupMenu();
	virtual void SetupMenuClick(int id, SETUP_ACTION action);

	virtual void LoadLanguage(unsigned int lang);

	virtual void Open(MODULE_STATE state);
	virtual void Close();

	virtual void TurnOn();
	virtual	void TurnOff();

	SetupMenu::Item* GetConfigurator() { return &_cameraSettings; }

private:
	virtual void InitLanguageVariables();

	SetupMenu::List _cameraProfilType;
	SetupMenu::CameraSettings _cameraSettings;

	enum SETUP_ID
	{
		idNull = SetupMenu::idNull,
		idVideoConfig
	};

	int _value;

	const char* lang_NAME;
	const char* lang_CAMERA_SETTINGS;
	const char* lang_CAMERA_PROFILE;
	const char* lang_CAMERA_PROFILE_LIST;
	const char* lang_CAMERA_CONFIG;
};
#endif /* CAMERA_H_ */
