
#include <RoboCatClientPCH.h>

#if _WIN32
int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    
    if( Client::StaticInit() )
    {
        return Client::sInstance->Run();
    }
    else
    {
        SDL_Quit();
        //error
        return 1;
    }
    
}
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv)
{
    __argc = argc;
    __argv = argv;
    
    if( Client::StaticInit() )
    {
        return Client::sInstance->Run();
    }
    else
    {
        SDL_Quit();
        //error
        return 1;
    }
}
#endif

class Client : public Engine
{
public:
    
    static bool StaticInit( );
    
protected:
    
    Client();
    
    virtual void	DoFrame() override;
    virtual void	HandleEvent( SDL_Event* inEvent ) override;
    
private:
    
    
    
};


#include <RoboCatClientPCH.h>

bool Client::StaticInit() {
    // Create the Client pointer first because it initializes SDL
    Client *client = new Client();
    
    if (WindowManager::StaticInit() == false) {
        return false;
    }
    
    if (GraphicsDriver::StaticInit(WindowManager::sInstance->GetMainWindow()) ==
        false) {
        return false;
    }
    
    TextureManager::StaticInit();
    RenderManager::StaticInit();
    InputManager::StaticInit();
    
    HUD::StaticInit();
    
    sInstance.reset(client);
    
    return true;
}

Client::Client() {
    GameObjectRegistry::sInstance->RegisterCreationFunction(
                                                            'RCAT', RoboCatClient::StaticCreate);
    GameObjectRegistry::sInstance->RegisterCreationFunction(
                                                            'MOUS', MouseClient::StaticCreate);
    GameObjectRegistry::sInstance->RegisterCreationFunction(
                                                            'YARN', YarnClient::StaticCreate);
    
    //	string destination = StringUtils::GetCommandLineArg( 1 );
    string destination = "127.0.0.1:45000";
    string name = StringUtils::GetCommandLineArg(2);
    
    SocketAddressPtr serverAddress =
    SocketAddressFactory::CreateIPv4FromString(destination);
    
    NetworkManagerClient::StaticInit(*serverAddress, "DUDUD");
    
    // NetworkManagerClient::sInstance->SetDropPacketChance( 0.6f );
    // NetworkManagerClient::sInstance->SetSimulatedLatency( 0.25f );
    // NetworkManagerClient::sInstance->SetSimulatedLatency( 0.5f );
    // NetworkManagerClient::sInstance->SetSimulatedLatency( 0.1f );
}

void Client::DoFrame() {
    InputManager::sInstance->Update();
    
    Engine::DoFrame();
    
    NetworkManagerClient::sInstance->ProcessIncomingPackets();
    
    RenderManager::sInstance->Render();
    
    NetworkManagerClient::sInstance->SendOutgoingPackets();
}

void Client::HandleEvent(SDL_Event *inEvent) {
    switch (inEvent->type) {
        case SDL_KEYDOWN:
            InputManager::sInstance->HandleInput(EIA_Pressed, inEvent->key.keysym.sym);
            break;
        case SDL_KEYUP:
            InputManager::sInstance->HandleInput(EIA_Released, inEvent->key.keysym.sym);
            break;
        default:
            break;
    }
}

#include <RoboCatPCH.h>
#include <SDL.h>
#include <iostream>
#include <time.h>

std::unique_ptr<Engine> Engine::sInstance;

Engine::Engine() : mShouldKeepRunning(true) {
    SocketUtil::StaticInit();
    
    srand(static_cast<uint32_t>(time(nullptr)));
    
    GameObjectRegistry::StaticInit();
    
    World::StaticInit();
    
    ScoreBoardManager::StaticInit();
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}

Engine::~Engine() {
    SocketUtil::CleanUp();
    
    SDL_Quit();
}

int Engine::Run() { return DoRunLoop(); }

void Engine::HandleEvent(SDL_Event *inEvent) {
    // Default implementation does nothing, up to derived classes to handle them,
    // if they so choose
    (void)inEvent;
}

