#include "gameobjects/InputState.h"
#include "networking/Logger.h"
#include "packets/InputStatePacket.h"

bool InputState::Write(OutputMemoryBitStream& inOutputStream) const
{
    auto packet = InputStatePacket(GetDesiredHorizontalDelta(), GetDesiredVerticalDelta());
    packet.Serialize(inOutputStream);

    return false;
}

bool InputState::Read(InputMemoryBitStream& inInputStream)
{
    auto packet = InputStatePacket();
    // packet.Serialize(inInputStream);
    packet.Serialize(inInputStream);
    TRACE("{} and {}", mRightAmount, mForwardAmount);
    mRightAmount = packet.getXDelta();
    mForwardAmount = packet.getYDelta();
    TRACE("{} and {}", mRightAmount, mForwardAmount);

    return true;
}

bool InputState::HasInput() { return GetDesiredVerticalDelta() || GetDesiredHorizontalDelta(); }
