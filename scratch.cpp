
class NetworkManagerServer : public NetworkManager
{
  public:
    static NetworkManagerServer* sInstance;

    static bool StaticInit( uint16_t inPort );

    virtual void ProcessPacket( InputMemoryBitStream& inInputStream,
                                const SocketAddress& inFromAddress ) override;
    virtual void
    HandleConnectionReset( const SocketAddress& inFromAddress ) override;

    void SendOutgoingPackets();
    void CheckForDisconnects();

    void RegisterGameObject( GameObjectPtr inGameObject );
    inline GameObjectPtr RegisterAndReturn( GameObject* inGameObject );
    void UnregisterGameObject( GameObject* inGameObject );
    void SetStateDirty( int inNetworkId, uint32_t inDirtyState );

    void RespawnCats();

    ClientProxyPtr GetClientProxy( int inPlayerId ) const;

  private:
    NetworkManagerServer();

    void HandlePacketFromNewClient( InputMemoryBitStream& inInputStream,
                                    const SocketAddress& inFromAddress );
    void ProcessPacket( ClientProxyPtr inClientProxy,
                        InputMemoryBitStream& inInputStream );

    void SendWelcomePacket( ClientProxyPtr inClientProxy );
    void UpdateAllClients();

    void AddWorldStateToPacket( OutputMemoryBitStream& inOutputStream );
    void AddScoreBoardStateToPacket( OutputMemoryBitStream& inOutputStream );

    void SendStatePacketToClient( ClientProxyPtr inClientProxy );
    void WriteLastMoveTimestampIfDirty( OutputMemoryBitStream& inOutputStream,
                                        ClientProxyPtr inClientProxy );

    void HandleInputPacket( ClientProxyPtr inClientProxy,
                            InputMemoryBitStream& inInputStream );

    void HandleClientDisconnected( ClientProxyPtr inClientProxy );

    int GetNewNetworkId();

    typedef unordered_map<int, ClientProxyPtr> IntToClientMap;
    typedef unordered_map<SocketAddress, ClientProxyPtr> AddressToClientMap;

    AddressToClientMap mAddressToClientMap;
    IntToClientMap mPlayerIdToClientMap;

    int mNewPlayerId;
    int mNewNetworkId;

    float mTimeOfLastSatePacket;
    float mTimeBetweenStatePackets;
    float mClientDisconnectTimeout;
};

inline GameObjectPtr
NetworkManagerServer::RegisterAndReturn( GameObject* inGameObject )
{
    GameObjectPtr toRet( inGameObject );
    RegisterGameObject( toRet );
    return toRet;
}

#include <RoboCatServerPCH.h>

NetworkManagerServer* NetworkManagerServer::sInstance;

NetworkManagerServer::NetworkManagerServer()
    : mNewPlayerId( 1 ), mNewNetworkId( 1 ), mTimeBetweenStatePackets( 0.033f ),
      mClientDisconnectTimeout( 3.f )
{
}

bool NetworkManagerServer::StaticInit( uint16_t inPort )
{
    sInstance = new NetworkManagerServer();
    return sInstance->Init( inPort );
}

void NetworkManagerServer::HandleConnectionReset(
    const SocketAddress& inFromAddress )
{
    // just dc the client right away...
    auto it = mAddressToClientMap.find( inFromAddress );
    if ( it != mAddressToClientMap.end() )
    {
        HandleClientDisconnected( it->second );
    }
}

void NetworkManagerServer::ProcessPacket( InputMemoryBitStream& inInputStream,
                                          const SocketAddress& inFromAddress )
{
    // try to get the client proxy for this address
    // pass this to the client proxy to process
    auto it = mAddressToClientMap.find( inFromAddress );
    if ( it == mAddressToClientMap.end() )
    {
        // didn't find one? it's a new cilent..is the a HELO? if so, create a
        // client proxy...
        HandlePacketFromNewClient( inInputStream, inFromAddress );
    }
    else
    {
        ProcessPacket( ( *it ).second, inInputStream );
    }
}

