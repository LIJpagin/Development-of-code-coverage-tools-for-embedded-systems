/*
 * cvideoreg.cpp
 *
 *  Created on: 17 мая 2019 г.
 *      Author: Konstantinov_V
 */

#include "csnvr.h"
#include "test_logger.h"

#ifdef _SNVR

#include "../../utils/cgui.h"
#include "../../utils/loggers/LogMaster.h"
#include "../../utils/setupmenu/valuedigital.h"
#include "../../utils/setupmenu/list.h"
#include "../../utils/setupmenu/camerarptrim.h"
#include "../../drivers/crs232.h"
#include "../../drivers/ci2c.h"
#include "../../drivers/cclock.h"

const char *CSNVR::Name[] = {"СНВР", "SVRS"};

CSNVR::CSNVR(CVideo *videoDriver, CRS232 *rs232, CClock *pMfd_clock)
    : CCameraBase(videoDriver),
      _rs232(rs232),
      mfd_clock(pMfd_clock),
      _trim(false),
      _bytesCountToRecieve(26) {
    _plln_x = SCR_WIDTH / 2;
    _plln_y = SCR_HEIGHT / 2;

    InitLanguageVariables();
    PrepareSystemSetting();
    PrepareUserSetting();
    InitSetupMenu();
}

void CSNVR::Init() {
    _rs232->Init();

    _rs232->SetSpeed(CRS232::br115200);
    _rs232->SetStopBit(0);
    _rs232->SetParity(CRS232::ptNone);

    _rs232->SetRecieveMode(0xa5, 0xffff, 0xffff, 0xffff, _bytesCountToRecieve);
    CRS232::REG_CONTROL control(0);
    _rs232->SetControl(control);
    _rs232->SetTimeOut(10);

    selectedCamera = 0;
    _identValid = false;

    for (int n1 = 0; n1 < maxNumOfCameras; n1++) {
        _camStates[n1] = camInvalid;
    }
    _lostReceiving = 0;
    _prevReceiving = false;
    SetTimer(videoRegTime, msSvnTimer);

    _changeCamState = false;
    _controlBlocked = false;
    _camModesEqual = false;
    _changeCamStateScore = 0;
}

void CSNVR::PrepareSystemSetting() {
    sysParams.Add(&_loadingConfigType, lcAuto);

    sysParams.Add(&_manualConfig.startV, palSecamConfig.startV);
    sysParams.Add(&_manualConfig.endV, palSecamConfig.endV);
    sysParams.Add(&_manualConfig.startH, palSecamConfig.startH);
    sysParams.Add(&_manualConfig.endH, palSecamConfig.endH);
    sysParams.Add(&_manualConfig.quant, palSecamConfig.quant);
    sysParams.Add(&_manualConfig.reduce, palSecamConfig.reduce);

    sysParams.Add(reinterpret_cast<int *>(&_d_pitch), 0);
    sysParams.Add(reinterpret_cast<int *>(&_d_roll), 0);
    sysParams.Add(&_cameraIdentificator, 74);
    sysParams.Add(&_numOfCameras, 4);
    sysParams.Add(&selActiveCamera, 1);
    sysParams.Add(&_plln_ptr, 1);
    sysParams.Add(&_recCamTurnOffs[0], 0);
    sysParams.Add(&_recCamTurnOffs[1], 0);
    sysParams.Add(&_recCamTurnOffs[2], 0);
    sysParams.Add(&_recCamTurnOffs[3], 0);
}

void CSNVR::PrepareUserSetting() {
    AddUserSaveParam(&selActiveCamera, sizeof(selActiveCamera));
    AddUserSaveParam(recordingCameras, sizeof(recordingCameras));
    AddUserSaveParam(selRecordingCamera, sizeof(int) * maxNumOfCameras);
}

void CSNVR::UserDefault() {
    CModule::UserDefault();
    selActiveCamera = 0;
    if (_numOfCameras < 0) {
        _numOfCameras = 0;
    }
    if (_numOfCameras > 4) {
        _numOfCameras = 4;
    }
    for (int n1 = 0; n1 < _numOfCameras; n1++) {
        selRecordingCamera[n1] = true;
    }
}

void CSNVR::TurnOn() {
    LoadConfig();
    gui->SetPLDLayer(CVideo::videoLayer);
}

void CSNVR::TurnOff() {
    gui->SetPLDLayer(lsDisabled);
    _videoDriver->TurnOff();
}

bool CSNVR::GetWarning(const char *&buff, WARNING_PRIORITY &prior, COLOR &color) {
    if (_state == stPrimary) {
        if (paramState[snvrNoData] == mxFail) {
            buff = lang_NODATASNVR;
            color = clYellow;
            return true;
        }
        if (paramState[snvrControlFail] == mxFail) {
            buff = lang_NO_SNVR_CONTROL;
            color = clYellow;
            return true;
        }
    }
    return false;
}