int Engine::DoRunLoop() {
    // Main message loop
    bool quit = false;
    SDL_Event event;
    memset(&event, 0, sizeof(SDL_Event));
    
    while (!quit && mShouldKeepRunning) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else {
                HandleEvent(&event);
            }
        } else {
            Timing::sInstance.Update();
            std::cout << Timing::sInstance.GetTime() << std::endl;
            
            DoFrame();
        }
    }
    
    return event.type;
}

void Engine::DoFrame() { World::sInstance->Update(); }



#include <RoboCatPCH.h>

// zoom hardcoded at 100...if we want to lock players on screen, this could be
// calculated from zoom
const float HALF_WORLD_HEIGHT = 3.6f;
const float HALF_WORLD_WIDTH = 6.4f;

RoboCat::RoboCat()
: GameObject(), mMaxRotationSpeed(5.f), mMaxLinearSpeed(50.f),
mVelocity(Vector3::Zero), mWallRestitution(0.1f), mCatRestitution(0.1f),
mThrustDir(0.f), mPlayerId(0), mIsShooting(false), mHealth(10) {
    SetCollisionRadius(0.5f);
}

void RoboCat::ProcessInput(float inDeltaTime, const InputState &inInputState) {
    // process our input....
    
    // turning...
    float newRotation = GetRotation() +
    inInputState.GetDesiredHorizontalDelta() *
    mMaxRotationSpeed * inDeltaTime;
    SetRotation(newRotation);
    
    // moving...
    float inputForwardDelta = inInputState.GetDesiredVerticalDelta();
    mThrustDir = inputForwardDelta;
    
    mIsShooting = inInputState.IsShooting();
}

void RoboCat::AdjustVelocityByThrust(float inDeltaTime) {
    // just set the velocity based on the thrust direction -- no thrust will lead
    // to 0 velocity
    // simulating acceleration makes the client prediction a bit more complex
    Vector3 forwardVector = GetForwardVector();
    mVelocity = forwardVector * (mThrustDir * inDeltaTime * mMaxLinearSpeed);
}

void RoboCat::SimulateMovement(float inDeltaTime) {
    // simulate us...
    AdjustVelocityByThrust(inDeltaTime);
    
    if (mVelocity.mX != 0 || mVelocity.mY != 0) {
        SetLocation(GetLocation() + mVelocity * inDeltaTime);
    } else {
        SetLocation(GetLocation() + mVelocity * inDeltaTime);
    }
    
    ProcessCollisions();
}

void RoboCat::Update() {}

