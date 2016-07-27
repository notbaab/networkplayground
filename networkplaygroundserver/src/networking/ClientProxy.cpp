//
//  ClientProxy.cpp
//  networkplayground
//
//  Created by Erik Parreira on 7/13/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include "networking/ClientProxy.h"
#include "timing/Timing.h"

ClientProxy::ClientProxy( const SocketAddress& inSocketAddress,
                          const std::string& inName, int inPlayerId )
    : mSocketAddress( inSocketAddress ), mName( inName ),
      mPlayerId( inPlayerId )
{
    UpdateLastPacketTime();
}

void ClientProxy::UpdateLastPacketTime()
{
    mLastPacketFromClientTime = Timing::sInstance.GetTimef();
}