bool CSNVR::GetInvalid(const char *&buff, COLOR &color) {
    if (GetState() == stDisabled && _fail) {
        buff = lang_NAME;
        color = clYellow;
        return true;
    }
    return false;
}

void CSNVR::Open(MODULE_STATE state) {
    CModule::Open(state);
    TurnOn();

    ShowInterface();
    invCamAppered = false;
}

void CSNVR::Close() {
    SendEvent(this, IEventSender::evtCameraConfigOff);
    TurnOff();
    gui->Fill(lsTawsRadar, clTransparent);
    gui->Fill(lsTaws1, clTransparent);
    CModule::Close();
}

CSNVR::CAM_TRIM_STATE CSNVR::GetCameraTrimState() {
    if (_plln_ptr == 0) {
        return ctsNotChosen;
    }
    if (paramState[snvrNoData] == mxFail) {
        return ctsNoDataSNVR;
    }
    if (_camStates[_plln_ptr - 1] != camValid) {
        return ctsInvalid;
    }
    return ctsValid;
}

void CSNVR::DrawGPSAtten() {
    const int center_X = SCR_RIGHT - CAM_WIDTH;
    const int center_Y = SCR_HEIGHT / 2 + 180;

    gui->Bmp(center_X - 75, center_Y - 10, bmpGPS);
    gui->WaitForBmp();
}

void CSNVR::DrawCamAtten() {
    const int center_X = SCR_RIGHT - 2 * CAM_WIDTH;
    const int center_Y = SCR_HEIGHT / 2 + 180;

    gui->SetPen(clYellow);
    gui->SetBrush(clNone);
    gui->Bmp(center_X - 45, center_Y + 6, bmpCamera, repaint);
    gui->WaitForBmp();
}

void CSNVR::DrawPlumbLine() {
    const int r = SCR_WIDTH / 40;
    const int len = SCR_WIDTH / 5;
    const COLOR col = clCyan;

    if (paramState[PitchFail] == mxFail || paramState[RollFail] == mxFail ||
        paramState[PitchFail] == mxNoData || paramState[RollFail] == mxNoData) {
        return;
    }

    if (GetAircraftInfo().pitch == NAV_NODATA || GetAircraftInfo().roll == NAV_NODATA) {
        return;
    }

    if (!_trim) {
        _plln_x = sin((GetAircraftInfo().roll + _d_roll) * toRad) * (SCR_WIDTH / 2) + (SCR_WIDTH / 2);
        _plln_y = sin((GetAircraftInfo().pitch + _d_pitch) * toRad) * (SCR_HEIGHT / 2) + (SCR_HEIGHT / 2);
    }

    // окружность - окантовка
    gui->SetPen(clBlack);
    gui->Circle(_plln_x, _plln_y, r - 1, false);
    gui->Circle(_plln_x, _plln_y, r + 1, false);

    // горизонтальная линия - окантовка
    gui->SetPen(clBlack);
    gui->Line(_plln_x - len / 2, _plln_y - 1, _plln_x + len / 2, _plln_y - 1);
    gui->Line(_plln_x - len / 2 - 1, _plln_y - 1, _plln_x - len / 2 - 1, _plln_y + 1);
    gui->Line(_plln_x - len / 2, _plln_y + 1, _plln_x + len / 2, _plln_y + 1);
    gui->Line(_plln_x + len / 2 + 1, _plln_y - 1, _plln_x + len / 2 + 1, _plln_y + 1);

    // вертикальная линия - окантовка
    gui->SetPen(clBlack);
    gui->Line(_plln_x - 1, _plln_y - len / 2, _plln_x - 1, _plln_y + len / 2);
    gui->Line(_plln_x - 1, _plln_y - len / 2 - 1, _plln_x + 1, _plln_y - len / 2 - 1);
    gui->Line(_plln_x + 1, _plln_y - len / 2, _plln_x + 1, _plln_y + len / 2);
    gui->Line(_plln_x - 1, _plln_y + len / 2 + 1, _plln_x + 1, _plln_y + len / 2 + 1);

    gui->SetPen(col);
    gui->Circle(_plln_x, _plln_y, r, false);                            // окружность
    gui->Line(_plln_x - len / 2, _plln_y, _plln_x + len / 2, _plln_y);  // горизонтальная линия
    gui->Line(_plln_x, _plln_y - len / 2, _plln_x, _plln_y + len / 2);  // вертикальная линия
}

