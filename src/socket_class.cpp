/*
	Module: socket_class.cpp
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 6-14-2012
	Functional: 10-14-2012, 8:20pm 
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2018 FireKing Security
    Version: 1.0

	This file has the socket handling functions. It will create a socket and listener, handle connections
	and receive and trasmit data.

	see socket_class.h for a complete description of the class initialization and functions

	several connections & listeners must be set up




	Maestro
	-------

	All CANopen PDO messages have 8 bytes in lil endian notation
	0-3		// message type, always 1
	4-7		// message ID
		4 = node number
		5 = bus number
		7 = message ID (message type)
	8-11	// WParm
	12-15	// LParm
	16-20	// timestamp

	To subscribe:
		char bytes[] = (1,0,0,0);
		send(bytes, bytes.length);

	Unsubscribe:
		char bytes[] = (2,0,0,0);
		send(bytes, bytes.length);

	Acknowledge:
		char bytes[] = (3,0,0,0);
		send(bytes, bytes.length);


	Startup:

		1. connect to port 43211
		2. subscribe by sending the subscription string above
		3. listen on the connection for data from the Maestro

	Close:
		1. simply close the connection






*/


using namespace std;

// enables deubbging messages
#define DEBUG

#include "hdr/config.h"	// includes socket_class.h

//#include "hdr/socket_class.h"

// common.cpp
#include "common.inc"

// logs.cpp
#include "logs.inc"


// this variable is used when a SEGFAULT is caught
// the value is written to the log file and may be used
// to track down the source of the error
/*
int program_state=0;
#define SF_MAIN 0
#define SF_CLIENTDATA 1
#define SF_SENDFILE 2
#define SF_CHECKEVENTS 3
#define SF_CHECKFORCONNECTION 4
#define SF_SENDQUEUED 5
#define SF_READQUEUE 6
#define SF_WRITEQUEUE 7
*/






/*
	our constructor

	call with TCP_CONNECTION or UDP_CONNECTION

*/

SOCKET::SOCKET(int type)
{
	int res;
	connection_type = type;
	connected=FALSE;
	timeout=5;

	res=MakeBuffers();
//	if (res != 0)
//		return res;


}



/*
	our destructor
*/

SOCKET::~SOCKET()
{
	if (recv_buffer != NULL)
		delete[] recv_buffer;

	if (my_buffer != NULL)
		delete[] my_buffer;

	close(my_socket);
}


/*
	create a socket listening on port
	RETURNS: 0 on success
	error code on failure
	704		unable to create receive buffer
	705		error creating and binding socket

*/

int SOCKET::Server(int port)
{
	int res;
	my_port=port;

//	res=MakeBuffers();
//	if (res != 0)
//		return res;

	// bind and listen on the socket
	res = InitSocket(my_port);	// sets my_socket
	if (res!= 0)
		return my_socket;

	// set various parms for calls to ReadMessage()
	InitMaster(my_socket);

	connected=TRUE;
	return 0;
}



/*
	create a recieve buffer and a general purpose buffer

	RETURNS: 0 on success, else errorcode

	error codes
	704		unable to create receive buffer
	705		unable to create general purpose buffer
*/

int SOCKET::MakeBuffers(void)
{
	char buffer[100];

	my_buffer = new (nothrow) char[BUF_SIZE];
	if (my_buffer == NULL)
	{
		sprintf(buffer,"SOCKET::MakeBuffers() Unable to allocate my_buffer");
		printf("%s\n",buffer);
		WriteSystemLog(buffer);
		return 705;
	}


	// allocate a receive buffer
	if ( ! InitReceiver() )
	{
		sprintf(buffer,"SOCKET::MakeBuffers() Unable to allocate recieve buffer");
		printf("%s\n",buffer);
		WriteSystemLog(buffer);
		return 704;
	}


	// init RecvBlocks[] struc
	for (int n=0; n<RECVBLOCKS; n++)
	{
		RecvBlocks[n] = NULL;
	}


	return 0;
}





/*
	connect to a remote IP:PORT
	RETURNS: 0 on success, else error string number for use by getMessage()

	error returns
	700		unable to create socket
	701		no such host ip
	702		unable to connect via socket
	(from MakeBuffers)
	704		unable to create receive buffer
	705		unable to create general purpose buffer
*/

