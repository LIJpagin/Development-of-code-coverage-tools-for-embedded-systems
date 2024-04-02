/*
 * svv.cpp
 *  Created on: 18.07.2023
 *      Author: Shpagin_D
 */

#include "svn.h"
#include "test_logger.h"

#ifdef _SVN

#include "../../utils/cgui.h"
#include "../../scrcoord.h"
#include "../../utils/setupmenu/list.h"
#include "../../utils/setupmenu/item.h"
#include "../../controls/clistboxview.h"

const char* SVN::Name[] = {"СВН", "VSS"};

bool SVN::_isReceiveSoftwareVersion{false};

int SVN::_softwareVersionDevice[MAX_NUMBER_OF_DEVICE] = {0};

SVN::State SVN::_deviceStates[MAX_NUMBER_OF_DEVICE];

UINT SVN::_numberOfFailedReceive[SVN::numberOfWords] = {0};

SVN::PARAM_STATE SVN::_wordState[SVN::numberOfWords];

SVN::InformationAboutSystem::InformationAboutSystem(IUsingSetupMenu* context, const char*& name,
                                                    SetupMenu::ACCESS_LEVEL AccessLevel, int id, UINT funcType)
    : Item(context, name, AccessLevel, id, funcType) {
    InitLanguageVariables();
}

void SVN::InformationAboutSystem::LoadLanguage(unsigned int lang) {
    Item::LoadLanguage(lang);
    switch (lang) {
        case 0:
            lang_DEVICE = "Устройство";
            lang_SOFTWARE_VERSION = "Версия ПО";
            lang_DEVICE_NAME[Device::BUK1AB] = "БУК-1АБ";
            lang_DEVICE_NAME[Device::TK2A] = "ТК-2А";
            lang_DEVICE_NAME[Device::TK3A] = "ТК-3А";
            lang_DEVICE_NAME[Device::TK4A] = "ТК-4А";
            break;
        case 1:
            lang_DEVICE = "Device";
            lang_SOFTWARE_VERSION = "Version SW";
            lang_DEVICE_NAME[Device::BUK1AB] = "BUK-1AB";  // TODO как перевести?
            lang_DEVICE_NAME[Device::TK2A] = "ТC-2А";
            lang_DEVICE_NAME[Device::TK3A] = "ТC-3А";
            lang_DEVICE_NAME[Device::TK4A] = "ТC-4А";
            break;
    }
}

void SVN::InformationAboutSystem::Enter() {
    _isReceiveSoftwareVersion = true;
    Item::Enter();
}

void SVN::InformationAboutSystem::Exit() {
    Item::Exit();
    _isReceiveSoftwareVersion = false;
}

void SVN::InformationAboutSystem::DrawForeground() {
    using namespace SetupMenu;
    DrawFond();
    DrawTitle();

    int y = SETUPMENU_TOP + VERTICAL_PADDING + ITEM_DY;
    int spiltX = ITEM_START_X + (ITEM_WIDTH / 2);

    // Шапка таблицы
    gui->TextOutAlign(lang_DEVICE, ITEM_START_X, spiltX, y, alignCenterTop);
    gui->TextOutAlign(lang_SOFTWARE_VERSION, spiltX, ITEM_START_X + ITEM_WIDTH, y, alignCenterTop);
    y += ITEM_DY;

    // Вывод информации об устройствах
    for (int device = Device::BUK1AB; device <= Device::TK4A; ++device) {
        // Имя устройства
        gui->TextOutAlign(lang_DEVICE_NAME[device], ITEM_START_X, spiltX, y, alignCenterTop);

        // Состояние номера устройства
        if (_wordState[WordBukSwVersion + device] != mxNoData &&
            (_wordState[WordBukSwVersion + device] == mxFail ||
             (_softwareVersionDevice[device] & 0xF) > 9 ||
             ((_softwareVersionDevice[device] >> 4) & 0xF) > 9 ||
             ((_softwareVersionDevice[device] >> 8) & 0xF) > 9 ||
             ((_softwareVersionDevice[device] >> 12) & 0xF) > 9)) {
            gui->SetBrush(clYellow);
        } else {
            gui->SetBrush(clWhite);
        }

        // Номер устройства
        if (_wordState[WordBukSwVersion + device] == mxNoData) {
            gui->TextOutAlign("-.-.-.-", spiltX, ITEM_START_X + ITEM_WIDTH, y, alignCenterTop);
        } else {
            gui->FormatAlign(spiltX, ITEM_START_X + ITEM_WIDTH, y, alignCenterTop,
                             "%X.%X.%X.%X", (_softwareVersionDevice[device] >> 12) & 0xF, (_softwareVersionDevice[device] >> 8) & 0xF,
                             (_softwareVersionDevice[device] >> 4) & 0xF, _softwareVersionDevice[device] & 0xF);
        }
        gui->SetBrush(clWhite);

        y += ITEM_DY;
    }

    // Кнопка выхода
    gui->Format(ITEM_START_X, SETUPMENU_BOTTOM - VERTICAL_PADDING - ITEM_DY, "9. %s", lang_menu_EXIT);
}

