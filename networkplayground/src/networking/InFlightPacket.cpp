#include "networking/InFlightPacket.h"
#include "timing/Timing.h"

InFlightPacket::InFlightPacket(PacketSequenceNumber inSequenceNumber)
    : mSequenceNumber(inSequenceNumber),
      mTimeDispatched(Timing::sInstance.GetTimef())
{
}

void InFlightPacket::HandleDeliveryFailure(
    DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    for (const auto& pair : mTransmissionDataMap)
    {
        pair.second->HandleDeliveryFailure(inDeliveryNotificationManager);
    }
}

void InFlightPacket::HandleDeliverySuccess(
    DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    for (const auto& pair : mTransmissionDataMap)
    {
        pair.second->HandleDeliverySuccess(inDeliveryNotificationManager);
    }
}
