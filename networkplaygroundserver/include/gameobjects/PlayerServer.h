#ifndef PlayerServer_h
#define PlayerServer_h

#include "gameobjects/Player.h"
#include "networking/NetworkManagerServer.h"

enum PlayerControlType
{
    PCT_HUMAN,
    PCT_AI
};

class PlayerServer : public Player
{
  public:
    static GameObjectPtr StaticCreate()
    {
        return NetworkManagerServer::sInstance->RegisterAndReturn(new PlayerServer());
    }

    virtual void Update();

    void SetCatControlType(PlayerControlType inType) { mControlType = inType; }

  protected:
    PlayerServer();

  private:
    PlayerControlType mControlType;
};
#endif /* PlayerServer_h */