DISPATCH_MESSAGE_RESULT SVN::InformationAboutSystem::MFD_DispatchMessage(MESSAGES message, DWORD lParam, DWORD wParam) {
    DISPATCH_MESSAGE_RESULT result = dmrDispatched;
    if (message == msKey && wParam == 1 && lParam == keyN2) {
        Exit();
    } else {
        result = dmrNotDispatched;
    }
    return result;
}

void SVN::InformationAboutSystem::InitLanguageVariables() {
    lang_DEVICE = IUsingLanguage::error_lang;
    lang_SOFTWARE_VERSION = IUsingLanguage::error_lang;
    lang_STATE = IUsingLanguage::error_lang;
    for (int i = 0; i < MAX_NUMBER_OF_DEVICE; ++i) {
        lang_DEVICE_NAME[i] = IUsingLanguage::error_lang;
    }
}

void SVN::ArincWord::SendingToSvn::SetDisplayedCamera(const SVN::Camera camera) {
    switch (camera) {
        case Camera::None:
            videoSourceCode = 0b000;
            break;
        case Camera::TK4Adeg10:
            videoSourceCode = 0b001;
            break;
        case Camera::TK3A:
            videoSourceCode = 0b010;
            break;
        case Camera::TK4Adeg30:
            videoSourceCode = 0b011;
            break;
        case Camera::TK2A:
            videoSourceCode = 0b100;
            break;
        case Camera::TK4Adeg20:
            videoSourceCode = 0b101;
            break;
        case Camera::TK4Adeg0:
            videoSourceCode = 0b110;
            break;
        case Camera::TK4Adeg40:
            videoSourceCode = 0b111;
            break;
    }
}

SVN::Camera SVN::ArincWord::ReceptionToSvn::GetDisplayedCamera() const {
    switch (videoSourceCode) {
        case 0b000:
            return Camera::None;
        case 0b001:
            return Camera::TK4Adeg10;
        case 0b010:
            return Camera::TK3A;
        case 0b011:
            return Camera::TK4Adeg30;
        case 0b100:
            return Camera::TK2A;
        case 0b101:
            return Camera::TK4Adeg20;
        case 0b110:
            return Camera::TK4Adeg0;
        case 0b111:
            return Camera::TK4Adeg40;
    }
}

SVN::State SVN::ArincWord::ReceptionToSvn::GetDeviceState(const Device device) const {
    switch (device) {
        case Device::BUK1AB:
            return isSvnServiceability == 1 ? State::Normal : State::Denial;
        case Device::TK2A:
            return isVideoFromTK2A == 1 ? State::Normal : State::Denial;
        case Device::TK3A:
            return isVideoFromTK3A == 1 ? State::Normal : State::Denial;
        case Device::TK4A:
            return isVideoFromTK4A == 1 ? State::Normal : State::Denial;
        default:
            return State::Denial;
    }
}

UINT SVN::ArincWord::SoftwareVersion::GetSoftwareVersion() const {
    return symbol1 << 12 | symbol2 << 8 | symbol3 << 4 | symbol4;
}

SVN::SVN(CVideo* videoDriver, CArinc429In* arinc429in, CArinc429Out* arinc429out)
    : CCameraBase(videoDriver), _arinc429in(arinc429in), _arinc429out(arinc429out), _cameraSettings(this, lang_SVN_CAMERA_SETTINGS, SetupMenu::aclSystem, SetupMenu::idNull, setEnter | setExit, &_i2cDriver, _videoDriver), _systemState(lang_SYSTEM_STATE, this, pmSvnSystemState) {
    InitSetupMenu();
    InitLanguageVariables();
    PrepareSystemSetting();
    PrepareUserSetting();
}

SetupMenu::Item* SVN::GetConfigurator() {
    return &_cameraSettings;
}

void SVN::Init() {
    _arinc429in->ReceiveOnlyCorrectWords();
    _arinc429in->SetSpeed(SEND_RECEIVE_SPEED);
    _arinc429in->TakeHold();
    _arinc429out->SetRate(SEND_RECEIVE_SPEED);
    _arinc429out->TakeHold();
    SetFastTimer(DATA_UPDATE_TIME, msSvnTimer);
    SetTimer(FAILURE_CONTROL_TIME, msSvnFailureControl);
}

