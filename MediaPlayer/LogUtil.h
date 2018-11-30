#pragma once
#include <string>
#include <string.h>
#include <stdio.h>


void LogInit(const char *logfile);
void LogDeinit();



extern void suck(const char* fmt, ...);
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
//#define makePrefix(fmt) std::string(__FILENAME__).append("::").append(__FUNCTION__).append("() - ").append(fmt).c_str()
//#define makePrefix(fmt) std::string(fmt).c_str()
#define makePrefix(fmt) std::string("[]").append(__FUNCTION__).append("()-L:").append(std::to_string(__LINE__)).append(" ").append(fmt).c_str()

#define LogDebug(fmt, ...)	suck(makePrefix(fmt), ##__VA_ARGS__)
//#define LogDebug(fmt, ...) do {} while (false)
#define LogInfo(fmt, ...)	suck(makePrefix(fmt), ##__VA_ARGS__)
#define LogNotice(fmt, ...)	suck(makePrefix(fmt), ##__VA_ARGS__)
#define LogError(fmt, ...)	suck(makePrefix(fmt), ##__VA_ARGS__)
// #define LogError(fmt, ...) do {} while (false)
#define FunEntry(...) LogInfo(" Entry... " ##__VA_ARGS__)
#define FunExit(...) LogInfo(" Exit... " ##__VA_ARGS__)
