#include "gameobjects/PlayerClient.h"
#include "gameobjects/PlayerMessage.h"
#include "input/InputManager.h"
#include "networking/NetworkManagerClient.h"
#include "networking/StringUtils.h"
#include "const/consts.h"

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
            
            LOG("Doing move %.2f", deltaTime);
            ProcessInput( deltaTime, pendingMove->GetInputState() );

            SimulateMovement( TIME_STEP );
        }
    }
}

void PlayerClient::Read( InputMemoryBitStream& inInputStream )
{

    // Ignore server side code
//    PlayerMessage::SerializeInternal( inInputStream, new PlayerClient() );
    PlayerMessage::SerializeInternal( inInputStream, this );

    // Do interpolation and shiΩΩΩt for it
}
