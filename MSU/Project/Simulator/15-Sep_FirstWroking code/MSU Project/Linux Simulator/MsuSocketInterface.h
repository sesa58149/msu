/**	@file MsuSocketInterface.hpp
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

#ifndef __MSU_SOCKET_INFO_H__
#define __MSU_SOCKET_INFO_H__

#include "MsuPlatform.h"


/**  @def MSU_SOCKET_ERROR 
*    @brief Macro to be defined for Error value (-1)
*/
#define MSU_SOCKET_ERROR -1

/**  @def MSU_SOCKET_SUCCESS 
*    @brief Macro to be defined for Success value (0)
*/
#define MSU_SOCKET_SUCCESS 0

/**  @def MSU_MCAST_SYNC_PORT 
*    @brief Macro to be defined for default UDP socket port number
*/
#define MSU_MCAST_SYNC_PORT 567

/**  @def MSU_UNICAST_PORT 
*    @brief Macro to be defined for unicast socket Port number
*/
#define MSU_UNICAST_PORT 568

/**  @def MSU_SOCKET_TIMEOUT 
*    @brief Macro to be defined for max socket timeout value
*/
#define MSU_SOCKET_TIMEOUT 3600



#ifdef ETHERBRICK_PLATFORM
	extern UINT32 clientIP;
#else
	extern char clientIP[40];
	//extern UINT32 clientIP;
#endif

/*   @struct msuSockinfo
*    @brief Structure defining UINT32 IP Address and UINT16 port number
*    @details Defines structure with two variables, one for the 32 bit IP Address 
*             and other for 16 bit port number
*/
typedef struct
{
	UINT32 ipAddress;
	UINT16 port;
}msuSockinfo;


/**  @fn msuIpfromStr(char*)
*    @brief Function converts string IP address to UINT32 format IP Address
*/ 
UINT32 msuIpfromStr(char*);

/**  @fn msuSocketCreate( UINT16 port, UINT32 ipAddress  )
*    @brief Create a socket with specified port number and register with specified multicast IP Address
*/ 
MSU_SOCKET msuSocketCreate( UINT16 port, UINT32 ipAddress  );

/**  @fn msuSocketRecvFrom( Sint32 socketfd, void *buf,  UINT16 len,msuSockinfo *from)
*    @brief Used to receive data from specified socket
*/ 
SINT32  msuSocketRecvFrom( SINT32 socketfd, void *buf,  UINT16 len,msuSockinfo *from);

/**  @fn msuNonBlockingSocketRecvFrom( Sint32 socketfd, void *buf,  UINT16 len, msuSockinfo *from)
*    @brief Used to receive data from specified Non Blocking sockets 
*/ 
SINT32  msuNonBlockingSocketRecvFrom( SINT32 socketfd, void *buf,  UINT16 len, msuSockinfo *from);

/**  @fn msuSocketSendTo( Sint32 socket, const void *msg, Sint32 len, const  msuSockinfo* to )
*    @brief Used to send data to specified destination IP Address and Port number
*/ 
SINT32 msuSocketSendTo( SINT32 socket, const void *msg, SINT32 len, const  msuSockinfo* to );

/**  @fn msuSocketClose(Sint32 Socket)
*    @brief Used to close specified socket
*/ 
SINT16 msuSocketClose(SINT32 Socket);

/**  @fn setTimeoutVal( Sint32 socketfd, UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
void setTimeoutVal( SINT32 socketfd, UINT8 timeout_sec );




#endif/*#ifndef __MSU_SOCKET_H__*/
