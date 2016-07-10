//
//  NetworkManager.h
//  networkplayground
//
#ifndef NetworkedManager_h
#define NetworkedManager_h

#include "IO/MemoryBitStream.h"
#include "networking/SocketAddress.h"
#include "networking/UDPSocket.h"

#include "queue"
#include "list"

//typedef unordered_map< int, GameObjectPtr > IntToGameObjectMap;
class NetworkManager
{
public:
     // 4 byte message identifier
    static const uint32_t   kHelloCC = 'HELO'; // client connect message
    static const uint32_t   kWelcomeCC = 'WLCM'; // server welcome message
    
    NetworkManager();
    virtual ~NetworkManager();
    
    bool Init( uint16_t inPort );
    void ProcessIncomingPackages();
    virtual void ProcessPacket( InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress );
    
    void SendPacket( const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress);
    
    void ReadIncomingPacketsIntoQueue();
    void ReadIncomingPacket();
    void ProcessQueuedPackets();
  
private:

    /**
     * Inner class to represent a received packet
     */
    class ReceivedPacket
    {
    public:
        ReceivedPacket( float inRecievedTime,
                        InputMemoryBitStream& inInputMemoryBitStream,
                        const SocketAddress& inAddress );

        // get the address this packet came from
        const SocketAddress& GetFromAddress() const { return mFromAddress; }

        // When was this packet received
        float GetRecievedTime() const { return mRecievedTime; }

        // Get the stream of  packet data
        InputMemoryBitStream& GetPacketBuffer() { return mPacketDataStream; }

    private:
        float                 mRecievedTime;
        InputMemoryBitStream  mPacketDataStream;
        SocketAddress         mFromAddress;
    };

    UDPSocketPtr	mSocket;
    //
    std::queue< ReceivedPacket, std::list< ReceivedPacket > > mPacketQueue;
//    WeightedTimedMovingAverage	mBytesReceivedPerSecond;
//    WeightedTimedMovingAverage	mBytesSentPerSecond;
};

#endif