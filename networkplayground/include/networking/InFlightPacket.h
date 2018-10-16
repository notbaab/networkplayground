#ifndef InflightPacket_h
#define InflightPacket_h

#include "networking/TransmissionData.h"
#include <cstdint>
#include <memory>
#include <unordered_map>

class DeliveryNotificationManager;

typedef std::uint16_t PacketSequenceNumber;

class InFlightPacket
{
  public:
    InFlightPacket(PacketSequenceNumber inSequenceNumber);

    PacketSequenceNumber GetSequenceNumber() const { return mSequenceNumber; }
    float GetTimeDispatched() const { return mTimeDispatched; }

    void SetTransmissionData(int inKey, TransmissionDataPtr inTranmissiondata)
    {
        mTransmissionDataMap[inKey] = inTranmissiondata;
    }

    const TransmissionDataPtr GetTransmissionData(int key) const
    {
        auto it = mTransmissionDataMap.find(key);
        return (it != mTransmissionDataMap.end() ? it->second : nullptr);
    }

    void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const;

    void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;

  private:
    PacketSequenceNumber mSequenceNumber;
    float mTimeDispatched;

    std::unordered_map<int, TransmissionDataPtr> mTransmissionDataMap;
};
#endif /* InflightPacket_h */
