#ifndef MoveList_h
#define MoveList_h

#include "gameobjects/Move.h"
#include <deque>

class MoveList
{
  public:
    typedef std::deque<Move>::const_iterator const_iterator;
    typedef std::deque<Move>::const_reference const_reverse_iterator;

    MoveList() : mLastMoveTimestamp(-1.f) {}

    const Move& AddMove(const InputState& inInputState, float inTimeStamp);
    bool AddMoveIfNew(const Move& inMove);
    void RemoveProcessedMoves(float inLAstMoveProcessedTimestamp);

    float GetLastMoveTimestamp() const { return mLastMoveTimestamp; }
    const Move& GetLatestmove() const { return mMoves.back(); }
    void Clear() { mMoves.clear(); }
    bool HasMoves() const { return !mMoves.empty(); }
    int GetMoveCount() const { return (int)mMoves.size(); }

    const_iterator begin() const { return mMoves.begin(); }
    const_iterator end() const { return mMoves.end(); }

  private:
    float mLastMoveTimestamp;
    std::deque<Move> mMoves;
};

#endif /* MoveList_h */