void NetworkManagerServer::ProcessPacket( ClientProxyPtr inClientProxy,
                                          InputMemoryBitStream& inInputStream )
{
    // remember we got a packet so we know not to disconnect for a bit
    inClientProxy->UpdateLastPacketTime();

    uint32_t packetType;
    inInputStream.Read( packetType );
    switch ( packetType )
    {
    case kHelloCC:
        // need to resend welcome. to be extra safe we should check the name
        // is the one we expect from this address,
        // otherwise something weird is going on...
        SendWelcomePacket( inClientProxy );
        break;
    case kInputCC:
        HandleInputPacket( inClientProxy, inInputStream );
        break;
    default:
        LOG( "Unknown packet type received from %s",
             inClientProxy->GetSocketAddress().ToString().c_str() );
        break;
    }
}

void NetworkManagerServer::HandlePacketFromNewClient(
    InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress )
{
    // read the beginning- is it a hello?
    uint32_t packetType;
    inInputStream.Read( packetType );
    if ( packetType == kHelloCC )
    {
        // read the name
        string name;
        inInputStream.Read( name );
        ClientProxyPtr newClientProxy = std::make_shared<ClientProxy>(
            inFromAddress, name, mNewPlayerId++ );
        mAddressToClientMap[inFromAddress] = newClientProxy;
        mPlayerIdToClientMap[newClientProxy->GetPlayerId()] = newClientProxy;

        // tell the server about this client, spawn a cat, etc...
        // if we had a generic message system, this would be a good use for
        // it...
        // instead we'll just tell the server directly
        static_cast<Server*>( Engine::sInstance.get() )
            ->HandleNewClient( newClientProxy );

        // and welcome the client...
        SendWelcomePacket( newClientProxy );

        // and now init the replication manager with everything we know about!
        for ( const auto& pair : mNetworkIdToGameObjectMap )
        {
            newClientProxy->GetReplicationManagerServer().ReplicateCreate(
                pair.first, pair.second->GetAllStateMask() );
        }
    }
    else
    {
        // bad incoming packet from unknown client- we're under attack!!
        LOG( "Bad incoming packet from unknown client at socket %s",
             inFromAddress.ToString().c_str() );
    }
}

void NetworkManagerServer::SendWelcomePacket( ClientProxyPtr inClientProxy )
{
    OutputMemoryBitStream welcomePacket;

    welcomePacket.Write( kWelcomeCC );
    welcomePacket.Write( inClientProxy->GetPlayerId() );

    LOG( "Server Welcoming, new client '%s' as player %d",
         inClientProxy->GetName().c_str(), inClientProxy->GetPlayerId() );

    SendPacket( welcomePacket, inClientProxy->GetSocketAddress() );
}

void NetworkManagerServer::RespawnCats()
{
    for ( auto it = mAddressToClientMap.begin(),
               end = mAddressToClientMap.end();
          it != end; ++it )
    {
        ClientProxyPtr clientProxy = it->second;

        clientProxy->RespawnCatIfNecessary();
    }
}

void NetworkManagerServer::SendOutgoingPackets()
{
    // let's send a client a state packet whenever their move has come in...
    for ( auto it = mAddressToClientMap.begin(),
               end = mAddressToClientMap.end();
          it != end; ++it )
    {
        ClientProxyPtr clientProxy = it->second;
        if ( clientProxy->IsLastMoveTimestampDirty() )
        {
            SendStatePacketToClient( clientProxy );
        }
    }
}

void NetworkManagerServer::UpdateAllClients()
{
    for ( auto it = mAddressToClientMap.begin(),
               end = mAddressToClientMap.end();
          it != end; ++it )
    {
        SendStatePacketToClient( it->second );
    }
}

void NetworkManagerServer::SendStatePacketToClient(
    ClientProxyPtr inClientProxy )
{
    // build state packet
    OutputMemoryBitStream statePacket;

    // it's state!
    statePacket.Write( kStateCC );

    WriteLastMoveTimestampIfDirty( statePacket, inClientProxy );

    AddScoreBoardStateToPacket( statePacket );

    inClientProxy->GetReplicationManagerServer().Write( statePacket );
    SendPacket( statePacket, inClientProxy->GetSocketAddress() );
}

