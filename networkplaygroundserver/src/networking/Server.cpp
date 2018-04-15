#include "gameobjects/GameObjectRegistry.h"
#include "gameobjects/PlayerServer.h"
#include "networking/Server.h"
#include "networking/Logger.h"
#include "timing/Timing.h"

bool Server::StaticInit()
{
    sInstance.reset( new Server() );

    return true;
}

Server::Server()
{
    // Register objects with registry
    GameObjectRegistry::sInstance->RegisterCreationFunction(
        Player::kClassId, PlayerServer::StaticCreate );
    InitNetworkManager();
    mNextPhysicsTick = 0.f;
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
    //    std::string portString = Logger::GetCommandLineArg( 1 );
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


    Timing::sInstance.Update();

    // Run "Physics" at 60 hertz
    float time = Timing::sInstance.GetFrameStartTime();
    if ( time > mNextPhysicsTick )
    {
        mNextPhysicsTick += TIME_BETWEEN_TICKS;
        Engine::DoFrame();
        networkManager->SendOutgoingPackets();
    }

    return true;
}

void Server::HandleNewClient( ClientProxyPtr inClientProxy )
{
    int playerId = inClientProxy->GetPlayerId();

    SpawnPlayer( playerId );
}

void Server::HandleLostClient( ClientProxyPtr inClientProxy )
{
    CRITICAL("WE LOST ONE!, {}", inClientProxy->GetName().c_str());
}

void Server::SpawnPlayer( int inPlayerId )
{
    INFO("Spawning player {}", inPlayerId);
    PlayerPtr player = std::static_pointer_cast<Player>(
        GameObjectRegistry::sInstance->CreateGameObject( Player::kClassId ) );

    player->SetPlayerId( inPlayerId );
    // gotta pick a better spawn location than this...
    player->SetLocation(
        Vector3( 1.f - static_cast<float>( inPlayerId ), 0.f, 0.f ) );
}