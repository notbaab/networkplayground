#include "IO/MemoryBitStream.h"
#include "networking/Logger.h"
#include <bitset>

using std::cout;
using std::endl;

void printStream( uint32_t bufferSize, const char* streamBuffer )
{
    Logger::Log( Logger::DEBUG, "Hex: " );
    for ( int charIdx = 0; charIdx < bufferSize; charIdx++ )
    {
        Logger::Log( Logger::DEBUG,"%x", streamBuffer[charIdx] );
    }
    Logger::Log( Logger::DEBUG,"\nBinary: " );

    //    std::cout << std::endl << "As Binary " << std::endl;

    for ( int charIdx = 0; charIdx < bufferSize; charIdx++ )
    {
        std::bitset<8> asBits( streamBuffer[charIdx] );
        std::string asString = asBits.to_string<char, std::string::traits_type,
                                                std::string::allocator_type>();

        Logger::Log( Logger::DEBUG,asString.c_str() );
    }
}

void OutputMemoryBitStream::ReallocBuffer( uint32_t inNewBitLength )
{
    if ( mBuffer == nullptr )
    {
        // just need to memset on first allocation
        mBuffer = static_cast<char*>( std::malloc( inNewBitLength >> 3 ) );
        memset( mBuffer, 0, inNewBitLength >> 3 );
    }
    else
    {
        // need to memset, then copy the buffer
        char* tempBuffer =
            static_cast<char*>( std::malloc( inNewBitLength >> 3 ) );
        memset( tempBuffer, 0, inNewBitLength >> 3 );
        memcpy( tempBuffer, mBuffer, mBitCapacity >> 3 );
        std::free( mBuffer );
        mBuffer = tempBuffer;
    }

    // handle realloc failure
    //...
    mBitCapacity = inNewBitLength;
}

void OutputMemoryBitStream::WriteBits( uint8_t inData, uint32_t inBitCount )
{
    uint32_t nextBitHead = mBitHead + static_cast<uint32_t>( inBitCount );

    if ( nextBitHead > mBitCapacity )
    {
        ReallocBuffer( std::max( mBitCapacity * 2, nextBitHead ) );
    }

    // calculate the byteOffset into our buffer
    // by dividing the head by 8
    // and the bitOffset by taking the last 3 bits
    uint32_t byteOffset = mBitHead >> 3;
    uint32_t bitOffset = mBitHead & 0x7;

    uint8_t currentMask = ~( 0xff << bitOffset );
    mBuffer[byteOffset] =
        ( mBuffer[byteOffset] & currentMask ) | ( inData << bitOffset );

    // calculate how many bits were not yet used in
    // our target byte in the buffer
    uint32_t bitsFreeThisByte = 8 - bitOffset;

    // if we needed more than that, carry to the next byte
    if ( bitsFreeThisByte < inBitCount )
    {
        // we need another byte
        mBuffer[byteOffset + 1] = inData >> bitsFreeThisByte;
    }

    mBitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits( const void* inData, uint32_t inBitCount )
{
    const char* srcByte = static_cast<const char*>( inData );
    // write all the bytes
    while ( inBitCount > 8 )
    {
        WriteBits( *srcByte, 8 );
        ++srcByte;
        inBitCount -= 8;
    }
    // write anything left
    if ( inBitCount > 0 )
    {
        WriteBits( *srcByte, inBitCount );
    }
}

void OutputMemoryBitStream::PrintByteArray()
{
    uint32_t bytesToWrite = GetByteLength();
    const char* buffer = GetBufferPtr();
    printf( "printing %d bytes\n", bytesToWrite );

    for ( int i = 0; i < bytesToWrite; i++ )
    {
        printf( "%x", buffer[i] );
    }

    cout << endl;
}

void OutputMemoryBitStream::printStream() const
{
    const char* streamBuffer = GetBufferPtr();
    uint32_t bufferSize = GetByteLength();
    ::printStream( bufferSize, streamBuffer );
}

// =======================Input Memory Stream Implementation=================

// basic bit Read. Reads from mBuffer the number of bits into the single byte
// outData
void InputMemoryBitStream::ReadBits( uint8_t& outData, uint32_t inBitCount )
{
    uint32_t byteOffset =
        mBitHead >> 3; // how many bytes have we written already
    uint32_t bitOffset = mBitHead & 0x7; // how manay bits have been written

    // Point outData to the correct location in the current buffer by grabing
    // the current byte and shift it by the amount of bits written
    outData = static_cast<uint8_t>( mBuffer[byteOffset] ) >> bitOffset;

    // How many bits are actually open to use at this location
    uint32_t bitsFreeThisByte = 8 - bitOffset;

    // If not enought bits free to read,
    if ( bitsFreeThisByte < inBitCount )
    {
        // we need another byte, grab the next byte but shift it back the
        // amount of bits that were free.
        outData |= static_cast<uint8_t>( mBuffer[byteOffset + 1] )
                   << bitsFreeThisByte;
    }

    // Mask out the data we don't want, leaving the bits we want in the last
    // LSB
    outData &= ( ~( 0x00ff << inBitCount ) );

    mBitHead += inBitCount;
}

/**
 * Read any number of bits from the current mBuffer and put them into the
 * void pointer
 */
void InputMemoryBitStream::ReadBits( void* outData, uint32_t inBitCount )
{
    uint8_t* destByte = reinterpret_cast<uint8_t*>( outData );

    // Write all bytes first
    while ( inBitCount > 8 )
    {
        ReadBits( *destByte, 8 );
        ++destByte;
        inBitCount -= 8;
    }

    // If any bits are left, write the rest
    if ( inBitCount > 0 )
    {
        ReadBits( *destByte, inBitCount );
    }
}

void InputMemoryBitStream::printStream() const
{
    const char* streamBuffer = GetBufferPtr();
    uint32_t bufferSize = GetByteCapacity();

    ::printStream( bufferSize, streamBuffer );
}