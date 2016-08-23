//
//  PlayerMessage.h
//  Message sent/read about the player
//
//  Created by Erik Parreira on 8/15/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef PlayerMessage_h
#define PlayerMessage_h
#include "IO/MemoryBitStream.h"
#include "gameobjects/Player.h"

class Player;

class PlayerMessage
{
  public:
    friend class Player;

    template <typename Stream>
    static bool Serialize( Stream& stream, Player* inPlayer )
    {
        stream.serialize( inPlayer->mPlayerId );
        stream.serialize( inPlayer->mVelocity.mX );
        stream.serialize( inPlayer->mVelocity.mY );
        stream.serialize( inPlayer->mVelocity.mZ );

        return true;
    }

    static bool SerializeInternal( OutputMemoryBitStream& stream,
                                   Player* inPlayer )
    {
        Serialize( stream, inPlayer );
        return true;
    }

    static bool SerializeInternal( InputMemoryBitStream& stream,
                                   Player* inPlayer )
    {
        Serialize( stream, inPlayer );
        return true;
    }
};
#endif /* PlayerMessage_h */
