/**	@file MsuSocketInterface.c
 *	<TABLE>
 *	@brief This file provide the Abstract socket functions which will be used in Application
 *         to create socket, transmit and receive data
 *	@copy
 *	<TABLE>
 *	<TR><TD> Copyright </TD></TR>
 *	<TR>
 *	<TD>Schneider Electric India (Pvt) Ltd.
 *	Copyright (c) 2011 - All rights reserved.
 *	</TR>
 *	<TR>
 *	<TD> No part of this document may be reproduced in any form without the express
 * 	written consent of Schneider Electric India (Pvt) Ltd.</TD>
 *	</TR>
 *	</TABLE>
 *
 *	@par
 *	@author Rajinikanth Reddy
 *	@par HISTORY
 *
 *	<TABLE>
 *	<TR>
 *	<TD>Author</TD>		<TD>Date</TD>		<TD>Description</TD>
 *	</TR>
 *	<TR>
 *	<TD>Rajinikanth Reddy </TD>	<TD>07-Jan-2012</TD>	<TD>First creation</TD>
 *	</TR>
 *	</TABLE>
 *	</TABLE>
 *
 */

#ifndef __MSU_SOCKETINTERFACE_C__
#define __MSU_SOCKETINTERFACE_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include "MsuSocketInterface.h"


static struct timeval timeout;
static SINT16 maxFileDescriptor=0;

#define MSU_MAX_PACKET_SIZE  1500

UINT32 msuIpfromStr(char* ipStr)
{
	return inet_addr(ipStr);
}


/**  @fn msuSocketCreate( UINT16 port, UINT32 ipAddress )
 *   @brief msuSocketCreate() is used to create a socket
 *   @details  msuSocketCreate() function is used create a socket and binds the socket with
 *             specified port number. this function also registers multicast IP address to the socket
 *             by using setsockopt() call to register multicast frames.
 *             port number and multicast IP address passed as a parameters to this function.
 *   @return socket descriptor if the operation is successful
 *           Error code if the operation is failed.
 *   @retval socket descriptor ( integer value) if created successfully
 *   @retval -1 if failed to create socket
  */
MSU_SOCKET msuSocketCreate( UINT16 port, UINT32 ipAddress )
{
	MSU_SOCKET tempSocket = MSU_INVALID_SOCKET;
	struct ip_mreq multicastAddr;
	SINT32 socketStatus = MSU_SOCKET_SUCCESS;
	struct sockaddr_in sockAddress;
	SINT32 trueFlag = TRUE;

	/*Create a socket for UDP message*/
	tempSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (tempSocket == MSU_INVALID_SOCKET)
	{
#ifdef Debug_level1
		printf("\nSocket Creation Failed \n");
#endif
	}
	else
	{
		memset((char *) &sockAddress, 0, sizeof(sockAddress));
		sockAddress.sin_family = AF_INET;
		sockAddress.sin_port = msuHtoNs(port);
		sockAddress.sin_addr.s_addr = msuHtoNl(INADDR_ANY);
		socketStatus = setsockopt(tempSocket, SOL_SOCKET, SO_REUSEADDR,
					(char *) &trueFlag, sizeof(trueFlag));
		if (socketStatus == MSU_SOCKET_ERROR)
		{
#ifdef Debug_level1
			printf("\nSocketopt reuse option failed \n");
#endif
		}
		else
		{	/*Bind the socket to the port*/
			socketStatus = bind(tempSocket, (struct sockaddr *) &sockAddress,sizeof(sockAddress));
			if (socketStatus == MSU_SOCKET_ERROR)
			{
#ifdef Debug_level1
				printf("\nSocket binding failed for port %d\n", port);
#endif
			}
			else
			{	/* Join a multicast group on all interfaces*/
#ifdef ETHERBRICK_PLATFORM
				    multicastAddr.imr_multiaddr.s_addr = ipAddress;
				    multicastAddr.imr_interface.s_addr = clientIP;
  					joinIgmpMldGroupForSwitch((unsigned char *)&ipAddress,IGMP_MESSAGE);
#else
  					multicastAddr.imr_multiaddr.s_addr = ipAddress;
  					multicastAddr.imr_interface.s_addr = (inet_addr((char *)clientIP));
#endif
				socketStatus = setsockopt(tempSocket, IPPROTO_IP,IP_ADD_MEMBERSHIP, (char *) &multicastAddr,
						sizeof(multicastAddr));
				if (socketStatus == MSU_SOCKET_ERROR)
				{
#ifdef Debug_level1
					printf("\nAdd membership socket option failed\n");
#endif
				}
			}
		}
		if (socketStatus == MSU_SOCKET_ERROR)
		{
#ifdef Debug_level1
			printf("\nSocketopt error = %d\n",errno);
#endif
			close(tempSocket);
			tempSocket = MSU_INVALID_SOCKET;
		}
	}
	return tempSocket;
}