void CSNVR::InitSetupMenu() {
    SetupMenu::ValueDigital *pID = new SetupMenu::ValueDigital(this, lang_ID, SetupMenu::aclUser, SetupMenu::idNull, setNull, &_cameraIdentificator, 0, 255, 1);
    SetupMenu::ValueDigital *numOfCameras = new SetupMenu::ValueDigital(this, lang_NUM_OF_CAMERAS, SetupMenu::aclUser, SetupMenu::idNull, setNull, &_numOfCameras, 1, 4, 1);
    SetupMenu::List *plLnCam = new SetupMenu::List(this, lang_PLUMB_LINE_PTR, SetupMenu::aclUser, SetupMenu::idNull, setNull, &_plln_ptr, lang_PLUMB_LINE_CAM);

    SetupMenu::CameraRPTrim *trim = new SetupMenu::CameraRPTrim(this, lang_ROLL_PITCH_CORR, SetupMenu::aclUser, SetupMenu::idNull, setNull, this);

    _setupMenu = new SetupMenu::Submenu(this, lang_NAME, SetupMenu::aclUser, SetupMenu::idNull, setNull);

    _setupMenu->Add(pID);
    _setupMenu->Add(numOfCameras);
    _setupMenu->Add(plLnCam);
    _setupMenu->Add(trim);

    SetupMenu::Submenu *recCamTurnOff = new SetupMenu::Submenu(this, lang_VIDEO_REC_OFF, SetupMenu::aclSystem, SetupMenu::idNull, setNull);

    SetupMenu::List *recCamTurnOffs1 = new SetupMenu::List(this, lang_VIDEO_REC_OFF_1, SetupMenu::aclSystem, SetupMenu::idNull,
                                                           setNull, &_recCamTurnOffs[0], lang_VIDEO_REC_OFF_LIST);
    SetupMenu::List *recCamTurnOffs2 = new SetupMenu::List(this, lang_VIDEO_REC_OFF_2, SetupMenu::aclSystem, SetupMenu::idNull,
                                                           setNull, &_recCamTurnOffs[1], lang_VIDEO_REC_OFF_LIST);
    SetupMenu::List *recCamTurnOffs3 = new SetupMenu::List(this, lang_VIDEO_REC_OFF_3, SetupMenu::aclSystem, SetupMenu::idNull,
                                                           setNull, &_recCamTurnOffs[2], lang_VIDEO_REC_OFF_LIST);
    SetupMenu::List *recCamTurnOffs4 = new SetupMenu::List(this, lang_VIDEO_REC_OFF_4, SetupMenu::aclSystem, SetupMenu::idNull,
                                                           setNull, &_recCamTurnOffs[3], lang_VIDEO_REC_OFF_LIST);

    recCamTurnOff->Add(recCamTurnOffs1);
    recCamTurnOff->Add(recCamTurnOffs2);
    recCamTurnOff->Add(recCamTurnOffs3);
    recCamTurnOff->Add(recCamTurnOffs4);
    _setupMenu->Add(recCamTurnOff);
}

void CSNVR::SetupMenuClick(int id, SETUP_ACTION action) {
    switch (id) {
        case idVideoConfig:
            switch (action) {
                case setEnter:
                    SendEvent(this, evtCameraConfigOn);
                    // подключить слой
                    break;
                case setExit:
                    RefreshConfig();
                    SendEvent(this, evtCameraConfigOff);
                    break;
                default:
                    break;
            }
            break;
    }
}

void CSNVR::ShowInterface() {
    showCameras = true;
    invCamAppered = false;
    KillTimer(msHideInterface);
    SetTimer(showCamerasTime, msHideInterface);
}

