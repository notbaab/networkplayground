//
//  NetworkManagerServer.cpp
//  networkplayground
//
//  Created by Erik Parreira on 7/10/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include <stdio.h>

#include "networking/NetworkManagerServer.h"

NetworkManagerServer* NetworkManagerServer::sInstance;

NetworkManagerServer::NetworkManagerServer()
    : NetworkManager(), mNextPlayerId( 1 ), mNextNetworkId( 1 ),
      mTimeBetweenStatePackets( 0.033f )
{
}

bool NetworkManagerServer::StaticInit( uint16_t inPort )
{
    sInstance = new NetworkManagerServer();

    // Parent does the heavy lifting
    return sInstance->Init( inPort );
}

void NetworkManagerServer::ProcessPacket( InputMemoryBitStream& inStream,
                                          const SocketAddress& inFromAddress )
{
    // read the packet and do stuff
}
