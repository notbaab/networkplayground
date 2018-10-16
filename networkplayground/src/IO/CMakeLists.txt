set(IO_SRC
    ${CMAKE_SOURCE_DIR}/networkplayground/src/IO/MemoryBitStream.cpp
)

add_library(bitstream SHARED ${IO_SRC})
target_include_directories(bitstream PUBLIC
    ${CMAKE_SOURCE_DIR}/networkplayground/include/IO/
)
