#include "gameobjects/Player.h"
#include "gameobjects/PlayerMessage.h"
#include "networking/StringUtils.h"

Player::Player() : GameObject(), mVelocity( Vector3::Zero ) {}

void Player::ProcessInput( float inDeltaTime, const InputState& inInputState )
{
    mVelocity.mX += inInputState.GetDesiredHorizontalDelta() * inDeltaTime;
    mVelocity.mY += inInputState.GetDesiredVerticalDelta() * inDeltaTime;
}

void Player::AdjustVelocityByThrust( float inDeltaTime ) { LOG( "Implment" ); }

void Player::SimulateMovement( float inDeltaTime )
{
    AdjustVelocityByThrust( inDeltaTime );
    SetLocation( GetLocation() + mVelocity * inDeltaTime );
}

void Player::Update() {}

uint32_t Player::Write( OutputMemoryBitStream& inOutputStream,
                        uint32_t inDirtyState )
{
    PlayerMessage* message = new PlayerMessage();

    message->SerializeInternal( inOutputStream, this );

    return inDirtyState;
}
