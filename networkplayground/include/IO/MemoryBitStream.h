/**
 *
 * MemoryBitStream.hpp
 * networkplayground
 *
 * Based on https://github.com/MultiplayerBook/MultiplayerBook.git
 *
 */
#ifndef MemoryBitStream_h
#define MemoryBitStream_h

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string.h>

class GameObject;

/**
 * Converts a float to an int
 */
inline uint32_t ConvertToFixed(float inNumber, float inMin, float inPrecision)
{
    return static_cast<int>((inNumber - inMin) / inPrecision);
}

inline uint32_t ConvertFromFixed(float inNumber, float inMin, float inPrecision)
{
    return static_cast<float>(inNumber) * inPrecision + inMin;
}

/**
 * Class for writing data to a buffer stream
 */
class OutputMemoryBitStream
{
  public:
    OutputMemoryBitStream() : mBuffer(nullptr), mBitHead(0)
    {
        ReallocBuffer(1500 * 8); // init 1500 Byte size buffer
    }

    ~OutputMemoryBitStream() { std::free(mBuffer); }

    void WriteBits(uint8_t inData, uint32_t inBitCount);
    void WriteBits(const void* inData, uint32_t inBitCount);

    // Get read only pointer into buffer
    const char* GetBufferPtr() const { return mBuffer; }

    // Get number of bits written to the buffer
    uint32_t GetBitLength() const { return mBitHead; }

    // Get number of bytes written to the buffer
    uint32_t GetByteLength() const { return (mBitHead + 7) >> 3; }

    // Convient wrapper around write bits for byte aligned data
    void WriteBytes(const void* inData, uint32_t inByteCount)
    {
        WriteBits(inData, inByteCount << 3);
    }

    // Generic templatized write for primative values
    template <typename T>
    void Write(T inData, uint32_t inBitCount = sizeof(T) * 8)
    {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                      "Generic Write only supports primitive data types");
        WriteBits(&inData, inBitCount);
    }

    // Writer for bool data, which should only be 1 bit
    void Write(bool inData) { WriteBits(&inData, 1); }

    // Writer for string data
    void Write(const std::string& inString)
    {
        uint32_t elementCount = static_cast<uint32_t>(inString.size());
        Write(elementCount);
        for (const auto& element : inString)
        {
            Write(element);
        }
    }

    template <typename T>
    void serialize(T inData)
    {
        Write(inData);
    }

    void PrintByteArray();

    void printStream() const;

  private:
    void ReallocBuffer(uint32_t inNewBitCapacity);

    char* mBuffer;         // buffer pointer
    uint32_t mBitHead;     // how many bits have data
    uint32_t mBitCapacity; // how many bits the current buffer can hold
};

class InputMemoryBitStream
{
  public:
    InputMemoryBitStream(char* inBuffer, uint32_t inBitCount)
        : mBuffer(inBuffer), mBitHead(0), mBitCapacity(inBitCount),
          mIsBufferOwner(false)
    {
    }

    InputMemoryBitStream(const InputMemoryBitStream& inOther)
        : mBitHead(inOther.mBitHead), mBitCapacity(inOther.mBitCapacity),
          mIsBufferOwner(true)
    {
        // allocate buffer of right size
        int byteCount = mBitCapacity / 8;
        mBuffer = static_cast<char*>(malloc(byteCount));
        // copy
        memcpy(mBuffer, inOther.mBuffer, byteCount);
    }

    ~InputMemoryBitStream()
    {
        if (mIsBufferOwner)
        {
            free(mBuffer);
        };
    }

    const char* GetBufferPtr() const { return mBuffer; }
    uint32_t GetRemainingBitCount() const { return mBitCapacity - mBitHead; }
    uint32_t GetByteCapacity() const { return mBitCapacity >> 3; }

    void ReadBits(uint8_t& outData, uint32_t inBitCount);
    void ReadBits(void* outData, uint32_t inBitCount);

    void ReadBytes(void* outData, uint32_t inByteCount) { ReadBits(outData, inByteCount << 3); }

    bool serialize_bits(uint32_t& outData, uint32_t inBitCount = 32);

    template <typename T>
    void Read(T& inData, uint32_t inBitCount = sizeof(T) * 8)
    {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                      "Generic Read only supports primitive data types");
        ReadBits(&inData, inBitCount);
    }

    template <typename T>
    void serialize(T& inData)
    {
        Read(inData);
    }

    void Read(uint32_t& outData, uint32_t inBitCount = 32) { ReadBits(&outData, inBitCount); }
    void Read(int& outData, uint32_t inBitCount = 32) { ReadBits(&outData, inBitCount); }
    void Read(float& outData) { ReadBits(&outData, 32); }

    void Read(uint16_t& outData, uint32_t inBitCount = 16) { ReadBits(&outData, inBitCount); }
    void Read(int16_t& outData, uint32_t inBitCount = 16) { ReadBits(&outData, inBitCount); }

    void Read(uint8_t& outData, uint32_t inBitCount = 8) { ReadBits(&outData, inBitCount); }
    void Read(bool& outData) { ReadBits(&outData, 1); }

    void ResetToCapacity(uint32_t inByteCapacity)
    {
        mBitCapacity = inByteCapacity << 3;
        mBitHead = 0;
    }

    // This seems...dumb
    void Read(std::string& inString)
    {
        uint32_t elementCount;
        Read(elementCount);
        inString.resize(elementCount);
        for (auto& element : inString)
        {
            Read(element);
        }
    }

    void printStream() const;

  private:
    char* mBuffer;
    uint32_t mBitHead;
    uint32_t mBitCapacity;
    bool mIsBufferOwner;
};

#endif
