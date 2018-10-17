#ifndef InputState_h
#define InputState_h

#include "IO/MemoryBitStream.h"

class InputState
{
  public:
    InputState()
        : mLeftAmount(0), mRightAmount(0), mForwardAmount(0), mBackAmount(0), mIsShooting(false)
    {
    }

    float GetDesiredHorizontalDelta() const { return mRightAmount - mLeftAmount; };
    float GetDesiredVerticalDelta() const { return mForwardAmount - mBackAmount; };

    bool HasInput();

    bool Write(OutputMemoryBitStream& inOutputStream) const;
    bool Read(InputMemoryBitStream& inInputStream);

    float mLeftAmount, mRightAmount, mForwardAmount, mBackAmount;
    bool mIsShooting;
};

#endif /* InputState_h */