void NetworkManagerServer::WriteLastMoveTimestampIfDirty(
    OutputMemoryBitStream& inOutputStream, ClientProxyPtr inClientProxy )
{
    // first, dirty?
    bool isTimestampDirty = inClientProxy->IsLastMoveTimestampDirty();
    inOutputStream.Write( isTimestampDirty );
    if ( isTimestampDirty )
    {
        inOutputStream.Write(
            inClientProxy->GetUnprocessedMoveList().GetLastMoveTimestamp() );
        inClientProxy->SetIsLastMoveTimestampDirty( false );
    }
}

// should we ask the server for this? or run through the world ourselves?
void NetworkManagerServer::AddWorldStateToPacket(
    OutputMemoryBitStream& inOutputStream )
{
    const auto& gameObjects = World::sInstance->GetGameObjects();

    // now start writing objects- do we need to remember how many there are? we
    // can check first...
    inOutputStream.Write( gameObjects.size() );

    for ( GameObjectPtr gameObject : gameObjects )
    {
        inOutputStream.Write( gameObject->GetNetworkId() );
        inOutputStream.Write( gameObject->GetClassId() );
        gameObject->Write( inOutputStream, 0xffffffff );
    }
}

void NetworkManagerServer::AddScoreBoardStateToPacket(
    OutputMemoryBitStream& inOutputStream )
{
    ScoreBoardManager::sInstance->Write( inOutputStream );
}

int NetworkManagerServer::GetNewNetworkId()
{
    int toRet = mNewNetworkId++;
    if ( mNewNetworkId < toRet )
    {
        LOG( "Network ID Wrap Around!!! You've been playing way too long...",
             0 );
    }

    return toRet;
}

void NetworkManagerServer::HandleInputPacket(
    ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream )
{
    uint32_t moveCount = 0;
    Move move;
    inInputStream.Read( moveCount, 2 );

    for ( ; moveCount > 0; --moveCount )
    {
        if ( move.Read( inInputStream ) )
        {
            if ( inClientProxy->GetUnprocessedMoveList().AddMove( move ) )
            {
                inClientProxy->SetIsLastMoveTimestampDirty( true );
            }
        }
    }
}

ClientProxyPtr NetworkManagerServer::GetClientProxy( int inPlayerId ) const
{
    auto it = mPlayerIdToClientMap.find( inPlayerId );
    if ( it != mPlayerIdToClientMap.end() )
    {
        return it->second;
    }

    return nullptr;
}

void NetworkManagerServer::CheckForDisconnects()
{
    vector<ClientProxyPtr> clientsToDC;

    float minAllowedLastPacketFromClientTime =
        Timing::sInstance.GetTimef() - mClientDisconnectTimeout;
    for ( const auto& pair : mAddressToClientMap )
    {
        if ( pair.second->GetLastPacketFromClientTime() <
             minAllowedLastPacketFromClientTime )
        {
            // can't remove from map while in iterator, so just remember for
            // later...
            clientsToDC.push_back( pair.second );
        }
    }

    for ( ClientProxyPtr client : clientsToDC )
    {
        HandleClientDisconnected( client );
    }
}

void NetworkManagerServer::HandleClientDisconnected(
    ClientProxyPtr inClientProxy )
{
    mPlayerIdToClientMap.erase( inClientProxy->GetPlayerId() );
    mAddressToClientMap.erase( inClientProxy->GetSocketAddress() );
    static_cast<Server*>( Engine::sInstance.get() )
        ->HandleLostClient( inClientProxy );

    // was that the last client? if so, bye!
    if ( mAddressToClientMap.empty() )
    {
        Engine::sInstance->SetShouldKeepRunning( false );
    }
}

