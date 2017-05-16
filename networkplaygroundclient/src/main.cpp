#include "gameobjects/Client.h"
#include "networking/Logger.h"


const char** __argv;
int __argc;

Logger::LogLevel logLevel = Logger::ALL;

int main( int argc, const char* argv[] )
{

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