void CSNVR::LoadLanguage(unsigned int lang) {
    lang_NAME = Name[lang];
    switch (lang) {
        case 0:
            lang_NODATASNVR = "НЕТ ДАННЫХ ОТ СНВР";

            lang_INVAL_CORR_ROLL_PITCH = "Корректировка невозможна:";
            lang_NO_ROLL = "Отсутствует крен";
            lang_NO_PITCH = "Отсутствует тангаж";
            lang_NO_CAMERA = "Отсутствует камера";
            lang_APPLY = "Применить";
            lang_EXIT = "Выход";
            lang_NO_SNVR_CONTROL = "НЕТ УПРАВЛЕНИЯ СНВР";

            lang_ID = "Идентификатор";
            lang_NUM_OF_CAMERAS = "Количество камер";
            lang_PLUMB_LINE_PTR = "Указатель отвесной линии";
            lang_PLUMB_LINE_CAM = "Отключен;Камера 1;Камера 2;Камера 3;Камера 4;";
            lang_ROLL_PITCH_CORR = "Корректировка крена и тангажа";
            lang_VIDEO_REC_OFF = "Отключение видеозаписи";
            lang_VIDEO_REC_OFF_LIST = "Запрещено;Разрешено;";
            lang_VIDEO_REC_OFF_1 = "Камера 1";
            lang_VIDEO_REC_OFF_2 = "Камера 2";
            lang_VIDEO_REC_OFF_3 = "Камера 3";
            lang_VIDEO_REC_OFF_4 = "Камера 4";
            lang_VIDEO_REC_OFF1 = "Отключение записи";
            lang_VIDEO_REC_OFF2 = "видеокамеры %d";
            lang_VIDEO_REC_OFF3 = "заблокировано";
            break;
        case 1:
            lang_NODATASNVR = "SVRS NO DATA";

            lang_INVAL_CORR_ROLL_PITCH = "Trim not available";  //"Корректировка невозможна:";
            lang_NO_ROLL = "No roll";                           //"Отсутствует крен";
            lang_NO_PITCH = "No pitch";                         //"Отсутствует тангаж";
            lang_NO_CAMERA = "No camera";                       //"Отсутствует камера";
            lang_APPLY = "Apply";
            lang_EXIT = "Exit";
            lang_NO_SNVR_CONTROL = "SVRS NO CONTROL";  //"Нет управления СНВР";

            lang_ID = "ID";                                                         //"Идентификатор";
            lang_NUM_OF_CAMERAS = "Number of cameras";                              //"Количество камер";
            lang_PLUMB_LINE_PTR = "Plumb line pointer";                             //"Указатель отвесной линии";
            lang_PLUMB_LINE_CAM = "Disabled;Camera 1;Camera 2;Camera 3;Camera 4;";  //"Отключен;Камера 1;Камера 2;Камера 3;Камера 4;";
            lang_ROLL_PITCH_CORR = "Roll and pitch trim";                           //"Корректировка крена и тангажа";
            lang_VIDEO_REC_OFF = "Recording stop";                                  //"Отключение видеозаписи";					// TO DO уточнить перевод
            lang_VIDEO_REC_OFF_LIST = "Not available;Available;";                   //"Запрещено;Разрешено;";
            lang_VIDEO_REC_OFF_1 = "Camera 1";                                      //"Камера 1";
            lang_VIDEO_REC_OFF_2 = "Camera 2";                                      //"Камера 2";
            lang_VIDEO_REC_OFF_3 = "Camera 3";                                      //"Камера 3";
            lang_VIDEO_REC_OFF_4 = "Camera 4";                                      //"Камера 4";
            lang_VIDEO_REC_OFF1 = "Recording stop";                                 //"Отключение записи";						// TO DO уточнить перевод
            lang_VIDEO_REC_OFF2 = "for camera %d";                                  //"видеокамеры %d";								// TO DO уточнить перевод
            lang_VIDEO_REC_OFF3 = "not available";                                  //"заблокировано";									// TO DO уточнить перевод
            break;
    }
}

void CSNVR::ApplyTrim() {
    _d_roll = asinf(2.0f * (_plln_x - SCR_WIDTH / 2) / SCR_WIDTH) * toDeg - GetAircraftInfo().roll;
    _d_pitch = asinf(2.0f * (_plln_y - SCR_HEIGHT / 2) / SCR_HEIGHT) * toDeg - GetAircraftInfo().pitch;
}

void CSNVR::InitLanguageVariables() {
    lang_NODATASNVR = IUsingLanguage::error_lang;

    lang_INVAL_CORR_ROLL_PITCH = IUsingLanguage::error_lang;
    lang_NO_ROLL = IUsingLanguage::error_lang;
    lang_NO_PITCH = IUsingLanguage::error_lang;
    lang_NO_CAMERA = IUsingLanguage::error_lang;
    lang_APPLY = IUsingLanguage::error_lang;
    lang_EXIT = IUsingLanguage::error_lang;
    lang_NO_SNVR_CONTROL = IUsingLanguage::error_lang;

    lang_NAME = IUsingLanguage::error_lang;
    lang_ID = IUsingLanguage::error_lang;
    lang_NUM_OF_CAMERAS = IUsingLanguage::error_lang;
    lang_PLUMB_LINE_PTR = IUsingLanguage::error_lang;
    lang_PLUMB_LINE_CAM = IUsingLanguage::error_lang;
    lang_ROLL_PITCH_CORR = IUsingLanguage::error_lang;
    lang_VIDEO_REC_OFF = IUsingLanguage::error_lang;
    lang_VIDEO_REC_OFF_LIST = IUsingLanguage::error_lang;
    lang_VIDEO_REC_OFF_1 = IUsingLanguage::error_lang;
    lang_VIDEO_REC_OFF_2 = IUsingLanguage::error_lang;
    lang_VIDEO_REC_OFF_3 = IUsingLanguage::error_lang;
    lang_VIDEO_REC_OFF_4 = IUsingLanguage::error_lang;
    lang_VIDEO_REC_OFF1 = IUsingLanguage::error_lang;
    lang_VIDEO_REC_OFF2 = IUsingLanguage::error_lang;
    lang_VIDEO_REC_OFF3 = IUsingLanguage::error_lang;
}

