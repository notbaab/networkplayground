#ifndef Player_h
#define Player_h

#include "gameobjects/GameObject.h"
#include "gameobjects/InputState.h"

#include "gameobjects/PlayerMessage.h"

#define PLAYER_CLASS_IDENTIFICATION 0xffffffff
const float PLAYER_SPEED = 500.f;

class Player : public GameObject
{
  public:
    friend class PlayerMessage;

    CLASS_IDENTIFICATION(PLAYER_CLASS_IDENTIFICATION, Player)

    enum PlayerReplicationState
    {
        PRS_PID = 1 << 0,  // Player Id
        PRS_POSI = 1 << 1, // Player position and rotation
        ALL_STATE = PRS_POSI | PRS_PID,
    };
    virtual uint32_t GetAllStateMask() const override { return ALL_STATE; }
    virtual void Update() override;

    void ProcessInput(float inDeltaTime, const InputState& inInputState);
    void SimulateMovement(float inDeltaTime);

    void SetPlayerId(uint32_t inPlayerId) { mPlayerId = inPlayerId; }
    uint32_t GetPlayerId() { return mPlayerId; }

    static GameObject* StaticCreate() { return new Player(); }

    void SetVelocity(const Vector3& inVelocity) { mVelocity = inVelocity; }
    const Vector3& GetVelocity() const { return mVelocity; }

    virtual uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) override;

  protected:
    Player();

  private:
    void AdjustVelocityByThrust(float inDeltaTime);

    uint32_t mPlayerId;
    Vector3 mVelocity;
    float mLastMoveTimestamp;
    PlayerMessage::ReplicationState mState;
};

typedef std::shared_ptr<Player> PlayerPtr;
#endif /* Player_h */