void RoboCat::ProcessCollisions() {
    // right now just bounce off the sides..
    ProcessCollisionsWithScreenWalls();
    
    float sourceRadius = GetCollisionRadius();
    Vector3 sourceLocation = GetLocation();
    
    // now let's iterate through the world and see what we hit...
    // note: since there's a small number of objects in our game, this is fine.
    // but in a real game, brute-force checking collisions against every other
    // object is not efficient.
    // it would be preferable to use a quad tree or some other structure to
    // minimize the
    // number of collisions that need to be tested.
    for (auto goIt = World::sInstance->GetGameObjects().begin(),
         end = World::sInstance->GetGameObjects().end();
         goIt != end; ++goIt) {
        GameObject *target = goIt->get();
        if (target != this && !target->DoesWantToDie()) {
            // simple collision test for spheres- are the radii summed less than the
            // distance?
            Vector3 targetLocation = target->GetLocation();
            float targetRadius = target->GetCollisionRadius();
            
            Vector3 delta = targetLocation - sourceLocation;
            float distSq = delta.LengthSq2D();
            float collisionDist = (sourceRadius + targetRadius);
            if (distSq < (collisionDist * collisionDist)) {
                // first, tell the other guy there was a collision with a cat, so it can
                // do something...
                
                if (target->HandleCollisionWithCat(this)) {
                    // okay, you hit something!
                    // so, project your location far enough that you're not colliding
                    Vector3 dirToTarget = delta;
                    dirToTarget.Normalize2D();
                    Vector3 acceptableDeltaFromSourceToTarget =
                    dirToTarget * collisionDist;
                    // important note- we only move this cat. the other cat can take care
                    // of moving itself
                    SetLocation(targetLocation - acceptableDeltaFromSourceToTarget);
                    
                    Vector3 relVel = mVelocity;
                    
                    // if other object is a cat, it might have velocity, so there might be
                    // relative velocity...
                    RoboCat *targetCat = target->GetAsCat();
                    if (targetCat) {
                        relVel -= targetCat->mVelocity;
                    }
                    
                    // got vel with dir between objects to figure out if they're moving
                    // towards each other
                    // and if so, the magnitude of the impulse ( since they're both just
                    // balls )
                    float relVelDotDir = Dot2D(relVel, dirToTarget);
                    
                    if (relVelDotDir > 0.f) {
                        Vector3 impulse = relVelDotDir * dirToTarget;
                        
                        if (targetCat) {
                            mVelocity -= impulse;
                            mVelocity *= mCatRestitution;
                        } else {
                            mVelocity -= impulse * 2.f;
                            mVelocity *= mWallRestitution;
                        }
                    }
                }
            }
        }
    }
}

void RoboCat::ProcessCollisionsWithScreenWalls() {
    Vector3 location = GetLocation();
    float x = location.mX;
    float y = location.mY;
    
    float vx = mVelocity.mX;
    float vy = mVelocity.mY;
    
    float radius = GetCollisionRadius();
    
    // if the cat collides against a wall, the quick solution is to push it off
    if ((y + radius) >= HALF_WORLD_HEIGHT && vy > 0) {
        mVelocity.mY = -vy * mWallRestitution;
        location.mY = HALF_WORLD_HEIGHT - radius;
        SetLocation(location);
    } else if (y <= (-HALF_WORLD_HEIGHT - radius) && vy < 0) {
        mVelocity.mY = -vy * mWallRestitution;
        location.mY = -HALF_WORLD_HEIGHT - radius;
        SetLocation(location);
    }
    
    if ((x + radius) >= HALF_WORLD_WIDTH && vx > 0) {
        mVelocity.mX = -vx * mWallRestitution;
        location.mX = HALF_WORLD_WIDTH - radius;
        SetLocation(location);
    } else if (x <= (-HALF_WORLD_WIDTH - radius) && vx < 0) {
        mVelocity.mX = -vx * mWallRestitution;
        location.mX = -HALF_WORLD_WIDTH - radius;
        SetLocation(location);
    }
}

uint32_t RoboCat::Write(OutputMemoryBitStream &inOutputStream,
                        uint32_t inDirtyState) const {
    uint32_t writtenState = 0;
    
    if (inDirtyState & ECRS_PlayerId) {
        inOutputStream.Write((bool)true);
        inOutputStream.Write(GetPlayerId());
        
        writtenState |= ECRS_PlayerId;
    } else {
        inOutputStream.Write((bool)false);
    }
    
    if (inDirtyState & ECRS_Pose) {
        inOutputStream.Write((bool)true);
        
        Vector3 velocity = mVelocity;
        inOutputStream.Write(velocity.mX);
        inOutputStream.Write(velocity.mY);
        
        Vector3 location = GetLocation();
        inOutputStream.Write(location.mX);
        inOutputStream.Write(location.mY);
        
        inOutputStream.Write(GetRotation());
        
        writtenState |= ECRS_Pose;
    } else {
        inOutputStream.Write((bool)false);
    }
    
    // always write mThrustDir- it's just two bits
    if (mThrustDir != 0.f) {
        inOutputStream.Write(true);
        inOutputStream.Write(mThrustDir > 0.f);
    } else {
        inOutputStream.Write(false);
    }
    
    if (inDirtyState & ECRS_Color) {
        inOutputStream.Write((bool)true);
        inOutputStream.Write(GetColor());
        
        writtenState |= ECRS_Color;
    } else {
        inOutputStream.Write((bool)false);
    }
    
    if (inDirtyState & ECRS_Health) {
        inOutputStream.Write((bool)true);
        inOutputStream.Write(mHealth, 4);
        
        writtenState |= ECRS_Health;
    } else {
        inOutputStream.Write((bool)false);
    }
    
    return writtenState;
}


