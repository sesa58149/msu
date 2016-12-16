/**@file MsuSocketInterface.c
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

#include "TcpSocketImpl.h"
#include "trsocket.h"
#include <errno.h>
#include "IgmpSnooping.h"
#include "MsuSocketInterface.hpp"
#include "LogMsg.h"

static struct timeval timeout;
static Sint16 maxFileDescriptor=0;

#pragma optimization_level 0

#define MSU_MAX_PACKET_SIZE  1500

UINT32 msuIpfromStr(char* ipStr)
{
	return inet_addr(ipStr);
}


/**  @fn msuSocketCreate( UINT16 port, UINT32 ipAddress )
 *   @brief msuSocketCreate() is used to create a socket
 *   @details  msuSocketCreate() function is used create a socket and binds the socket with
 *             specified port number. this function also registers multicast IP address to the socket
 *             by using setsockopt() call to receive multicast frames.
 *   @param[in] : Port number 
 *   @param[in] : Multicast IP address UINT32 passed as a parameter 
 *   @return socket descriptor if the operation is successful
 *           Error code if the operation is failed.
 *   @retval socket descriptor ( integer value) if created successfully
 *   @retval -1 if failed to create socket
  */
MSU_SOCKET msuSocketCreate( UINT16 port, UINT32 ipAddress )
{
	MSU_SOCKET tempSocket = MSU_INVALID_SOCKET;
	struct ip_mreq multicastAddr;
	Sint32 socketStatus = MSU_SOCKET_SUCCESS;
	struct trSockAddrIn sockAddress;
	Sint32 trueFlag = TRUE;
   Sint16 retVal = 0;
   
	/*Create a socket for UDP message*/
   retVal = trSocket( AF_INET, SOCK_DGRAM, 0, &tempSocket, "create socket");	
   
	if (retVal != TR_SUCCESS)
	{
#ifdef DEBUG_LEVEL2  	
		msuLogMsg( ERRMSG,"\nSocket Creation Failed \n");
#endif		
	}
	else
	{
		msuMemset((char *) &sockAddress, 0, sizeof(sockAddress));
		sockAddress.sinFamily = AF_INET;
		sockAddress.sinPort = trHtoNs(port);
		sockAddress.sinAddr.sAddr = trHtoNl(INADDR_ANY);
		
		socketStatus = trSetSockOpt( tempSocket, TRSOL_SOCKET, TRSO_REUSEADDR,(char *) &trueFlag, sizeof(trueFlag),"set sock opt");
					
		if (socketStatus != TR_SUCCESS)
		{
#ifdef DEBUG_LEVEL2  		
			msuLogMsg( ERRMSG,"\nSocketopt reuse option failed \n");
#endif			
		}
		else
		{	/*Bind the socket to the port*/
         socketStatus = trBind( (UINT32)tempSocket,  (struct trSockAddr *) &sockAddress,sizeof(sockAddress),"bind");			
			
			if (socketStatus != TR_SUCCESS)
			{
#ifdef DEBUG_LEVEL2  			
				msuLogMsg( ERRMSG,"\nSocket binding failed for port %d\n", port);
#endif				
			}
			else
			{	/* Join a multicast group on all interfaces*/
				multicastAddr.imr_multiaddr.s_addr = ipAddress;
				#ifdef ETHERBRICK_PLATFORM
  					multicastAddr.imr_interface.s_addr = clientIP;
  				//	joinIgmpMldGroupForSwitch((unsigned char *)&ipAddress,IGMP_MESSAGE);
				#else
  					multicastAddr.imr_interface.s_addr = (inet_addr((char *)clientIP));
				#endif 

            socketStatus = trSetSockOpt( tempSocket, TRIPPROTO_IP, IP_ADD_MEMBERSHIP,(char *) &multicastAddr, sizeof(multicastAddr),"set sock opt");						
				if (socketStatus != TR_SUCCESS)
				{
#ifdef DEBUG_LEVEL2  				
					msuLogMsg( ERRMSG,"\nAdd membership socket option failed\n");
#endif					
				}
			}
		}
		if (socketStatus != TR_SUCCESS)
		{
#ifdef DEBUG_LEVEL2  		
			msuLogMsg( ERRMSG,"\nSocketopt error = %d\n",errno);
#endif			
			trClose( (UINT32)tempSocket, "close");
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
 *   @param[in] :  socketfd  - socket file descriptor
 *   @param[in] :  timeout_sec - time out in seconds used by select system call
 *   @return   None
 *
 *   @retval   None
 *   @retval   None
  */
void setTimeoutVal( Sint32 socketfd, UINT8 timeout_sec )
{
	timeout.tv_sec = timeout_sec;
	timeout.tv_usec = 0;

	maxFileDescriptor = (Sint16)(socketfd + 1);
}


/**  @fn SINT32  msuNonBlockingSocketRecvFrom( SINT32 socketfd, void *buf, UINT16 len, msuSockinfo *from)
 *   @brief  This function is used to receive data from Non blocking sockets
 *   @details  This function is used to receive data from socket, it uses select system call to check the 
               activity on the specified socket,if activity happend on the socket then it reads the data
               from the socket. 
 *   @param[in] : socketfd - socket descriptor
 *   @param[in] : buf - receive buffer 
 *   @param[in] : len - length of the data to receive
 *   @param[in] : from - This is the structure pointer of msuSockinfo which contains port number and IP address variables,
                  This structure will be populated by fuction once it receive data from the source, This IP and port numbers 
                  are source IP and port from which frame received.
 *   @return returns number of bytes count read successful
 *           Error code if the operation is failed.
 *   @retval Bytes count if read successfully
 *   @retval -1 if failed 
 */

Sint32  msuNonBlockingSocketRecvFrom( Sint32 socketfd, void *buf, UINT16 len, msuSockinfo *from)
{
	struct  trtimeval timeout_value;   
	trfd_set readSet;
	struct trSockAddrIn fromAddr;
	
	UINT32 slen = 0;
	Sint32 nBytes = MSU_SOCKET_TIMEOUT_ERROR;
	Uint32 eventCount = 0;
	Sint16 retStatus = TR_SUCCESS;


	slen = sizeof(struct trSockAddrIn);

	//Adjust the waiting timeout if required
	timeout_value.tv_sec = timeout.tv_sec;
	timeout_value.tv_usec = 0;

	//Initialize the file descriptor
	TRFD_ZERO( &readSet );

    //Add passive multicast socket and unicast socket to file descriptor set
	if(socketfd != MSU_INVALID_SOCKET)
	{
	  TRFD_SET(socketfd, &readSet);
	}

	//Call select to check the activity on the sockets
  	retStatus = trSelect( maxFileDescriptor, &readSet,NULL,NULL,&timeout_value,&eventCount);
  	if( retStatus == TR_SUCCESS )
  	{
  	   if( eventCount > 0)
  	   {
  	   	if(socketfd != MSU_INVALID_SOCKET)
	      {
            if(TRFD_ISSET(socketfd, &readSet))
            {
    
    				retStatus = trRecvFrom( (Sint16)socketfd, (void *)buf, (Sint32)len, (Sint16)0, (struct trSockAddr *)&fromAddr, (Sint32 *)&slen, (Sint32 *)&nBytes);               
    				if(retStatus != TR_SUCCESS)
    				{
						#ifdef DEBUG_LEVEL2    				
						    		         msuLogMsg( ERRMSG,"\nSocketopt error = %d\n",errno);
						#endif   
						nBytes = MSU_SOCKET_RECEIVE_ERROR; 		         
    				}

               from->ipAddress = trNtoHl(fromAddr.sinAddr.sAddr);
					from->port      = trNtoHs(fromAddr.sinPort);
            }
	      }

  	    }
   }
	
	return nBytes;
}


/**  @fn SINT32  msuSocketRecvFrom( SINT32 socketfd, void *buf, UINT16 len, msuSockinfo *from)
 *   @brief  This function is used to receive data from blocking sockets
 *   @details  This function is a blocking call used to receive data from socket, once the user calls
               this function, it blocks the call till the data will be read from the socket.  * 
 *   @param[in] : socketfd - socket descriptor
 *   @param[in] : buf - receive buffer 
 *   @param[in] : len - length of the data to receive
 *   @param[in] : from - This is the structure pointer of msuSockinfo which contains port number and IP address variables,
                  This structure will be populated by fuction once it receive data from the source, This IP and port numbers 
                  are source IP and port from which frame received.
 *   @return returns number of bytes count read successful
 *           Error code if the operation is failed.
 *   @retval Bytes count if read successfully
 *   @retval -1 if failed 
 */

Sint32  msuSocketRecvFrom( Sint32 socketfd, void *buf, UINT16 len, msuSockinfo *from)
{
    Sint32 slen = 0;
    Sint32 nBytes = 0;
    struct trSockAddrIn fromAddr;
    Sint16 retStatus = TR_SUCCESS;

    slen = sizeof(struct trSockAddrIn);
    
    retStatus = trRecvFrom( (Sint16)socketfd, buf, len, 0, (struct trSockAddr *)&fromAddr, &slen, &nBytes);    
    if( retStatus == TR_SUCCESS)
    {
		from->ipAddress = trNtoHl(fromAddr.sinAddr.sAddr);
		from->port      = trNtoHs(fromAddr.sinPort);
    }
    
	return(nBytes);
}

/**  @fn SINT32  msuSocketSendTo( SINT32 socketfd, void *msg, UINT16 len, msuSockinfo *to)
 *   @brief  This function is used to send data 
 *   @details  This function is used to send data to the specified destination address and port number.
 *   @param[in] : socketfd - socket descriptor
 *   @param[in] : buf - transmit buffer contains data
 *   @param[in] : len - length of the data to sent
 *   @param[in] : to - This is the structure pointer of msuSockinfo which contains destination port number and IP address,
                  This address will be used by the call to send data to the destination.
 *   @return returns number of bytes sent successfully
 *           Error code if the operation is failed.
 *   @retval Bytes count sent successfully
 *   @retval -1 if failed 
 */

Sint32 msuSocketSendTo( Sint32 socket, const void *msg, Sint32 len, const msuSockinfo *to )
{
	struct sockaddr_in  destUnicastAddr;
	
   Sint16 sentStatus = 0;
   Sint32 actualByteSent = 0;

	destUnicastAddr.sin_family      = TRAF_INET;
	destUnicastAddr.sin_port        = trHtoNs(to->port);
	destUnicastAddr.sin_addr.s_addr = trHtoNl(to->ipAddress);

	
	sentStatus = trSendTo( socket, msg, len,0, (struct trSockAddr *)&destUnicastAddr,sizeof(struct trSockAddr), &actualByteSent);//trSockAddrIn

	if(sentStatus != TR_SUCCESS)
	{
#ifdef DEBUG_LEVEL2	
		msuLogMsg( ERRMSG,"\nSent socket failed error = %d\n",errno);
#endif
      actualByteSent = -1;		
	}
   
   
   return(actualByteSent);

}

/**  @fn SINT16  msuSocketClose( SINT32 socket)
 *   @brief  This function is used to close the socket
 *   @details  This function is used to close the specified socket.
 *   @param[in] : Socket - socket descriptor
 *   @return returns success if socket closed successfully
 *           Error code if the operation is failed.
 *   @retval TR_SUCCESS id socket closed successfully
 *   @retval TR_ERROR (-1) if failed 
 */
Sint16 msuSocketClose(Sint32 Socket)
{
    Sint16 retStatus = TR_SUCCESS;

    retStatus = trClose( (UINT32)Socket, "close");
    
    if( retStatus == TR_SUCCESS)
       return TR_SUCCESS;
    else
       return TR_ERROR;
    
}


#endif /*#ifndef __MSU_SOCKET_C__*/
