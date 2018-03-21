#include "networking/Logger.h"
#include <fstream>
#include <iostream>

namespace Logger
{

std::shared_ptr<spdlog::logger> the_log;

void InitLog(spdlog::level::level_enum level, std::string name)
{
    // spdlog::set_async_mode(4096);
    the_log = spdlog::stdout_color_mt(name);
    the_log->set_level(spdlog::level::trace);
}

void SetLevel(spdlog::level::level_enum level)
{
    the_log->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger> GetSpdLogger() {
    return the_log;
}

}