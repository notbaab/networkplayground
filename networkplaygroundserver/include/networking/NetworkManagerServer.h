//
//  NetworkManagerServer.h
//  networkplayground
//
//  Created by Erik Parreira on 7/10/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef NetworkManagerServer_h
#define NetworkManagerServer_h

#include "gameobjects/GameObject.h"
#include "networking/ClientProxy.h"
#include "networking/NetworkManager.h"
#include <unordered_map>

// class ClientProxyPtr;
class InputMemoryBitStream;
class OutputMemoryBitStream;

class NetworkManagerServer : public NetworkManager
{
  public:
    static NetworkManagerServer* sInstance;
    static bool StaticInit( uint16_t inPort );

    virtual void ProcessPacket( InputMemoryBitStream& inStream,
                                const SocketAddress& inFromAddress ) override;

    void SendOutgoingPackets();
    void CheckForDisconnects();

    void RegisterGameObject( GameObjectPtr inGameObjet );
    inline GameObjectPtr RegisterAndReturn( GameObject* inGameObject );
    void UnregisterGameObject( GameObject* inGameObject );

    ClientProxyPtr GetClientProxy( int inPlayerId ) const;

  private:
    // Only have the static constructor
    NetworkManagerServer();

    // Packet Building Routinestypedef shared_ptr<ClientProxy> ClientProxyPtr;
    void HandlePacketFromNewClient( InputMemoryBitStream& inInputStream,
                                    const SocketAddress& inFromAddress );

    void ProcessPacket( ClientProxyPtr inClientProxy,
                        InputMemoryBitStream& inInputStream );

    void SendWelcomePacket( ClientProxyPtr inClientProxy );
    void UpdateAllClients();
    void AddWorldStateToPacket( OutputMemoryBitStream& inOutputStream );
    void SendStatePacketToClient( ClientProxyPtr inClientProxy );
    void WriteLastMoveTimestampIfDirty( OutputMemoryBitStream& inOutputStream,
                                        ClientProxyPtr inClientProxy );

    void HandleInputPacket( ClientProxyPtr inClientProxy,
                            InputMemoryBitStream& inInputStream );

    int GetNewNetworkId();

    int mNextPlayerId;
    int mNextNetworkId;
    float mTimeBetweenStatePackets;

    typedef std::unordered_map<int, ClientProxyPtr> IntToClientMap;
    typedef std::unordered_map<SocketAddress, ClientProxyPtr>
        AddressToClientMap;

    AddressToClientMap mAddressToClientMap;
    IntToClientMap mPlayerIdToClientMap;
};

#endif /* NetworkManagerServer_h */
