#include "networking/ReplicationManagerTransmissionData.h"
#include "networking/DeliveryNotificationManager.h"
#include "networking/Logger.h"
#include "networking/NetworkManagerServer.h"

void ReplicationManagerTransmissionData::AddTransmission(
    int inNetworkId, ReplicationAction inAction, uint32_t inState)
{
    mTransmissions.emplace_back(inNetworkId, inAction, inState);
}

// ==========================Failure handlers===================================

// What do we do when it fails
void ReplicationManagerTransmissionData::HandleDeliveryFailure(
    DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    // run through the transmissions
    for (const ReplicationTransmission& rt : mTransmissions)
    {
        // is it a create? then we have to redo the create.
        int networkId = rt.GetNetworkId();

        switch (rt.GetAction())
        {
        case RA_CREATE:
            HandleCreateDeliveryFailure(networkId);
            break;
        case RA_UPDATE:
            HandleUpdateStateDeliveryFailure(networkId, rt.GetState(),
                                             inDeliveryNotificationManager);
            break;
        case RA_DESTROY:
            HandleDestroyDeliveryFailure(networkId);
            break;
        default:
            TRACE("No failure handler for {}", rt.GetAction());
        }
    }
}

// Just sends the create again if the object still exists
void ReplicationManagerTransmissionData::HandleCreateDeliveryFailure(
    int inNetworkId) const
{
    GameObjectPtr gameObject =
        NetworkManagerServer::sInstance->GetGameObject(inNetworkId);

    if (!gameObject)
    {
        return;
    }

    // Failed? Send it again
    mReplicationManagerServer->ReplicateCreate(inNetworkId,
                                               gameObject->GetAllStateMask());
}

void ReplicationManagerTransmissionData::HandleDestroyDeliveryFailure(
    int inNetworkId) const
{
    // Destroy it again!
    mReplicationManagerServer->ReplicateDestory(inNetworkId);
}

void ReplicationManagerTransmissionData::HandleUpdateStateDeliveryFailure(
    int inNetworkId, uint32_t inState,
    DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    if (!NetworkManagerServer::sInstance->GetGameObject(inNetworkId))
    {
        // object is dead, don't bother with anything
        return;
    }

    // TODO: Not dead, go through the inflight packets to see if the state that
    // failed was included an any future packet. If not, the the
    // ReplicationManagerServer to set this objects state as dirty
    for (const auto& inFlightPacket :
         inDeliveryNotificationManager->GetInFlightPackets())
    {
        ReplicationManagerTransmissionDataPtr rmtdp =
            std::static_pointer_cast<ReplicationManagerTransmissionData>(
                inFlightPacket.GetTransmissionData('RPLM'));

        for (const ReplicationTransmission& otherRT : rmtdp->mTransmissions)
        {
            // Mask out the state that is being transmitted
            inState &= ~otherRT.GetState();
        }
    }

    // if there's still any dirty state, mark it
    if (inState)
    {
        mReplicationManagerServer->SetStateDirty(inNetworkId, inState);
    }
}

void ReplicationManagerTransmissionData::HandleDeliverySuccess(
    DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    for (const ReplicationTransmission& rt : mTransmissions)
    {
        switch (rt.GetAction())
        {
        case RA_CREATE:
            HandleCreateDeliverySuccess(rt.GetNetworkId());
            break;
        case RA_DESTROY:
            HandleDestroyDeliverySuccess(rt.GetNetworkId());
            break;
        default:
            ERROR("Can't handle delivery success for {} with network id {}",
                  rt.GetAction(), rt.GetNetworkId());
            continue;
            // LOG( "No success handler for {}", rt.GetAction() );
        }
    }
}

void ReplicationManagerTransmissionData::HandleCreateDeliverySuccess(
    int inNetworkId) const
{
    // Tell the server the create succeded for this object
    DEBUG("Create delivery success for id {}", inNetworkId);
    mReplicationManagerServer->HandleCreateAckd(inNetworkId);
}

void ReplicationManagerTransmissionData::HandleDestroyDeliverySuccess(
    int inNetworkId) const
{
    mReplicationManagerServer->RemoveFromReplication(inNetworkId);
}
