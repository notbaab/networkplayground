#ifndef ReplicationCommand_h
#define ReplicationCommand_h

#include "IO/MemoryBitStream.h"

enum ReplicationAction
{
    RA_CREATE=1,
    RA_UPDATE,
    RA_DESTROY,
    RA_RPC,
    RA_MAX
};

class ReplicationManagerTransmissionData;

struct ReplicationCommand
{
  public:
    ReplicationCommand() {}
    ReplicationCommand( uint32_t inIntialDirtyState )
        : mAction( RA_CREATE ), mDirtyState( inIntialDirtyState )
    {
    }

    // our create command was acked, stop sending create packets and send
    // just update ones
    void HandleCreateAckd()
    {
        if ( mAction == RA_CREATE )
            mAction = RA_UPDATE;
    }

    void AddDirtyState( uint32_t inState ) { mDirtyState |= inState; }

    bool HasDirtyState() const
    {
        return ( mAction == RA_DESTROY ) || ( mDirtyState != 0 );
    }

    ReplicationAction GetAction() const { return mAction; }
    uint32_t GetDirtyState() const { return mDirtyState; }
    inline void ClearDirtyState( uint32_t inStateToClear );

    //    void Write( OutputMemryBitStream& inStream, int inNetworkId,
    //                ReplicationManagerTransmissionData* ioTransactionData );
    //    void Read( InputMemoryBitStream& inStream, int inNetworkId );

  private:
    uint32_t mDirtyState;
    ReplicationAction mAction;
};

inline void ReplicationCommand::ClearDirtyState( uint32_t inStateToClear )
{
    mDirtyState &= ~inStateToClear;

    if ( mAction == RA_DESTROY )
    {
        // Don't destroy the object if we want to clear it's state
        mAction = RA_UPDATE;
    }
}

#endif /* ReplicationCommand_h */
