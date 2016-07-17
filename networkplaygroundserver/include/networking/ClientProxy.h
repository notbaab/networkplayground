//
//  ClientProxy.h
//  networkplayground
//
//  Created by Erik Parreira on 7/13/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef ClientProxy_h
#define ClientProxy_h

#include <string>

#include "networking/SocketAddress.h"

class ClientProxy
{
  public:
    ClientProxy( const SocketAddress& inSocketAddres, const std::string& inName,
                 int inPlayerId );

    const SocketAddress& GetSocketAddress() const { return mSocketAddress; }

  private:
    SocketAddress mSocketAddress;
    std::string mName;
    int mPlayerId;

    float mLastPacketFromClientTime;

    //    MoveList mUnprocessedMoveList;
};

#endif /* ClientProxy_h */