void CSNVR::DrawForeground() {
    if (_changeCamState) {
        gui->SetPen(clYellow);
        gui->SetBrush(clNone);
        gui->SetFont(Fonts::FONT_M_C);
        gui->FormatAlign(0, SCR_WIDTH, SCR_HEIGHT / 2 - 30, alignCenterCenter, lang_VIDEO_REC_OFF1);
        gui->FormatAlign(0, SCR_WIDTH, SCR_HEIGHT / 2, alignCenterCenter, lang_VIDEO_REC_OFF2, blockCamRecordOff + 1);
        gui->FormatAlign(0, SCR_WIDTH, SCR_HEIGHT / 2 + 30, alignCenterCenter, lang_VIDEO_REC_OFF3);
    }

    if (showCameras) {
        DrawCameras();
    }

    if (invCamAppered) {
        DrawCamAtten();
    }

    if (_noGPS) {
        DrawGPSAtten();
    }

    if (_recorderState != 1 && paramState[snvrNoData] == mxNormal) {
        DrawRecAtten();
    }

    if ((selActiveCamera == _plln_ptr - 1) && activeCameras[_plln_ptr - 1]) {  // есть ли крен и тангаж?
        DrawPlumbLine();
    }
}

void CSNVR::DrawCameras() {
    const int x = CAM_RECT_LEFTTOP_X + 15;
    int y = CAM_HIGHEST_TOP_Y + 8;

    for (int n1 = 0; n1 < _numOfCameras; n1++) {
        gui->SetBrush(clNone);
        gui->SetPen(clWhite);
        if (_camStates[n1] == camInvalid) {
            gui->SetPen(clYellow);
        }
        if (_camStates[n1] == camNotConn) {
            gui->SetPen(clLightGray);
        }
        gui->Bmp(CAM_RECT_LEFTTOP_X, CAM_HIGHEST_TOP_Y + CAM_VERT_SPACE * n1, bmpCanvas, repaint);
        gui->WaitForBmp();

        if (n1 == selectedCamera) {
            if (!_controlBlocked || (_changeCamStateScore > 10 /*!= -1*/ && !((_changeCamStateScore / (changeCamStateScoreLimit / 6)) & 1))) {
                gui->SetPen(clCyan);
                gui->Bmp(CAM_RECT_LEFTTOP_X, CAM_HIGHEST_TOP_Y + CAM_VERT_SPACE * n1, bmpBorder, repaint);
                gui->WaitForBmp();
            }
        }

        // Отрисовка значков
        if (activeCameras[n1]) {
            gui->SetPen(clGreen);
        } else {
            if (_camStates[n1] == camValid) {
                gui->SetPen(clWhite);
            } else if (_camStates[n1] == camInvalid) {
                gui->SetPen(clYellow);
            } else {
                gui->SetPen(clLightGray);
            }
        }
        gui->Bmp(x, y + 2, bmpCamera, repaint);
        gui->WaitForBmp();
        gui->SetPen(clBlack);
        gui->SetFont(Fonts::FONT_S);
        gui->Format(x + 8, y + 0 + 3, "%d", n1 + 1);

        // Отметка пишущей камеры
        gui->SetBrush(clBlack);
        gui->SetPen(clBlack);
        if (recordingCameras[n1]) {
            gui->Circle(CAM_RECT_RIGHTTOP_X - 15, CAM_HIGHEST_TOP_Y + CAM_VERT_SPACE * n1 + 20, 6, true);
        } else {
            gui->Circle(CAM_RECT_RIGHTTOP_X - 15, CAM_HIGHEST_TOP_Y + CAM_VERT_SPACE * n1 + 20, 6, false);
            gui->Circle(CAM_RECT_RIGHTTOP_X - 15, CAM_HIGHEST_TOP_Y + CAM_VERT_SPACE * n1 + 20, 5, false);
        }

        y += CAM_VERT_SPACE;
    }

    if (_numOfCameras > 1) {  // Отрисовка значка все камеры
        DrawAllCams();
    }
}

