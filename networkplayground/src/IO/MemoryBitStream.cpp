//
//  MemoryBitStream.cpp
//  networkplayground
//
//  Created by Erik Parreira on 2/21/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include "IO/MemoryBitStream.h"

using std::cout;
using std::endl;

void OutputMemoryBitStream::ReallocBuffer( uint32_t inNewBitLength )
{
    if( mBuffer == nullptr )
    {
        //just need to memset on first allocation
        mBuffer = static_cast<char*>( std::malloc( inNewBitLength >> 3 ) );
        memset( mBuffer, 0, inNewBitLength >> 3 );
    }
    else
    {
        //need to memset, then copy the buffer
        char* tempBuffer = static_cast<char*>( std::malloc( inNewBitLength >> 3 ) );
        memset( tempBuffer, 0, inNewBitLength >> 3 );
        memcpy( tempBuffer, mBuffer, mBitCapacity >> 3 );
        std::free( mBuffer );
        mBuffer = tempBuffer;
    }
    
    //handle realloc failure
    //...
    mBitCapacity = inNewBitLength;
}

void OutputMemoryBitStream::WriteBits( uint8_t inData,
                                      uint32_t inBitCount )
{
    uint32_t nextBitHead = mBitHead + static_cast< uint32_t >( inBitCount );
    
    if( nextBitHead > mBitCapacity )
    {
        ReallocBuffer( std::max( mBitCapacity * 2, nextBitHead ) );
    }
    
    //calculate the byteOffset into our buffer
    //by dividing the head by 8
    //and the bitOffset by taking the last 3 bits
    uint32_t byteOffset = mBitHead >> 3;
    uint32_t bitOffset = mBitHead & 0x7;
    
    uint8_t currentMask = ~( 0xff << bitOffset );
    mBuffer[ byteOffset ] = ( mBuffer[ byteOffset ] & currentMask ) | ( inData << bitOffset );
    
    //calculate how many bits were not yet used in
    //our target byte in the buffer
    uint32_t bitsFreeThisByte = 8 - bitOffset;
    
    //if we needed more than that, carry to the next byte
    if( bitsFreeThisByte < inBitCount )
    {
        //we need another byte
        mBuffer[ byteOffset + 1 ] = inData >> bitsFreeThisByte;
    }
    
    mBitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits( const void* inData, uint32_t inBitCount )
{
    const char* srcByte = static_cast< const char* >( inData );
    //write all the bytes
    while( inBitCount > 8 )
    {
        WriteBits( *srcByte, 8 );
        ++srcByte;
        inBitCount -= 8;
    }
    //write anything left
    if( inBitCount > 0 )
    {
        WriteBits( *srcByte, inBitCount );
    }
}

void OutputMemoryBitStream::PrintByteArray()
{
    uint32_t bytesToWrite = GetByteLength();
    const char* buffer = GetBufferPtr();
    printf("printing %d bytes\n", bytesToWrite);
    
    for (int i = 0; i < bytesToWrite; i++) {
        printf( "%x", static_cast<char>( buffer[i] ) );
//        cout << "Value of x  " << std::hex << static_cast<void*>(bufferStart) << "  hexadecimal" << endl;
//        cout << bufferStart << " ";
//        bufferStart++;
    }
    
    cout << endl;
    
}