/**  @fn setTimeoutVal( SINT32 socketfd, UINT8 timeout_sec )
 *   @brief setTimeoutVal() is used to set the socket timeout value
 *   @details  setTimeoutVal() function is used set the timeout value of the non blocking sockets.
 *             this time out value will be used by select call to block sockets for specified timing
 *             interval to observe socket events.
 *             This function takes socket file descriptor and timeout parameter as arguments
 *   @return   None
 *
 *   @retval   None
 *   @retval   None
  */
void setTimeoutVal( SINT32 socketfd, UINT8 timeout_sec )
{
	timeout.tv_sec = timeout_sec;
	timeout.tv_usec = 0;

	maxFileDescriptor = socketfd + 1;
}


/**  @fn SINT32  msuNonBlockingSocketRecvFrom( SINT32 socketfd, void *buf, UINT16 len, msuSockinfo *from)
 *   @brief  Is used to receive data from Non blocking sockets
 *   @details  This function is used to receive data from socket.
 *
 *
 *   @return Success code if the operation is successful
 *           Error code if the operation is failed.
 *   @retval TR_SUCCESS If task started successfully
 *   @retval TR_EINVAL Task start failed
 *   @pre Component state should be COMPONENT_STATE_CONFIGURED
 *   @post Component state will be COMPONENT_STATE_STARTED
 */

SINT32  msuNonBlockingSocketRecvFrom( SINT32 socketfd, void *buf, UINT16 len, msuSockinfo *from)
{
	struct timeval timeout_value;
	fd_set readSet;
//	SINT16 maxFileDescriptor = 0;
	UINT32 slen = 0;
	SINT32 nBytes = MSU_SOCKET_ERROR;
	struct sockaddr_in destUnicastAddr;

	slen = sizeof(struct sockaddr_in);

	//Adjust the waiting timeout if required
	timeout_value.tv_sec = timeout.tv_sec;
	timeout_value.tv_usec = timeout.tv_usec;	/* time out after MSU_SOCKET_TIMEOUT seconds */

	//Initialize the file descriptor
	FD_ZERO( &readSet );

    //Add passive multicast socket and unicast socket to file descriptor set
	if(socketfd != MSU_INVALID_SOCKET)
	{
	  FD_SET(socketfd, &readSet);
//	  maxFileDescriptor = socketfd;
	}
//	maxFileDescriptor += 1;

	//Call select to check the activity on the sockets
	if(select(maxFileDescriptor, &readSet, NULL, NULL, &timeout_value) > 0)
	{
      //Activity detected on the sockets
	  if(socketfd != MSU_INVALID_SOCKET)
	  {
         if(FD_ISSET(socketfd, &readSet))
         {
    	    //Handle messages on multicast socket
    	    nBytes = recvfrom(socketfd, buf, len, 0, (struct sockaddr *)&destUnicastAddr,(socklen_t *) &slen);
    		from->ipAddress = msuNtoHl(destUnicastAddr.sin_addr.s_addr);
    		from->port      = msuNtoHs(destUnicastAddr.sin_port);

        	//printf("\nBytes received = %d\n",(int)nBytes);
    	    if(nBytes == MSU_SOCKET_ERROR)
#ifdef Debug_level0
    		   printf("\nSocketopt error = %d\n",errno);
#endif
         }
	   }
	 }

	return nBytes;
}