void CSNVR::DrawAllCams() {
    const int x = CAM_RECT_LEFTTOP_X + 15;
    const int y = CAM_HIGHEST_TOP_Y + CAM_VERT_SPACE * _numOfCameras + 8;

    gui->SetBrush(clNone);

    gui->SetPen(clWhite);

    gui->Bmp(CAM_RECT_LEFTTOP_X, CAM_HIGHEST_TOP_Y + CAM_VERT_SPACE * _numOfCameras, bmpCanvas, repaint);
    gui->WaitForBmp();

    if (_numOfCameras == selectedCamera &&
        (!_controlBlocked ||
         (_changeCamStateScore != -1 &&
          !((_changeCamStateScore / (changeCamStateScoreLimit / 6)) & 1)))) {
        gui->SetPen(clCyan);
        gui->Bmp(CAM_RECT_LEFTTOP_X, CAM_HIGHEST_TOP_Y + CAM_VERT_SPACE * _numOfCameras, bmpBorder, repaint);
        gui->WaitForBmp();
    }

    gui->SetPen(clWhite);
    gui->Bmp(x + 3, y - 0, bmpCamera, repaint);
    gui->WaitForBmp();
    gui->Bmp(x - 3, y + 4, bmpCamera, repaint);
    gui->WaitForBmp();
}

void CSNVR::DrawRecAtten() {
    const int x = SCR_RIGHT - CAM_WIDTH;
    const int y = SCR_HEIGHT / 2 + 180;

    gui->Bmp(x - 15, y - 11, bmpRecorder);
    gui->WaitForBmp();
}

