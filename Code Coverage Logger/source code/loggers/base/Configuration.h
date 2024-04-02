#pragma once

/// Директива, подключающая всё логирование.
#define USING_LOGGING

/// Директива, подключающая usb логирование.
//#define USING_LOGGING_USB

/// Директива, подключающая логирование на экране МФИ.
//#define USING_LOGGING_MFD

/// Директива, подключающая логирование покрытие кода.
#define USING_LOGGING_COVERAGE

/// Директива, определяющая вид имени файла: если присутствует, то имя файла будет содержать путь к файлу
#define USING_FULL_FILE_NAME

namespace Logging {

constexpr auto SIZE_OF_BUFFERS = 512;
constexpr auto SIZE_OF_ENTRIES_IN_MFD_SINK = 100;
constexpr auto SIZE_OF_ENTRIES_IN_USB_SINK = 1024;
constexpr auto SIZE_OF_ENTRIES_IN_COV_SINK = 65536;

}