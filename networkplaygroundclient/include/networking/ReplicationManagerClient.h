#ifndef ReplicationManagerClient_h
#define ReplicationManagerClient_h

class InputMemoryBitStream;

class ReplicationManagerClient
{
  public:
    void Read( InputMemoryBitStream& inInputStream );

  private:
    void ReadAndDoCreateAction( InputMemoryBitStream& inInputStream,
                                int inNetworkId );
    void ReadAndDoUpdateAction( InputMemoryBitStream& inInputStream,
                                int inNetworkId );
    void ReadAndDoDestroyAction( InputMemoryBitStream& inInputStream,
                                 int inNetworkId );
};

#endif /* ReplicationManagerClient_h */