DISPATCH_MESSAGE_RESULT CSNVR::MFD_DispatchMessage(MESSAGES message, DWORD lParam, DWORD wParam) {
    static bool keyERPressed = false;
    DISPATCH_MESSAGE_RESULT result = dmrDispatched;
    switch (message) {
        case msHideInterface:
            showCameras = false;
            break;
        case msKeyUp:
            if (_controlBlocked || paramState[snvrNoData] == mxFail) {
                break;
            }

            if (showCameras && lParam == keyER) {
                keyERPressed = false;
                if (wParam < SETUP_DELAY / 2 || selectedCamera == _numOfCameras) {
                    if (selectedCamera < _numOfCameras || (selectedCamera == _numOfCameras)) {
                        selActiveCamera = selectedCamera;
                        _controlBlocked = true;
                    }
                }
            } else {
                result = dmrNotDispatched;
            }
            break;
        case msKey:
            if (_controlBlocked || paramState[snvrNoData] == mxFail) {
                break;
            }
            if (showCameras) {
                if (lParam == keyER && wParam >= SETUP_DELAY / 2 && _numOfCameras > 0 && !keyERPressed && selectedCamera < _numOfCameras) {
                    if (_recCamTurnOffs[selectedCamera]) {
                        keyERPressed = true;
                        selRecordingCamera[selectedCamera] = !selRecordingCamera[selectedCamera];

                        _controlBlocked = true;
                    } else {
                        _changeCamState = true;
                        blockCamRecordOff = selectedCamera;
                        KillTimer(msSNVRBlockRecordTimer);
                        SetTimer(blockRecordTime, msSNVRBlockRecordTimer);
                    }
                }
            }
            break;
        case msCoordDisagrTimer:
            KillTimer(msCoordDisagrTimer);
            _noGPS = true;
            break;
        case msSNVRBlockRecordTimer:
            _changeCamState = false;
            break;
        case msEnRightIn:
            ShowInterface();
            if (_controlBlocked) {
                break;
            }

            if (showCameras) {
                selectedCamera += (int)lParam;
                if (selectedCamera > _numOfCameras) {
                    selectedCamera = _numOfCameras;
                }
                if (selectedCamera < 0) {
                    selectedCamera = 0;
                }
            }
            // ShowInterface();
            break;
        case msSvnTimer: {
            if (_changeCamStateScore != -1) {
                _changeCamStateScore++;
            }
            if (_changeCamStateScore >= changeCamStateScoreLimit) {
                _camModesEqual = false;
                _controlBlocked = false;
                _changeCamStateScore = -1;
            }

            UpdateFails();
            ReadIncomingInfo();
            /*
                                    1	Старт пакета	0xA5
                                    2	Старт пакета	0xA3
                                    3	Идентификатор	Идентификатор ВР
                                    4	Достоверность данных с GPS
                                            0-	нет данных
                                            1-	отказ
                                            2-	исправно
                                    5–8	Дата и время	Дата и время в секундах от 01.01.1970 00:00:00 LittleEndian см. Функцию gmtime()
                                    9-12	Широта	В формате float
                                    13-16	Долгота	В формате float
                                    17	Режим работы каналов видеокамер и вывод из через CVBS	№ бита в байте
                                            0–1 камера №1
                                            (значение	0 – нет отображения, нет записи,
                                                                    1 – нет отображения, есть запись,
                                                                    2 – есть отображение, нет записи,
                                                                    3 – есть отображение, есть запись)
                                            2–3 камера №2 (аналогично камере №1)
                                            4–5 камера №3 (аналогично камере №1)
                                            6–7 камера №4 (аналогично камере №1)
                                    18-19	Резерв	0
                                    20	Конец пакета	0xA4
            */

            BYTE buff[20];
            buff[0] = 0xa5;                  // маркер a5
            buff[1] = 0xa3;                  // маркер a3
            buff[2] = _cameraIdentificator;  // идентификатор ВР

            if (paramState[BosnFail] == mxNormal) {
                buff[3] = 2;  // исправно
            }
            if (paramState[BosnFail] == mxFail || paramState[BosnFail] == mxTest) {  // TO DO уточнить, как интерпретировать mxTest
                buff[3] = 1;                                                         // неисправно
            }
            if (paramState[BosnFail] == mxNoData) {
                buff[3] = 0;  // нет данных
            }

            if (buff[3] != 2) {
                _unixt = mfd_clock->GetTimeLong();
            } else {
                if (GetAircraftInfo().date.day != 0xFF && GetAircraftInfo().date.month != 0xFF && GetAircraftInfo().date.year != 0xFF &&
                    GetAircraftInfo().timeMSC.sec != 0xFF && GetAircraftInfo().timeMSC.min != 0xFF && GetAircraftInfo().timeMSC.hour != 0xFF) {
                    _unixt = ToUnixTime(GetAircraftInfo().date.day, GetAircraftInfo().date.month, GetAircraftInfo().date.year,
                                        GetAircraftInfo().timeMSC.hour, GetAircraftInfo().timeMSC.min, GetAircraftInfo().timeMSC.sec);
                } else {
                    _unixt = mfd_clock->GetTimeLong();
                }
            }

            memcpy(&buff[4], &_unixt, sizeof(_unixt));
            _latNav = GetAircraftInfo().latitude;
            _longNav = GetAircraftInfo().longitude;
            if (buff[3] == 2) {
                memcpy(&buff[8], &_latNav, sizeof(_latNav));
                memcpy(&buff[12], &_longNav, sizeof(_longNav));
            } else {
                float nan = 0;
                memcpy(&buff[8], &nan, sizeof(nan));
                memcpy(&buff[12], &nan, sizeof(nan));
            }
            buff[16] = 0;  // состояние камер
            for (int n1 = 0; n1 < _numOfCameras; n1++) {
                if (selRecordingCamera[n1]) {
                    buff[16] |= 1 << (2 * n1);
                }
            }
            if (selActiveCamera == _numOfCameras) {
                buff[16] |= 0xAA;  // Все камеры активны
            } else {
                buff[16] |= (1 << (2 * selActiveCamera + 1));
            }

            camerasMode = buff[16];
            buff[17] = buff[18] = 0;  // Резерв
            buff[19] = 0xa4;          // маркер a4
            _rs232->SendBuff((char *)buff, 20);
            // CConsole::Trace("recordingCameras %d %d %d %d\n",recordingCameras[0],recordingCameras[1],recordingCameras[2],recordingCameras[3]);
            // CConsole::Trace("selRecordingCamera %d %d %d %d\n",selRecordingCamera[0],selRecordingCamera[1],selRecordingCamera[2],selRecordingCamera[3]);
            // CConsole::Trace("Камеры %x\n",(unsigned int)buff[16]);
        } break;
        default:
            result = dmrNotDispatched;
            break;
    }
    return result;
}

void CSNVR::LostReceiving() {
    if (_lostReceiving < maxLostReceiving) {
        _lostReceiving++;
    } else {
        for (int n1 = 0; n1 < _numOfCameras; n1++) {
            _camStates[n1] = camNotConn;
            recordingCameras[n1] = false;
            activeCameras[n1] = false;
        }
        _recorderState = 0;
        _noGPS = false;

        if (_prevReceiving) {
            ShowInterface();
        }
        _prevReceiving = false;
    }
}

