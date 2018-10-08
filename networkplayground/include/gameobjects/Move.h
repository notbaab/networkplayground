#ifndef Move_h
#define Move_h

#include "IO/MemoryBitStream.h"
#include "gameobjects/InputState.h"

class Move
{
  public:
    Move() {}
    Move(const InputState& inInputState, float inTimeStamp, float inDeltaTime)
        : mInputState(inInputState), mTimeStamp(inTimeStamp),
          mDeltaTime(inDeltaTime)
    {
    }

    const InputState& GetInputState() const { return mInputState; }
    float GetTimeStamp() const { return mTimeStamp; }
    float GetDeltaTime() const { return mDeltaTime; }

    bool Write(OutputMemoryBitStream& inOutputStream) const;
    bool Read(InputMemoryBitStream& inInputStream);

  private:
    InputState mInputState;
    float mTimeStamp;
    float mDeltaTime;
};

#endif /* Move_h */