void SVN::UpdateSystemFail() {
    // Ошибка отказ СВН
    if (_wordState[WordDataReceptionSvn] == mxFail) {
        paramState[svnFail] = mxFail;
    }
    // Ошибка нет данных от СВН
    else if (_wordState[WordDataReceptionSvn] == mxNoData) {
        paramState[svnNoData] = mxFail;
    } else {
        paramState[svnNoData] = mxNormal;
    }

    // Ошибка отказ БУК
    if (_deviceStates[Device::BUK1AB] == State::Denial) {
        paramState[svnFailBuk] = mxFail;
    } else {
        paramState[svnFailBuk] = mxNormal;
    }

    // Если установилась ошибка нет данных от СВН, нужно снять все остальные ошибки
    if (paramState[svnNoData] == mxFail) {
        paramState[svnFailCtrl] = mxNormal;
        paramState[svnFail] = mxNormal;
        paramState[svnFailBuk] = mxNormal;
    }
}

void SVN::InitSetupMenu() {
    using namespace SetupMenu;
    _setupMenu = new Submenu(this, lang_NAME, aclUser, idNull, setNull);
    _setupMenu->Add(new List(this, lang_SWITCH_TO_VSS_AT_MFI_START, aclUser, idNull, setNull, &_switchToSvnAtMfiStart, lang_NO_YES));
    //_setupMenu->Add(new List(this, lang_CAMERA_PROFILE, aclSystem, idNull, setNull, &_loadingConfigType, lang_CAMERA_PROFILE_LIST));
    _setupMenu->Add(new InformationAboutSystem(this, lang_SYSTEM_INFORMATION, aclUser, idNull, setNull));
}

void SVN::UserDefault() {
    CModule::UserDefault();
}

void SVN::PrepareUserSetting() {
    CModule::PrepareUserSetting();
}

void SVN::PrepareSystemSetting() {
    sysParams.Add(&_loadingConfigType, lcAuto);
    sysParams.Add(&_manualConfig.startV, palSecamConfig.startV);
    sysParams.Add(&_manualConfig.endV, palSecamConfig.endV);
    sysParams.Add(&_manualConfig.startH, palSecamConfig.startH);
    sysParams.Add(&_manualConfig.endH, palSecamConfig.endH);
    sysParams.Add(&_manualConfig.quant, palSecamConfig.quant);
    sysParams.Add(&_manualConfig.reduce, palSecamConfig.reduce);
    // sysParams.Add(&_selectedCamera, Camera::TK2A);  <- это не системная настройка
    sysParams.Add(&_switchToSvnAtMfiStart, 0);
    // if (_selectedCamera != Camera::TK2A &&
    //	_selectedCamera != Camera::TK3A)
    //	_selectedDegreesTK4A = _selectedCamera;
    // else
    //	_selectedDegreesTK4A = Camera::TK4Adeg0;
}

void SVN::TurnOn() {
    LoadConfig();
    gui->SetPLDLayer(CVideo::videoLayer);
}

void SVN::TurnOff() {
    gui->SetPLDLayer(lsDisabled);
    _videoDriver->TurnOff();
}

void SVN::Open(MODULE_STATE state) {
    if (_switchToSvnAtMfiStart) {
        SendEvent(this, evtDisableInvalidWarnings);
    }
    CModule::Open(state);
    TurnOn();
}

void SVN::Close() {
    TurnOff();
    CModule::Close();
    if (_switchToSvnAtMfiStart) {
        SendEvent(this, evtEnableInvalidWarnings);
    }
}

void SVN::SetupMenuClick(int id, SETUP_ACTION action) {
}

