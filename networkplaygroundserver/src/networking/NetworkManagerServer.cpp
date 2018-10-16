#include "networking/NetworkManagerServer.h"
#include "gameobjects/GameObject.h"
#include "gameobjects/Move.h"
#include "networking/Logger.h"
#include "networking/ReplicationManagerTransmissionData.h"
#include "networking/Server.h"
#include "timing/Timing.h"

NetworkManagerServer* NetworkManagerServer::sInstance;

NetworkManagerServer::NetworkManagerServer()
    : NetworkManager(), mNextPlayerId(1), mNextNetworkId(1), mTimeBetweenStatePackets(0.033f),
      mClientDisconnectTimeout(3.f), mDontAllowClientDisconects(true)
{
}

bool NetworkManagerServer::StaticInit(uint16_t inPort)
{
    sInstance = new NetworkManagerServer();

    // Parent does the heavy lifting
    return sInstance->Init(inPort);
}

// void NetworkManagerServer::HandleConnectionReset( const SocketAddress&
// inFromAddress )
//{
//    //just dc the client right away...
//    auto it = mAddressToClientMap.find( inFromAddress );
//    if( it != mAddressToClientMap.end() )
//    {
//        HandleClientDisconnected( it->second );
//    }
//}

void NetworkManagerServer::ProcessPacket(InputMemoryBitStream& inStream,
                                         const SocketAddress& inFromAddress)
{
    // read the packet and do stuff
    auto it = mAddressToClientMap.find(inFromAddress);

    if (it == mAddressToClientMap.end())
    {
        TRACE("New client");
        HandlePacketFromNewClient(inStream, inFromAddress);
    }
    else
    {
        TRACE("Processing packet");
        ProcessPacket((*it).second, inStream);
    }
}

void NetworkManagerServer::ProcessPacket(ClientProxyPtr inClientProxy,
                                         InputMemoryBitStream& inInputStream)
{
    inClientProxy->UpdateLastPacketTime();

    uint8_t packetType;

    inInputStream.Read(packetType);

    switch (packetType)
    {
    case kHelloCC:
        DEBUG("Sending welcome packet");
        SendWelcomePacket(inClientProxy);
        break;

    case kInputCC:
        // Reads the state and adds it to the pending packets to be acked
        if (inClientProxy->GetDeliveryNotificationManager().ReadAndProcessState(inInputStream))
        {
            HandleInputPacket(inClientProxy, inInputStream);
        }

    default:
        break;
    }
}

bool proccessMovePacket(ClientProxyPtr inClientProxy, Move move,
                        InputMemoryBitStream& inInputStream)
{
    bool out = false;

    if (!move.Read(inInputStream))
    {
        return out;
    }

    out = inClientProxy->GetUnprocessedMoveList().AddMoveIfNew(move);
    if (out)
    {
        inClientProxy->SetIsLastMoveTimestampDirty(true);
    }

    return out;
}

void NetworkManagerServer::HandleInputPacket(ClientProxyPtr inClientProxy,
                                             InputMemoryBitStream& inInputStream)
{
    uint32_t moveCount = 0;
    Move move;
    // TODO: Have a move packet so I don't need to change the bit count
    // everywhere
    inInputStream.Read(moveCount, 8);
    TRACE("Processing {} Moves", moveCount);
    for (; moveCount > 0; --moveCount)
    {
        proccessMovePacket(inClientProxy, move, inInputStream);
    }
}

void NetworkManagerServer::HandlePacketFromNewClient(InputMemoryBitStream& inInputStream,
                                                     const SocketAddress& inFromAddress)
{
    uint8_t packetType;
    DEBUG("Reading new packet");
    inInputStream.Read(packetType);
    if (packetType == kHelloCC)
    {
        DEBUG("Sending hello packet");
        HandleHelloPacket(inInputStream, inFromAddress);
    }
    else
    {
        CRITICAL("Bad packet from client {}", inFromAddress.ToString().c_str());
    }
}

void NetworkManagerServer::HandleHelloPacket(InputMemoryBitStream& inInputStream,
                                             const SocketAddress& inFromAddress)
{
    // Do stuff
    std::string name;
    inInputStream.Read(name);
    ClientProxyPtr newClientProxy =
        std::make_shared<ClientProxy>(inFromAddress, name, GetNewPlayerId());

    mAddressToClientMap[inFromAddress] = newClientProxy;
    mPlayerIdToClientMap[newClientProxy->GetPlayerId()] = newClientProxy;

    // Book Says this is a terrible way to do this. I agree
    static_cast<Server*>(Engine::sInstance.get())->HandleNewClient(newClientProxy);

    SendWelcomePacket(newClientProxy);

    INFO("Sending world state to player {} for {} objects", newClientProxy->GetPlayerId(),
         mNetworkIdToGameObjectMap.size());
    for (const auto& pair : mNetworkIdToGameObjectMap)
    {
        newClientProxy->GetReplicationManagerServer().ReplicateCreate(
            pair.first, pair.second->GetAllStateMask());
    }
}

/**
 * Send a welcome packet to a client. Note, this will send it immediatly, it
 * will not queue up to be sent with the server tick.
 */
void NetworkManagerServer::SendWelcomePacket(ClientProxyPtr inClientProxy)
{
    OutputMemoryBitStream welcomePacket;

    welcomePacket.Write(kWelcomeCC);
    welcomePacket.Write((inClientProxy->GetPlayerId()));

    INFO("Welcomed new client '{}' as player {}", inClientProxy->GetName().c_str(),
         inClientProxy->GetPlayerId());

    SendPacket(welcomePacket, inClientProxy->GetSocketAddress());
    // sends the first location of the user to the client
    // Technically this should be handled by sending replicate create?
    SendStatePacketToClient(inClientProxy);
}

