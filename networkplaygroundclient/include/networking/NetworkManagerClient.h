#ifndef NetworkManagerClient_h
#define NetworkManagerClient_h

#include "WeightedTimedMovingAverage.h"
//#include "WeightedTimedMovingAverage.h"
#include "networking/DeliveryNotificationManager.h"
#include "networking/NetworkManager.h"
#include "networking/ReplicationManagerClient.h"
#include "networking/SocketAddress.h"

class InputMemoryBitStream;

class NetworkManagerClient : public NetworkManager
{
    enum NetworkClientState
    {
        NCS_Uninitialized,
        NCS_SayingHello,
        NCS_Welcomed
    };

  public:
    static NetworkManagerClient* sInstance;

    static void StaticInit( const SocketAddress& inServerAddress,
                            const std::string& inName );

    void SendOutgoingPackets();

    virtual void ProcessPacket( InputMemoryBitStream& inInputStream,
                                const SocketAddress& inFromAddress ) override;

    const WeightedTimedMovingAverage& GetAvgRoundTripTime() const
    {
        return mAvgRoundTripTime;
    }

    float GetRoundTripTime() const { return mAvgRoundTripTime.GetValue(); }

    int GetPlayerId() const { return mPlayerId; }
    float GetLastMoveProcessedByServerTimestamp() const
    {
        return mLastMoveProcessedByServerTimestamp;
    }

  private:
    NetworkManagerClient();
    void Init( const SocketAddress& inServerAddress,
               const std::string& inName );

    void UpdateSayingHello();
    void SendHelloPacket();

    // Packet handling functions
    void HandleWelcomePacket( InputMemoryBitStream& inInputStream );
    void HandleStatePacket( InputMemoryBitStream& inInputStream );
    void ReadLastMoveProcessedOnServerTimestamp(
        InputMemoryBitStream& inInputStream );

    void HandleGameObjectState( InputMemoryBitStream& inInputStream );

    void UpdateSendingInputPacket();
    void SendInputPacket();

    void DestroyGameObjectsInMap(
        std::unordered_map<int, GameObjectPtr>& inObjectsToDestroy );

    ReplicationManagerClient mReplicationManagerClient;
    DeliveryNotificationManager mDeliveryNotificationManager;

    SocketAddress mServerAddress;
    NetworkClientState mState;

    float mTimeOfLastHello;
    float mTimeOfLastInputPacket;

    std::string mName = "dude";
    int mPlayerId;

    float mLastMoveProcessedByServerTimestamp;
    WeightedTimedMovingAverage mAvgRoundTripTime;
    float mLastRoundTripTime;
};

#endif /* NetworkManagerClient_h */
