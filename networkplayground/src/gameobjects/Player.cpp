#include "gameobjects/Player.h"
#include "gameobjects/PlayerMessage.h"
#include "networking/Logger.h"

Player::Player() : GameObject(), mVelocity( Vector3::Zero ) {}

// TODO: Remove Delta time
void Player::ProcessInput( float inDeltaTime, const InputState& inInputState )
{
    TRACE("Processing input for move {}, {}, located at {}, {}, ",
        inInputState.GetDesiredHorizontalDelta(),
        inInputState.GetDesiredVerticalDelta(),
        GetLocation().mX, GetLocation().mY);
    mVelocity.mX = (inInputState.GetDesiredHorizontalDelta()  * PLAYER_SPEED);
    // y is reversed
    mVelocity.mY = (inInputState.GetDesiredVerticalDelta() * -1 * PLAYER_SPEED) ;
}

void Player::AdjustVelocityByThrust( float inDeltaTime )
{
    ERROR("Implement" );
}

void Player::SimulateMovement( float inDeltaTime )
{
//    AdjustVelocityByThrust( inDeltaTime );
    // TRACE("Moving Player {} by {} and {}, from location {}, {}", GetPlayerId(), mVelocity.mX, mVelocity.mY, GetLocation().mX, GetLocation().mY);
    SetLocation( GetLocation() + mVelocity * inDeltaTime );
    // TRACE("Moved to location {}, {}", GetLocation().mX, GetLocation().mY);
}

void Player::Update() {
}

uint32_t Player::Write( OutputMemoryBitStream& inOutputStream,
                        uint32_t inDirtyState )
{
    TRACE("Writing State");
    PlayerMessage* message = new PlayerMessage();

    // This is pretty wrong, where is everything dirty?
    this->mState = static_cast<PlayerReplicationState>(inDirtyState & ALL_STATE);
    message->Serialize( inOutputStream, this );

    return inDirtyState;
}
