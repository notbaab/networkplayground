//
//  main.cpp
//  unit_tests
//
//  Created by Erik Parreira on 2/22/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"
#include "IO/MemoryBitStream.h"

using std::cout;
using std::endl;

// Test if you can write to the buffer stream
TEST (OutputMemoryBitStream, WritePrimatives)
{
    OutputMemoryBitStream x = OutputMemoryBitStream();
    
    x.Write( 12 ); // write 4 bytes equal to 12
    x.Write( 2 );  // write 4 bytes equal to 2
    x.Write( -2 );  // write 4 bytes equal to -2
    x.Write( 0 );  // write 4 bytes equal to 0
    
    const char* buffer = x.GetBufferPtr();
    
    // Check if each value was written to the stream
    EXPECT_EQ( int(*buffer), 12 );
    buffer = buffer + 4;
    EXPECT_EQ( int(*buffer), 2 );
    buffer = buffer + 4;
    EXPECT_EQ( int(*buffer), -2 );
    buffer = buffer + 4;
    EXPECT_EQ( int(*buffer), 0 );
}

// Testing
TEST (OutputMemoryBitStream, GetBitLength)
{
    OutputMemoryBitStream x = OutputMemoryBitStream();
    
    x.Write( 12 );
    x.Write( 2 );
    x.Write( -2 );
    x.Write( 0 );
    
    uint32_t streamSize = x.GetBitLength();
    
    // 4 entries * bytesInInt * 8 bits
    uint32_t numOfBitsWritten = 4 * sizeof(uint32_t) * 8;
    
    EXPECT_EQ( streamSize, numOfBitsWritten );
    
    x.Write(0);  // Write 32 more bits
    streamSize = x.GetBitLength();
    EXPECT_EQ( streamSize, numOfBitsWritten + sizeof(uint32_t) * 8 );

    x.Write(true);  // 2 more bits
    x.Write(true);
    streamSize = x.GetBitLength();
    EXPECT_EQ( streamSize, numOfBitsWritten + sizeof(uint32_t) * 8 + 2 );
}

TEST (OutputMemoryBitStream, GetByteLength)
{
    OutputMemoryBitStream x = OutputMemoryBitStream();
    
    x.Write( 12 );
    x.Write( 2 );
    x.Write( true );
    
    uint32_t streamSize = x.GetByteLength();
    
    uint32_t numOfBytesWritten = 9;
    
    EXPECT_EQ(streamSize, numOfBytesWritten);
    
    x.Write(true);  // Writing one a bool won't add another bit
    EXPECT_EQ( streamSize, numOfBytesWritten );

    x.Write(8);  // Write 32 more bits
    streamSize = x.GetByteLength();
    
    EXPECT_NE( streamSize, numOfBytesWritten );
}

TEST (OutputMemoryBitStream, TestScratch)
{
    OutputMemoryBitStream x = OutputMemoryBitStream();
    
    x.Write( 12 );
    x.Write( std::string("s") );
    std::string b = "blahblahb";
    x.Write(b);
    
    
    x.PrintByteArray();
}


// TODO: This was needed for xcode testing but not the command line
// int main(int argc, char * argv[])
// {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
