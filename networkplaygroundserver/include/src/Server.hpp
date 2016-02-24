//
//  Server.hpp
//  networkplayground
//
//  Created by Erik Parreira on 2/21/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

class Server //: public Engine
{
public:

    // Allow only one Server instance
    static bool StaticInit();
    
    // Simulate a frame of gameplay
    virtual void DoFrame() override;
    
    // 
    virtual int Run();
    
    void HandleNewClient( ClientProxyPtr inClientProxy );
    void HandleLostClient( ClientProxyPtr inClientProxy );
    
    RoboCatPtr	GetCatForPlayer( int inPlayerId );
    void	SpawnCatForPlayer( int inPlayerId );
    
    
private:
    Server();
    
    bool	InitNetworkManager();
    void	SetupWorld();
    
};