DISPATCH_MESSAGE_RESULT SVN::MFD_DispatchMessage(MESSAGES message, DWORD lParam, DWORD wParam) {
    DISPATCH_MESSAGE_RESULT result = dmrDispatched;

    // Обработка таймера приема и передачи данных между МФИ и БУК
    if (message == msSvnTimer) {
        ReceiveAndProcessData();
        PackAndSendData();
        UpdateSystemFail();
        return result;
    }

    if (_isDrawSystemState) {
        if (lParam == keyN2) {
            _isDrawSystemState = false;
            SendEvent(this, evtEnableWarnings);
            if (_switchToSvnAtMfiStart != 1) {
                SendEvent(this, evtEnableInvalidWarnings);
            }
            return result;
        } else {
            return result;
        }
    }
    switch (message) {
        // Формирование отказа управления
        case msSvnFailureControl:
            if (_showCamera == _selectedCamera) {
                paramState[svnFailCtrl] = mxNormal;
                KillTimer(msSvnFailureControl);
                _isTimerFailureControlEnable = false;
            } else if (paramState[svnNoData] != mxFail) {
                paramState[svnFailCtrl] = mxFail;
            }
            break;
        case msKeyUp:
            if ((lParam == keyER) && (wParam < MENU_DELAY)) {
                // Блокировка управления камерами при отсутствии данных от СВН
                if (_wordState[WordDataReceptionSvn] == mxNoData) {
                    break;
                }
                if (_selectedCamera != Camera::None &&
                    _selectedCamera != Camera::TK2A &&
                    _selectedCamera != Camera::TK3A &&
                    wParam < MENU_DELAY) {
                    if (_selectedCamera != Camera::TK4Adeg0) {
                        _hasStateOfSystemChanged = true;
                    }
                    _selectedDegreesTK4A = _selectedCamera = Camera::TK4Adeg0;
                }
            } else {
                result = dmrNotDispatched;
            }
            break;
        case msKey:
            // Блокировка других кнопок управления или кнопок камер при отсутствии данных от СВН
            if (wParam != 1 || _wordState[WordDataReceptionSvn] == mxNoData) {
                break;
            }
            switch (lParam) {
                // Выбор отображения видео с ТК-2А
                case keyN0:
                    _selectedCamera = Camera::TK2A;
                    _hasStateOfSystemChanged = true;
                    break;
                // Выбор отображения видео с ТК-3А
                case keyN1:
                    _selectedCamera = Camera::TK3A;
                    _hasStateOfSystemChanged = true;
                    break;
                // Выбор отображения видео с ТК-4А
                case keyN2:
                    _selectedCamera = _selectedDegreesTK4A;
                    _hasStateOfSystemChanged = true;
                    break;
                default:
                    result = dmrNotDispatched;
                    break;
            }
            break;
        // Управление углом наклона ТК-4А
        case msEnRightIn:
            // Блокировка управления камерами при отсутствии данных от СВН
            if (paramState[svnNoData] == mxNoData) {
                break;
            }
            if (_selectedCamera != Camera::None &&
                _selectedCamera != Camera::TK2A &&
                _selectedCamera != Camera::TK3A) {
                Camera previousSelectedCamera = _selectedCamera;
                if ((int)_selectedCamera + (int)lParam < (int)Camera::TK4Adeg0) {
                    _selectedCamera = Camera::TK4Adeg0;
                } else if ((int)_selectedCamera + (int)lParam > (int)Camera::TK4Adeg40) {
                    _selectedCamera = Camera::TK4Adeg40;
                } else {
                    _selectedCamera = (Camera)((int)_selectedCamera + (int)lParam);
                }
                // Сохранение предыдущего выбора угла наклона этой камеры
                _selectedDegreesTK4A = _selectedCamera;
                // Проверка на изменение выводимой на экран камеры
                if (previousSelectedCamera != _selectedCamera) {
                    _hasStateOfSystemChanged = true;
                }
            }
            break;
        default:
            result = dmrNotDispatched;
            break;
    }

    if (_hasStateOfSystemChanged) {
        KillTimer(msSvnFailureControl);
        SetTimer(FAILURE_CONTROL_TIME, msSvnFailureControl);
        _isTimerFailureControlEnable = true;
    }
    return result;
}

MODULE_ID SVN::GetID() {
    return id;
}

bool SVN::GetWarning(const char*& buff, WARNING_PRIORITY& prior, COLOR& color) {
    if (GetState() == stPrimary && paramState[svnNoData] == mxFail) {
        buff = lang_NO_DATA_FROM_SVN;
        color = clYellow;
        return true;
    }
    if (GetState() == stPrimary && paramState[svnFail] == mxFail) {
        buff = lang_SVN_FAILURE;
        color = clYellow;
        return true;
    }
    return false;
}

bool SVN::GetInvalid(const char*& buff, COLOR& color) {
    if (GetState() == stDisabled && (paramState[svnNoData] == mxFail || paramState[svnFail] == mxFail || paramState[svnFailCtrl] == mxFail || _deviceStates[Device::BUK1AB] == State::Denial)) {
        buff = lang_NAME;
        color = clYellow;
        return true;
    }
    return false;
}

int SVN::SwitchToSvnAtMfiStart() const {
    return _switchToSvnAtMfiStart;
}

void SVN::DrawForeground() {
    if (paramState[svnNoData] != mxFail) {
        DrawCamerasIcons();
        DrawFailure();
    }
    if (_isDrawSystemState) {
        DrawSystemState();
    }
}

