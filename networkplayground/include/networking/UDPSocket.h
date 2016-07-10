#ifndef UDPSocket_h
#define UDPSocket_h

#include <sys/socket.h>
#include <memory>
#include <unistd.h>

#include "networking/SocketAddress.h"
#include "networking/SocketConsts.h"


class UDPSocket
{
public:

	~UDPSocket();

	int Bind( const SocketAddress& inToAddress );
	int SendTo( const void* inToSend, int inLength, const SocketAddress& inToAddress );
	int ReceiveFrom( void* inToReceive, int inMaxLength, SocketAddress& outFromAddress );

	/*
	int SendTo( const MemoryOutputStream& inMOS, const SocketAddress& inToAddress );
	int ReceiveFrom( MemoryInputStream& inMIS, SocketAddress& outFromAddress );
	*/

	int SetNonBlockingMode( bool inShouldBeNonBlocking );

private:
	friend class SocketUtil;
	UDPSocket( SOCKET inSocket ) : mSocket( inSocket ) {}
	SOCKET mSocket;

};

typedef std::shared_ptr< UDPSocket >	UDPSocketPtr;

#endif
