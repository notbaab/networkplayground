#include "networking/ClientProxy.h"
#include "timing/Timing.h"

ClientProxy::ClientProxy(const SocketAddress& inSocketAddress,
                         const std::string& inName, int inPlayerId)
    : mSocketAddress(inSocketAddress), mName(inName), mPlayerId(inPlayerId),
      mDeliveryNotificationManager(false, true),
      mIsLastMoveTimestampDirty(false), mTimeToRespawn(0.f)
{
    UpdateLastPacketTime();
}

void ClientProxy::UpdateLastPacketTime()
{
    mLastPacketFromClientTime = Timing::sInstance.GetTimef();
}
