#include "gameobjects/InputState.h"

void WriteSignedBinaryValue( OutputMemoryBitStream& inOutputStream,
                             float inValue )
{
    bool isNonZero = ( inValue != 0.f );
    inOutputStream.Write( isNonZero );

    if ( isNonZero )
    {
        // False if the value is negative, true if positive
        inOutputStream.Write( inValue > 0.f );
    }
}

void ReadSignedBinaryValue( InputMemoryBitStream& inInputStream,
                            float& outValue )
{
    bool isNonZero;
    inInputStream.Read( isNonZero );

    if ( !isNonZero )
    {
        outValue = 0.f;
        return;
    }

    // assign outvalue 1 or -1
    bool isPositive;
    inInputStream.Read( isPositive );
    outValue = isPositive ? 1.f : -1.f;
}

bool InputState::Write( OutputMemoryBitStream& inOutputStream ) const
{
    // write out the horizontal input state
    WriteSignedBinaryValue( inOutputStream, GetDesiredHorizontalDelta() );
    // write out the vertical input state
    WriteSignedBinaryValue( inOutputStream, GetDesiredVerticalDelta() );

    return false;
}

bool InputState::Read( InputMemoryBitStream& inInputStream )
{
    // write out the horizontal input state
    ReadSignedBinaryValue( inInputStream, mRightAmount );
    // write out the vertical input state
    ReadSignedBinaryValue( inInputStream, mForwardAmount );

    return true;
}