bool CSNVR::ReadIncomingInfo() {
    char inBuff[128];

    DWORD bytesRead = 0;
    if (_rs232->GetStatus().eop_rd) {
        bytesRead = _rs232->ReadBuff(inBuff, 128);
    } else {
        _rs232->ResetStatus();
    }

    Converter *conv = reinterpret_cast<Converter *>(inBuff);

    if (bytesRead < 26 || conv->marker != 0xA3A5) {
        LostReceiving();
        return false;
    }

    if ((conv->identificator != 1)) {
        LostReceiving();
    }

    if (!_prevReceiving) {
        ShowInterface();
    }
    _lostReceiving = 0;
    _prevReceiving = true;
    _identValid = (conv->identificator == 1);

    static BYTE prevStateCamera = 0;
    _time = conv->dateTime;
    _lat = conv->latitude;
    _long = conv->longtitude;
    _recorderState = conv->stateStorage;

    if ((fabs(_latNav - _lat) >= 1. / 60) || (fabs(_longNav - _long) >= 1. / 60) || (m_abs((int)_time - _unixt) > 5)) {
        if (!_isCoordDisagrTimerOn) {
            _isCoordDisagrTimerOn = true;
            KillTimer(msCoordDisagrTimer);
            SetTimer(coordDisagrTime, msCoordDisagrTimer);
        }
    } else {
        _isCoordDisagrTimerOn = false;
        _noGPS = false;
        KillTimer(msCoordDisagrTimer);
    }

    for (int n1 = 0; n1 < _numOfCameras; n1++) {
        recordingCameras[n1] = (conv->camerasMode >> (2 * n1)) & 1;
        activeCameras[n1] = (bool)((conv->camerasMode >> (2 * n1)) & 2);
    }

    for (int n1 = 0; n1 < _numOfCameras; n1++) {
        _camStates[n1] = (CAM_STATE)((conv->stateCamera >> (2 * n1)) & 3);
        if (_camStates[n1] == camReserv) {
            _camStates[n1] = camNotConn;
        }
    }

    if (!showCameras && (conv->stateCamera & ~prevStateCamera)) {
        invCamAppered = true;
    }

    if ((conv->camerasMode) == (camerasMode)) {
        _camModesEqual = true;
        _controlBlocked = false;
        _changeCamStateScore = -1;
    } else if (_changeCamStateScore == -1) {
        _changeCamStateScore = 0;
    }

    prevStateCamera = conv->stateCamera;

    return true;
}

void CSNVR::UpdateFails() {
    paramState[snvrNoData] = (_lostReceiving == maxLostReceiving) ? mxFail : mxNormal;

    if (paramState[snvrNoData] == mxNormal) {
        paramState[snvrControlFail] = (_identValid && _camModesEqual) ? mxNormal : mxFail;

        for (int n1 = 0; n1 < _numOfCameras; n1++) {
            paramState[snvrFailCam1Fail + n1] = (_camStates[n1] != camInvalid) ? mxNormal : mxFail;
            paramState[snvrNoCam1Fail + n1] = (_camStates[n1] == camNotConn) ? mxFail : mxNormal;
        }

        switch (_recorderState) {
            case 0:  // Нет накопителя
                paramState[snvrRecorderFail] = mxNormal;
                paramState[snvrNoRecorder] = mxFail;
                break;
            case 1:  // Накопитель есть, исправен
                paramState[snvrRecorderFail] = mxNormal;
                paramState[snvrNoRecorder] = mxNormal;
                break;
            case 2:  // Накопитель есть, но неисправен
                paramState[snvrRecorderFail] = mxFail;
                paramState[snvrNoRecorder] = mxNormal;
                break;
            default:  // резерв
                paramState[snvrRecorderFail] = mxFail;
                paramState[snvrNoRecorder] = mxFail;
                break;
        }
        paramState[snvrGPSFail] = _noGPS ? mxFail : mxNormal;
    } else {
        paramState[snvrControlFail] = mxNoData;
        for (int n1 = 0; n1 < _numOfCameras; n1++) {
            paramState[snvrFailCam1Fail + n1] = mxNoData;
            paramState[snvrNoCam1Fail + n1] = mxNoData;
        }
        paramState[snvrRecorderFail] = mxNoData;
        paramState[snvrNoRecorder] = mxNoData;
        paramState[snvrGPSFail] = mxNoData;
    }

    _fail = paramState[snvrNoData] == mxFail || paramState[snvrControlFail] == mxFail || paramState[snvrRecorderFail] != mxNormal || paramState[snvrNoRecorder] == mxFail;
}

UINT CSNVR::ToUnixTime(UINT day, UINT mon, UINT year, UINT hour, UINT min, UINT sec) {
    const int t_month[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    year += 100;
    mon--;
    day--;

    UINT unixt = (year - 70) * 365 * 86400;
    for (UINT n1 = 70; n1 < year; n1++) {
        if (n1 % 4 == 0) {
            unixt += 86400;
        }
    }

    unixt += t_month[mon] * 86400;
    if (year % 4 == 0 && (mon + 1) >= 2) {
        unixt += 86400;
    }

    unixt += day * 86400;

    unixt += hour * 3600 + min * 60 + sec;
    return unixt;
}

#endif
