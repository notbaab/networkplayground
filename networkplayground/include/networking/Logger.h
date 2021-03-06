#ifndef StringUtils_h
#define StringUtils_h

#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <string>

namespace Logger
{
void InitLog(spdlog::level::level_enum level, std::string loggerName);
void InitLog(spdlog::level::level_enum level, std::string loggerName, std::string filename);
void SetLevel(spdlog::level::level_enum level);
std::shared_ptr<spdlog::logger> GetSpdLogger();

} // namespace Logger

#define TRACE(fmt, ...) SPDLOG_TRACE(Logger::GetSpdLogger(), fmt, ##__VA_ARGS__);
#define DEBUG(fmt, ...) SPDLOG_DEBUG(Logger::GetSpdLogger(), fmt, ##__VA_ARGS__);
#define INFO(fmt, ...) Logger::GetSpdLogger()->info(fmt, ##__VA_ARGS__);
#define WARNING(fmt, ...) Logger::GetSpdLogger()->warn(fmt, ##__VA_ARGS__);
#define ERROR(fmt, ...) Logger::GetSpdLogger()->error(fmt, ##__VA_ARGS__);
#define CRITICAL(fmt, ...) Logger::GetSpdLogger()->critical(fmt, ##__VA_ARGS__);

#endif