class RoboCat : public GameObject
{
public:
    CLASS_IDENTIFICATION( 'RCAT', GameObject )
    
{    enum ECatReplicationState
        {
            ECRS_Pose = 1 << 0,
            ECRS_Color = 1 << 1,
            ECRS_PlayerId = 1 << 2,
            ECRS_Health = 1 << 3,

            ECRS_AllState = ECRS_Pose | ECRS_Color | ECRS_PlayerId | ECRS_Health
        };
    }
    
    static	GameObject*	StaticCreate()			{ return new RoboCat(); }
    
    virtual uint32_t GetAllStateMask()	const override	{ return ECRS_AllState; }
    
    virtual	RoboCat*	GetAsCat()	{ return this; }
    
    virtual void Update()	override;
    
    void ProcessInput( float inDeltaTime, const InputState& inInputState );
    void SimulateMovement( float inDeltaTime );
    
    void ProcessCollisions();
    void ProcessCollisionsWithScreenWalls();
    
    void		SetPlayerId( uint32_t inPlayerId )			{ mPlayerId = inPlayerId; }
    uint32_t	GetPlayerId()						const 	{ return mPlayerId; }
    
    void			SetVelocity( const Vector3& inVelocity )	{ mVelocity = inVelocity; }
    const Vector3&	GetVelocity()						const	{ return mVelocity; }
    
    virtual uint32_t	Write( OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState ) const override;
    
protected:
    RoboCat();
    
private:
    
    
    void	AdjustVelocityByThrust( float inDeltaTime );
    
    Vector3				mVelocity;
    
    
    float				mMaxLinearSpeed;
    float				mMaxRotationSpeed;
    
    //bounce fraction when hitting various things
    float				mWallRestitution;
    float				mCatRestitution;
    
    
    uint32_t			mPlayerId;
    
protected:
    
    ///move down here for padding reasons...
    
    float				mLastMoveTimestamp;
    
    float				mThrustDir;
    int					mHealth;
    
    bool				mIsShooting;
    
    
    
    
    
};

typedef shared_ptr< RoboCat >	RoboCatPtr;











enum ECatControlType { ESCT_Human, ESCT_AI };

class RoboCatServer : public RoboCat {
public:
    static GameObjectPtr StaticCreate() {
        return NetworkManagerServer::sInstance->RegisterAndReturn(
                                                                  new RoboCatServer());
    }
    virtual void HandleDying() override;
    
    virtual void Update();
    
    void SetCatControlType(ECatControlType inCatControlType) {
        mCatControlType = inCatControlType;
    }
    
    void TakeDamage(int inDamagingPlayerId);
    
protected:
    RoboCatServer();
    
private:
    void HandleShooting();
    
    ECatControlType mCatControlType;
    
    float mTimeOfNextShot;
    float mTimeBetweenShots;
};


#include <RoboCatServerPCH.h>

RoboCatServer::RoboCatServer() :
mCatControlType( ESCT_Human ),
mTimeOfNextShot( 0.f ),
mTimeBetweenShots( 0.2f )
{}

void RoboCatServer::HandleDying()
{
    NetworkManagerServer::sInstance->UnregisterGameObject( this );
}