void SVN::DrawCamerasIcons() const {
    int x, y;
    ALIGN align;
    // Какая иконка камеры будет выделена при выборе пользователем
    int selectedDevice = 0;
    switch (_selectedCamera) {
        case Camera::TK2A:
            selectedDevice = Device::TK2A;
            break;
        case Camera::TK3A:
            selectedDevice = Device::TK3A;
            break;
        case Camera::TK4Adeg0:
        case Camera::TK4Adeg10:
        case Camera::TK4Adeg20:
        case Camera::TK4Adeg30:
        case Camera::TK4Adeg40:
            selectedDevice = Device::TK4A;
            break;
        case Camera::None:
            selectedDevice = 0;
            break;
    }
    int showDevice = 0;
    // Какая иконка камеры будет выделена при отображении СВН
    switch (_showCamera) {
        case Camera::TK2A:
            showDevice = Device::TK2A;
            break;
        case Camera::TK3A:
            showDevice = Device::TK3A;
            break;
        case Camera::TK4Adeg0:
        case Camera::TK4Adeg10:
        case Camera::TK4Adeg20:
        case Camera::TK4Adeg30:
        case Camera::TK4Adeg40:
            showDevice = Device::TK4A;
            break;
        case Camera::None:
            showDevice = 0;
            break;
    }

    for (int camera = Device::TK2A; camera <= Device::TK4A; ++camera) {
        x = BUTTONS_LABELS_X[6 + camera];
        y = BUTTONS_LABELS_Y[6 + camera];
        align = BUTTONS_LABELS_ANCHOR[6 + camera];
        gui->SetBrush(clNone);

        // Окантовка выбранной камеры
        if (camera == selectedDevice) {
            gui->SetPen(clDarkGreen);
            gui->Bmp(x + 4, y, Bmp::Border, repaint, align);
            gui->WaitForBmp();
        }

        // Окантовка отображаемой камеры
        if (camera == showDevice) {
            gui->SetPen(clGreen);
            gui->Bmp(x + 4, y, Bmp::Border, repaint, align);
            gui->WaitForBmp();
        }

        // Иконка камеры
        if (_deviceStates[camera] == State::Normal) {
            gui->SetPen(clWhite);
        } else {
            gui->SetPen(clYellow);
        }
        gui->Bmp(x - 5, y, Bmp::Camera, repaint, align);
        gui->WaitForBmp();

        // Номер камеры
        gui->SetPen(clBlack);
        gui->SetFont(Fonts::FONT_S);
        gui->FormatAlign(x - 63, x, y, alignCenterCenter, "%d", camera);
    }

    // Изменение цвета угла наклона после при несовпадении
    gui->SetPen(clWhite);
    if (showDevice == Device::TK4A) {
        gui->SetPen(clGreen);
    }
    if (selectedDevice == Device::TK4A && _selectedCamera != _showCamera) {
        gui->SetPen(clDarkGreen);
    }

    gui->SetFont(Fonts::FONT_S_C);
    const char* degreesStr = "";
    Camera showDegrees = (selectedDevice != Device::TK4A && _selectedDegreesTK4A != _showCamera && showDevice == Device::TK4A) ? _showCamera : _selectedDegreesTK4A;
    switch (showDegrees) {
        case Camera::TK4Adeg0:
            degreesStr = "0°";
            break;
        case Camera::TK4Adeg10:
            degreesStr = "10°";
            break;
        case Camera::TK4Adeg20:
            degreesStr = "20°";
            break;
        case Camera::TK4Adeg30:
            degreesStr = "30°";
            break;
        case Camera::TK4Adeg40:
            degreesStr = "40°";
            break;
        default:
            degreesStr = "";
    }
    // Вывод градуса наклона ТК-4А
    gui->FormatAlign(x - 50, x - 50, y, align, "%s", degreesStr);
}

void SVN::DrawFailure() const {
    // Поле вывода информации о неисправных блоках СВВ-1АБ (отказ БУК-1АБ)
    if (_deviceStates[Device::BUK1AB] == State::Denial || paramState[svnFailCtrl] == mxFail) {
        CListBoxView lbv(Fonts::FONT_S);
        lbv.Add(lang_FAILURE, ALARM_COLOR);
        if (_deviceStates[Device::BUK1AB] == State::Denial) {
            lbv.Add(lang_BUK_1AB, ALARM_COLOR);
        }
        if (paramState[svnFailCtrl] == mxFail) {
            lbv.Add(lang_FAILURE_CONTROLE, ALARM_COLOR);
        }
        lbv.Draw(RADAR_DIGIT_X, RADAR_DIGIT_Y, alignRightTop);
    }
}

