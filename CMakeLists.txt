cmake_minimum_required(VERSION 3.6)
project(networkplayground)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
SET(CMAKE_C_FLAGS_DEBUG "-D_DEBUG")
INCLUDE_DIRECTORIES(networkplayground/include/)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_Image REQUIRED)

SET(BASE_SOURCE
    networkplayground/src/gameobjects/Engine.cpp
    networkplayground/src/gameobjects/GameObject.cpp
    networkplayground/src/gameobjects/GameObjectRegistry.cpp
    networkplayground/src/gameobjects/InputState.cpp
    networkplayground/src/gameobjects/Move.cpp
    networkplayground/src/gameobjects/MoveList.cpp
    networkplayground/src/gameobjects/Player.cpp
    networkplayground/src/gameobjects/PlayerMessage.cpp
    networkplayground/src/gameobjects/World.cpp
    networkplayground/src/IO/MemoryBitStream.cpp
    networkplayground/src/math/Vector3.cpp
    networkplayground/src/networking/AckRange.cpp
    networkplayground/src/networking/DeliveryNotificationManager.cpp
    networkplayground/src/networking/InFlightPacket.cpp
    networkplayground/src/networking/NetworkedData.cpp
    networkplayground/src/networking/NetworkManager.cpp
    networkplayground/src/networking/SocketAddress.cpp
    networkplayground/src/networking/SocketAddressFactory.cpp
    networkplayground/src/networking/SocketUtil.cpp
    networkplayground/src/networking/StringUtils.cpp
    networkplayground/src/networking/TCPSocket.cpp
    networkplayground/src/networking/UDPSocket.cpp
    networkplayground/src/timing/Timing.cpp
)

set(CLIENT_SOURCE
    networkplaygroundclient/src/gameobjects/Client.cpp
    networkplaygroundclient/src/gameobjects/PlayerClient.cpp
    networkplaygroundclient/src/graphics/GraphicsDriver.cpp
    networkplaygroundclient/src/graphics/RenderManager.cpp
    networkplaygroundclient/src/graphics/SpriteComponent.cpp
    networkplaygroundclient/src/graphics/TextureManager.cpp
    networkplaygroundclient/src/graphics/WindowManager.cpp
    networkplaygroundclient/src/input/InputManager.cpp
    networkplaygroundclient/src/networking/NetworkManagerClient.cpp
    networkplaygroundclient/src/networking/ReplicationManagerClient.cpp
        networkplaygroundclient/src/gameobjects/Ghost.cpp)

set(SERVER_SOURCE
    networkplaygroundserver/src/gameobjects/PlayerServer.cpp
    networkplaygroundserver/src/networking/ClientProxy.cpp
    networkplaygroundserver/src/networking/NetworkManagerServer.cpp
    networkplaygroundserver/src/networking/ReplicationManagerServer.cpp
    networkplaygroundserver/src/networking/ReplicationManagerTransmissionData.cpp
    networkplaygroundserver/src/networking/Server.cpp
    networkplaygroundserver/tests/tests.cpp
    networkplaygroundserver/tests/tests.hpp
)

set(UNIT_TEST_SOURCE
        unit_tests/main.cpp
)

add_executable(networkplaygroundclient ${BASE_SOURCE} ${CLIENT_SOURCE} networkplaygroundclient/src/main.cpp)
add_executable(networkplaygroundserver ${BASE_SOURCE} ${SERVER_SOURCE} networkplaygroundserver/src/main.cpp)
add_executable(unit_tests ${BASE_SOURCE} ${SERVER_SOURCE} ${UNIT_TEST_SOURCE} networkplayground/tests/OutputMemoryBitStream_test.cpp)



target_include_directories(networkplaygroundserver PUBLIC networkplayground/include/ networkplaygroundserver/include/)
target_include_directories(networkplaygroundclient
        PUBLIC
        networkplayground/include/
        networkplaygroundclient/include/
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR})

target_include_directories(unit_tests PUBLIC networkplayground/include/ networkplaygroundserver/include/ networkplaygroundclient/include/)

target_link_libraries(networkplaygroundclient ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES})

# These should go in their own directory
add_subdirectory(lib/googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

target_link_libraries(unit_tests gtest gtest_main)

