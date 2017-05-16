#include "networking/DeliveryNotificationManager.h"
#include "networking/Logger.h"
#include "timing/Timing.h"

static const float kDelayBeforeAckTimeout = 0.5f;

DeliveryNotificationManager::DeliveryNotificationManager( bool inSendAcks,
                                                          bool inProcessAcks )
    : mNextOutgoingSequenceNumber( 0 ), mNextExpectedSequenceNumber( 0 ),
      mShouldSendAcks( inSendAcks ), mShouldProcessAcks( inProcessAcks ),
      mDeliveredPacketCount( 0 ), mDroppedPacketCount( 0 ),
      mDispatchedPacketCount( 0 )
{
}

DeliveryNotificationManager::~DeliveryNotificationManager()
{
    LOG(Logger::INFO, "DNM destructor. Delivery rate %d%%, Drop rate %d%%",
         ( 100 * mDeliveredPacketCount ) / mDispatchedPacketCount,
         ( 100 * mDroppedPacketCount ) / mDispatchedPacketCount );
}

InFlightPacket* DeliveryNotificationManager::WriteSequenceNumber(
    OutputMemoryBitStream& inOutputStream )
{
    PacketSequenceNumber sequenceNumber = mNextOutgoingSequenceNumber++;
    inOutputStream.Write( sequenceNumber );

    mDispatchedPacketCount++;

    if ( !mShouldProcessAcks )
    {
        return nullptr;
    }

    // Sequence number will be unique for 65,536 packets so it should be good as
    // a key
    mInFlightPackets.emplace_back( sequenceNumber );
    return &mInFlightPackets.back();
}

// The first field in a packet. Process the sequence number if it's one we care
// about
bool DeliveryNotificationManager::ProcessSequenceNumber(
    InputMemoryBitStream& inInputStream )
{
    PacketSequenceNumber sequenceNumber;
    inInputStream.Read( sequenceNumber );
    // LOGMORE("Read %d packet", sequenceNumber);

    // Uh...old packet, we have moved on to greater things
    if ( sequenceNumber < mNextExpectedSequenceNumber )
    {
        return false;
    }

    // The packet is either matching our sequence number or greater than it,
    // either way we move on, we only go forward
    mNextExpectedSequenceNumber = sequenceNumber + 1;
    if ( mShouldSendAcks )
    {
        AddPendingAck( sequenceNumber );
    }

    return true;
}

void DeliveryNotificationManager::WriteAckData(
    OutputMemoryBitStream& inOutputStream )
{
    // What do we need to tell them we have recieved?
    bool hasAcks = ( mPendingAcks.size() > 0 );
    inOutputStream.Write( hasAcks );

    // Don't write anything else
    if ( !hasAcks )
    {
        return;
    }

    // Write out one ack range at a time should be fine...
    mPendingAcks.front().Write( inOutputStream );
    mPendingAcks.pop_front();
}

// Tell inflight packets if they have been dropped or delivered
void DeliveryNotificationManager::ProcessAcks(
    InputMemoryBitStream& inInputStream )
{
    bool hasAcks;
    inInputStream.Read( hasAcks );

    if ( !hasAcks ) // nothing to ack
    {
        return;
    }

    AckRange ackRange;
    ackRange.Read( inInputStream );

    PacketSequenceNumber nextAckdSequenceNumber = ackRange.GetStart();
    uint32_t maxAckdSequenceNumber =
        nextAckdSequenceNumber + ackRange.GetCount();

    while ( nextAckdSequenceNumber < maxAckdSequenceNumber &&
            !mInFlightPackets.empty() )
    {
        const auto& nextInflightPacket = mInFlightPackets.front();

        PacketSequenceNumber nextSequenceNumber =
            nextInflightPacket.GetSequenceNumber();

        if ( nextSequenceNumber < nextAckdSequenceNumber )
        {
            // Copy Droped packet, didn't get an ack for it. Note, don't
            // increment ack number, we need to find the packet for it!
            auto inFlightPacket = nextInflightPacket;
            mInFlightPackets.pop_front();
            HandlePacketDeliveryFailure( inFlightPacket );
        }
        else if ( nextSequenceNumber == nextAckdSequenceNumber )
        {
            HandlePacketDeliverySuccess( nextInflightPacket );
            mInFlightPackets.pop_front();
            ++nextAckdSequenceNumber;
        }
        else if ( nextSequenceNumber > nextAckdSequenceNumber )
        {
            // Well this is embarassing, we already dropped that one...
            nextAckdSequenceNumber++;
        }
    }
}

void DeliveryNotificationManager::ProcessTimedOutPackets()
{
    float timeoutTime = Timing::sInstance.GetTimef() - kDelayBeforeAckTimeout;

    while ( !mInFlightPackets.empty() )
    {
        const auto& nextInFlightPacket = mInFlightPackets.front();

        // if the next packet is to early to remove, the rest are also good!
        if ( nextInFlightPacket.GetTimeDispatched() >= timeoutTime )
        {
            break;
        }

        // remove the packet cause it's old as shit!
        HandlePacketDeliveryFailure( nextInFlightPacket );
        mInFlightPackets.pop_front();
    }
}

void DeliveryNotificationManager::AddPendingAck(
    PacketSequenceNumber inSequenceNumber )
{
    if ( mPendingAcks.size() == 0 ||
         !mPendingAcks.back().ExtendIfShould( inSequenceNumber ) )
    {
        mPendingAcks.emplace_back( inSequenceNumber );
    }
}

void DeliveryNotificationManager::HandlePacketDeliveryFailure(
    const InFlightPacket& inFlightPacket )
{
    ++mDroppedPacketCount;
    inFlightPacket.HandleDeliveryFailure( this );
}

void DeliveryNotificationManager::HandlePacketDeliverySuccess(
    const InFlightPacket& inFlightPacket )
{
    ++mDeliveredPacketCount;
    inFlightPacket.HandleDeliverySuccess( this );
}