void SVN::DrawSystemState() const {
    using namespace SetupMenu;

    int y = SETUPMENU_TOP + VERTICAL_PADDING + 2 * ITEM_DY;
    int spiltX = ITEM_START_X + (ITEM_WIDTH / 2);

    gui->SetBrush(clBlack);
    gui->SetPen(clWhite);
    gui->Rectangle(SCR_LEFT, SCR_TOP, SCR_RIGHT, SCR_BOTTOM);
    gui->WaitForRectangle();

    gui->SetFont(Fonts::FONT_XXL);
    gui->TextOutAlign(lang_SYSTEM_STATE_UPPER, ITEM_START_X, ITEM_START_X + ITEM_WIDTH, y, alignCenterTop);
    y += 3 * ITEM_DY;

    // Шапка таблицы
    gui->SetFont(Fonts::FONT_L);
    gui->TextOutAlign(lang_DEVICE, ITEM_START_X, spiltX, y, alignCenterTop);
    gui->TextOutAlign(lang_STATE, spiltX, ITEM_START_X + ITEM_WIDTH, y, alignCenterTop);
    y += 2 * ITEM_DY;

    // Вывод информации об устройствах
    for (int device = Device::BUK1AB; device <= Device::TK4A; ++device) {
        // Имя устройства
        gui->TextOutAlign(lang_DEVICE_NAME[device], ITEM_START_X, spiltX, y, alignCenterTop);

        const char* state{IUsingLanguage::error_lang};
        // Если не было 303 слова
        if (paramState[svnNoData] == mxFail) {
            gui->SetPen(clYellow);
            state = lang_NO_DATA;
        }
        // Состояние устройств
        else {
            state = _deviceStates[device] == State::Normal ? lang_SERVICEABLE : lang_FAILURE;
            gui->SetPen(_deviceStates[device] == State::Normal ? clGreen : clYellow);
        }
        gui->FormatAlign(spiltX, ITEM_START_X + ITEM_WIDTH, y, alignCenterTop, "%s", state);
        gui->SetPen(clWhite);

        y += ITEM_DY + ITEM_DY / 2;
    }
    gui->ButtonArrowLabel(9, lang_EXIT);
}

