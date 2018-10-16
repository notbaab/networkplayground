#include "networking/NetworkManager.h"
#include "gameobjects/GameObject.h"
#include "math/Random.h"
#include "networking/Logger.h"
#include "networking/SocketUtil.h"
#include "timing/Timing.h"

#include <ctime>

// TODO: Temp for now
void printStream(InputMemoryBitStream& inInputStream);

NetworkManager::NetworkManager()
    : mRecordRequestPackets(false), mRecordRespondPackets(false), mBytesSentThisFrame(0),
      mDropPacketChance(0.f), mSimulatedLatency(0.f)
{
}

NetworkManager::~NetworkManager() {}
bool NetworkManager::Init(uint16_t inPort)
{
    // Create the underlaying socket for this port
    mSocket = SocketUtil::CreateUDPSocket(INET);

    // Create a wildcard input address to bind to
    SocketAddress ownAddress(INADDR_ANY, inPort);
    mSocket->Bind(ownAddress);

    //    mBytesReceivedPerSecond = WeightedTimedMovingAverage( 1.f );
    //    mBytesSentPerSecond = WeightedTimedMovingAverage( 1.f );

    // did we bind okay?
    if (mSocket == nullptr)
    {
        return false;
    }

    if (mSocket->SetNonBlockingMode(true) != NO_ERROR)
    {
        return false;
    }

    return true;
}

// Wrapper for doing all the network related tasks
void NetworkManager::ProcessIncomingPackets()
{
    ReadIncomingPacketsIntoQueue();
    ProcessQueuedPackets();

    // UpdateBytesSentLastFrame();
}

void NetworkManager::ProcessQueuedPackets()
{
    // look at the front packet...
    while (!mPacketQueue.empty())
    {
        ReceivedPacket& nextPacket = mPacketQueue.front();

        // Log(Logger::INFO, "Time %2.2f, other time %2.2f",
        //         Timing::sInstance.GetTimef(), nextPacket.GetReceivedTime());
        // TODO: Don't break, we will need to simulate jitter and out of order
        // packets
        if (Timing::sInstance.GetTimef() < nextPacket.GetReceivedTime())
        {
            // Log(Logger::INFO, "Simulating Lag: Time %.f, other time %.f",
            //     Timing::sInstance.GetTimef(), nextPacket.GetReceivedTime());
            break;
        }

        ProcessPacket(nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress());
        mPacketQueue.pop();
    }
}

void NetworkManager::ProcessPacket(InputMemoryBitStream& inInputStream,
                                   const SocketAddress& inFromAddress)
{

    // For now, just output the packet data
    // for(int i = 0 ; i < readByteCount ; i ++ )
    // {
    //     std::cout << packetMem[i] ;//Looping 5 times to print out
    //     [0],[1],[2],[3],[4]
    // }
}

void NetworkManager::ReadIncomingPacketsIntoQueue()
{
    int totalPacketsRecieved = 0;
    int totalBytesRead = 0;

    SocketAddress fromAddress;
    // TODO: Find out why I can't pass kMaxPacketSize to the vector constructor
    const int packetSize = kMaxPacketSize;

    while (totalPacketsRecieved < kMaxPacketsPerFrameCount)
    {
        auto packetMem = std::make_shared<std::vector<char>>(packetSize);

        // Stream we will read each packet into
        InputMemoryBitStream inputStream(packetMem, kPacketSizeInBits);
        // mSocket->ReceiveFrom(void *inToReceive, int inMaxLength, SocketAddress &outFromAddress)
        int readByteCount = mSocket->ReceiveFrom(packetMem->data(), kMaxPacketSize, fromAddress);

        if (readByteCount == 0)
        {
            // nothing to read
            break;
        }
        else if (readByteCount == -WSAECONNRESET)
        {
            // got a connection reset? In a udp socket?
            INFO("Client disconnected");
            break;
        }

        // Resize input stream to the number of bytes
        inputStream.ResetToCapacity(readByteCount);
        ++totalPacketsRecieved;
        totalBytesRead += readByteCount;

        if (Math::GetRandomFloat() < mDropPacketChance)
        {
            DEBUG("Simulated a dropped packet!");
            break;
        }

        float recievedTime = Timing::sInstance.GetTimef() + mSimulatedLatency;
        // float recievedTime = Timing::sInstance.GetTimef();

        // create a new RecievedPacket, copying the memory stream
        mPacketQueue.emplace(recievedTime, inputStream, fromAddress);

        // Eh, probably not really need at this point
        if (mRecordRequestPackets)
        {
            // TODO: Record should just pass the shared stream pointer to
            // something
            inputStream.printStream();
        }
    }
    if (totalPacketsRecieved != 0)
    {
        DEBUG("Read {} packets", totalPacketsRecieved);
    }
}

void NetworkManager::SendPacket(const OutputMemoryBitStream& inOutputStream,
                                const SocketAddress& inFromAddress)
{
    int streamByteLength = inOutputStream.GetByteLength();
    const void* packetDataToSend = inOutputStream.GetBufferPtr();

    int sentByteCount = mSocket->SendTo(packetDataToSend, streamByteLength, inFromAddress);

    if (mRecordRespondPackets)
    {
        inOutputStream.printStream();
    }

    if (sentByteCount)
    {
        // TODO: Keep track of how many bytes have been sent
        //        mBytesSentThisFrame += sentByteCount;
    }
}

NetworkManager::ReceivedPacket::ReceivedPacket(float inReceivedTime,
                                               InputMemoryBitStream& inInputMemoryBitStream,
                                               const SocketAddress& inFromAddress)
    : mRecievedTime(inReceivedTime), mPacketDataStream(inInputMemoryBitStream),
      mFromAddress(inFromAddress)
{
}

void NetworkManager::AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
    mNetworkIdToGameObjectMap[inGameObject->GetNetworkId()] = inGameObject;
}

void NetworkManager::RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
    mNetworkIdToGameObjectMap.erase(inGameObject->GetNetworkId());
}