void RoboCatServer::Update()
{
    RoboCat::Update();
    
    Vector3 oldLocation = GetLocation();
    Vector3 oldVelocity = GetVelocity();
    float oldRotation = GetRotation();
    
    //are you controlled by a player?
    //if so, is there a move we haven't processed yet?
    if( mCatControlType == ESCT_Human )
    {
        ClientProxyPtr client = NetworkManagerServer::sInstance->GetClientProxy( GetPlayerId() );
        if( client )
        {
            MoveList& moveList = client->GetUnprocessedMoveList();
            for( const Move& unprocessedMove : moveList )
            {
                const InputState& currentState = unprocessedMove.GetInputState();
                
                float deltaTime = unprocessedMove.GetDeltaTime();
                
                ProcessInput( deltaTime, currentState );
                SimulateMovement( deltaTime );
                
                //LOG( "Server Move Time: %3.4f deltaTime: %3.4f left rot at %3.4f", unprocessedMove.GetTimestamp(), deltaTime, GetRotation() );
                
            }
            
            moveList.Clear();
        }
    }
    else
    {
        //do some AI stuff
        SimulateMovement( Timing::sInstance.GetDeltaTime() );
    }
    
    HandleShooting();
    
    if( !RoboMath::Is2DVectorEqual( oldLocation, GetLocation() ) ||
       !RoboMath::Is2DVectorEqual( oldVelocity, GetVelocity() ) ||
       oldRotation != GetRotation() )
    {
        NetworkManagerServer::sInstance->SetStateDirty( GetNetworkId(), ECRS_Pose );
    }
    
}

void RoboCatServer::HandleShooting()
{
    float time = Timing::sInstance.GetFrameStartTime();
    if( mIsShooting && Timing::sInstance.GetFrameStartTime() > mTimeOfNextShot )
    {
        //not exact, but okay
        mTimeOfNextShot = time + mTimeBetweenShots;
        
        //fire!
        YarnPtr yarn = std::static_pointer_cast< Yarn >( GameObjectRegistry::sInstance->CreateGameObject( 'YARN' ) );
        yarn->InitFromShooter( this );
    }
}

void RoboCatServer::TakeDamage( int inDamagingPlayerId )
{
    mHealth--;
    if( mHealth <= 0.f )
    {
        //score one for damaging player...
        ScoreBoardManager::sInstance->IncScore( inDamagingPlayerId, 1 );
        
        //and you want to die
        SetDoesWantToDie( true );
        
        //tell the client proxy to make you a new cat
        ClientProxyPtr clientProxy = NetworkManagerServer::sInstance->GetClientProxy( GetPlayerId() );
        if( clientProxy )
        {
            clientProxy->HandleCatDied();
        }
    }
    
    //tell the world our health dropped
    NetworkManagerServer::sInstance->SetStateDirty( GetNetworkId(), ECRS_Health );
}


#include <RoboCatClientPCH.h>



RoboCatClient::RoboCatClient() :
mTimeLocationBecameOutOfSync( 0.f ),
mTimeVelocityBecameOutOfSync( 0.f )
{
    mSpriteComponent.reset( new SpriteComponent( this ) );
    mSpriteComponent->SetTexture( TextureManager::sInstance->GetTexture( "cat" ) );
}

void RoboCatClient::HandleDying()
{
    RoboCat::HandleDying();
    
    //and if we're local, tell the hud so our health goes away!
    if( GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() )
    {
        HUD::sInstance->SetPlayerHealth( 0 );
    }
}


void RoboCatClient::Update()
{
    //is this the cat owned by us?
    if( GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() )
    {
        const Move* pendingMove = InputManager::sInstance->GetAndClearPendingMove();
        //in theory, only do this if we want to sample input this frame / if there's a new move ( since we have to keep in sync with server )
        if( pendingMove ) //is it time to sample a new move...
        {
            float deltaTime = pendingMove->GetDeltaTime();
            
            //apply that input
            
            ProcessInput( deltaTime, pendingMove->GetInputState() );
            
            //and simulate!
            
            SimulateMovement( deltaTime );
            
            //LOG( "Client Move Time: %3.4f deltaTime: %3.4f left rot at %3.4f", latestMove.GetTimestamp(), deltaTime, GetRotation() );
        }
    }
    else
    {
        SimulateMovement( Timing::sInstance.GetDeltaTime() );
        
        if( RoboMath::Is2DVectorEqual( GetVelocity(), Vector3::Zero ) )
        {
            //we're in sync if our velocity is 0
            mTimeLocationBecameOutOfSync = 0.f;
        }
    }
}

