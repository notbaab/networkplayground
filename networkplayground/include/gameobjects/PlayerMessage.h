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

    template <typename Stream, typename DataContainer>
    static bool Serialize(Stream& stream, DataContainer dataContainer)
    {
        stream.serialize(dataContainer->mState);
        bool writePlayerId = dataContainer->mState & ReplicationState::PRS_PID;
        bool writePosition = dataContainer->mState & ReplicationState::PRS_POSI;

        if (writePlayerId)
        {
            stream.serialize(dataContainer->mPlayerId);
        }

        if (writePosition)
        {
            TRACE("Writting message with x={}, y={}, mX={}, mY={}", dataContainer->GetLocation().mX,
                  dataContainer->GetLocation().mY, dataContainer->mVelocity.mX,
                  dataContainer->mVelocity.mY);
            stream.serialize(dataContainer->mVelocity.mX);
            stream.serialize(dataContainer->mVelocity.mY);

            stream.serialize(dataContainer->mLocation.mX);
            stream.serialize(dataContainer->mLocation.mY);
        }

        return true;
    }
};
#endif /* PlayerMessage_h */
