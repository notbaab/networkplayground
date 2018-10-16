//
//  NetworkManager.h
//  networkplayground
//
#ifndef NetworkedManager_h
#define NetworkedManager_h

#include "IO/MemoryBitStream.h"
#include "gameobjects/GameObject.h"
#include "networking/SocketAddress.h"
#include "networking/UDPSocket.h"
#include <list>
#include <queue>
#include <unordered_map>

// typedef unordered_map< int, GameObjectPtr > IntToGameObjectMap;
class NetworkManager
{
  public:
    // 4 byte message identifier
    static const uint32_t kHelloCC = 'HELO';   // client connect message
    static const uint32_t kWelcomeCC = 'WLCM'; // server welcome message
    static const uint32_t kStateCC = 'STAT';
    static const uint32_t kInputCC = 'INPT';
    static const int kMaxPacketsPerFrameCount = 10;

    NetworkManager();
    virtual ~NetworkManager();

    bool Init(uint16_t inPort);
    void ProcessIncomingPackets();
    virtual void ProcessPacket(InputMemoryBitStream& inInputStream,
                               const SocketAddress& inFromAddress);

    void SendPacket(const OutputMemoryBitStream& inOutputStream,
                    const SocketAddress& inFromAddress);

    void ReadIncomingPacketsIntoQueue();
    void ReadIncomingPacket();
    void ProcessQueuedPackets();

    // TODO: Does this need game object knowledge? Can we abstract it?
    inline GameObjectPtr GetGameObject(int inNetworkId) const;
    void AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject);
    void RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject);

  protected:
    std::unordered_map<int, GameObjectPtr> mNetworkIdToGameObjectMap;
    bool mRecordRequestPackets;
    bool mRecordRespondPackets;

  private:
    /**
     * Inner class to represent a received packet
     */
    class ReceivedPacket
    {
      public:
        ReceivedPacket(float inRecievedTime, InputMemoryBitStream& inInputMemoryBitStream,
                       const SocketAddress& inAddress);

        // get the address this packet came from
        const SocketAddress& GetFromAddress() const { return mFromAddress; }

        // When was this packet received
        float GetReceivedTime() const { return mRecievedTime; }

        // Get the stream of  packet data
        InputMemoryBitStream& GetPacketBuffer() { return mPacketDataStream; }

      private:
        float mRecievedTime;
        InputMemoryBitStream mPacketDataStream;
        SocketAddress mFromAddress;
    };

    std::queue<ReceivedPacket, std::list<ReceivedPacket>> mPacketQueue;
    UDPSocketPtr mSocket;
    int mBytesSentThisFrame;
    float mDropPacketChance;
    float mSimulatedLatency;
    //    WeightedTimedMovingAverage	mBytesReceivedPerSecond;
    //    WeightedTimedMovingAverage	mBytesSentPerSecond;
};

inline GameObjectPtr NetworkManager::GetGameObject(int inNetworkId) const
{
    auto gameObjectIt = mNetworkIdToGameObjectMap.find(inNetworkId);
    if (gameObjectIt != mNetworkIdToGameObjectMap.end())
    {
        return gameObjectIt->second;
    }
    else
    {
        return GameObjectPtr();
    }
}

#endif
