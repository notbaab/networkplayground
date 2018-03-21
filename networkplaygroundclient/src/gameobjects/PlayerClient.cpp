#include "gameobjects/PlayerClient.h"
#include "const/consts.h"
#include "gameobjects/Ghost.h"
#include "gameobjects/PlayerMessage.h"
#include "input/InputManager.h"
#include "networking/Logger.h"
#include "networking/NetworkManagerClient.h"

PlayerClient::PlayerClient()
    : mTimeLocationBecameOutOfSync( 0.f ), mTimeVelocityBecameOutOfSync( 0.f )
{
    mSpriteComponent.reset( new SpriteComponent( this ) );
    mSpriteComponent->SetColor( new Vector3( 255, 255, 0 ) );
    mServerGhost.reset( new Ghost() );
    // add the ghost to the game world?
}

// Update called every frame on the client
void PlayerClient::Update()
{
    if ( !IsCreatedOnServer() )
    {
        TRACE("{} Not Local Player", GetPlayerId() );
        return;
    }
    // Player::Update();
    // mServerGhost->PrintInfo();
    const Move* pendingMove = InputManager::sInstance->GetAndClearPendingMove();

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

    TRACE("Read in state");
    // If we aren't created on the server, read state directly into player
    // object, else, read into our server ghost
    if ( !IsCreatedOnServer() )
    {
        PlayerMessage::Serialize( inInputStream, this );
        TRACE("Not a local player, moved to {}-{}", mLocation.mX, mLocation.mY );
        // HandleStatePacket(inInputStream);
    }
    else
    {
        if ( GetPlayerId() != NetworkManagerClient::sInstance->GetPlayerId() )
        {
            return;
        }

        auto oldLocation = mLocation;
        HandleStatePacket( inInputStream );

        // TODO: ....hmmmmm
        ApplyUnAckedMoves( 111 );
        TRACE("old location {}, {} new {}, {}", oldLocation.mX,
             oldLocation.mY, mLocation.mX, mLocation.mY );
    }
}

// Read player into a ghost object and interpolate from there
void PlayerClient::HandleStatePacket( InputMemoryBitStream& inInputStream )
{
    TRACE("Reading in State" );
    PlayerMessage::Serialize( inInputStream, this );
    mServerGhost->mLocation.mX = mLocation.mX;
    mServerGhost->mLocation.mY = mLocation.mY;
}

void PlayerClient::ApplyUnAckedMoves( uint32_t inReadState )
{
    if ( ( inReadState & PRS_POSI ) == 0 )
    {
        INFO("No Move came from the server, don't do anything?" );
        return;
    }

    const MoveList& moveList = InputManager::sInstance->GetMoveList();
    TRACE("Moving");
    for ( auto& move : moveList )
    {
        TRACE("Processing move unacked by server" );
        // float deltaTime = move.GetDeltaTime();
        ProcessInput( TIME_STEP, move.GetInputState() );
        SimulateMovement( TIME_STEP );
    }

    return;
}

bool PlayerClient::IsCreatedOnServer()
{
    return GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId();
}