/*
SINT32  msuNonBlockingSocketRecvFrom( SINT32 socketfd, void *buf, UINT16 len, msuSockinfo *from)
{


    UINT16 slen = 0;
    SINT32 nBytes = MSU_SOCKET_ERROR;
    struct sockaddr_in destUnicastAddr;
	fd_set readSet;

    slen = sizeof(struct sockaddr_in);
    timeout.tv_sec =30;
    FD_ZERO( &readSet);
	FD_CLR( socketfd, &readSet );
	FD_SET(socketfd, &readSet);

	//Call select to check the activity on the sockets
	if(select(maxFileDescriptor, &readSet, NULL, NULL, &timeout) > 0)
	{
       if(FD_ISSET(socketfd, &readSet))
       {
    	   printf("\n socket activity happend \n");

    	   nBytes = recvfrom(socketfd,(UINT8 *) buf, len, 0, (struct sockaddr *)&destUnicastAddr,(socklen_t *) &slen);
    	   from->ipAddress = msuNtoHl( destUnicastAddr.sin_addr.s_addr);
    	   from->port      = msuNtoHs(destUnicastAddr.sin_port);
    	   if(nBytes == MSU_SOCKET_ERROR)
    		   printf("\nSocketopt error = %d\n",errno);
       }
	}
	else
	{
		printf("\nSelect Time Out  error = %d\n",errno);
	}
	return(nBytes);
}

*/

/**  @fn msuSocketCreate( UINT16 port, UINT32 ipAddress )
 *   @brief starts the data mirror task
 *   @details  If task start is successful and if the component state is
 *             COMPONENT_STATE_CONFIGURED then changes the componet state to
 *             COMPONENT_STATE_STARTED and returns SUCCESS
 *   @return Success code if the operation is successful
 *           Error code if the operation is failed.
 *   @retval TR_SUCCESS If task started successfully
 *   @retval TR_EINVAL Task start failed
 *   @pre Component state should be COMPONENT_STATE_CONFIGURED
 *   @post Component state will be COMPONENT_STATE_STARTED
 */

SINT32  msuSocketRecvFrom( SINT32 socketfd, void *buf, UINT16 len, msuSockinfo *from)
{
    UINT16 slen = 0;
    SINT32 nBytes = 0;
    struct sockaddr_in destUnicastAddr;

    slen = sizeof(struct sockaddr_in);
    nBytes = recvfrom(socketfd, buf, len, 0, (struct sockaddr *)&destUnicastAddr,(socklen_t *) &slen);
	from->ipAddress = msuNtoHl(destUnicastAddr.sin_addr.s_addr);
	from->port      = msuNtoHs(destUnicastAddr.sin_port);

	return(nBytes);
}

/**  @fn msuSocketCreate( UINT16 port, UINT32 ipAddress )
 *   @brief starts the data mirror task
 *   @details  If task start is successful and if the component state is
 *             COMPONENT_STATE_CONFIGURED then changes the componet state to
 *             COMPONENT_STATE_STARTED and returns SUCCESS
 *   @return Success code if the operation is successful
 *           Error code if the operation is failed.
 *   @retval TR_SUCCESS If task started successfully
 *   @retval TR_EINVAL Task start failed
 *   @pre Component state should be COMPONENT_STATE_CONFIGURED
 *   @post Component state will be COMPONENT_STATE_STARTED
 */

SINT32 msuSocketSendTo( SINT32 socket, const void *msg, SINT32 len, const msuSockinfo *to )
{
	struct sockaddr_in destUnicastAddr;
    SINT32 sentStatus = 0;

    destUnicastAddr.sin_family = AF_INET;
	destUnicastAddr.sin_port = msuHtoNs(to->port);
	destUnicastAddr.sin_addr.s_addr = msuHtoNl(to->ipAddress);
	sentStatus = sendto( socket, (UINT8*)msg, len, 0, (struct sockaddr*)&destUnicastAddr, sizeof(struct sockaddr) );

	if(sentStatus == MSU_SOCKET_ERROR)
	{
#ifdef Debug_level1
		printf("\nSent socket failed error = %d\n",errno);
#endif
	}
    return(sentStatus);

}

/**  @fn msuSocketCreate( UINT16 port, UINT32 ipAddress )
 *   @brief starts the data mirror task
 *   @details  If task start is successful and if the component state is
 *             COMPONENT_STATE_CONFIGURED then changes the componet state to
 *             COMPONENT_STATE_STARTED and returns SUCCESS
 *   @return Success code if the operation is successful
 *           Error code if the operation is failed.
 *   @retval TR_SUCCESS If task started successfully
 *   @retval TR_EINVAL Task start failed
 *   @pre Component state should be COMPONENT_STATE_CONFIGURED
 *   @post Component state will be COMPONENT_STATE_STARTED
 */

SINT16 msuSocketClose(SINT32 Socket)
{
    SINT16 retStatus;

    retStatus = close(Socket);

    return(retStatus);
}







#endif /*#ifndef __MSU_SOCKET_C__*/
