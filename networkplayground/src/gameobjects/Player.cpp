#include "gameobjects/Player.h"
#include "gameobjects/PlayerMessage.h"
#include "networking/Logger.h"

Player::Player() : GameObject(), mVelocity( Vector3::Zero ) {}

void Player::ProcessInput( float inDeltaTime, const InputState& inInputState )
{
    mVelocity.mX = (inInputState.GetDesiredHorizontalDelta()  * PLAYER_SPEED);
    // y is reversed
    mVelocity.mY = (inInputState.GetDesiredVerticalDelta() * -1 * PLAYER_SPEED) ;
}

void Player::AdjustVelocityByThrust( float inDeltaTime )
{
    LOG( Logger::ERROR, "Implment" );
}

void Player::SimulateMovement( float inDeltaTime )
{
//    AdjustVelocityByThrust( inDeltaTime );
    // LOG("Moving Player by %.2f and %.2f, from location %.2f, %.2f time %.2f", mVelocity.mX, mVelocity.mY, GetLocation().mX, GetLocation().mY, inDeltaTime);
    SetLocation( GetLocation() + mVelocity * inDeltaTime );
}

void Player::Update() {
//    LOG("At %.2f, %.2f", GetLocation().mX, GetLocation().mY);
}

uint32_t Player::Write( OutputMemoryBitStream& inOutputStream,
                        uint32_t inDirtyState )
{
    PlayerMessage* message = new PlayerMessage();

    // This is pretty wrong, where is everything dirty?
    this->mState = static_cast<PlayerReplicationState>(inDirtyState & ALL_STATE);
    message->Serialize( inOutputStream, this );

    return inDirtyState;
}
