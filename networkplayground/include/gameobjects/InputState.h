//
//  InputState.h
//  networkplayground
//
//  Simple class for reading the inputs storing the player inputs. The inputs
//  are stored as 1 if the button they correspond to is pressed. 
//  Created by Erik Parreira on 7/13/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef InputState_h
#define InputState_h

#include "IO/MemoryBitStream.h"


class InputState
{
public:
    InputState() :
    mLeftAmount( 0 ),
    mRightAmount( 0 ),
    mForwardAmount( 0 ),
    mBackAmount( 0 ),
    mIsShooting( false )
    {}
    
    float GetDesiredHorizontalDelta() const
        { return mRightAmount - mLeftAmount; };
    float GetDesiredVerticalDelta() const
        { return mForwardAmount - mBackAmount; };
    
    bool Write( OutputMemoryBitStream& inOutputStream ) const;
    bool Read( InputMemoryBitStream& inInputStream );
    
private:
    friend class InputManager; // client only class. Why?
    float mLeftAmount, mRightAmount, mForwardAmount, mBackAmount;
    
    bool mIsShooting;
};

#endif /* InputState_h */
