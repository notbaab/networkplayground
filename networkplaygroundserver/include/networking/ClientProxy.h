//
//  ClientProxy.h
//  networkplayground
//
//  Created by Erik Parreira on 7/13/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef ClientProxy_h
#define ClientProxy_h

#include "gameobjects/InputState.h"
#include "gameobjects/MoveList.h"
#include "networking/SocketAddress.h"
#include <memory>
#include <string>

// class InputState;

class ClientProxy
{
  public:
    ClientProxy( const SocketAddress& inSocketAddres, const std::string& inName,
                 int inPlayerId );

    const SocketAddress& GetSocketAddress() const { return mSocketAddress; }
    int GetPlayerId() const { return mPlayerId; }
    const std::string& GetName() const { return mName; }

    void SetInputState( const InputState& inInputState )
    {
        mInputState = inInputState;
    }
    const InputState& GetInputState() const { return mInputState; }

    void UpdateLastPacketTime();

    const MoveList& GetUnprocessedMoveList() const
    {
        return mUnprocessedMoveList;
    }
    MoveList& GetUnprocessedMoveList() { return mUnprocessedMoveList; }

    void SetIsLastMoveTimestampDirty( bool inIsDirty )
    {
        mIsLastMoveTimestampDirty = inIsDirty;
    }

    bool IsLastMoveTimestampDirty() const { return mIsLastMoveTimestampDirty; }

  private:
    SocketAddress mSocketAddress;
    InputState mInputState;
    std::string mName;
    int mPlayerId;

    float mLastPacketFromClientTime;

    MoveList mUnprocessedMoveList;
    bool mIsLastMoveTimestampDirty;
};

#endif /* ClientProxy_h */
typedef std::shared_ptr<ClientProxy> ClientProxyPtr;
