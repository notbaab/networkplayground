#include "gameobjects/PlayerClient.h"
#include "gameobjects/Ghost.h"
#include "gameobjects/PlayerMessage.h"
#include "input/InputManager.h"
#include "networking/NetworkManagerClient.h"
#include "networking/Logger.h"
#include "const/consts.h"

PlayerClient::PlayerClient()
    : mTimeLocationBecameOutOfSync( 0.f ), mTimeVelocityBecameOutOfSync( 0.f )
{
    mSpriteComponent.reset( new SpriteComponent( this ) );
    mSpriteComponent->SetColor(new Vector3(255, 255, 0));
    mServerGhost.reset( new Ghost() );
    // add the ghost to the game world?
}

// Update called every frame on the client
void PlayerClient::Update()
{
    if ( !IsCreatedOnServer() )
    {
        Log(Logger::TRACE, "Not created yet");
        return;
    }
    // Player::Update();
    // mServerGhost->PrintInfo();
    const Move* pendingMove =
        InputManager::sInstance->GetAndClearPendingMove();

    if ( !pendingMove )
    {
        return;
    }

    float deltaTime = pendingMove->GetDeltaTime();

    // LOG("Doing move %.2f", deltaTime);
    ProcessInput( TIME_STEP, pendingMove->GetInputState() );

    SimulateMovement( TIME_STEP );
    Player::Update();
    mServerGhost->PrintInfo();
}

// Called everytime we get a packet from the server
void PlayerClient::Read( InputMemoryBitStream& inInputStream )
{
    // If we aren't created on the server, read state directly into player object, else,
    // read into our server ghost
    if(!IsCreatedOnServer())
    {
        PlayerMessage::Serialize( inInputStream, this );
        Log(Logger::TRACE, "Created");
        // HandleStatePacket(inInputStream);
    } else {
        HandleStatePacket(inInputStream);
    }
}

// Read player into a ghost object and interpolate from there
void PlayerClient::HandleStatePacket(InputMemoryBitStream& inInputStream )
{
    Log(Logger::TRACE, "Reading in State");
    PlayerMessage::Serialize( inInputStream, mServerGhost );
}


bool PlayerClient::IsCreatedOnServer()
{
    return GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId();
}