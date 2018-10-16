#ifndef ClientProxy_h
#define ClientProxy_h

#include "gameobjects/InputState.h"
#include "gameobjects/MoveList.h"
#include "networking/DeliveryNotificationManager.h"
#include "networking/ReplicationManagerServer.h"
#include "networking/SocketAddress.h"
#include <memory>
#include <string>

// class InputState;

class ClientProxy
{
  public:
    ClientProxy(const SocketAddress& inSocketAddres, const std::string& inName, int inPlayerId);

    const SocketAddress& GetSocketAddress() const { return mSocketAddress; }
    int GetPlayerId() const { return mPlayerId; }
    const std::string& GetName() const { return mName; }

    void SetInputState(const InputState& inInputState) { mInputState = inInputState; }
    const InputState& GetInputState() const { return mInputState; }

    void UpdateLastPacketTime();
    float GetLastPacketFromClientTime() const { return mLastPacketFromClientTime; }

    const MoveList& GetUnprocessedMoveList() const { return mUnprocessedMoveList; }
    MoveList& GetUnprocessedMoveList() { return mUnprocessedMoveList; }

    void SetIsLastMoveTimestampDirty(bool inIsDirty) { mIsLastMoveTimestampDirty = inIsDirty; }

    DeliveryNotificationManager& GetDeliveryNotificationManager()
    {
        return mDeliveryNotificationManager;
    }
    ReplicationManagerServer& GetReplicationManagerServer() { return mReplicationManagerServer; }

    bool IsLastMoveTimestampDirty() const { return mIsLastMoveTimestampDirty; }

  private:
    DeliveryNotificationManager mDeliveryNotificationManager;
    ReplicationManagerServer mReplicationManagerServer;

    SocketAddress mSocketAddress;
    InputState mInputState;
    std::string mName;
    int mPlayerId;

    float mLastPacketFromClientTime;
    float mTimeToRespawn;

    MoveList mUnprocessedMoveList;
    bool mIsLastMoveTimestampDirty;
};

typedef std::shared_ptr<ClientProxy> ClientProxyPtr;
#endif /* ClientProxy_h */
