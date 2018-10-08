#ifndef InputManager_h
#define InputManager_h

#include "gameobjects/InputState.h"
#include "gameobjects/MoveList.h"
#include <memory>

class Move;

enum EInputAction
{
    EIA_Pressed,
    EIA_Repeat,
    EIA_Released,
};

class InputManager
{
  public:
    static void StaticInit();
    static std::unique_ptr<InputManager> sInstance;

    void HandleInput(EInputAction inInputAction, int inKeyCode);

    const InputState& GetState() const { return mCurrentState; }
    MoveList& GetMoveList() { return mMoveList; }

    const Move* GetAndClearPendingMove()
    {
        auto toRet = mPendingMove;
        mPendingMove = nullptr;
        return toRet;
    }

    void Update();

  private:
    InputState mCurrentState;

    InputManager();

    bool IsTimeToSampleInput();
    bool shouldSampleMove();
    const Move& SampleInputAsMove();

    MoveList mMoveList;
    float mNextTimeToSampleInput;
    const Move* mPendingMove;
};

#endif /* InputManager_h */