void NetworkManagerServer::SendOutgoingPackets()
{
    for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
    {
        ClientProxyPtr clientProxy = it->second;

        clientProxy->GetDeliveryNotificationManager().ProcessTimedOutPackets();

        if (clientProxy->IsLastMoveTimestampDirty())
        {
            SendStatePacketToClient(clientProxy);
        }
    }
}

void NetworkManagerServer::UpdateAllClients() {}

// The thing that actually sends the world state to the clients
void NetworkManagerServer::SendStatePacketToClient(ClientProxyPtr inClientProxy)
{
    TRACE("Sending State");
    OutputMemoryBitStream statePacket;

    //    auto replicationServer = inClientProxy->GetReplicationManagerServer();

    // Mark packet as a state packet
    statePacket.Write(kStateCC);

    // Write the current packet state i.e. add sequence number and ack any
    // packets that haven't been ack'd
    InFlightPacket* ifp = inClientProxy->GetDeliveryNotificationManager().WriteState(statePacket);

    // Tell the client which move we last saw from them
    WriteLastMoveTimestampIfDirty(statePacket, inClientProxy);

    ReplicationManagerTransmissionData* rmtd =
        new ReplicationManagerTransmissionData(&inClientProxy->GetReplicationManagerServer());

    // Write anything that we need to replicate to this client!
    inClientProxy->GetReplicationManagerServer().Write(statePacket, rmtd);

    // Record the transmission
    ifp->SetTransmissionData(ReplicationManagerTransmissionData::kReplicationTransmissionData,
                             TransmissionDataPtr(rmtd));

    // SHIP IT!
    SendPacket(statePacket, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream,
                                                         ClientProxyPtr inClientProxy)
{
    bool isTimestampDirty = inClientProxy->IsLastMoveTimestampDirty();
    inOutputStream.Write(isTimestampDirty);

    // Check the unprocessed moves because we got them we just haven't
    // processed them
    // TODO: Why?
    if (isTimestampDirty)
    {
        inOutputStream.Write(inClientProxy->GetUnprocessedMoveList().GetLastMoveTimestamp());
        inClientProxy->SetIsLastMoveTimestampDirty(false);
    }
}

ClientProxyPtr NetworkManagerServer::GetClientProxy(int inPlayerId) const
{
    auto it = mPlayerIdToClientMap.find(inPlayerId);
    if (it != mPlayerIdToClientMap.end())
    {
        return it->second;
    }

    return nullptr;
}

void NetworkManagerServer::CheckForDisconnects()
{
    if (mDontAllowClientDisconects)
    {
        return; // don't do anything
    }

    std::vector<ClientProxyPtr> clientsToDC;

    float minAllowedLastPacketFromClientTime =
        Timing::sInstance.GetTimef() - mClientDisconnectTimeout;
    for (const auto& pair : mAddressToClientMap)
    {
        if (pair.second->GetLastPacketFromClientTime() < minAllowedLastPacketFromClientTime)
        {
            // can't remove from map while in iterator, so just remember for
            // later...
            clientsToDC.push_back(pair.second);
        }
    }

    for (ClientProxyPtr client : clientsToDC)
    {
        HandleClientDisconnected(client);
    }
}

void NetworkManagerServer::HandleClientDisconnected(ClientProxyPtr inClientProxy)
{
    mPlayerIdToClientMap.erase(inClientProxy->GetPlayerId());
    mAddressToClientMap.erase(inClientProxy->GetSocketAddress());
    static_cast<Server*>(Engine::sInstance.get())->HandleLostClient(inClientProxy);

    // was that the last client? if so, bye!
    if (mAddressToClientMap.empty())
    {
        //        Engine::sInstance->SetShouldKeepRunning( false );
    }
}

void NetworkManagerServer::RegisterGameObject(GameObjectPtr inGameObject)
{
    // assign network id
    int newNetworkId = GetNewNetworkId();
    inGameObject->SetNetworkId(newNetworkId);

    // add mapping from network id to game object
    mNetworkIdToGameObjectMap[newNetworkId] = inGameObject;
    TRACE("Registering Game Object {}", newNetworkId);

    // tell all client proxies this is new...
    INFO("Replicating to {} clients", mAddressToClientMap.size());
    for (const auto& pair : mAddressToClientMap)
    {
        DEBUG("Sending {} to {}", pair.second->GetPlayerId(),
              pair.second->GetSocketAddress().ToString());
        pair.second->GetReplicationManagerServer().ReplicateCreate(newNetworkId,
                                                                   inGameObject->GetAllStateMask());
    }
}

void NetworkManagerServer::UnregisterGameObject(GameObject* inGameObject)
{
    int networkId = inGameObject->GetNetworkId();
    mNetworkIdToGameObjectMap.erase(networkId);

    // tell all client proxies to STOP replicating!
    // tell all client proxies this is new...
    //    for( const auto& pair: mAddressToClientMap )
    //    {
    //        pair.second->GetReplicationManagerServer().ReplicateDestroy(
    //        networkId );
    //    }
}

void NetworkManagerServer::SetStateDirty(int inNetworkId, uint32_t inDirtyState)
{
    // tell everybody this is dirty
    for (const auto& pair : mAddressToClientMap)
    {
        pair.second->GetReplicationManagerServer().SetStateDirty(inNetworkId, inDirtyState);
    }
}

int NetworkManagerServer::GetNewPlayerId() { return mNextPlayerId++; }
int NetworkManagerServer::GetNewNetworkId() { return mNextNetworkId++; }
