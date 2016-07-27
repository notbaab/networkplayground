//
//  Server.h
//  networkplayground
//
//  Created by Erik Parreira on 7/25/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef Server_h
#define Server_h

#include "gameobjects/Engine.h"
#include "networking/ClientProxy.h"

class Server : public Engine
{
  public:
    static bool StaticInit();

    virtual void DoFrame() override;

    virtual int Run() override;

    void HandleNewClient( ClientProxyPtr inClientProxy );
    void HandleLostClient( ClientProxyPtr inClientProxy );

  private:
    Server();

    bool InitNetworkManager();
    void SetupWorld();
};

#endif /* Server_h */
