set(CONSOLE_SRC
    ${CMAKE_SOURCE_DIR}/networkplayground/src/console/console.cpp
    ${CMAKE_SOURCE_DIR}/lib/linenoise/linenoise.c
    ${CMAKE_SOURCE_DIR}/networkplayground/src/datastructures/Trie.cpp
)

add_library(console SHARED ${CONSOLE_SRC})
target_include_directories(console PUBLIC
    ${CMAKE_SOURCE_DIR}/networkplayground/include/console
    ${CMAKE_SOURCE_DIR}/networkplayground/include/
    ${CMAKE_SOURCE_DIR}/lib/linenoise
    ${CMAKE_SOURCE_DIR}/lib/spdlog/include
)
