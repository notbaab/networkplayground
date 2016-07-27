#include "gameobjects/GameObject.h"
#include "gameobjects/Move.h"
#include "networking/NetworkManagerServer.h"
#include "networking/StringUtils.h"

NetworkManagerServer* NetworkManagerServer::sInstance;

NetworkManagerServer::NetworkManagerServer()
    : NetworkManager(), mNextPlayerId( 1 ), mNextNetworkId( 1 ),
      mTimeBetweenStatePackets( 0.033f )
{
}

bool NetworkManagerServer::StaticInit( uint16_t inPort )
{
    sInstance = new NetworkManagerServer();

    // Parent does the heavy lifting
    return sInstance->Init( inPort );
}

void NetworkManagerServer::ProcessPacket( InputMemoryBitStream& inStream,
                                          const SocketAddress& inFromAddress )
{
    // read the packet and do stuff
    auto it = mAddressToClientMap.find( inFromAddress );

    if ( it == mAddressToClientMap.end() )
    {
        HandlePacketFromNewClient( inStream, inFromAddress );
    }
    else
    {
        ProcessPacket( ( *it ).second, inStream );
    }
}

void NetworkManagerServer::ProcessPacket( ClientProxyPtr inClientProxy,
                                          InputMemoryBitStream& inInputStream )
{
    inClientProxy->UpdateLastPacketTime();

    uint32_t packetType;

    inInputStream.Read( packetType );

    switch ( packetType )
    {
    case kHelloCC:
        SendWelcomePacket( inClientProxy );
        break;

    case kInputCC:
        // TODO: Check Input PAcke with delivery notificaiton manager
        HandleInputPacket( inClientProxy, inInputStream );
    default:
        break;
    }
}

bool proccessMovePacket( ClientProxyPtr inClientProxy, Move move,
                         InputMemoryBitStream& inInputStream )
{
    bool out = false;

    if ( !move.Read( inInputStream ) )
    {
        return out;
    }

    out = inClientProxy->GetUnprocessedMoveList().AddMoveIfNew( move );
    if ( out )
    {
        inClientProxy->SetIsLastMoveTimestampDirty( true );
    }

    return out;
}

void NetworkManagerServer::HandleInputPacket(
    ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream )
{
    uint32_t moveCount = 0;
    Move move;
    inInputStream.Read( moveCount, 2 );

    for ( ; moveCount > 0; --moveCount )
    {
        proccessMovePacket( inClientProxy, move, inInputStream );
    }
}

void NetworkManagerServer::HandlePacketFromNewClient(
    InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress )
{
    uint32_t packetType;
    inInputStream.Read( packetType );
    if ( packetType == kHelloCC )
    {
        // Do stuff
    }
    else
    {
        LOG( "Bad packet from client %s", inFromAddress.ToString().c_str() );
    }
}

/**
 * Send a welcome packet to a client. Note, this will send it immediatly, it
 * will not queue up to be sent with the server tick.
 */
void NetworkManagerServer::SendWelcomePacket( ClientProxyPtr inClientProxy )
{
    OutputMemoryBitStream welcomePacket;

    welcomePacket.Write( kWelcomeCC );
    welcomePacket.Write( ( inClientProxy->GetPlayerId() ) );

    LOG( "Welcomed new client '%s' as player %d",
         inClientProxy->GetName().c_str(), inClientProxy->GetPlayerId() );

    SendPacket( welcomePacket, inClientProxy->GetSocketAddress() );
}

void NetworkManagerServer::SendOutgoingPackets() {}
void NetworkManagerServer::UpdateAllClients() {}
void NetworkManagerServer::SendStatePacketToClient(
    ClientProxyPtr inClientProxy )
{
}

void NetworkManagerServer::AddWorldStateToPacket(
    OutputMemoryBitStream& inOutputStream )
{
}
