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
    //     NetworkManagerServer::sInstance->SetDropPacketChance( 0.8f );
    //     NetworkManagerServer::sInstance->SetSimulatedLatency( 0.25f );
    //     NetworkManagerServer::sInstance->SetSimulatedLatency( 0.5f );
    //     NetworkManagerServer::sInstance->SetSimulatedLatency( 0.1f );
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

bool Server::DoFrame()
{
    NetworkManagerServer* networkManager = NetworkManagerServer::sInstance;

    // Read all the incoming packets
    networkManager->ProcessIncomingPackets();
    networkManager->CheckForDisconnects();

    Engine::DoFrame();

    networkManager->SendOutgoingPackets();

    return true;
}

void Server::HandleNewClient( ClientProxyPtr inClientProxy )
{
    int playerId = inClientProxy->GetPlayerId();

    // TODO: add a player object to the world
}

void Server::HandleLostClient( ClientProxyPtr inClientProxy ) {}
