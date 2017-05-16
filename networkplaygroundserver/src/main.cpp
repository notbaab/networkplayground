//
//  main.cpp
//  networkplaygroundclient
//
//  Created by Erik Parreira on 2/15/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include "Networking/Server.h"
#include "networking/Logger.h"

// const int port = 3200;

// Some basic setup.
int setup()
{
    setbuf( stdout, NULL );
    return 0;
}

const char** __argv;
int __argc;
int main( int argc, const char* argv[] )
{
    Logger::InitLog(Logger::ALL, "/Users/erikparreira/Dropbox/Programming/"
                            "networkplayground/server_log.txt");
    if ( Server::StaticInit() )
    {
        return Server::sInstance->Run();
    }
}
