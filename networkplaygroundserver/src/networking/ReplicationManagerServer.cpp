#include "gameobjects/GameObject.h"
#include "networking/Logger.h"
#include "networking/NetworkManagerServer.h"
#include "networking/ReplicationManagerTransmissionData.h"

void ReplicationManagerServer::ReplicateCreate(int inNetworkId, uint32_t inInitialDirtyState)
{
    if (inInitialDirtyState == 10)
    {
        DEBUG("DA FUCK");
    }
    if (mNetworkIdToReplicationCommand.find(inNetworkId) == mNetworkIdToReplicationCommand.end())
    {
        DEBUG("Replicating create on new user {}", inNetworkId);
    }
    mNetworkIdToReplicationCommand[inNetworkId] = ReplicationCommand(inInitialDirtyState);
    ;
}

void ReplicationManagerServer::ReplicateDestory(int inNetworkId)
{
    ERROR("Destroy not implemented yet");
    //        mNetworkIdToReplicationCommand[inNetworkId].SetDestroy();
}

void ReplicationManagerServer::RemoveFromReplication(int inNetworkId)
{
    mNetworkIdToReplicationCommand.erase(inNetworkId);
}

void ReplicationManagerServer::HandleCreateAckd(int inNetworkId)
{
    mNetworkIdToReplicationCommand[inNetworkId].HandleCreateAckd();
}

// TODO: I'm not sure I like the replication manager managing dirty state
void ReplicationManagerServer::SetStateDirty(int inNetworkId, uint32_t inDirtyState)
{
    mNetworkIdToReplicationCommand[inNetworkId].AddDirtyState(inDirtyState);
}

/**
 * Writes any pending replication commands to the output stream
 */
void ReplicationManagerServer::Write(OutputMemoryBitStream& inOutputStream,
                                     ReplicationManagerTransmissionData* ioTransmissinData)
{
    DEBUG("Writing shit to {} ids", mNetworkIdToReplicationCommand.size());
    for (auto& pair : mNetworkIdToReplicationCommand)
    {
        // What is the command we are trying to replicate?
        ReplicationCommand& replicationCommmand = pair.second;

        // Only do stuff if the command is "dirty"
        if (!replicationCommmand.HasDirtyState())
        {
            DEBUG("No dirty State for {}", pair.first);
            continue;
        }
        else
        {
            DEBUG("Dirty state for {}", pair.first);
        }

        // Dirty, better write it out to the stream
        int networkId = pair.first;
        // Which object are we writing?
        inOutputStream.Write(networkId);

        ReplicationAction action = replicationCommmand.GetAction();

        // TODO: Take the log of the max ReplicaitonAction Enum to get
        // how many bits to write for the enum
        inOutputStream.Write(action, 2);

        uint32_t writtenState = 0;
        uint32_t dirtyState = replicationCommmand.GetDirtyState();

        switch (action)
        {
        case RA_CREATE:
            DEBUG("Creating a new user {}", networkId);
            writtenState = WriteCreateAction(inOutputStream, networkId, dirtyState);
            break;
        case RA_UPDATE:
            DEBUG("Writing update packet for {}", networkId);
            writtenState = WriteUpdateAction(inOutputStream, networkId, dirtyState);
            break;
        default:
            ERROR("Don't Recognize Action");
            return;
        }
        // TODO: Log in flight packets
        ioTransmissinData->AddTransmission(networkId, action, writtenState);

        // Stop writting that state
        replicationCommmand.ClearDirtyState(writtenState);
    }
}

uint32_t ReplicationManagerServer::WriteCreateAction(OutputMemoryBitStream& inOutputStream,
                                                     int inNetworkId, uint32_t inDirtyState)
{
    GameObjectPtr gameObject = NetworkManagerServer::sInstance->GetGameObject(inNetworkId);

    // write class id
    inOutputStream.Write(gameObject->GetClassId());

    uint32_t writtenState = gameObject->Write(inOutputStream, inDirtyState);

    return writtenState;
}

uint32_t ReplicationManagerServer::WriteUpdateAction(OutputMemoryBitStream& inOutputStream,
                                                     int inNetworkId, uint32_t inDirtyState)
{
    // need object
    GameObjectPtr gameObject = NetworkManagerServer::sInstance->GetGameObject(inNetworkId);

    // TODO: What should happen if the object doesn't exist on the other side?
    // What should be done it we keep sending create packets until we
    // are ack'd the creation by everyone.
    uint32_t writtenState = gameObject->Write(inOutputStream, inDirtyState);

    return writtenState;
}
