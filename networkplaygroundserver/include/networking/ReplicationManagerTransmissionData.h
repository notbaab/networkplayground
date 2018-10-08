#ifndef ReplicationManagerTransmissionData_h
#define ReplicationManagerTransmissionData_h

#include "networking/ReplicationCommand.h"
#include "networking/TransmissionData.h"
#include <vector>

class ReplicationManagerServer;

class ReplicationManagerTransmissionData : public TransmissionData
{
  public:
    static const uint32_t kReplicationTransmissionData = 'RPLM';

    ReplicationManagerTransmissionData(
        ReplicationManagerServer* inReplicationManagerServer)
        : mReplicationManagerServer(inReplicationManagerServer)
    {
    }

    class ReplicationTransmission
    {
      public:
        ReplicationTransmission(int inNetworkId, ReplicationAction inAction,
                                uint32_t inState)
            : mNetworkId(inNetworkId), mAction(inAction), mState(inState)
        {
        }

        int GetNetworkId() const { return mNetworkId; }
        ReplicationAction GetAction() const { return mAction; }
        uint32_t GetState() const { return mState; }

      private:
        int mNetworkId;
        ReplicationAction mAction;
        uint32_t mState;
    };

    void AddTransmission(int inNetworkId, ReplicationAction inAction,
                         uint32_t instate);

    virtual void HandleDeliveryFailure(
        DeliveryNotificationManager* inDeliveryNotificationManager)
        const override;

    virtual void HandleDeliverySuccess(
        DeliveryNotificationManager* inDeliveryNotificationManager)
        const override;

  private:
    // Failure handlers
    void HandleCreateDeliveryFailure(int inNetworkId) const;
    void HandleUpdateStateDeliveryFailure(
        int inNetworkId, uint32_t inState,
        DeliveryNotificationManager* inDeliveryNotificationManager) const;
    void HanldeDestroyDeliveryFailure(int inNetworkId) const;

    // Success handlers
    void HandleDestroyDeliveryFailure(int inNetworkId) const;
    void HandleCreateDeliverySuccess(int inNetworkId) const;
    void HandleDestroyDeliverySuccess(int inNetworkId) const;

    ReplicationManagerServer* mReplicationManagerServer;
    std::vector<ReplicationTransmission> mTransmissions;
};

typedef std::shared_ptr<ReplicationManagerTransmissionData>
    ReplicationManagerTransmissionDataPtr;

#endif /* ReplicationManagerTransmissionData_h */
