//
//  NetworkManager.cpp
//  networkplayground
//
//  Created by Erik Parreira on 2/29/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include <networking/Networking.h>

bool NetworkManager::Init( uint16_t inPort )
{
    mSocket = SocketUtil::CreateUDPSocket( INET );
    SocketAddress ownAddress( INADDR_ANY, inPort );
    mSocket->Bind( ownAddress );
    
    LOG( "Initializing NetworkManager at port %d", inPort );
    
//    mBytesReceivedPerSecond = WeightedTimedMovingAverage( 1.f );
//    mBytesSentPerSecond = WeightedTimedMovingAverage( 1.f );
    
    //did we bind okay?
    if( mSocket == nullptr )
    {
        return false;
    }
    
    if( mSocket->SetNonBlockingMode( true ) != NO_ERROR )
    {
        return false;
    }
    
    return true;
}
