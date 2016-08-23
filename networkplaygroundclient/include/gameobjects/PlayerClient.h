#ifndef PlayerClient_h
#define PlayerClient_h

#include "IO/MemoryBitStream.h"
#include "gameobjects/Player.h"
#include "graphics/SpriteComponent.h"

class PlayerClient : public Player
{
  public:
    static GameObjectPtr StaticCreate()
    {
        return GameObjectPtr( new PlayerClient() );
    }

    virtual void Update() override;
    virtual void Read( InputMemoryBitStream& inInputStream ) override;

    void DoClientSidePredectionAfterReplicationForLocalPlayer(
        uint32_t inReadState );

    void DoClientSidePredectionAfterReplicationForRemotePlayer(
        uint32_t inReadState );

  protected:
    PlayerClient();

  private:
    void InterpolateClientSidePrediction( const Vector3& inOldLocation,
                                          const Vector3& inOldVelocity,
                                          bool inIsRemotePlayer );

    float mTimeLocationBecameOutOfSync;
    float mTimeVelocityBecameOutOfSync;

    SpriteComponentPtr mSpriteComponent;
};
#endif /* PlayerClient_h */
