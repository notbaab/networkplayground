#ifndef NetworkingShared_h
#define NetworkingShared_h

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "WinSock2.h"
#include "Windows.h"
#include "Ws2tcpip.h"
typedef int socklen_t;
// typedef char* receiveBufer_t;
#else
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// typedef void* receiveBufer_t;
typedef int SOCKET;
const int NO_ERROR = 0;
const int INVALID_SOCKET = -1;
const int WSAECONNRESET = ECONNRESET;
const int WSAEWOULDBLOCK = EAGAIN;
const int SOCKET_ERROR = -1;
#endif

#include "memory"

#include "cassert"
#include "deque"
#include "list"
#include "queue"
#include "string"
#include "unordered_map"
#include "unordered_set"
#include "vector"

using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::queue;
using std::list;
using std::deque;
using std::unordered_map;
using std::string;
using std::unordered_set;

class RoboCat;
class GameObject;

#include "SocketAddress.h"
#include "SocketAddressFactory.h"
#include "SocketUtil.h"
#include "StringUtils.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

// I don't know how headers work.
#include "IO/MemoryBitStream.h"

#include "NetworkManager.h"

#endif NetworkingShared_h /* */