void NetworkManagerServer::RegisterGameObject( GameObjectPtr inGameObject )
{
    // assign network id
    int newNetworkId = GetNewNetworkId();
    inGameObject->SetNetworkId( newNetworkId );

    // add mapping from network id to game object
    mNetworkIdToGameObjectMap[newNetworkId] = inGameObject;

    // tell all client proxies this is new...
    for ( const auto& pair : mAddressToClientMap )
    {
        pair.second->GetReplicationManagerServer().ReplicateCreate(
            newNetworkId, inGameObject->GetAllStateMask() );
    }
}

void NetworkManagerServer::UnregisterGameObject( GameObject* inGameObject )
{
    int networkId = inGameObject->GetNetworkId();
    mNetworkIdToGameObjectMap.erase( networkId );

    // tell all client proxies to STOP replicating!
    // tell all client proxies this is new...
    for ( const auto& pair : mAddressToClientMap )
    {
        pair.second->GetReplicationManagerServer().ReplicateDestroy(
            networkId );
    }
}

void NetworkManagerServer::SetStateDirty( int inNetworkId,
                                          uint32_t inDirtyState )
{
    // tell everybody this is dirty
    for ( const auto& pair : mAddressToClientMap )
    {
        pair.second->GetReplicationManagerServer().SetStateDirty(
            inNetworkId, inDirtyState );
    }
}

class ClientProxy
{
  public:
    ClientProxy( const SocketAddress& inSocketAddress, const string& inName,
                 int inPlayerId );

    const SocketAddress& GetSocketAddress() const { return mSocketAddress; }
    int GetPlayerId() const { return mPlayerId; }
    const string& GetName() const { return mName; }
    void UpdateLastPacketTime();
    float GetLastPacketFromClientTime() const
    {
        return mLastPacketFromClientTime;
    }

    ReplicationManagerServer& GetReplicationManagerServer()
    {
        return mReplicationManagerServer;
    }

    const MoveList& GetUnprocessedMoveList() const
    {
        return mUnprocessedMoveList;
    }
    MoveList& GetUnprocessedMoveList() { return mUnprocessedMoveList; }
    void SetIsLastMoveTimestampDirty( bool inIsDirty )
    {
        mIsLastMoveTimestampDirty = inIsDirty;
    }
    bool IsLastMoveTimestampDirty() const { return mIsLastMoveTimestampDirty; }
    void HandleCatDied();
    void RespawnCatIfNecessary();

  private:
    ReplicationManagerServer mReplicationManagerServer;

    SocketAddress mSocketAddress;
    string mName;
    int mPlayerId;

    float mLastPacketFromClientTime;
    float mTimeToRespawn;

    MoveList mUnprocessedMoveList;
    bool mIsLastMoveTimestampDirty;
};

typedef shared_ptr<ClientProxy> ClientProxyPtr;

#include <RoboCatServerPCH.h>

namespace
{
const float kRespawnDelay = 3.f;
}

ClientProxy::ClientProxy( const SocketAddress& inSocketAddress,
                          const string& inName, int inPlayerId )
    : mSocketAddress( inSocketAddress ), mName( inName ),
      mPlayerId( inPlayerId ), mIsLastMoveTimestampDirty( false ),
      mTimeToRespawn( 0.f )
{
    UpdateLastPacketTime();
}

void ClientProxy::UpdateLastPacketTime()
{
    mLastPacketFromClientTime = Timing::sInstance.GetTimef();
}

void ClientProxy::HandleCatDied()
{
    mTimeToRespawn = Timing::sInstance.GetFrameStartTime() + kRespawnDelay;
}

void ClientProxy::RespawnCatIfNecessary()
{
    if ( mTimeToRespawn != 0.f &&
         Timing::sInstance.GetFrameStartTime() > mTimeToRespawn )
    {
        static_cast<Server*>( Engine::sInstance.get() )
            ->SpawnCatForPlayer( mPlayerId );
        mTimeToRespawn = 0.f;
    }
}

class ReplicationManagerServer
{
  public:
    void ReplicateCreate( int inNetworkId, uint32_t inInitialDirtyState );
    void ReplicateDestroy( int inNetworkId );
    void SetStateDirty( int inNetworkId, uint32_t inDirtyState );
    void RemoveFromReplication( int inNetworkId );

