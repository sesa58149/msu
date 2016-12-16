/**	@file MsuSocket.h
 *	<TABLE>
 *	@brief Implementation of Mass Software Upgrade Socket header
 *
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
 *	@author Deepak G
 *	@par HISTORY
 *
 *	<TABLE>
 *	<TR>
 *	<TD>Author</TD>			<TD>Date</TD>			<TD>Description</TD>
 *	</TR>
 *	<TR>
 *	<TD>Deepak G</TD>	<TD>03-Oct-2011</TD>	<TD>First creation</TD>
 *	</TR>
 *
 *	</TABLE>
 *	</TABLE>
 */

#ifndef __MSU_SOCKET_H__
#define __MSU_SOCKET_H__

#include "MsuPlatform.h"
#include "MsuLeech.h"
#include "MsuSocketInterface.h"

#define MSU_MCAST_SYNC_PORT 	567
#define MSU_MCAST_DEFAULT_IP 	"239.254.1.2"


/**  @fn void msuSocketInit(void)
*    @brief This function is used to intialize socket variables
*/ 
void msuSocketInit(void);

/**  @fn Sint32 msuDefaultSocketCreate()
*    @brief This function is used to create a default socket
*/ 
SINT32 msuDefaultSocketCreate();

/**  @fn Sint32 msuUpdateSocketCreate(UINT16 port, UINT32 mIpAddr)
*    @brief This function is used to create updated socket
*/ 
SINT32 msuUpdateSocketCreate(UINT16 port, UINT32 mIpAddr);

/**  @fn BOOL msuSocketGetPacket()
*    @brief msuSocketGetPacket( ) function uses default socket to receive messages in blocking mode
*/ 
BOOL msuSocketGetPacket();

/**  @fn BOOL msuSendScmMsg(UINT32 bitMap, UINT32 chunkNum, UINT8 seqCount  )
*    @brief This function is used to send SCM message to server
*/ 
BOOL msuSendScmMsg(UINT32 bitMap, UINT32 chunkNum, UINT8 seqCount  );

/**  @fn BOOL msuSendCcmMsg( UINT32 totalChnksExpected )
*    @brief This function is used to send CCM message to server
*/ 
BOOL msuSendCcmMsg( UINT32 totalChnksExpected );

/**  @fn BOOL msuSendUpgradeStatusMsg(BOOL upgradeStatus)
*    @brief This function is used to send Upgrade status message
*/ 
BOOL msuSendUpgradeStatusMsg(BOOL upgradeStatus);

/**  @fn BOOL msuSocketCheckActivity(void)
*    @brief This function is used to monitor activitied on updated socket
*/ 
BOOL msuSocketCheckActivity(void);

/**  @fn void msuSocketRelease(void)
*    @brief This function is used to close default and updated sockets
*/ 
void msuSocketRelease(void);

/**  @fn void msuRegisterCallBackMethod(UINT8 callBackType, void (*fptr)(EMSUState),void* arg)
*    @brief This function is used to register call back function, this call back function executes when 
*           socket receives Abort message
*/  
void msuRegisterCallBackMethod(UINT8 callBackType, void (*fptr)(EMSUState),void* arg);

/**  @fn void msuSendGroupCreationAckMsg( UINT8 *gMsg  )
*    @brief This function is used to send Group creation Ack message
*/ 
void msuSendGroupCreationAckMsg( UINT8 *gMsg  );

/**  @fn void msuSocketProcessDiscoveryMessage(UINT8 *pMsg, UINT32 msgSize)
*    @brief This function is used to handle discovery message and sends discovery response message
*/ 
void msuSocketProcessDiscoveryMessage(UINT8 *pMsg, UINT32 msgSize);

BOOL msuSendAuthenticationResponse(MSUAuthenticationRequestPdu *pAuthenticationRequestPdu,UINT32 msgSize);

#endif/*#ifndef __MSU_SOCKET_HPP__*/




