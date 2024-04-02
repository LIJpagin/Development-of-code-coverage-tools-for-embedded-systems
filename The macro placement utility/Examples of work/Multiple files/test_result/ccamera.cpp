/*
 * сamera.cpp
 *
 *  Created on: 15.05.2014
 *      Author: Plaksin_A
 */

#include "ccamera.h"

#include "../../utils/cgui.h"
#include "test_logger.h"

const char* CCamera::Name[] = {"Камера", "Camera"};

CCamera::CCamera(CVideo* videoDriver)
    : CCameraBase(videoDriver),
      _cameraProfilType(this, lang_CAMERA_PROFILE, SetupMenu::aclSystem, SetupMenu::idNull, setNull, &_loadingConfigType, lang_CAMERA_PROFILE_LIST),
      _cameraSettings(this, lang_CAMERA_CONFIG, SetupMenu::aclSystem, (int)idVideoConfig, setEnter | setExit, &_i2cDriver, _videoDriver) {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    InitLanguageVariables();
    PrepareSystemSetting();
    InitSetupMenu();
}

void CCamera::PrepareSystemSetting() {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    sysParams.Add(&_loadingConfigType, lcAuto);

    sysParams.Add(&_manualConfig.startV, palSecamConfig.startV);
    sysParams.Add(&_manualConfig.endV, palSecamConfig.endV);
    sysParams.Add(&_manualConfig.startH, palSecamConfig.startH);
    sysParams.Add(&_manualConfig.endH, palSecamConfig.endH);
    sysParams.Add(&_manualConfig.quant, palSecamConfig.quant);
    sysParams.Add(&_manualConfig.reduce, palSecamConfig.reduce);
}

void CCamera::Open(MODULE_STATE state) {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    CModule::Open(state);
    TurnOn();
}

void CCamera::Close() {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    TurnOff();
    gui->Fill(lsTawsRadar, clTransparent);
    gui->Fill(lsTaws1, clTransparent);
    CModule::Close();
}

void CCamera::TurnOn() {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    LoadConfig();
    gui->SetPLDLayer(CVideo::videoLayer);
}

void CCamera::TurnOff() {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    gui->SetPLDLayer(lsDisabled);
    _videoDriver->TurnOff();
}

void CCamera::InitSetupMenu() {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    _setupMenu = new SetupMenu::Submenu(this, lang_NAME, SetupMenu::aclUser, SetupMenu::idNull, setNull);
    _setupMenu->Add(&_cameraProfilType);
}

void CCamera::SetupMenuClick(int id, SETUP_ACTION action) {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    // CConsole::Trace("CCamera::SetupMenuClick\n");
}

void CCamera::LoadLanguage(unsigned int lang) {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    lang_NAME = Name[lang];
    switch (lang) {
        case 0:
            COV_LOG_CASE(1700);COV_LOG_CASE(1697);
            lang_CAMERA_SETTINGS = "Настройки камеры";
            lang_CAMERA_PROFILE = "Профиль камеры";
            lang_CAMERA_PROFILE_LIST = "автоматический;пользовательский;";
            lang_CAMERA_CONFIG = "Настройка камеры";
            break;
        case 1:
            COV_LOG_CASE(1700);COV_LOG_CASE(1697);
            lang_CAMERA_SETTINGS = "Camera settings";
            lang_CAMERA_PROFILE = "Camera profile";
            lang_CAMERA_PROFILE_LIST = "automatic;custom;";
            lang_CAMERA_CONFIG = "Camera configuration";
            break;
    }
}

void CCamera::InitLanguageVariables() {COV_LOG_FUNC(1700);
    COV_LOG_FUNC(1697);
    lang_CAMERA_SETTINGS = IUsingLanguage::error_lang;
    lang_CAMERA_PROFILE = IUsingLanguage::error_lang;
    lang_CAMERA_PROFILE_LIST = IUsingLanguage::error_lang;
    lang_CAMERA_CONFIG = IUsingLanguage::error_lang;
}
