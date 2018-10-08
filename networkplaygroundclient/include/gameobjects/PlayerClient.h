#ifndef PlayerClient_h
#define PlayerClient_h

#include "IO/MemoryBitStream.h"
#include "gameobjects/Player.h"
#include "graphics/SpriteComponent.h"

#define DEBUG_DRAW_SERVER_GHOST 1

class Ghost;

class PlayerClient : public Player
{
  public:
    static GameObjectPtr StaticCreate()
    {
        return GameObjectPtr(new PlayerClient());
    }

    virtual void Update() override;
    virtual void Read(InputMemoryBitStream& inInputStream) override;

    void
    DoClientSidePredictionAfterReplicationForLocalPlayer(uint32_t inReadState);

    void InterpolateClientSidePrediction(float inOldRotation,
                                         const Vector3& inOldLocation,
                                         const Vector3& inOldVelocity,
                                         bool inIsForRemotePlayer);

    bool IsLocalPlayer();

  protected:
    PlayerClient();

  private:
    void ApplyUnAckedMoves(uint32_t inReadState);
    void HandleStatePacket(InputMemoryBitStream& inInputStream);

    float mTimeLocationBecameOutOfSync;
    float mTimeVelocityBecameOutOfSync;

    SpriteComponentPtr mSpriteComponent;

    std::shared_ptr<Ghost> mServerGhost;
};
#endif /* PlayerClient_h */
