//
//  NetworkManagerServer.h
//  networkplayground
//
//  Created by Erik Parreira on 7/10/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef NetworkManagerServer_h
#define NetworkManagerServer_h

#include "networking/NetworkManager.h"
#include "gameobjects/GameObject.h"

class NetworkManagerServer : public NetworkManager
{
public:
    static NetworkManagerServer* sInstance;
    static bool StaticInit( uint16_t inPort );
    
    virtual void ProcessPacket( InputMemoryBitStream& inStream,
                                const SocketAddress& inFromAddress ) override;
    
    
    void SendOutGoingPackets();
    
    void RegisterGameObject( GameObjectPtr inGameObjet );
    inline GameObject* RegisterAndReturn( GameObject* inGameObject );
    void UnregisterGameObject( GameObject* inGameObject );
    
//    ClientProxyPtr GetClientProxy( int inPlayerId ) const;
    
private:
    NetworkManagerServer();
    
    void HandlePacketFromNewClient( InputMemoryBitStream& inInputStream,
                                    const SocketAddress& inFromAddress );
    
//    void ProcessPacket( ClientProxyPtr inClientProxy,
//                        InputMemoryBitStream&  inInputStream );
//    
    
    int mNextPlayerId;
    int mNextNetworkId;
    float mTimeBetweenStatePackets;

};

#endif /* NetworkManagerServer_h */