void RoboCatClient::Read( InputMemoryBitStream& inInputStream )
{
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.Read( stateBit );
    if( stateBit )
    {
        uint32_t playerId;
        inInputStream.Read( playerId );
        SetPlayerId( playerId );
        readState |= ECRS_PlayerId;
    }
    
    float oldRotation = GetRotation();
    Vector3 oldLocation = GetLocation();
    Vector3 oldVelocity = GetVelocity();
    
    float replicatedRotation;
    Vector3 replicatedLocation;
    Vector3 replicatedVelocity;
    
    inInputStream.Read( stateBit );
    if( stateBit )
    {
        inInputStream.Read( replicatedVelocity.mX );
        inInputStream.Read( replicatedVelocity.mY );
        
        SetVelocity( replicatedVelocity );
        
        inInputStream.Read( replicatedLocation.mX );
        inInputStream.Read( replicatedLocation.mY );
        
        SetLocation( replicatedLocation );
        
        inInputStream.Read( replicatedRotation );
        SetRotation( replicatedRotation );
        
        readState |= ECRS_Pose;
    }
    
    inInputStream.Read( stateBit );
    if( stateBit )
    {
        inInputStream.Read( stateBit );
        mThrustDir = stateBit ? 1.f : -1.f;
    }
    else
    {
        mThrustDir = 0.f;
    }
    
    inInputStream.Read( stateBit );
    if( stateBit )
    {
        Vector3 color;
        inInputStream.Read( color );
        SetColor( color );
        readState |= ECRS_Color;
    }
    
    inInputStream.Read( stateBit );
    if( stateBit )
    {
        mHealth = 0;
        inInputStream.Read( mHealth, 4 );
        readState |= ECRS_Health;
    }
    
    if( GetPlayerId() == NetworkManagerClient::sInstance->GetPlayerId() )
    {
        //did we get health? if so, tell the hud!
        if( ( readState & ECRS_Health ) != 0 )
        {
            HUD::sInstance->SetPlayerHealth( mHealth );
        }
        
        DoClientSidePredictionAfterReplicationForLocalCat( readState );
        
        //if this is a create packet, don't interpolate
        if( ( readState & ECRS_PlayerId ) == 0 )
        {
            InterpolateClientSidePrediction( oldRotation, oldLocation, oldVelocity, false );
        }
    }
    else
    {
        DoClientSidePredictionAfterReplicationForRemoteCat( readState );
        
        //will this smooth us out too? it'll interpolate us just 10% of the way there...
        if( ( readState & ECRS_PlayerId ) == 0 )
        {
            InterpolateClientSidePrediction( oldRotation, oldLocation, oldVelocity, true );
        }
        
    }
    
    
}




void RoboCatClient::DoClientSidePredictionAfterReplicationForLocalCat( uint32_t inReadState )
{
    if( ( inReadState & ECRS_Pose ) != 0 )
    {
        //simulate pose only if we received new pose- might have just gotten thrustDir
        //in which case we don't need to replay moves because we haven't warped backwards
        
        //all processed moves have been removed, so all that are left are unprocessed moves
        //so we must apply them...
        const MoveList& moveList = InputManager::sInstance->GetMoveList();
        
        for( const Move& move : moveList )
        {
            float deltaTime = move.GetDeltaTime();
            ProcessInput( deltaTime, move.GetInputState() );
            
            SimulateMovement( deltaTime );
        }
    }
    
    
    
}