int SOCKET::Client(char * ip, int port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
//	char my_buffer[BUF_SIZE];
//	bzero(my_buffer,BUF_SIZE);


//	int res=MakeBuffers();
//	if (res != 0)
//		return res;


	strcpy(remote_ip,ip);
	my_port=port;


	// create a socket
	// AF_INET = address family, internet = IP address
	// PF_INET = protocol family, internet = IP,TCP/IP or UDP/IP
//    my_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (connection_type == TCP_CONNECTION)
		my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	else
		my_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (my_socket < 0)
	{
		// unable to create a socket
		sprintf(my_buffer,"Unable to create a socket");
		WriteSystemLog(my_buffer);
		return 700;
	}
	else
	{
		// socket created successfully
		sprintf(my_buffer,"Client Socket created successfully");
		printf("SOCKET::Client: %s\n",my_buffer);
		WriteSystemLog(my_buffer);
	}

	// set various parms for calls to ReadMessage()
	InitMaster(my_socket);

    server = gethostbyname(remote_ip);
    if (server == NULL)
	{
		// no such host ip
		sprintf(my_buffer,"gethostbyname(%s) failed",remote_ip);
		WriteSystemLog(my_buffer);
		return 701;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
	// set for TCP
    serv_addr.sin_family = AF_INET;

	// copy the server address
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

	// convert port number to string
    serv_addr.sin_port = htons(my_port);

	// attempt the connection
	// connect returns -1 on error, 0 on success
	if (connect(my_socket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		// unable to connect
		sprintf(my_buffer,"Unable to connect to socket %d",my_socket);
		WriteSystemLog(my_buffer);
		return 702;
	}

	#ifdef DEBUG
		printf("SOCKET::Client->Connected to IP:%s:%d\r\n",ip,port);
	#endif

	sprintf(my_buffer,"SOCKET::[%s:%s:%d]Client->Connected to IP:%s:%d",__FILE__,__func__,__LINE__,ip,port);
	WriteSystemLog(my_buffer);

	connected=TRUE;
	return 0;
}





/*
	We must call this function to initialize things for ReceiveMessage()
*/

void SOCKET::InitMaster(int socket)
{
    // Create a descriptor set containing our socket.
	FD_ZERO(&master);           // init socket descriptor set to zeros
	FD_SET(socket, &master);    // add our socket to the socket descriptor set, master
	master_counter=socket;
}



/*
	Allocate and Initialize the recv_buffer for receiving data

	RETURNS: TRUE on success
			FALSE otherwise
*/

bool SOCKET::InitReceiver(void)
{
//	char my_buffer[BUF_SIZE];
	bzero(my_buffer,BUF_SIZE);


	recv_buffer = new (nothrow) char[RECV_BUFFER];
	if (recv_buffer == NULL)
	{
		sprintf(my_buffer,"SOCKET::InitReceiver() Unable to allocate recv_buffer");
		printf("%s\n",my_buffer);
		WriteSystemLog(my_buffer);
		return FALSE;
	}
#ifdef DEBUG
	else
	{
		printf("SOCKET::recv_buffer created\r\n");
	}
#endif

	return TRUE;
}



/*
	transmit an ASCII message

	msg is the message to send
	socket is the sockeet to send msg on

	RETURNS: TRUE on success
			FALSE otherwise
*/

bool SOCKET::SendMessage(char *msg, int socket)
{
//	char my_buffer[BUF_SIZE];
	bzero(my_buffer,BUF_SIZE);
	string errstr;
//	int socket = my_socket;

//========================================================
//GRC - this fixed the sendmessage() failure on the server
// THIS IS A TEMP WORK-AROUND, NOT A SOLUTION
//========================================================
//	socket+=2;
//========================================================
//========================================================


	// if we're not connected, then just return
	if (!connected)
		return FALSE;

	// returns bytes written. on error, -1 and errno is set
    int bytes_written = write(socket,msg,strlen(msg));

    if (bytes_written < 0)
	{
		errstr=GetErrCode();

		sprintf(my_buffer,"SOCKET::[%s:%s:%d]::ERROR writing to socket:%d [%s]",__FILE__,__func__,__LINE__,socket,errstr.c_str() );
		WriteSystemLog(my_buffer);
		return FALSE;
	}
	else
	{
        sprintf(my_buffer,"SOCKET::[%s:%s:%d]:: wrote data to socket:%d",__FILE__,__func__,__LINE__,socket);
		WriteSystemLog(my_buffer);
	}


	return TRUE;
}




char*  SOCKET::GetErrCode()
{
	string errstr;

        switch(errno)
        {
//        case EAGAIN:        errstr="EAGAIN"; break;
        case EWOULDBLOCK:   errstr="EWOULDBLOCK"; break;
        case EBADF:         errstr="EBADF"; break;			// bad file descriptor
        case EDESTADDRREQ:  errstr="EDESTADDRREQ"; break;	// dest address required
        case EDQUOT:        errstr="EDQUOT"; break;			// disk quota exceeded
        case EFAULT:        errstr="EFAULT"; break;			// badd address
        case EFBIG:         errstr="EFBIG"; break;			// file too large
        case EINTR:         errstr="EINTR"; break;			//  interrupted fn call
        case EINVAL:        errstr="EINVAL"; break;			// invalid argument
        case EIO:           errstr="EIO"; break;			// I/O error
        case ENOSPC:        errstr="ENOSPC"; break;			// no space left on device
        case EPIPE:         errstr="EPIPE"; break;			// broken pipe
        }

	sprintf(buff,"%s",errstr.c_str() );
	return buff;
}



/*
	transmit a BINARY message
	RETURNS: TRUE on success
			FALSE otherwise
*/

bool SOCKET::SendMessageBinary(char *msg, int count)
{
//	char my_buffer[BUF_SIZE];
	bzero(my_buffer,BUF_SIZE);

	// if we're not connected, then just return
	if (!connected)
		return FALSE;

    int bytes_written = write(my_socket,msg,count);

    if (bytes_written < 0)
	{
		sprintf(my_buffer,"ERROR writing to socket");
		WriteSystemLog(my_buffer);
		return FALSE;
	}

	return TRUE;
}




/*
	check our socket to see if we have a connection or data
	if data is present on the socket, then store it to *data

	socket is a returned value, the socket we received the message ON. this normally needs to be saved by the caller
	for subsequent calls to SendMessage()

	RETURNS:	if no data, sets bytecount=0 and returns a NULL ptr
				if we have received data, set bytecount and return &recieve_buffer
*/

char * SOCKET::ReceiveMessage(int *bytecount, int *socket)
{


	static fd_set read_sd;	// a structure of two bit-mapped long arrays
    struct sockaddr_in temp_recv_struc;
	struct timeval timeout;
	int rsd;		// 'ready socket descriptors'
	int newconn;
	unsigned int size = sizeof(temp_recv_struc);
	int counter;
	int filesindex;
	char my_buffer[BUF_SIZE];
	bzero(my_buffer,BUF_SIZE);

	// preset the passbacks
	*bytecount= (int) 0;
	*socket= (int) 0;


	// if we're not connected, then just return
	if (! connected)
		return NULL;

	// Set time limit.
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	// man select_tut gives a lot of good info and examples
	// select( int num_fds, fd_set * readfds, fd_set * writefds, fdset * exceptfds,const struct timeval * timeout);

/*
	If you set the fields in struct timeval to 0, select() will timeout immediately,
	effectively polling all the file descriptors in the sets.
	If you set the parameter timeout to NULL, it will never timeout, and will wait until
	the first file descriptor is ready.
	Finally, if you don't care about waiting for a certain set, you can just set it to NULL
	in the call to select().
*/

	read_sd=master;
    rsd = select(master_counter+1, &read_sd, NULL, NULL, &timeout);

	// returns 0 on timeout
	// on success returns number of file descriptors present in the master set
	// -1 on error and errno set

//	#ifdef DEBUG
//		printf("master_counter: %d\n",master_counter);
//	#endif


if ( rsd == -1 )
{
	#ifdef DEBUG
		perror("Select call failed\n");
	#endif


	switch(errno)
	{
		case EBADF:	// bad file descriptor
			sprintf(my_buffer,"Bad File Descriptor");
			WriteSystemLog(my_buffer);
			#ifdef DEBUG
				printf("%s\n",my_buffer);
			#endif
			break;
		case EINTR:	// select() was interrupted
			sprintf(my_buffer,"Select() was interrupted");
			WriteSystemLog(my_buffer);
			#ifdef DEBUG
				printf("%s\n",my_buffer);
			#endif
			break;
		case EWOULDBLOCK:
			sprintf(my_buffer,"Select() would block");
			WriteSystemLog(my_buffer);
			#ifdef DEBUG
				printf("%s",my_buffer);
			#endif
			break;
	}

	sprintf(my_buffer,"ERROR: Select() call failed returned -1, errno:%d..%s",errno,strerror(errno));
	WriteSystemLog(my_buffer);
	return NULL;
}


	// if select() timed out, then just return
	if ( rsd == 0 ) return NULL;


//	#ifdef DEBUG
		printf("Select() result %d (number of bits set in the SD set)\n",rsd);
//	#endif



/*
	service multiple connects on the socket
*/

	// iterate existing connections looking for data to read
	counter=master_counter+1;

//counter=FD_SETSIZE;

	for (int sd=0; sd < counter; sd++)
	{
		if (FD_ISSET(sd, &read_sd))
		{
			// master socket (eg ,my_socket) ?
			if ( sd == my_socket )
			{
				// connection request on socket master
				newconn = accept(my_socket, (struct sockaddr *) &temp_recv_struc, &size);
				// newconn is our socket descriptor

				switch(errno)
				{
				case EBADF:
					printf("my_socket is invalid file descriptor\r\n");
					break;
				case ENOTSOCK:
					printf("my_socket is not a socket\r\n");
					break;
				case EOPNOTSUPP:
					printf("my_socket does not support this operation\r\n");
					break;
				case EWOULDBLOCK:
					printf("my_socket is in non-blocking mode, the operation would block\r\n");
					break;
				}

				if (newconn < 0)
				{
					sprintf(my_buffer,"Failure accepting connection");
					WriteSystemLog(my_buffer);
					#ifdef DEBUG
						printf("%s\r\n",my_buffer);
					#endif
					continue;
				}
				sprintf(my_buffer,"Server: connect from IP:%s:%d, socket:%d",inet_ntoa(temp_recv_struc.sin_addr), ntohs(temp_recv_struc.sin_port),my_socket);
				WriteSystemLog(my_buffer);
				#ifdef DEBUG
					printf("%s\n",my_buffer);
				#endif

				sprintf(remote_ip,"%s",inet_ntoa(temp_recv_struc.sin_addr));
				filesindex=GetFreeIndex();
				if (filesindex == -1)
				{
					sprintf(my_buffer,"ERROR: unable to locate a free entry in RecvBlocks for socket descriptor %d\n",newconn);
					WriteSystemLog(my_buffer);
                    #ifdef DEBUG
                        printf("%s\n",my_buffer);
                    #endif
					continue;
				}

				// store information about the new connection
				RecvBlocks[filesindex]= (struct RecvBlock *) malloc(sizeof( struct RecvBlock));
				assert(RecvBlocks[filesindex] != NULL);
				if (RecvBlocks[filesindex] == NULL)		continue;
				RecvBlocks[filesindex]->socket_descriptor=newconn;
				strcpy(RecvBlocks[filesindex]->sender_IP,remote_ip);
				RecvBlocks[filesindex]->newSD=TRUE;
                RecvBlocks[filesindex]->timestamp=EpochTime();	//seconds since epoch

				FD_SET (newconn, &master);		// add the descriptor into our master set
				SetNonBlocking(newconn);
				if (newconn > master_counter)
					master_counter=newconn;

			}
			else
			{
				// we have pending activity on an existing connection, handle it
				// data is ready, go get it
				#ifdef DEBUG
					printf("Getting data for SD: %d\n",sd);
				#endif

				// let's do a lil housekeeping
				filesindex=GetIndex(sd);
				if (filesindex ==-1)
				{
					sprintf(my_buffer,"ERROR: unable to locate socket descriptor %d in RecvBlocks",sd);
					WriteSystemLog(my_buffer);
					#ifdef DEBUG
						printf("%s\n",my_buffer);
					#endif
					continue;
				}


				// get the received data

				// RETURNS: -1 on timeout or connection closed
				//          -2 on error
				//          bytecount if recv_buffer has data

				int res=ReceiveData(sd);
				switch(res)
				{
				case -1:
					#ifdef DEBUG
						printf("Timeout error on receive\r\n");
					#endif
					bytes_received=0;
					// this connection is dead n stinkin, kill it and start over
					close(sd);
					free(RecvBlocks[filesindex]);
					RecvBlocks[filesindex] = NULL;
					FD_CLR(sd,&master);
					WriteSystemLog( (char *)"Closing socket");
					return NULL;
					break;
				case -2:
					WriteSystemLog( (char *) "Error on receiver");
					#ifdef DEBUG
						printf("error on receive\r\n");
					#endif

					bytes_received=0;
					return NULL;
					break;
				default:

//========================================================
// THIS MAY BE THE SOLUTION
//GRC 7-7-19
//========================================================
//my_socket=sd;
//========================================================

					*socket=sd;			// push our socket back to caller
					bytes_received=res;
					*bytecount=res;
					sprintf(my_buffer,"Received message: %s on socket: %d",recv_buffer,sd);
					WriteSystemLog(my_buffer);
					return recv_buffer;
					break;
				}
				return NULL;	// this should never happen, however...


			}
		}	// end if (FD_ISSET..)

	}	// end for


	return NULL;

}




/*
	Read data from the client connection (if any)
	RETURNS:	NULL on error or no data and bytecount is set to 0
				*recv_buffer if data ready and bytecount is set
*/

char * SOCKET::ClientReceiveMessage(int *bytecount)
{
	int res;

	// RETURNS: -1 on timeout or connection closed
	//          -2 on error
	//          bytecount if recv_buffer has data
	res = ReceiveData(my_socket);
	if (res < 0)
	{
		*bytecount=0;
		return NULL;
	}

	bytes_received = res;
	*bytecount=res;
	return recv_buffer;
}



/*
	get a pointer to the received data
*/

char * SOCKET::GetReceivePtr( int *bytecount)
{
	*bytecount=bytes_received;
	return recv_buffer;
}




/*
	construct, bind and listen on a socket


	RETURNS:	0 on success, errorcode on failure
*/

int SOCKET::InitSocket(int port)
{
	int reuse_addr=1;
	int rv;
//	char my_buffer[BUF_SIZE];
	bzero(my_buffer,BUF_SIZE);

	struct addrinfo hints, *servinfo, *p;
	memset(&hints, 0, sizeof hints);

	if (connection_type == UDP_CONNECTION)
	{
		hints.ai_family = AF_INET;				// internet socket
		hints.ai_socktype = SOCK_DGRAM;			// use datagrams
		hints.ai_protocol = IPPROTO_UDP; 		// use UDP protocol
		hints.ai_flags = AI_PASSIVE; 			// use my IP
	}
	else
	{
		hints.ai_family = AF_UNSPEC;			// says use either IP4 or IP6
		hints.ai_socktype = SOCK_STREAM;		// use streaming
		hints.ai_protocol = IPPROTO_TCP;		// TCP connection
		hints.ai_flags = AI_PASSIVE; 			// use my IP
	}

	sprintf(my_buffer,"%d",port);



	// fill a linked list structure with all available socket information for the passed port
	if ((rv = getaddrinfo(NULL, my_buffer, &hints, &servinfo)) < 0)
	{
		sprintf(my_buffer,"server: %s\n",gai_strerror(rv));
		WriteSystemLog(my_buffer);
		return ERROR;
	}


	// loop thru all available sockets till we find one we can use
	for (p = servinfo; p != NULL; p=p->ai_next)
	{
		if (( my_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
			continue;	// not this one, skip it


		// found a socket, set the options
		if (setsockopt(my_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse_addr,sizeof(int) ) == -1)
		{
			perror("setsockopt failed:");
			return ERROR;
		}

		// and bind our socket to the port
		if (bind(my_socket, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(my_socket);
			sprintf(my_buffer,"Unable to bind socket %d", my_socket);
			WriteSystemLog(my_buffer);
			printf("%s\n",my_buffer);
			return ERROR;
		}

		break;

	} // end for


	if (p == NULL)
	{
		sprintf(my_buffer, "server: failed to bind");
		WriteSystemLog(my_buffer);
		return ERROR;
	}

	freeaddrinfo(servinfo);


	// from everything I can find, AF_INET and PF_INET are the same
	// one include even has #define PF_INET AF_INET, qed

	// set for non-blocking
	SetNonBlocking(my_socket);

    // Listen on the server socket
    if (listen(my_socket, MAXPENDING) < 0)
    {
		sprintf(my_buffer,"Unable to listen on socket %d",my_socket);
		WriteSystemLog(my_buffer);
		return ERROR;
    }

	sprintf(my_buffer,"Listening on Port: %d, Socket: %d",port,my_socket);

	#ifdef DEBUG
		printf("%s\r\n",my_buffer);
	#endif
		WriteSystemLog(my_buffer);

	return 0;
}



/*
	Set the socket or socket descriptor to NON-BLOCKING mode


*/


void SOCKET::SetNonBlocking(int socket)
{
	int opts;

	// if we are using blocking ports, then simply return
	#ifdef USEBLOCKING
		return;
	#endif

    // set for non-blocking
    opts=fcntl(socket,F_GETFL);
    if (opts <0)
	{
		perror("Unable to get socket options\n");
		WriteSystemLog( (char *) "Unable to get socket options");
		return;
	}

    opts= (opts | O_NONBLOCK);
    if (fcntl(socket,F_SETFL,opts) < 0)
    {
        perror("Unable to set socket options\n");
		WriteSystemLog( (char *) "Unable to set socket options");
		return;
	}

}



void SOCKET::CloseConnection(void)
{
	close(my_socket);
}




// the connection has already been accepted
// try to retrieve any client data else return with a timeout status
// RETURNS: -1 on timeout or connection closed
//          -2 on error
//          bytecount if recv_buffer has data

// https://www.linuxjournal.com/article/9815
// http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#select
// http://pubs.opengroup.org/onlinepubs/009695399/functions/recv.html
// http://linux.die.net/man/3/recv

int SOCKET::ReceiveData (int socket_descriptor)
{
	fd_set fds;
	struct timeval tv;
	int result;
	int nbytes;
	int pkt_len=0;


//	#ifdef DEBUG
//		printf("Packets:");
//	#endif

	FD_ZERO(&fds);
	FD_SET(socket_descriptor,&fds);
	tv.tv_sec=timeout;
	tv.tv_usec=0;

	// man select_tut gives a lot of good info and examples
	// select( int num_fds, fd_set * readfds, fd_set * writefds, fdset * exceptfds,const struct timeval * timeout);


//	If you set the fields in struct timeval to 0, select() will timeout immediately,
//	effectively polling all the file descriptors in the sets.
//	If you set the parameter timeout to NULL, it will never timeout, and will wait until
//	the first file descriptor is ready.
//	Finally, if you don't care about waiting for a certain set, you can just set it to NULL
//	in the call to select().


	result = select(socket_descriptor+1,&fds,NULL,NULL,&tv);
	// returns 0 on timeout
	// on success returns number of file descriptors present in the master set
	// -1 on error and errno set

	if (result == 0)
	{
		// timed out, connection may be closed
		#ifdef DEBUG
//			printf("0 bytes,timeout\n");
		#endif
		sprintf(my_buffer,"Timeout waiting for port data on %d",socket_descriptor);
		WriteSystemLog(my_buffer);
		return -1;
	}

	if (result < 0)
	{
		// error
        #ifdef DEBUG
            printf("0 bytes,error\n");
        #endif
		sprintf(my_buffer,"General error trying to receive port data on %d",socket_descriptor);
		WriteSystemLog(my_buffer);
		return -2;
	}


	// if we get here, then we have data, retrieve it

	// flags are MSG_PEEK, MSG_OOB, MSG_WAITALL
	// returns the number of bytes received
	bzero(recv_buffer,RECV_BUFFER);
    nbytes = recv(socket_descriptor,recv_buffer,RECV_BUFFER,MSG_WAITALL);

	if (nbytes == 0)
	{
		#ifdef DEBUG
			printf("timeout error on receive\n");
		#endif
		sprintf(my_buffer,"Remote connection closed by remote end, on socket: %d",socket_descriptor);
		WriteSystemLog(my_buffer);
		return -1;	// connection was closed by remote
	}

	if (nbytes < 0)
	{
       #ifdef DEBUG
            printf("error on receive packet\n");
        #endif
		sprintf(my_buffer,"General error retrieving data on socket: %d",socket_descriptor);
		WriteSystemLog(my_buffer);
		return -2;
	}

	#ifdef DEBUG
//		printf("Packets:+%d bytes  ",nbytes);
//	   for (int n=0; n< nbytes; n++)
//	        printf("%02X ",(unsigned char) recv_buffer[n]);

	#endif



  	return nbytes;
}





// find the next available entry in our RecvBlocks array of ptrs to structures
// returns -1 if failure

int SOCKET::GetFreeIndex(void)
{
	int n;

	for (n=0; n<RECVBLOCKS; n++)
	{
		if (RecvBlocks[n] == NULL)
			return n;
	}

	return -1;
}




// get the index corresponding to the passed in socket_descriptor

int SOCKET::GetIndex(int socket_descriptor)
{
	int n;

	for (n=0; n<RECVBLOCKS; n++)
	{
		if (RecvBlocks[n] != NULL)
		{
			if (RecvBlocks[n]->socket_descriptor ==  socket_descriptor)
				return n;
		}
	}

	return -1;

}












