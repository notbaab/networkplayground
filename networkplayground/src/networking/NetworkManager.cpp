//
//  NetworkManager.cpp
//  networkplayground
//
//  Created by Erik Parreira on 2/29/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include "networking/Networking.h"
#include <ctime>

// TODO: Temp for now
void printStream( InputMemoryBitStream &inInputStream );

NetworkManager::NetworkManager() //:
// mBytesSentThisFrame( 0 ),
//mDropPacketChance( 0.f ),
//mSimulatedLatency( 0.f )
{
}


NetworkManager::~NetworkManager()
{
}

bool NetworkManager::Init( uint16_t inPort )
{
    // Create the underlaying socket for this port
    mSocket = SocketUtil::CreateUDPSocket( INET );
    
    // Create a wildcard input address to bind to
    SocketAddress ownAddress( INADDR_ANY, inPort );
    mSocket->Bind( ownAddress );
    
    printf( "Initializing NetworkManager at port %d\n", inPort );
    
//    mBytesReceivedPerSecond = WeightedTimedMovingAverage( 1.f );
//    mBytesSentPerSecond = WeightedTimedMovingAverage( 1.f );
    
    //did we bind okay?
    if( mSocket == nullptr )
    {
        return false;
    }
    
    if( mSocket->SetNonBlockingMode( true ) != NO_ERROR )
    {
        return false;
    }
    
    return true;
}

// Wrapper for doing all the network related tasks
void NetworkManager::ProcessIncomingPackages()
{
    ReadIncomingPacketsIntoQueue();
    ProcessQueuedPackets();

    // UpdateBytesSentLastFrame();

}

void NetworkManager::ProcessQueuedPackets()
{
    //look at the front packet...
    while( !mPacketQueue.empty() )
    {

        ReceivedPacket& nextPacket = mPacketQueue.front();
        // if( Timing::sInstance.GetTimef() > nextPacket.GetReceivedTime() )
        // {
        ProcessPacket( nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress() );
        mPacketQueue.pop();
        // }
        // else
        // {
        //     break;
        // }
    }
}

void NetworkManager::ProcessPacket(
    InputMemoryBitStream &inInputStream,
    const SocketAddress &inFromAddress
)
{
    printStream(inInputStream);
    std::cout << inFromAddress.ToString() << std::endl;
    // For now, just output the packet data
    // for(int i = 0 ; i < readByteCount ; i ++ )
    // {
    //     std::cout << packetMem[i] ;//Looping 5 times to print out [0],[1],[2],[3],[4]
    // }

}

void printStream( InputMemoryBitStream &inInputStream )
{
    const char* streamBuffer = inInputStream.GetBufferPtr();
    uint32_t bufferSize = inInputStream.GetByteCapacity();

    std::cout << "Reading " << bufferSize << " Bytes" << std::endl;
    for( int charIdx = 0; charIdx < bufferSize; charIdx++ )
    {
        std::cout << streamBuffer[charIdx];
    }
    std::cout << std::endl;
}

void NetworkManager::ReadIncomingPacketsIntoQueue()
{
    // 'byte' array to read each packet into
    char packetMem[ 1500 ];
    int packetSize = sizeof(packetMem);
    int packetSizeInBits = packetSize * 8;
    
    // Stream we will read each packet into
    InputMemoryBitStream inputStream( packetMem, packetSizeInBits );
    
    int totalPacketsRecieved = 0;
    int totalBytesRead = 0;
    
    SocketAddress fromAddress;
    
    // XXX: Hard code 10 total packets received for now
    while (totalPacketsRecieved < 10)
    {
        int readByteCount = mSocket->ReceiveFrom( packetMem,
                                                  packetSize,
                                                  fromAddress );
        
        if (readByteCount == 0)
        {
            // nothing to read
            break;
        }
        // Reset? How would that work with udp sockets?
        // else if( readByteCount == -WSAECONNRESET )
        // {
        //     //port closed on other end, so DC this person immediately
        //     HandleConnectionReset( fromAddress );
        // }


        // Resize input stream to the number of bytes
        inputStream.ResetToCapacity( readByteCount );
        ++totalPacketsRecieved;
        totalBytesRead += readByteCount;

        // TODO: Simulate dropping packet change and latency
        float recievedTime = time(0);
        // std::cout << recievedTime << std::endl;

        // create a new RecievedPacket, copying the memory stream
        mPacketQueue.emplace( recievedTime, inputStream, fromAddress );
    }
}

void NetworkManager::SendPacket(
    const OutputMemoryBitStream &inOutputStream,
    const SocketAddress &inFromAddress
) {
    int streamByteLength = inOutputStream.GetByteLength();
    const void* packetDataToSend = inOutputStream.GetBufferPtr();
    
    int sentByteCount = mSocket->SendTo( packetDataToSend,
                                         streamByteLength,
                                         inFromAddress );
    
    if (sentByteCount )
    {
        // TODO: Keep track of how many bytes have been sent
        // mBytesSentThisFrame += sentByteCount;
        std::cout << "Sent " << sentByteCount << " bytes";
    }
}



NetworkManager::ReceivedPacket::ReceivedPacket(
    float inReceivedTime,
    InputMemoryBitStream& inInputMemoryBitStream,
    const SocketAddress& inFromAddress
) :
    mRecievedTime( inReceivedTime ),
    mPacketDataStream( inInputMemoryBitStream ),
    mFromAddress( inFromAddress )
{}

