#ifndef PlayerMessage_h
#define PlayerMessage_h

#include "IO/MemoryBitStream.h"
#include "networking/Logger.h"

class PlayerMessage
{
  public:
    enum ReplicationState
    {
        PRS_PID = 1 << 0,  // Player Id
        PRS_POSI = 1 << 1, // Player position and rotation
        ALL_STATE = PRS_POSI | PRS_PID,
    };

    PlayerMessage() {}

    template <typename PlayerLikeObject>
    PlayerMessage(PlayerLikeObject player) {
        state = player->mState;
        id = player->mPlayerId;
        xVel = player->mVelocity.mX;
        yVel = player->mVelocity.mY;
        xLoc = player->mLocation.mX;
        yLoc = player->mLocation.mY;
    }


    template <typename PlayerLikeObject>
    bool copyToPlayer(PlayerLikeObject player) {
        player->mState = state;
        if(hasId) {
            player->mPlayerId = id;
        }
        if (hasPosition) {
            player->mVelocity.mX = xVel;
            player->mVelocity.mY = yVel;
            player->mLocation.mX = xLoc;
            player->mLocation.mY = yLoc;
        }
    }

    ReplicationState state;
    uint32_t id;
    bool hasPosition, hasId;
    float xVel, yVel, xLoc, yLoc;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        stream.serialize(state);
        hasId = state & ReplicationState::PRS_PID;
        hasPosition = state & ReplicationState::PRS_POSI;

        if (hasId)
        {
            stream.serialize(id);
        }

        if (hasPosition)
        {
            // TRACE("Writting message with x={}, y={}, mX={}, mY={}",
            // dataContainer->GetLocation().mX,
            //       dataContainer->GetLocation().mY, dataContainer->mVelocity.mX,
            //       dataContainer->mVelocity.mY);
            stream.serialize(xVel);
            stream.serialize(yVel);
            stream.serialize(xLoc);
            stream.serialize(yLoc);
        }

        return true;
    }
};
#endif /* PlayerMessage_h */