void RoboCatClient::InterpolateClientSidePrediction( float inOldRotation, const Vector3& inOldLocation, const Vector3& inOldVelocity, bool inIsForRemoteCat )
{
    if( inOldRotation != GetRotation() && !inIsForRemoteCat )
    {
        LOG( "ERROR! Move replay ended with incorrect rotation!", 0 );
    }
    
    float roundTripTime = NetworkManagerClient::sInstance->GetRoundTripTime();
    
    if( !RoboMath::Is2DVectorEqual( inOldLocation, GetLocation() ) )
    {
        //LOG( "ERROR! Move replay ended with incorrect location!", 0 );
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::sInstance.GetFrameStartTime();
        if( mTimeLocationBecameOutOfSync == 0.f )
        {
            mTimeLocationBecameOutOfSync = time;
        }
        
        float durationOutOfSync = time - mTimeLocationBecameOutOfSync;
        if( durationOutOfSync < roundTripTime )
        {
            SetLocation( Lerp( inOldLocation, GetLocation(), inIsForRemoteCat ? ( durationOutOfSync / roundTripTime ) : 0.1f ) );
        }
    }
    else
    {
        //we're in sync
        mTimeLocationBecameOutOfSync = 0.f;
    }
    
    
    if( !RoboMath::Is2DVectorEqual( inOldVelocity, GetVelocity() ) )
    {
        //LOG( "ERROR! Move replay ended with incorrect velocity!", 0 );
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::sInstance.GetFrameStartTime();
        if( mTimeVelocityBecameOutOfSync == 0.f )
        {
            mTimeVelocityBecameOutOfSync = time;
        }
        
        //now interpolate to the correct value...
        float durationOutOfSync = time - mTimeVelocityBecameOutOfSync;
        if( durationOutOfSync < roundTripTime )
        {
            SetVelocity( Lerp( inOldVelocity, GetVelocity(), inIsForRemoteCat ? ( durationOutOfSync / roundTripTime ) : 0.1f ) );
        }
        //otherwise, fine...
        
    }
    else
    {
        //we're in sync
        mTimeVelocityBecameOutOfSync = 0.f;
    }
    
}


//so what do we want to do here? need to do some kind of interpolation...

void RoboCatClient::DoClientSidePredictionAfterReplicationForRemoteCat( uint32_t inReadState )
{
    if( ( inReadState & ECRS_Pose ) != 0 )
    {
        
        //simulate movement for an additional RTT
        float rtt = NetworkManagerClient::sInstance->GetRoundTripTime();
        //LOG( "Other cat came in, simulating for an extra %f", rtt );
        
        //let's break into framerate sized chunks though so that we don't run through walls and do crazy things...
        float deltaTime = 1.f / 30.f;
        
        while( true )
        {
            if( rtt < deltaTime )
            {
                SimulateMovement( rtt );
                break;
            }
         
            
            else
            {
                SimulateMovement( deltaTime );
                rtt -= deltaTime;
            }
        }
    }
    
}


class RoboCatClient : public RoboCat
{
public:
    static	GameObjectPtr	StaticCreate()		{ return GameObjectPtr( new RoboCatClient() ); }
    
    virtual void Update();
    virtual void	HandleDying() override;
    
    virtual void	Read( InputMemoryBitStream& inInputStream ) override;
    
    void DoClientSidePredictionAfterReplicationForLocalCat( uint32_t inReadState );
    void DoClientSidePredictionAfterReplicationForRemoteCat( uint32_t inReadState );
    
    
    
protected:
    RoboCatClient();
    
    
private:
    
    void InterpolateClientSidePrediction( float inOldRotation, const Vector3& inOldLocation, const Vector3& inOldVelocity, bool inIsForRemoteCat );
    
    float				mTimeLocationBecameOutOfSync;
    float				mTimeVelocityBecameOutOfSync;
    
    SpriteComponentPtr	mSpriteComponent;
};