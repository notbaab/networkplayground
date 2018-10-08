#include <iostream>
#include <string>
#include <thread>

#include "gameobjects/Client.h"
#include "gameobjects/World.h"
#include "networking/Logger.h"

const char** __argv;
int __argc;

#define EXIT "exit"

void interactive_console()
{
    setbuf(stdout, NULL);
    std::string command;
    do
    {
        std::cin >> command;

        if ("locate" == command)
        {

            World::PrintInfo();
        }
    } while (EXIT != command);
    std::cout << "Exiting";
}

int main(int argc, const char* argv[])
{
    // std::thread t(&interactive_console);   // t starts running

    __argc = argc;
    __argv = argv;

    Logger::InitLog(spdlog::level::trace, "client");

    if (Client::StaticInit())
    {
        int exitCode = Client::sInstance->Run();
        INFO("Exiting");
        SDL_Quit();
        return exitCode;
    }
    else
    {
        SDL_Quit();
        // error
        return 1;
    }
}
