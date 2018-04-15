#include "gameobjects/MoveList.h"
#include "input/InputManager.h"
#include "networking/NetworkManagerClient.h"
#include "networking/Logger.h"

const float kTimeBetweenHellos = 1.f;
const float kTimeBetweenInputPackets = 0.033f;

NetworkManagerClient* NetworkManagerClient::sInstance;
NetworkManagerClient::NetworkManagerClient()
    : mState( NCS_Uninitialized ), mDeliveryNotificationManager( true, false ),
      mLastRoundTripTime( 0.f )
{
}

void NetworkManagerClient::StaticInit( const SocketAddress& inServerAddress,
                                        const std::string& inName )
{
    sInstance = new NetworkManagerClient();
    sInstance->Init( inServerAddress, inName );
}

void NetworkManagerClient::Init( const SocketAddress& inServerAddress,
                                 const std::string& inName )
{
    // Init the good shit
    NetworkManager::Init( 0 );

    mServerAddress = inServerAddress;
    mState = NCS_SayingHello;
    mTimeOfLastHello = 0.f;
    mName = inName;
}

void NetworkManagerClient::ProcessPacket( InputMemoryBitStream& inInputStream,
                                          const SocketAddress& inFromAddress )
{
    uint32_t packetType;

    inInputStream.Read( packetType );

    DEBUG("Reading packet {} ", packetType);
    switch ( packetType )
    {
    case kWelcomeCC:
        HandleWelcomePacket( inInputStream );
        DEBUG("Welcome packet {} ", packetType);
        break;
    case kStateCC:
        if ( mDeliveryNotificationManager.ReadAndProcessState( inInputStream ) )
        {
            DEBUG("State packet {} ", packetType);
            HandleStatePacket( inInputStream );
        } else {
            ERROR("Don't care about state packetl");
        }
        break;
    default:
        ERROR("Can't read packet type {}", packetType);
        break;
    }
}

void NetworkManagerClient::SendOutgoingPackets()
{
    switch ( mState )
    {
    case NCS_SayingHello:
        UpdateSayingHello();
        break;
    case NCS_Welcomed:
        UpdateSendingInputPacket();
    default:
        break;
    }
}

void NetworkManagerClient::UpdateSayingHello()
{
    float time = Timing::sInstance.GetTimef();

    if ( time > mTimeOfLastHello + kTimeBetweenHellos )
    {
        SendHelloPacket();
        mTimeOfLastHello = time;
    }
}

void NetworkManagerClient::SendHelloPacket()
{
    OutputMemoryBitStream helloPacket;

    helloPacket.Write( kHelloCC );
    helloPacket.Write( mName );

    SendPacket( helloPacket, mServerAddress );
}

void NetworkManagerClient::HandleWelcomePacket(
    InputMemoryBitStream& inInputStream )
{
    if ( mState != NCS_SayingHello )
    {
        return; // already processed hello packet
    }

    // Read player id into state
    inInputStream.Read( mPlayerId );
    mState = NCS_Welcomed;
    DEBUG("'{}' was welcomed on client as player {}", mName.c_str(), mPlayerId );
}

void NetworkManagerClient::HandleStatePacket(
    InputMemoryBitStream& inInputStream )
{
    // Only process if welcomed
    if ( mState != NCS_Welcomed )
    {
        DEBUG("Not welcomed yet")
        return;
    }

    ReadLastMoveProcessedOnServerTimestamp( inInputStream );

    // Have the replication manager read the input packets, it will be copied
    // as moves or whatever is needed
    mReplicationManagerClient.Read( inInputStream );
}

void NetworkManagerClient::UpdateSendingInputPacket()
{
    float time = Timing::sInstance.GetTimef();

    if ( time > mTimeOfLastInputPacket + kTimeBetweenInputPackets )
    {
        SendInputPacket();
        mTimeOfLastInputPacket = time;
    }
}

void NetworkManagerClient::SendInputPacket()
{
    const MoveList& moveList = InputManager::sInstance->GetMoveList();

    if ( moveList.HasMoves() )
    {
        TRACE("Has Moves");
        OutputMemoryBitStream inputPacket;

        inputPacket.Write( kInputCC );

        // Write the squence number and any acks
        mDeliveryNotificationManager.WriteState( inputPacket );

        int moveCount = moveList.GetMoveCount();
        // int firstMoveIndex = moveCount - 3;
       int firstMoveIndex = 0;

        if ( firstMoveIndex < 3 )
        {
            firstMoveIndex = 0;
        }

        auto move = moveList.begin() + firstMoveIndex;

        inputPacket.Write( moveCount - firstMoveIndex, 8 );
        TRACE("Sending {} moves", moveCount - firstMoveIndex);
        for ( ; firstMoveIndex < moveCount; ++firstMoveIndex, ++move )
        {
            move->Write( inputPacket );
        }

        SendPacket( inputPacket, mServerAddress );
    }
}

void NetworkManagerClient::ReadLastMoveProcessedOnServerTimestamp(
    InputMemoryBitStream& inInputStream )

{
    bool isTimestampDirty;

    inInputStream.Read( isTimestampDirty );

    // Don't do anything if the packet wasn't dirty
    if ( !isTimestampDirty )
    {
        TRACE("Don't need i");
        return;
    }

    // TODO: You can't send timestamps, that is a good way to fuck up some stuff
    inInputStream.Read( mLastMoveProcessedByServerTimestamp );
    float rtt = Timing::sInstance.GetFrameStartTime() -
                mLastMoveProcessedByServerTimestamp;
    mLastRoundTripTime = rtt;

    mAvgRoundTripTime.Update( rtt );
    InputManager::sInstance->GetMoveList().RemoveProcessedMoves(
        mLastMoveProcessedByServerTimestamp );
}
