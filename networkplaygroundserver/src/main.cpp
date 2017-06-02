//
//  main.cpp
//  networkplaygroundclient
//
//  Created by Erik Parreira on 2/15/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//
#include <iostream>
#include <thread>
#include <string>

#include "networking/Server.h"
#include "networking/Logger.h"
#include "gameobjects/World.h"

// const int port = 3200;

#define EXIT "exit"
// Some basic setup.

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

const char** __argv;
int __argc;
int main( int argc, const char* argv[] )
{
    std::thread t(&interactive_console);   // t starts running

    Logger::InitLog(Logger::CRITICAL);
    Logger::Log(Logger::DEBUG, "Starting");

    if ( Server::StaticInit() )
    {
        return Server::sInstance->Run();
    }
}
