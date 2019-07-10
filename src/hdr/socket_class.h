
/*
	SOCKET Class Declaration Module
    Author: Gary Conway <codeboy@iglou.com>
    Created: 10-10-2012
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2012 Gary Conway


	This module is the class declaration for the SOCKET class and implements an IP connection
	manager, in that, it is able to create UDP or TCP connections. Once a connection is made,
	it is bi-directional using SendMessage() and ReadMessage()

	The member functions are contained in socket_class.cpp

	Call the Constructor with either UDP_CONNECTION -or- TCP_CONNECTION as the argument

	You can create ONE of the following connection scenarios, but not both

	OPTION 1:
	---------
	You can set up a LISTENER by calling Server() that monitors a port for connection request. once a
	request is recieved it will then accept the connection.


	OPTION 2:
	---------
	You can call Client() and connect to a remote IP:PORT.


	With either of the above options, you can call ReceiveMessage() to check for and retrieve any data
	that has been received. You can also call SendMessage() to transmit a message over either of the
	connection types.



	call one of the following two functions FIRST

	LISTEN CHANNEL
	--------------
	bool Server(int port);				// create a listener for port (sets listen_port)

	CONNECT CHANNEL
	---------------
	bool Client(char * ip, int port, int *socket);	// connect to ip:port (sets remote_ip and remote_port)



	each instantiation of this class can be set up as EITHER a listener -or- initiate a connection, but not both
	both of these connections are bi-directional

	Usage:

	#include socket_class.h



	EXAMPLES:

	// TCP example
	// ----------------------------------------------------------------------------------------
	int res;
	int client_socket;	// filled in by Client()
	SOCKET * tcp_connection = new SOCKET(TCP_CONNECTION);					// instantiate
	res=tcp_connection->Client(ip_address, port, &client_socket);			// connect to another computer
	if (res != 0)
	{
		// res has the error code
	}
	res=tcp_connection->SendMessage(gen_buffer,client_socket);			// send a message
	if (res != 0)
	{
		// res has the error code
	}
	int bytecount;
	char *ptr;
	int my_socket;
	ptr=tcp_connection->ClientReceiveMessage(&bytecount);			// see if any data recieved from other computer
	if (ptr == NULL)
	{
		// no data
	}
	else
	{
		ptr points to our char[] data
		bytecount has number of bytes in the buffer
	}

	// UDP example
	// ----------------------------------------------------------------------------------------
	SOCKET * udp_connection = new SOCKET(UDP_CONNECTION);	// instantiate
	res=udp_connect->Server(port);							// set up a listener
	if (res != 0)
	{
		// res has the error code
	}

	int bytecount;	// filld in by ReceiveMessage()
	int my_socket;	// filld in by ReceiveMessage()
    char *ptr;
    ptr=udp_connection->ReceiveMessage(&bytecount,&my_socket);          // see if any data recieved from other computer
    if (ptr == NULL)
    {
        // no data
    }
    else
    {
        ptr points to our char[] received data
        bytecount has number of bytes in the buffer
    }



	res=udp_connection->SendMessage(gen_buffer,my_socket);			// send a message
	if (res != 0)
	{
		// res has the error code
	}




*/

using namespace std;



#include "fire.h"

#include <string>
#include <ctype.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <utime.h>
#include <assert.h>
#include <netinet/in.h>	// for IPPORT_USERRESERVED
#include <fcntl.h>
#include <strings.h>
#include <stdio.h>

#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ioctl.h>	// for getting IP addr
#include <sys/types.h>



#define MAXPENDING 5    /* Max connection requests */

typedef unsigned long int ULONG;

#define UDP_CONNECTION 0
#define TCP_CONNECTION 1


#ifndef SOCKET_CLASS
#define SOCKET_CLASS

// if socket_descriptor == 0, then the block is free to use
// we init all socket_descriptors to 0


#define RECVBLOCKS 20

struct RecvBlock
{
	int socket_descriptor;
	char sender_IP[40];
	uintmax_t timestamp;	// tracks RecvBlocks that have been around too long
	bool newSD;
};

#define RECV_BUFFER 5000
#define BUF_SIZE 2000





class SOCKET
{
public:

	SOCKET(int connection_type);			// constructor, type = UDP_CONNECT -or- TCP_CONNECTION
	~SOCKET();								// destructor

	// with each instantiation, you may call one of the following two functions, BUT NOT BOTH
	int Server(int port);					// create a listener for port (sets listen_port)
	int Client(char * ip, int port, int *client_socket);		// connect to ip:port (sets remote_ip and remote_port)

	bool SendMessage(char *msg, int socket);					// send ASCII message
	bool SendMessageBinary(char *msg, int count, int socket);	// send BINARY message

	// for server
	char * ReceiveMessage(int *bytecount, int*socket);
	char * GetReceivePtr( int *bytecount);			// return ptr to received data and set bytecount

	// for client
	char * ClientReceiveMessage(int *bytecount);

	void CloseConnection(void);				// close the connection

	char * recv_buffer;						// receiver buffers for all connections
	char * my_buffer;						// general purpose buffer

	// SPECS
	// when one of the below ptrs is set to NULL, then entry is unused

	struct RecvBlock *RecvBlocks[RECVBLOCKS];    // holds enough for connections from 20 computers on the listener


	// filled in by the constructor, will be TCP_CONNECTION or UDP_CONNECTION
	int connection_type;

	int my_port;		// our port
	int my_socket;		// our socket

	char remote_ip[20];	// stored IP address of the remote that we connect to via Client() call

	int bytes_received;
	bool connected;			// set when we have a connection for Client() or an active listener via Server()

	int timeout;
	struct sockaddr_in socket_struc, client_struc;
	fd_set master;		// our master list of socket descriptors
	int master_counter;	// counter for members of master



private:
	int MakeBuffers(void);				// create recv_buffer and buffer
	int InitSocket(int port);			// construct, bind and listen on a socket
	void SetNonBlocking(int socket);	// make the socket NON-BLOCKING
	void InitMaster(int socket);		// initialize our listener socket
	bool InitReceiver(void);			// allocate and set up receive buffer
	int GetIndex(int socket_descriptor);
	int GetFreeIndex(void);
	int ReceiveData (int socket_descriptor);
	char* GetErrCode();

	char buff[30];
	string error_string;
};





#endif








