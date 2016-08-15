#include "gameobjects/MoveList.h"

const Move& MoveList::AddMove( const InputState& inInputState,
                               float inTimeStamp )
{
    float deltaTime =
        mLastMoveTimestamp >= 0.f ? inTimeStamp - mLastMoveTimestamp : 0.f;

    mMoves.emplace_back( inInputState, inTimeStamp, deltaTime );
    mLastMoveTimestamp = inTimeStamp;

    return mMoves.back();
}

bool MoveList::AddMoveIfNew( const Move& inMove )
{
    float timeStamp = inMove.GetTimeStamp();

    // only add if latter than current move
    if ( timeStamp <= mLastMoveTimestamp )
    {
        return false;
    }

    float deltaTime =
        mLastMoveTimestamp >= 0.f ? timeStamp = mLastMoveTimestamp : 0.f;

    mLastMoveTimestamp = timeStamp;

    mMoves.emplace_back( inMove.GetInputState(), timeStamp, deltaTime );
    return true;
}

void MoveList::RemoveProcessedMoves( float inLAstMoveProcessedTimestamp )
{
    // remove moves until we are up to the current time stamp
    while ( !mMoves.empty() &&
            mMoves.front().GetTimeStamp() <= inLAstMoveProcessedTimestamp )
    {
        mMoves.pop_front();
    }
}