void SVN::ReceiveAndProcessData() {
    ArincWord words[CArinc429In::FIFO_SIZE];
    UINT numberOfReceiveWords = _arinc429in->GetData((DWORD*)words, CArinc429In::FIFO_SIZE);
    if (numberOfReceiveWords != 0) {
        for (int numberWord = numberOfReceiveWords - 1; numberWord >= 0; --numberWord) {
            ArincWord* word = &words[numberWord];

            // Проверка бита ошибки
            if (word->receiveData.parity != 0) {
                continue;
            }

            UINT reverseAddress = REVERSE(word->receiveData.address);

            switch (reverseAddress) {
                // Обновления системы при получении данных от СВВ
                case ADDRESS_OF_DATA_RECEPTION_SVN: {
                    // Получили нужное слово, состояние нормальной работы останется, если нет других ошибок
                    paramState[svnNoData] = mxNormal;
                    _wordState[WordDataReceptionSvn] = mxNormal;
                    _numberOfFailedReceive[WordDataReceptionSvn] = 0;

                    // Проверка по матрице состояния, отказ СВН
                    if (GetMatrixState(word->receiveData.matrix) != MatrixState::Normal) {
                        paramState[svnFail] = mxFail;
                    } else {
                        paramState[svnFail] = mxNormal;
                    }

                    // Обновление отображаемой камеры
                    _showCamera = words[numberWord].receiveData.GetDisplayedCamera();

                    // Удаляем таймер, если камера переключилась
                    if (_showCamera == _selectedCamera) {
                        paramState[svnFailCtrl] = mxNormal;
                        KillTimer(msSvnFailureControl);
                        _isTimerFailureControlEnable = false;
                    } else if (_isTimerFailureControlEnable != true) {
                        SetTimer(FAILURE_CONTROL_TIME, msSvnFailureControl);
                        _isTimerFailureControlEnable = true;
                    }

                    // Ошибку управления можно выдать только если есть данные от СВН
                    if (_showCamera == Camera::None && paramState[svnNoData] != mxFail) {
                        paramState[svnFailCtrl] = mxFail;
                    }

                    // Обновление состояний устройств
                    for (int device = Device::BUK1AB; device <= Device::TK4A; ++device) {
                        _deviceStates[device] = word->receiveData.GetDeviceState((Device)device);
                    }
                    // Обновление флага первого приема
                    if (!_isStateWordReceived) {
                        _isStateWordReceived = true;
                        if (_showCamera != Camera::None) {
                            _selectedCamera = _showCamera;
                            if (_showCamera != Camera::TK2A || _showCamera != Camera::TK3A) {
                                _selectedDegreesTK4A = _showCamera;
                            }
                        } else {
                            _selectedCamera = Camera::TK2A;
                        }
                    }
                    // Если принимаем версии ПО, то не проверяем это слово
                    if (_isReceiveSoftwareVersion) {
                        break;
                    } else {
                        return;
                    }
                }
                // Обновления системы при получении версии ПО БУК-1АБ
                case ADDRESS_OF_BUK_SW_VERSION: {
                    _wordState[WordBukSwVersion] = mxNormal;
                    _numberOfFailedReceive[WordBukSwVersion] = 0;
                    // Проверка по матрице состояния
                    if (GetMatrixState(word->receiveData.matrix) != MatrixState::Normal) {
                        _wordState[WordBukSwVersion] = mxFail;
                    }
                    _softwareVersionDevice[Device::BUK1AB] = word->versionData.GetSoftwareVersion();
                    break;
                }
                // Обновления системы при получении версии ПО ТК-2А
                case ADDRESS_OF_TK_2A_SW_VERSION: {
                    _wordState[WordTk2aSwVersion] = mxNormal;
                    _numberOfFailedReceive[WordTk2aSwVersion] = 0;
                    // Проверка по матрице состояния
                    if (GetMatrixState(word->receiveData.matrix) != MatrixState::Normal) {
                        _wordState[WordTk2aSwVersion] = mxFail;
                    }
                    _softwareVersionDevice[Device::TK2A] = word->versionData.GetSoftwareVersion();
                    break;
                }
                // Обновления системы при получении версии ПО ТК-3А
                case ADDRESS_OF_TK_3A_SW_VERSION: {
                    _wordState[WordTk3aSwVersion] = mxNormal;
                    _numberOfFailedReceive[WordTk3aSwVersion] = 0;
                    // Проверка по матрице состояния
                    if (GetMatrixState(word->receiveData.matrix) != MatrixState::Normal) {
                        _wordState[WordTk3aSwVersion] = mxFail;
                    }
                    _softwareVersionDevice[Device::TK3A] = word->versionData.GetSoftwareVersion();
                    break;
                }
                // Обновления системы при получении версии ПО ТК-4А
                case ADDRESS_OF_TK_4A_SW_VERSION: {
                    _wordState[WordTk4aSwVersion] = mxNormal;
                    _numberOfFailedReceive[WordTk4aSwVersion] = 0;
                    // Проверка по матрице состояния
                    if (GetMatrixState(word->receiveData.matrix) != MatrixState::Normal) {
                        _wordState[WordTk4aSwVersion] = mxFail;
                    }
                    _softwareVersionDevice[Device::TK4A] = word->versionData.GetSoftwareVersion();
                    break;
                }
                default:
                    break;
            }
        }
    }
    // Неудачная попытка, нужных слов не найдено
    for (int word = WordDataReceptionSvn; word < numberOfWords; ++word) {
        ++_numberOfFailedReceive[word];
        // Состояние отказа, в случае превышения количества попыток приема
        if (_numberOfFailedReceive[word] > MAX_LOST_RECEIVING) {
            _wordState[WordDataReceptionSvn + word] = mxNoData;
        }
    }
    // Приведение к настройкам по умолчанию в случае отсутствия входных данных при старте
    if (!_isStateWordReceived && _wordState[WordDataReceptionSvn] == mxNoData) {
        _isStateWordReceived = true;
        _selectedCamera = Camera::TK2A;
    }
}

void SVN::OnPilotsMenu(FuncId id) {
    switch (id) {
        case pmSvnSystemState:
            _isDrawSystemState = true;
            SendEvent(this, evtDisableInvalidWarnings);
            SendEvent(this, evtDisableWarnings);
            break;
        default:
            break;
    }
}

void SVN::AddToMenu(PilotsMenu::Menu& menu) {
    if (_state == stPrimary) {
        menu.Add(&_systemState);
    }
}

void SVN::PackAndSendData() {
    static bool isChangeBit = 0;
    if (_isStateWordReceived) {
        ArincWord word(0);
        word.sendData.SetDisplayedCamera(_selectedCamera);
        word.sendData.address = REVERSE(ADDRESS_OF_SENDING_TO_SVN);
        if (_hasStateOfSystemChanged) {
            word.sendData.isChange = isChangeBit = !isChangeBit;
            _hasStateOfSystemChanged = false;
        }
        word.sendData.isChange = isChangeBit;
        _arinc429out->SendWord(word.word);
    }
}

bool SVN::IsPilotMenuAvailable() const {
    return true;
}

