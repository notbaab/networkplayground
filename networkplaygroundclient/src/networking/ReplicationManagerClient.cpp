#include "IO/MemoryBitStream.h"
#include "gameobjects/GameObjectRegistry.h"
#include "networking/Logger.h"
#include "networking/NetworkManagerClient.h"
#include "networking/ReplicationCommand.h"

void ReplicationManagerClient::Read(InputMemoryBitStream& inInputStream)
{

    uint8_t statePackRead = 0;
    while (inInputStream.GetRemainingBitCount() >= 32)
    {
        statePackRead++;
        int networkId;
        inInputStream.Read(networkId);

        // 2 bits for action
        uint8_t action;
        inInputStream.Read(action, 2);

        switch (action)
        {
        case RA_CREATE:
            DEBUG("Creating {}", networkId);
            ReadAndDoCreateAction(inInputStream, networkId);
            break;
        case RA_UPDATE:
            DEBUG("Updating {}", networkId);
            ReadAndDoUpdateAction(inInputStream, networkId);
            break;
        case RA_DESTROY:
            ReadAndDoDestroyAction(inInputStream, networkId);
            break;
        default:
            DEBUG("No Action found for {}", action);
        }
    }

    DEBUG("Read {} state packets", statePackRead);
}

// TODO: This should be managed by some sort of packet class
void ReplicationManagerClient::ReadAndDoCreateAction(InputMemoryBitStream& inInputStream,
                                                     int inNetworkId)
{
    uint32_t fourCCName;

    inInputStream.Read(fourCCName);

    GameObjectPtr gameObject = NetworkManagerClient::sInstance->GetGameObject(inNetworkId);

    if (!gameObject)
    {
        gameObject = GameObjectRegistry::sInstance->CreateGameObject(fourCCName);

        gameObject->SetNetworkId(inNetworkId);
        NetworkManagerClient::sInstance->AddToNetworkIdToGameObjectMap(gameObject);
    }

    gameObject->Read(inInputStream); // Read state
}

void ReplicationManagerClient::ReadAndDoUpdateAction(InputMemoryBitStream& inInputStream,
                                                     int inNetworkId)
{
    GameObjectPtr gameObject = NetworkManagerClient::sInstance->GetGameObject(inNetworkId);

    // Should be good
    gameObject->Read(inInputStream);
}

void ReplicationManagerClient::ReadAndDoDestroyAction(InputMemoryBitStream& inInputStream,
                                                      int inNetworkId)
{
    GameObjectPtr gameObject = NetworkManagerClient::sInstance->GetGameObject(inNetworkId);
    if (gameObject)
    {
        //        gameObject->Die();
        NetworkManagerClient::sInstance->RemoveFromNetworkIdToGameObjectMap(gameObject);
    }
}
