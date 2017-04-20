#include "input/InputManager.h"
#include "timing/Timing.h"
#include "networking/StringUtils.h"

std::unique_ptr<InputManager> InputManager::sInstance;

float kTimeBetweenInputSamples = 0.03f;
// float kTimeBetweenInputSamples = 0.3f;
// float kTimeBetweenInputSamples = 1.0f;

void InputManager::StaticInit() { sInstance.reset( new InputManager() ); }

InputManager::InputManager()
    : mNextTimeToSampleInput( 0.f ), mPendingMove( nullptr )
{
    mNextTimeToSampleInput = Timing::sInstance.GetFrameStartTime();
}

inline void UpdateDesireVariableFromKey( EInputAction inInputAction,
                                         bool& ioVariable )
{
    if ( inInputAction == EIA_Pressed )
    {
        ioVariable = true;
    }
    else if ( inInputAction == EIA_Released )
    {
        ioVariable = false;
    }
}

inline void UpdateDesireFloatFromKey( EInputAction inInputAction,
                                      float& ioVariable )
{
    if ( inInputAction == EIA_Pressed )
    {
        ioVariable = 1.f;
    }
    else if ( inInputAction == EIA_Released )
    {
        ioVariable = 0.f;
    }
}

void InputManager::HandleInput( EInputAction inInputAction, int inKeyCode )
{
    switch ( inKeyCode )
    {
    case 'a':
        UpdateDesireFloatFromKey( inInputAction, mCurrentState.mLeftAmount );
        break;
    case 'd':
        UpdateDesireFloatFromKey( inInputAction, mCurrentState.mRightAmount );
        break;
    case 'w':
        UpdateDesireFloatFromKey( inInputAction, mCurrentState.mForwardAmount );
        break;
    case 's':
        UpdateDesireFloatFromKey( inInputAction, mCurrentState.mBackAmount );
        break;
    case 'k':
        UpdateDesireVariableFromKey( inInputAction, mCurrentState.mIsShooting );
        break;
    default:
        break;
    }
}

const Move& InputManager::SampleInputAsMove()
{
    return mMoveList.AddMove( GetState(),
                              Timing::sInstance.GetFrameStartTime() );
}

bool InputManager::IsTimeToSampleInput()
{
    float time = Timing::sInstance.GetFrameStartTime();
    // LOG("Time %f and %f", time, mNextTimeToSampleInput);
    if ( time > mNextTimeToSampleInput )
    {
        // LOG("Sampling");
        mNextTimeToSampleInput += kTimeBetweenInputSamples;
        return true;
    }
    return false;
}

// Input Should be sampled if the follow criteria is met
// 1. It's time i.e only sample during a frame update.
// 2. There is input to sample
bool InputManager::shouldSampleMove()
{
    return IsTimeToSampleInput() && mCurrentState.HasInput();
}

void InputManager::Update()
{
    // TODO: No.
    if ( shouldSampleMove())
    {
        mPendingMove = &SampleInputAsMove();
    }
}