#pragma once

#ifdef LOGGER_RAYLIB
#if defined(_WIN32)
#define NOGDI
#define NOUSER

#include <Windows.h>

#undef near
#undef far
#endif

#include <config.h>
#include <raylib.h>
#endif

#include "spdlog/spdlog.h"

/**
 * @brief macro to main logger
 */
#define LOG spdlog::get("main")

namespace util {

/**
 * @brief Initialize logger
 */
void init_logger();

#ifdef LOGGER_RAYLIB
inline void logger_callback_raylib(int log_level, const char* text, va_list args)
{
    char message[MAX_TRACELOG_MSG_LENGTH] = { 0 };
    vsprintf_s(message, text, args);

    switch (log_level) {
    case LOG_TRACE:
        LOG->trace(message);
        break;
    case LOG_DEBUG:
        LOG->debug(message);
        break;
    case LOG_INFO:
        LOG->info(message);
        break;
    case LOG_WARNING:
        LOG->warn(message);
        break;
    case LOG_ERROR:
        LOG->error(message);
        break;
    case LOG_FATAL:
        LOG->critical(message);
        break;
    default:
        break;
    }
}
#endif

}
