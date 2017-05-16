#include "IO/MemoryBitStream.h"
#include "gameobjects/GameObjectRegistry.h"
#include "networking/NetworkManagerClient.h"
#include "networking/ReplicationCommand.h"
#include "networking/Logger.h"

void ReplicationManagerClient::Read( InputMemoryBitStream& inInputStream )
{
    while ( inInputStream.GetRemainingBitCount() >= 32 )
    {
        int networkId;
        inInputStream.Read( networkId );

        // 2 bits for action
        uint8_t action;
        inInputStream.Read( action, 2 );

        switch ( action )
        {
        case RA_CREATE:
            ReadAndDoCreateAction( inInputStream, networkId );
            break;

        case RA_UPDATE:
            ReadAndDoUpdateAction( inInputStream, networkId );
            break;
        case RA_DESTROY:
            ReadAndDoDestroyAction( inInputStream, networkId );
            break;

        default:
            LOG( Logger::DEBUG, "No Action found for %d", action );
        }
    }
}

// TODO: This should be managed by some sort of packet class
void ReplicationManagerClient::ReadAndDoCreateAction(
    InputMemoryBitStream& inInputStream, int inNetworkId )
{
    uint32_t fourCCName;

    inInputStream.Read( fourCCName );

    GameObjectPtr gameObject =
        NetworkManagerClient::sInstance->GetGameObject( inNetworkId );

    if ( !gameObject )
    {
        gameObject =
            GameObjectRegistry::sInstance->CreateGameObject( fourCCName );

        gameObject->SetNetworkId( inNetworkId );
        NetworkManagerClient::sInstance->AddToNetworkIdToGameObjectMap(
            gameObject );
    }

    gameObject->Read( inInputStream ); // Read state
}

void ReplicationManagerClient::ReadAndDoUpdateAction(
    InputMemoryBitStream& inInputStream, int inNetworkId )
{
    GameObjectPtr gameObject =
        NetworkManagerClient::sInstance->GetGameObject( inNetworkId );

    // Should be good
    gameObject->Read( inInputStream );
}

void ReplicationManagerClient::ReadAndDoDestroyAction(
    InputMemoryBitStream& inInputStream, int inNetworkId )
{
    GameObjectPtr gameObject =
        NetworkManagerClient::sInstance->GetGameObject( inNetworkId );
    if ( gameObject )
    {
        //        gameObject->Die();
        NetworkManagerClient::sInstance->RemoveFromNetworkIdToGameObjectMap(
            gameObject );
    }
}
