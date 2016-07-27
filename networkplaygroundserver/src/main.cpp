//
//  main.cpp
//  networkplaygroundclient
//
//  Created by Erik Parreira on 2/15/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include "Networking/Server.h"
#include <iostream>
#include <unistd.h>

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
    if ( Server::StaticInit() )
    {
        return Server::sInstance->Run();
    }
    //    setup();
    //
    //    // Start Server
    //    NetworkManager nm = NetworkManager();
    //    // TODO: Catch error
    //    nm.Init( port );
    //
    //    // std::cout << "Starting server on " << port << std::endl;
    //    while ( true )
    //    {
    //        nm.ProcessIncomingPackets();
    //        // std::cout << "blah" << std::endl;
    //        sleep( 1 );
    //    }
    //    return 0;
}
