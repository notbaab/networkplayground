#include <iostream>
#include <thread>
#include <string>

#include "gameobjects/Client.h"
#include "networking/Logger.h"
#include "gameobjects/World.h"


const char** __argv;
int __argc;

// Logger::LogLevel logLevel = Logger::ALL;
Logger::LogLevel logLevel = Logger::CRITICAL;

#define EXIT "exit"

void interactive_console()
{
    setbuf( stdout, NULL );
    std::string command;
    do {
        std::cin >> command;

        if ("locate" == command)
        {

            World::PrintInfo();
        }
    } while (EXIT != command);
    std::cout << "Exiting";
}

int main( int argc, const char* argv[] )
{
    std::thread t(&interactive_console);   // t starts running

    __argc = argc;
    __argv = argv;

    Logger::InitLog(logLevel);
    LOG(Logger::TRACE, "Starting");

    if ( Client::StaticInit() )
    {
        return Client::sInstance->Run();
    }
else
    {
        SDL_Quit();
        // error
        return 1;
    }
}
