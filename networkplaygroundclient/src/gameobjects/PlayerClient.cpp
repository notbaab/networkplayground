#include "gameobjects/PlayerClient.h"
#include "gameobjects/PlayerMessage.h"
#include "input/InputManager.h"
#include "networking/NetworkManagerClient.h"
#include "networking/StringUtils.h"

PlayerClient::PlayerClient()
    : mTimeLocationBecameOutOfSync( 0.f ), mTimeVelocityBecameOutOfSync( 0.f )
{
    mSpriteComponent.reset( new SpriteComponent( this ) );
}

void PlayerClient::Update()
{
    if ( GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() )
    {
        const Move* pendingMove =
            InputManager::sInstance->GetAndClearPendingMove();

        if ( pendingMove )
        {
            float deltaTime = pendingMove->GetDeltaTime();
            
            LOG("Doing move %d", deltaTime);
            ProcessInput( deltaTime, pendingMove->GetInputState() );

            SimulateMovement( deltaTime );
        }
    }
}

void PlayerClient::Read( InputMemoryBitStream& inInputStream )
{
    PlayerMessage::SerializeInternal( inInputStream, this );

    // Do interpolation and shit for it
}
