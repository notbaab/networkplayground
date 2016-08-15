//
//  ReplicationManagerServer.h
//  networkplayground
//
//  Created by Erik Parreira on 7/18/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef ReplicationManagerServer_h
#define ReplicationManagerServer_h

#include "IO/MemoryBitStream.h"
#include "networking/ReplicationCommand.h"
#include "networking/ReplicationManagerServer.h"
#include <stdio.h>
#include <unordered_map>
#include <vector>

class ReplicationManagerServer
{
  public:
    void ReplicateCreate( int inNetworkId, uint32_t inInitialDirtyState );
    void ReplicateDestory( int inNetworkId );
    void SetStateDirty( int inNetworkId, uint32_t inDirtyState );
    void HandleCreateAckd( int inNetworkId );
    void RemoveFromReplication( int inNetworkId );

    void Write( OutputMemoryBitStream& inOutputStream,
                ReplicationManagerTransmissionData* ioTransmissinData );

  private:
    uint32_t WriteCreateAction( OutputMemoryBitStream& inOutputStream,
                                int inNetworkId, uint32_t inDirtyState );
    uint32_t WriteUpdateAction( OutputMemoryBitStream& inOutputStream,
                                int inNetworkId, uint32_t inDirtyState );
    uint32_t WriteDestroyAction( OutputMemoryBitStream& inOutputStream,
                                 int inNetworkId, uint32_t inDirtyState );

    std::unordered_map<int, ReplicationCommand> mNetworkIdToReplicationCommand;
    std::vector<int> mNetworkIdsToRemove;
};

#endif /* ReplicationManagerServer_h */
