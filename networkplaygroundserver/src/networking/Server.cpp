//
//  Server.cpp
//  networkplayground
//
//  Created by Erik Parreira on 7/25/16.
//

#include "networking/NetworkManagerServer.h"
#include "networking/Server.h"
#include "networking/StringUtils.h"
#include <string.h>

bool Server::StaticInit()
{
    sInstance.reset( new Server() );

    return true;
}

Server::Server()
{
    // Register objects with registry
    InitNetworkManager();
}

int Server::Run()
{
    SetupWorld();

    return Engine::Run();
}

bool Server::InitNetworkManager()
{
    //    std::string portString = StringUtils::GetCommandLineArg( 1 );
    //    uint16_t port = stoi( portString );

    return NetworkManagerServer::StaticInit( 45000 );
}

void Server::SetupWorld()
{
    // Nothing yet
}

void Server::DoFrame()
{
    NetworkManagerServer* networkManager = NetworkManagerServer::sInstance;
    // Read all the incoming packets
    networkManager->ProcessIncomingPackets();

    Engine::DoFrame();

    networkManager->SendOutgoingPackets();
}
