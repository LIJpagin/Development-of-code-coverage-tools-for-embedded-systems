/*
 * сamera.cpp
 *
 *  Created on: 15.05.2014
 *      Author: Plaksin_A
 */

#include "ccamera.h"

#include "../../utils/cgui.h"


const char* CCamera::Name[] = {"Камера", "Camera" };

CCamera::CCamera(CVideo* videoDriver) :
		CCameraBase(videoDriver),
		_cameraProfilType(this, lang_CAMERA_PROFILE, SetupMenu::aclSystem, SetupMenu::idNull, setNull, &_loadingConfigType, lang_CAMERA_PROFILE_LIST),
		_cameraSettings(this, lang_CAMERA_CONFIG, SetupMenu::aclSystem, (int)idVideoConfig, setEnter | setExit, &_i2cDriver, _videoDriver)
{
	InitLanguageVariables();
	PrepareSystemSetting();
	InitSetupMenu();
}

void CCamera::PrepareSystemSetting()
{

	sysParams.Add(&_loadingConfigType, lcAuto);

	sysParams.Add(&_manualConfig.startV, palSecamConfig.startV);
	sysParams.Add(&_manualConfig.endV, palSecamConfig.endV);
	sysParams.Add(&_manualConfig.startH, palSecamConfig.startH);
	sysParams.Add(&_manualConfig.endH, palSecamConfig.endH);
	sysParams.Add(&_manualConfig.quant, palSecamConfig.quant);
	sysParams.Add(&_manualConfig.reduce, palSecamConfig.reduce);
}

void CCamera::Open(MODULE_STATE state)
{
	CModule::Open(state);
	TurnOn();
}

void CCamera::Close()
{
	TurnOff();
	gui->Fill(lsTawsRadar, clTransparent);
	gui->Fill(lsTaws1, clTransparent);
	CModule::Close();
}

void CCamera::TurnOn()
{
	LoadConfig();
	gui->SetPLDLayer(CVideo::videoLayer);
}

void CCamera::TurnOff()
{
	gui->SetPLDLayer(lsDisabled);
	_videoDriver->TurnOff();
}

void CCamera::InitSetupMenu()
{
	_setupMenu = new SetupMenu::Submenu(this, lang_NAME, SetupMenu::aclUser, SetupMenu::idNull, setNull);
	_setupMenu->Add(&_cameraProfilType);
}

void CCamera::SetupMenuClick(int id, SETUP_ACTION action)
{
	//CConsole::Trace("CCamera::SetupMenuClick\n");
}

void CCamera::LoadLanguage(unsigned int lang)
{
	lang_NAME = Name[lang];
	switch(lang)
	{
	case 0:
		lang_CAMERA_SETTINGS = "Настройки камеры";
		lang_CAMERA_PROFILE = "Профиль камеры";
		lang_CAMERA_PROFILE_LIST = "автоматический;пользовательский;";
		lang_CAMERA_CONFIG = "Настройка камеры";
		break;
	case 1:
		lang_CAMERA_SETTINGS = "Camera settings";
		lang_CAMERA_PROFILE = "Camera profile";
		lang_CAMERA_PROFILE_LIST = "automatic;custom;";
		lang_CAMERA_CONFIG = "Camera configuration";
		break;
	}
}


void CCamera::InitLanguageVariables()
{
	lang_CAMERA_SETTINGS = IUsingLanguage::error_lang;
	lang_CAMERA_PROFILE = IUsingLanguage::error_lang;
	lang_CAMERA_PROFILE_LIST = IUsingLanguage::error_lang;
	lang_CAMERA_CONFIG = IUsingLanguage::error_lang;
}
