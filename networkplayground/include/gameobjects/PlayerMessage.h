//  PlayerMessage.h
//  Message sent/read about the player.
//
//  Created by Erik Parreira on 8/15/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef PlayerMessage_h
#define PlayerMessage_h
#include "IO/MemoryBitStream.h"
#include "gameobjects/Player.h"
#include "networking/Logger.h"

class Player;

class PlayerMessage
{
  public:
    friend class Player;

    template <typename Stream, typename DataContainer>
    static bool Serialize( Stream& stream, DataContainer dataContainer )
    {
        stream.serialize( dataContainer->mState );
        bool writePlayerId = dataContainer->mState & Player::PlayerReplicationState::PRS_PID;
        bool writePosition = dataContainer->mState & Player::PlayerReplicationState::PRS_POSI;

        if(writePlayerId)
        {
            stream.serialize(dataContainer->mPlayerId);
        }

        if(writePosition)
        {
            TRACE("Writting message with x={}, y={}, mX={}, mY={}",
                dataContainer->GetLocation().mX,
                dataContainer->GetLocation().mY,
                dataContainer->mVelocity.mX,
                dataContainer->mVelocity.mY);
            stream.serialize( dataContainer->mVelocity.mX );
            stream.serialize( dataContainer->mVelocity.mY );

            stream.serialize( dataContainer->mLocation.mX );
            stream.serialize( dataContainer->mLocation.mY );
        }

        return true;
    }
};
#endif /* PlayerMessage_h */
