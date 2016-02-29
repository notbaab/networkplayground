//
//  NetworkManager.h
//  networkplayground
//

//typedef unordered_map< int, GameObjectPtr > IntToGameObjectMap;
class NetworkManager
{
public:
     // 4 byte message identifier
    static const uint32_t   kHelloCC = 'HELO'; // client connect message
    static const uint32_t   kWelcomeCC = 'WLCM'; // server welcome message
    
    NetworkManager();
    virtual ~NetworkManager();
    
    bool Init( uint16_t inPort );
    void ProcessIncomingPackages();
    
    void SendPacket( const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress);
  
private:
    UDPSocketPtr	mSocket;
//    WeightedTimedMovingAverage	mBytesReceivedPerSecond;
//    WeightedTimedMovingAverage	mBytesSentPerSecond;
};
