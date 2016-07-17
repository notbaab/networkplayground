//
//  Move.cpp
//  networkplayground
//
//  Created by Erik Parreira on 7/14/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include "gameobjects/Move.h"

bool Move::Write( OutputMemoryBitStream &inOutputStream ) const
{
    mInputState.Write( inOutputStream );
    inOutputStream.Write( mTimeStamp );
    
    return true;
}

bool Move::Read( InputMemoryBitStream &inInputStream )
{
    mInputState.Read( inInputStream );
    inInputStream.Read( mTimeStamp );
    
    return true;
}