    void Write( OutputMemoryBitStream& inOutputStream );

  private:
    uint32_t WriteCreateAction( OutputMemoryBitStream& inOutputStream,
                                int inNetworkId, uint32_t inDirtyState );
    uint32_t WriteUpdateAction( OutputMemoryBitStream& inOutputStream,
                                int inNetworkId, uint32_t inDirtyState );
    uint32_t WriteDestroyAction( OutputMemoryBitStream& inOutputStream,
                                 int inNetworkId, uint32_t inDirtyState );

    unordered_map<int, ReplicationCommand> mNetworkIdToReplicationCommand;
    vector<int> mNetworkIdsToRemove;
};

class InputState
{
  public:
    InputState()
        : mDesiredRightAmount( 0 ), mDesiredLeftAmount( 0 ),
          mDesiredForwardAmount( 0 ), mDesiredBackAmount( 0 ),
          mIsShooting( false )
    {
    }

    float GetDesiredHorizontalDelta() const
    {
        return mDesiredRightAmount - mDesiredLeftAmount;
    }
    float GetDesiredVerticalDelta() const
    {
        return mDesiredForwardAmount - mDesiredBackAmount;
    }
    bool IsShooting() const { return mIsShooting; }
    bool Write( OutputMemoryBitStream& inOutputStream ) const;
    bool Read( InputMemoryBitStream& inInputStream );

  private:
    friend class InputManager;

    float mDesiredRightAmount, mDesiredLeftAmount;
    float mDesiredForwardAmount, mDesiredBackAmount;
    bool mIsShooting;
};

#include <RoboCatPCH.h>

namespace
{
void WriteSignedBinaryValue( OutputMemoryBitStream& inOutputStream,
                             float inValue )
{
    bool isNonZero = ( inValue != 0.f );
    inOutputStream.Write( isNonZero );
    if ( isNonZero )
    {
        inOutputStream.Write( inValue > 0.f );
    }
}

void ReadSignedBinaryValue( InputMemoryBitStream& inInputStream,
                            float& outValue )
{
    bool isNonZero;
    inInputStream.Read( isNonZero );
    if ( isNonZero )
    {
        bool isPositive;
        inInputStream.Read( isPositive );
        outValue = isPositive ? 1.f : -1.f;
    }
    else
    {
        outValue = 0.f;
    }
}
}

bool InputState::Write( OutputMemoryBitStream& inOutputStream ) const
{
    WriteSignedBinaryValue( inOutputStream, GetDesiredHorizontalDelta() );
    WriteSignedBinaryValue( inOutputStream, GetDesiredVerticalDelta() );
    inOutputStream.Write( mIsShooting );

    return false;
}

bool InputState::Read( InputMemoryBitStream& inInputStream )
{
    ReadSignedBinaryValue( inInputStream, mDesiredRightAmount );
    ReadSignedBinaryValue( inInputStream, mDesiredForwardAmount );
    inInputStream.Read( mIsShooting );

    return true;
}

class Move
{
  public:
    Move() {}
    Move( const InputState& inInputState, float inTimestamp, float inDeltaTime )
        : mInputState( inInputState ), mTimestamp( inTimestamp ),
          mDeltaTime( inDeltaTime )
    {
    }

    const InputState& GetInputState() const { return mInputState; }
    float GetTimestamp() const { return mTimestamp; }
    float GetDeltaTime() const { return mDeltaTime; }
    bool Write( OutputMemoryBitStream& inOutputStream ) const;
    bool Read( InputMemoryBitStream& inInputStream );

  private:
    InputState mInputState;
    float mTimestamp;
    float mDeltaTime;
};

#include <RoboCatPCH.h>

bool Move::Write( OutputMemoryBitStream& inOutputStream ) const
{
    mInputState.Write( inOutputStream );
    inOutputStream.Write( mTimestamp );

    return true;
}

bool Move::Read( InputMemoryBitStream& inInputStream )
{
    mInputState.Read( inInputStream );
    inInputStream.Read( mTimestamp );

    return true;
}
