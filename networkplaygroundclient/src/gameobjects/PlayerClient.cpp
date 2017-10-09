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
        Log( Logger::TRACE, "Not created yet" );
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

    // If we aren't created on the server, read state directly into player
    // object, else, read into our server ghost
    if ( !IsCreatedOnServer() )
    {
        PlayerMessage::Serialize( inInputStream, this );
        Log( Logger::TRACE, "Created" );
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
        LOG( Logger::INFO, "old location %.f, %.f new %.f, %.f", oldLocation.mX,
             oldLocation.mY, mLocation.mX, mLocation.mY );
    }
}

// Read player into a ghost object and interpolate from there
void PlayerClient::HandleStatePacket( InputMemoryBitStream& inInputStream )
{
    Log( Logger::TRACE, "Reading in State" );
    // PlayerMessage::Serialize( inInputStream, mServerGhost );
    PlayerMessage::Serialize( inInputStream, this );
    mServerGhost->mLocation.mX = mLocation.mX;
    mServerGhost->mLocation.mY = mLocation.mY;
}

void PlayerClient::ApplyUnAckedMoves( uint32_t inReadState )
{
    if ( ( inReadState & PRS_POSI ) == 0 )
    {
        LOG( Logger::INFO, "Nope" );
        return;
    }

    const MoveList& moveList = InputManager::sInstance->GetMoveList();

    for ( auto& move : moveList )
    {
        LOG( Logger::INFO, "Processing move unacked by server" );
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
