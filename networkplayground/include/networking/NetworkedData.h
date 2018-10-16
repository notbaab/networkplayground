#ifndef NetworkedData_h
#define NetworkedData_h

class NetworkedData
{
  public:
    virtual NetworkedData(uint32_t identifier) = 0;
    ~NetworkedData();
    virtual void ReadNetworkedData(const OutputMemoryBitStream& inOutputStream) = 0;
    virtual void WriteNetworkedData(InputMemoryBitStream& inInputMemoryBitStream) = 0;

    // TODO: Play with the idea of 'registering' a member variable. That would
    // basically mean we would store the variables address and the size of it,
    // from there there would be a read and write function that just goes
    // down the list of address size pairs ands reads from or writes to that
    // address
};

#endif