#include <iostream>
#include <string>
#include <thread>

#include "console/console.h"
#include "gameobjects/World.h"
#include "networking/Logger.h"
#include "networking/Server.h"

#define EXIT "exit"

const char** __argv;
int __argc;
int main(int argc, const char* argv[])
{
    std::string log_file;
    std::thread t;
    while (argc > 1)
    {
        argc--;
        argv++;
        if (!strcmp(*argv, "--console"))
        {
            std::cout << "Starting a shell" << std::endl;
            t = std::thread(&interactive_console);
        }
        else if (!strcmp(*argv, "--log-file"))
        {
            argv++;
            argc--;
            log_file = *argv;
        }
    }

    // Logger::InitLog(spdlog::level::info, "server");
    if (!log_file.empty())
    {
        Logger::InitLog(spdlog::level::info, "server", log_file);
    }
    else
    {
        Logger::InitLog(spdlog::level::info, "server");
    }

    if (Server::StaticInit())
    {
        int exitCode = Server::sInstance->Run();
        t.join();
        return exitCode;
    }
}