void SVN::InitLanguageVariables() {
    lang_SWITCH_TO_VSS_AT_MFI_START = IUsingLanguage::error_lang;
    lang_SYSTEM_INFORMATION = IUsingLanguage::error_lang;
    lang_NO_YES = IUsingLanguage::error_lang;
    lang_NO_DATA_FROM_SVN = IUsingLanguage::error_lang;
    lang_SVN_FAILURE = IUsingLanguage::error_lang;
    lang_SVN_CAMERA_SETTINGS = IUsingLanguage::error_lang;
    lang_CAMERA_PROFILE = IUsingLanguage::error_lang;
    lang_CAMERA_PROFILE_LIST = IUsingLanguage::error_lang;
    lang_FAILURE = IUsingLanguage::error_lang;
    lang_BUK_1AB = IUsingLanguage::error_lang;
    lang_DEVICE = IUsingLanguage::error_lang;
    lang_STATE = IUsingLanguage::error_lang;
    lang_NO_DATA = IUsingLanguage::error_lang;
    lang_SERVICEABLE = IUsingLanguage::error_lang;
    for (int i = 0; i < MAX_NUMBER_OF_DEVICE; ++i) {
        lang_DEVICE_NAME[i] = IUsingLanguage::error_lang;
    }
}

void SVN::LoadLanguage(unsigned int lang) {
    CCameraBase::LoadLanguage(lang);
    lang_NAME = Name[lang];
    switch (lang) {
        case 0:
            lang_SWITCH_TO_VSS_AT_MFI_START = "Переход в СВН при старте МФИ";
            lang_SYSTEM_INFORMATION = "Информация о системе";
            lang_SYSTEM_STATE = "Состояние системы";
            lang_SYSTEM_STATE_UPPER = "СОСТОЯНИЕ СИСТЕМЫ";
            lang_NO_YES = "Нет;Да;";
            lang_NO_DATA_FROM_SVN = "НЕТ ДАННЫХ ОТ СВН";
            lang_SVN_FAILURE = "ОТКАЗ СВН";
            lang_SVN_CAMERA_SETTINGS = "Настройки камер СВН";
            lang_CAMERA_PROFILE = "Профиль камер";
            lang_CAMERA_PROFILE_LIST = "Автоматический;Пользовательский;";
            lang_FAILURE = "Отказ";
            lang_BUK_1AB = "БУК-1АБ";
            lang_FAILURE_CONTROLE = "Упр.";
            lang_DEVICE = "Устройство";
            lang_STATE = "Состояние";
            lang_DEVICE_NAME[Device::BUK1AB] = "БУК-1АБ";
            lang_DEVICE_NAME[Device::TK2A] = "ТК-2А";
            lang_DEVICE_NAME[Device::TK3A] = "ТК-3А";
            lang_DEVICE_NAME[Device::TK4A] = "ТК-4А";
            lang_NO_DATA = "Нет данных";
            lang_SERVICEABLE = "Исправно";
            lang_EXIT = "ВЫХОД";
            break;
        case 1:
            lang_SWITCH_TO_VSS_AT_MFI_START = "Switch to VSS at MFI start";
            lang_SYSTEM_INFORMATION = "Information about system";
            lang_SYSTEM_STATE = "System state";
            lang_SYSTEM_STATE_UPPER = "SYSTEM STATE";
            lang_NO_YES = "No;Yes;";
            lang_NO_DATA_FROM_SVN = "VSS NO DATA";
            lang_SVN_FAILURE = "VSS FAIL";
            lang_SVN_CAMERA_SETTINGS = "VSS cameras settings";
            lang_CAMERA_PROFILE = "Cameras profile";
            lang_CAMERA_PROFILE_LIST = "Automatic;Custom;";
            lang_FAILURE = "Fail";
            lang_BUK_1AB = "BUK-1AB";  // TODO как перевести?
            lang_FAILURE_CONTROLE = "Ctrl";
            lang_DEVICE = "Device";
            lang_STATE = "State";
            lang_DEVICE_NAME[Device::BUK1AB] = "BUK-1AB";  // TODO как перевести?
            lang_DEVICE_NAME[Device::TK2A] = "ТC-2А";
            lang_DEVICE_NAME[Device::TK3A] = "ТC-3А";
            lang_DEVICE_NAME[Device::TK4A] = "ТC-4А";
            lang_NO_DATA = "No data";
            lang_SERVICEABLE = "Working";
            lang_EXIT = "EXIT";
            break;
    }
}

SVN::MatrixState SVN::GetMatrixState(const UINT matrix) const {
    switch (matrix) {
        case 0b00:
            return MatrixState::Normal;
        case 0b10:
            return MatrixState::Unreliable;
        case 0b01:
            return MatrixState::NotUsed;
        case 0b11:
            return MatrixState::Failure;
    }
}

#endif  // _SVN
