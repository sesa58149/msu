#ifndef __MSU_SOCKET_CPP__
#define __MSU_SOCKET_CPP__

#include <stdio.h>
#include <string.h>
#include "MsuSocketInterface.hpp"
#include "MsuPdu.hpp"
#include "MsuSocket.hpp"
#include "MsuComplainMode.hpp"
#include "MsuLeech.hpp"
#include "LogMsg.h"

#pragma optimization_level 0

//extern UINT32 * missedChnkNums;
//extern UINT32 missedChnkNums[MAX_NUM_CHNKS];

/*Use for listening mode*/
static MSU_SOCKET msuDefaultSocket;
/*Used in update process*/
static MSU_SOCKET msuUpdateSocket;
/* Hold data in Big Endian Format*/
msuSockinfo destDeviceAddr;

MSUAuthenticationRequestPdu pAuthenticationRequestPdu;
static UINT8 gMsgBuffer[MSU_MAX_PACKET_SIZE];

#ifdef ETHERBRICK_PLATFORM
	UINT32 clientIP;
#else
	char clientIP[20];
#endif

//extern UINT32 missedChnkArr[MAX_NUM_CHNKS];
extern MsuDeviceInfo msuDeviceInfo;

extern UINT8   MsuGlobalError; 

BOOL msuSendDiscoveryResponse(MSUDiscoveryRequestPdu *discoveryRequestPdu);
BOOL msuSendAuthenticationResponse(MSUAuthenticationRequestPdu *pAuthenticationRequestPdu,UINT32 msgSize);
/*BOOL msuAuthenticationRequest(UINT8 *pMsg,UINT32 msgSize,MSUAuthenticationRequestPdu *pAuthenticationRequest,
							  MSUAuthenticationResponsePdu *msuAuthenticationResponse, msuSockinfo *socketip );*/
							  
static void (*ftprChangeDeviceState)(EMSUState state);


/**  @fn void msuSocketInit(void)
 *   @brief msuSocketInit() function is used to intialize variables 
 *   @param[in] : None
 *   @return    : None
 */
void msuSocketInit(void)
{
   msuDefaultSocket = MSU_INVALID_SOCKET;
	msuUpdateSocket = MSU_INVALID_SOCKET;

   msuMemset(&gMsgBuffer, 0, (sizeof(UINT8) * MSU_MAX_PACKET_SIZE));
}


/**  @fn void msuRegisterCallBackMethod(UINT8 callBackType, void (*fptr)(EMSUState),void* arg)
 *   @brief msuRegisterCallBackMethod() is used to register a function, This function will be called when ever socket 
 *          receives a abort message from server   
 *   @param[in] : UINT8 callBackType          Unused variable
 *   @param[in] : void (*fptr)(EMSUState)     function pointer which you want to register function
 *   @param[in] : void* arg                   arg not used 
 *   @return    : None
 */
void msuRegisterCallBackMethod(UINT8 callBackType, void (*fptr)(EMSUState),void* arg)
{
	ftprChangeDeviceState = fptr;
}


/**  @fn BOOL SINT32 msuDefaultSocketCreate()
 *   @details  msuDefaultSocketCreate() function is used to create a default socket with default port number
 *             and multicast Ip address.
 *   @param[in] : None
 *   @return    : MSU_SOCKET_SUCCESS, if default socket created successful, otherwise MSU_SOCKET_ERROR
 *   @retval  0  if successfu
 *   @retval  -1 if failed 
 */
SINT32 msuDefaultSocketCreate()
{
   SINT32 status = MSU_SOCKET_SUCCESS;

	msuDefaultSocket = msuSocketCreate(MSU_MCAST_SYNC_PORT,msuIpfromStr(MSU_MCAST_DEFAULT_IP));
	if(msuDefaultSocket == MSU_INVALID_SOCKET)
	{
		#ifdef DEBUG_LEVEL2	
			msuLogMsg( ERRMSG,"\nSetting up Default socket failed for port = %d\n",
					MSU_MCAST_SYNC_PORT);
		#endif				
		status = MSU_SOCKET_ERROR;
	}
	return status;
}

/**  @fn SINT32 msuUpdateSocketCreate(UINT16 port, UINT32 mIpAddr)
 *   @details  msuUpdateSocketCreate( ) function is used to create a updated socket with port number and 
 *             multicast Ip address received in Sync message
 *   @param[in] : UINT16 port     socket port number
 *   @param[in] : UINT32 mIpAddr  Multicast Ip Address
 *   @return    : MSU_SOCKET_SUCCESS, if socket created successful, otherwise MSU_SOCKET_ERROR
 *                 
 *   @retval  0  if successfu
 *   @retval  -1 if failed 
 */
SINT32 msuUpdateSocketCreate(UINT16 port, UINT32 mIpAddr)
{
   SINT32 status = MSU_SOCKET_SUCCESS;

	msuUpdateSocket = msuSocketCreate(port, mIpAddr);
	if(msuUpdateSocket == MSU_INVALID_SOCKET)
	{
		#ifdef DEBUG_LEVEL2		
			msuLogMsg( ERRMSG,"\nSetting up Updated socket failed for port = %d\n",
					MSU_MCAST_SYNC_PORT);
		#endif				
		status = MSU_SOCKET_ERROR;
	}
	else
	{
		setTimeoutVal(msuUpdateSocket,msuBroadcastPdu.msuSyncHeaderformat.updateTimeout);
	}
	return status;
}

/**  @fn BOOL msuSocketCheckActivity(void)
 *   @details  msuSocketCheckActivity( ) function internally calls msuSocketGetPacket() function to check activities
 *             on defualt socket.
 *   @param[in] : None
 *   @return    : TRUE or FALSE, 
 *                 
 *   @retval  1  if successful
 *   @retval  0 if failed 
 */
BOOL msuSocketCheckActivity(void)
{
	return  msuSocketGetPacket();
}


/**  @fn void msuSocketProcessDiscoveryMessage(UINT8 *pMsg, UINT32 msgSize)
 *   @details  msuSocketProcessDiscoveryMessage( ) function is responsible for handling Discovery request, validating 
 *             discovery request and sending discovery response to the server
 *   @param[in] : UINT8 *pMsg   discovery request message buffer
 *   @param[in] : UINT32 msgSize  size of discovery request message
 *   @return    : None
 *   @retval      None
 */
void msuSocketProcessDiscoveryMessage(UINT8 *pMsg, UINT32 msgSize)
{
   MSUDiscoveryRequestPdu discoverRequest;

   if (msuParseDiscoverRequest(pMsg, msgSize, &discoverRequest)== TRUE)
	   msuSendDiscoveryResponse(&discoverRequest);

}


/**  @fn BOOL msuSocketGetPacket()
 *   @details  msuSocketGetPacket( ) function uses default socket to receive messages in blocking mode, 
 *             once it receives a message it validates to check what type of message, it may be Discovery request message
 *             or Sync message or Device status update request message or group creation message, depends on message type
 *             it calls related functions to handle that type of request
 *   @param[in] : None
 *   @return    : TRUE, if it received Sync message and validtion passed,, otherwise FALSE
 *   @retval  1  if successfully  Sync message received and validation passed
 *   @retval  0  Handling other than Sync message 
 */
BOOL msuSocketGetPacket()
{
   BOOL status = FALSE;
 	UINT32 nByte = 0;
 	MSUCmd command = {0};
 	MSUHeader *msuHeader;

   nByte = msuSocketRecvFrom( msuDefaultSocket, gMsgBuffer, MSU_MAX_PACKET_SIZE,&destDeviceAddr);
	if (nByte != MSU_SOCKET_ERROR)
	{
	   /*Get the command type*/
		msuHeader = (MSUHeader*)gMsgBuffer;
		if(isMsuVersion(msuHeader)== FALSE) return 0;

		command = *((MSUCmd *)gMsgBuffer);
		/*Process the messages based on the message type*/
		if(command.opcode == MSU_OPCODE_DEVICE_DISCOVER && command.subcode == MSU_SUBCODE_DEVICE_DISCOVERY_REQUEST)
		{
			#ifdef DEBUG_LEVEL2
		 		 msuLogMsg( ERRMSG,"\n==> Discovery request\n");
			#endif		 	 
			msuSocketProcessDiscoveryMessage(gMsgBuffer, nByte);
		}
		else if (command.opcode == MSU_OPCODE_AUTHENTICATION && command.subcode == MSU_SUBCODE_AUTHENTICATION_REQUEST)
		
		{
			msuSendAuthenticationResponse(&pAuthenticationRequestPdu,nByte);
			
		}
		
	    else if(command.opcode == MSU_OPCODE_AUTHENTICATION && command.subcode == MSU_SUBCODE_DISCONNECT_REQUEST)
		{
//			msuPasrseDisconnectMessage(gMsgBuffer,nByte);
		}
		
		else if ((command.opcode == MSU_OPCODE_UPGRADE) && ( (command.subcode == MSU_SUBCODE_UPGRADE)||
		   		 (command.subcode == MSU_SUBCODE_FORCE_UPGRADE) || (command.subcode == MSU_SUBCODE_DOWNGRADE))
				  )
		{
			#ifdef DEBUG_LEVEL2		 
				msuLogMsg( ERRMSG,"\n==> Sync Message\n");
			#endif			 
			/*Process Sync Message*/
			status = msuSocketProcessSyncMessage(gMsgBuffer, nByte);
			if(status == TRUE)
			{	 /*Close the previously opened sockets*/
				msuSocketClose(msuDefaultSocket);
			}
		}
		else if ( (command.opcode == MSU_OPCODE_UPGRADE) && (command.subcode == MSU_SUBCODE_CLIENT_STATUS_REQUEST) )
		{
		   if (msuHnadleDeviceStatusUpdateRequest((char*)gMsgBuffer, (UINT32)nByte) == TRUE)
			   msuSendUpgradeStatusMsg(TRUE);
		}
		else if(command.opcode == MSU_OPCODE_DEVICE_GROUP)
		{
		   if(msuHandleDeviceGroupMsg(gMsgBuffer, (UINT32)nByte) == TRUE)
			   msuSendGroupCreationAckMsg( gMsgBuffer);
		}
	}
	return status;
}


/**  @fn BOOL msuSocketGetDataPackets( )
 *   @details  This function is used to receive data packets on updated socket in non blocking mode
 *             This received packets may be a actual data packets or Transfer complete request packet or
 *             CCM Transfer complete request or Abort request, this function calls other functions to handle 
 *             these requests. It calls msuPduParseMcastRequest( ) to handle Data transfer request, calls 
 *             msuPduHandleTransferCompleteRequest( ) for Tranfer complete request handling, calls 
 *             msuPduHandleCCMCompleteRequest() for CCM transfer complete request handling and call registered function
 *             handler for abort request
 *   @param[in] : None
 *   @return    : function returns TRUE or FALSE state from the requested calls
 */

BOOL msuSocketGetDataPackets( )
{
   SINT32 recvStatus  = MSU_SOCKET_SUCCESS;
 	BOOL status = FALSE;
 	MSUCmd command = {0};

 	if(msuUpdateSocket == MSU_INVALID_SOCKET) return status;

 	recvStatus = msuNonBlockingSocketRecvFrom(msuUpdateSocket, gMsgBuffer, MSU_MAX_PACKET_SIZE ,&destDeviceAddr);
 
 	if ( (recvStatus != MSU_SOCKET_TIMEOUT_ERROR) && (recvStatus != MSU_SOCKET_RECEIVE_ERROR) )
 	{
 	   /*Get the command type*/
	 	command = *((MSUCmd *)gMsgBuffer);
	 	if( command.opcode == MSU_OPCODE_UPGRADE )
	 	{
	 	   /*Process the multicast packet*/
		 	switch(command.subcode)
		 	{
		 		case MSU_SUBCODE_DATA_TRANSFER:
		 		case MSU_SUBCODE_UPGRADE:
			 		status = msuPduParseMcastRequest(gMsgBuffer, (UINT32) recvStatus);
		 			break;

		 		case MSU_SUBCODE_TRANSFER_COMPLETE:
		 			status = msuPduHandleTransferCompleteRequest( );//gMsgBuffer, (UINT32) recvStatus
		 			break;


		 		case MSU_SUBCODE_SCM_TRANSFER_COMPLETED:
		 			status = msuPduHandleSCMCompleteRequest(gMsgBuffer, (UINT32) recvStatus);
		 			break;

		 		case MSU_SUBCODE_CCM_TRANSFER_COMPLETED:
		 			status = msuPduHandleCCMCompleteRequest(gMsgBuffer, (UINT32) recvStatus);
		 			break;

		 		case MSU_SUBCODE_ABORT_TRANSFERT:
			 		ftprChangeDeviceState(eMsuStateAbort);
			 		status = FALSE;
		 			break;

		 		default:
		 			status = FALSE;
		 	}
	 	}
 	}
 	else if( recvStatus == MSU_SOCKET_TIMEOUT_ERROR)
 	{
	   EMSUState currState = getCurrentMsuState( );
	   if( currState == eMsuStateRunning) 
	  	   MsuGlobalError = MSU_UPDATE_TIMEOUT;
	   if( currState == eMsuStateSCM) 
	  	   MsuGlobalError = MSU_UPDATE_SCM_FAILED;
	   if( currState == eMsuStateCCM) 
	  	   MsuGlobalError = MSU_UPDATE_CCM_FAILED;
	  
	   ftprChangeDeviceState(eMsuStateAbort);
 	}
 	else if( recvStatus == MSU_SOCKET_RECEIVE_ERROR )
 	{
 		MsuGlobalError = MSU_UPDATE_SOCKET_RX_FAILED;
 	}
 return status;
}


/**  @fn void msuSocketRelease(void)
 *   @brief msuSocketRelease() function is used to close updated socket and default socket
 *   @param[in] : None
 *   @return    : None
 */
void msuSocketRelease(void)
{
   if(msuUpdateSocket != MSU_INVALID_SOCKET)
	{
		msuSocketClose(msuUpdateSocket);
		msuUpdateSocket = MSU_INVALID_SOCKET;
	}
	if(msuDefaultSocket != MSU_INVALID_SOCKET)
	{
		msuSocketClose(msuDefaultSocket);
		msuDefaultSocket = MSU_INVALID_SOCKET;
	}
}


/**  @fn BOOL msuSendScmMsg(UINT32 bitMap, UINT32 chunkNum, UINT8 missedSeqCount  )
 *   @details This function is used to send SCM message to server, it internally calls  
 *            msuScmMsgCreate( ) to construct a message and uses  msuSocketSendTo() to 
 *            send to the server 
 *   @param[in] : UINT32 bitMap      Sequence bit map value
 *   @param[in] : UINT32 chunkNum    Chunk number
 *   @param[in] : UINT8 missedSeqCount  missed sequence count number
 *   @return    : TRUE, if sent sucessful , otherwise returns FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */
BOOL msuSendScmMsg(UINT32 bitMap, UINT32 chunkNum, UINT8 missedSeqCount  )
{
	SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
    BOOL status = FALSE;
    msuSockinfo destAddr;

    MSUSCMPdu *scmMsgPdu = (MSUSCMPdu *) gMsgBuffer;

    msuScmMsgCreate( scmMsgPdu, bitMap, chunkNum, missedSeqCount );

    destAddr.port = msuBroadcastPdu.msuSyncHeaderformat.multicastPort;
    //destAddr.ipAddress = msuBroadcastPdu.msuSyncHeaderformat.serverIP;
    sentStatus = msuSocketSendTo( msuUpdateSocket, scmMsgPdu, sizeof(MSUSCMPdu), &destAddr );

    if(sentStatus != MSU_SOCKET_ERROR )
    	status = TRUE;
    else
       MsuGlobalError = MSU_UPDATE_SOCKET_TX_FAILED;

	return status;
}


/**  @fn BOOL msuSendDiscoveryResponse(MSUDiscoveryRequestPdu *discoveryRequestPdu)
 *   @details msuSendDiscoveryResponse( ) function is used send discovery response message
 *            to the client, it internally calls msuDiscoveryResponseMsgCreate() to construct
 *            response message and uses msuSocketSendTo() to send to server
 *   @param[in] : MSUDiscoveryRequestPdu *discoveryRequestPdu   pointer to a discovery request Pdu
 *   @return    : TRUE, if sent sucessful , otherwise returns FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */
BOOL msuSendDiscoveryResponse(MSUDiscoveryRequestPdu *discoveryRequestPdu)
{
   SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
   BOOL status = FALSE;
   MSUDiscoveryResponsePdu *msuDiscoveryResponse = (MSUDiscoveryResponsePdu *)gMsgBuffer;
   UINT16 dataPayloadSize = 0;

   msuDiscoveryResponseMsgCreate( discoveryRequestPdu, msuDiscoveryResponse,&dataPayloadSize );

   sentStatus = msuSocketSendTo( msuDefaultSocket, gMsgBuffer,
    		             				(sizeof(MSUDiscoveryResponsePdu) + dataPayloadSize), &destDeviceAddr);

   if(sentStatus == MSU_SOCKET_ERROR )
	{
		#ifdef DEBUG_LEVEL2	
		msuLogMsg( ERRMSG,"\n Failed to send Discovery response Socket error ");
		#endif
	}
	else
	{
		#ifdef DEBUG_LEVEL2	
		msuLogMsg( ERRMSG,"\n****Discovery response Sent*** !");
		#endif		
		status = TRUE;
	}
	return status;
}


BOOL msuSendAuthenticationResponse(MSUAuthenticationRequestPdu *pAuthenticationRequestPdu,UINT32 msgSize)
{
	
	SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
   	BOOL status = FALSE;
	MSUAuthenticationResponsePdu *pAuthenticationResponsePdu=(MSUAuthenticationResponsePdu *)gMsgBuffer;
	
	msuAuthenticationRequest(gMsgBuffer,msgSize,pAuthenticationRequestPdu,pAuthenticationResponsePdu,&destDeviceAddr);
	/*Populating the global variable with the transactionId value recieved in the Authentication request*/             				
	
	sentStatus = msuSocketSendTo( msuDefaultSocket, gMsgBuffer,
    		             		(sizeof(MSUAuthenticationResponsePdu) ), &destDeviceAddr);
    	
    if(sentStatus == MSU_SOCKET_ERROR )
	{
		#ifdef DEBUG_LEVEL2	
		msuLogMsg( ERRMSG,"\n Failed to send Authentication response Socket error ");
		#endif
	}
	else
	{
		#ifdef DEBUG_LEVEL2	
		msuLogMsg( ERRMSG,"\n****Authentication response Sent*** !");
		#endif	
		status = TRUE;	
	}         
	return status;
}


/**  @fn BOOL msuSendUpgradeStatusMsg(BOOL isDefault)
 *   @details msuSendUpgradeStatusMsg( ) function is used send upgrade status message to the server
 *            it calls msuUpgrateMsgCreate() function to construct upgrade message format
 *   @param[in] : BOOL isDefault     pass TRUE if want to send message using default socket
 *                                   pass FALSE if want to send message using Updated socket     
 *   @return    : TRUE, if sent sucessful , otherwise returns FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */
BOOL msuSendUpgradeStatusMsg(BOOL isDefault)
{
   SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
   BOOL status = FALSE;
   MSUUpgradeStatusPdu msuUpgradeStatus;
   EMSUUpgradeState upgradeStatus;
   UINT8 ccmRetryVal = 0;

   upgradeStatus = msuGetUpgradeProcessState();
   ccmRetryVal = getCcmRetryCount();
   msuUpgrateMsgCreate( &msuUpgradeStatus, (SINT32)upgradeStatus, ccmRetryVal );

   if(isDefault)
   {
      sentStatus = msuSocketSendTo( msuDefaultSocket, (MSUUpgradeStatusPdu*)&msuUpgradeStatus,
    															(sizeof(MSUUpgradeStatusPdu)), &destDeviceAddr );
   }
   else
   {
      sentStatus = msuSocketSendTo( msuUpdateSocket, (MSUUpgradeStatusPdu*)&msuUpgradeStatus,
    	    		             							(sizeof(MSUUpgradeStatusPdu)), &destDeviceAddr );
   }
   if(sentStatus == MSU_SOCKET_ERROR )
	{
		#ifdef DEBUG_LEVEL3	
		msuLogMsg( ERRMSG,"\n Failed to send upgrade status");
		#endif
	}
	else
	{
		#ifdef DEBUG_LEVEL3
		msuLogMsg( ERRMSG,"\n****Upgrade status Sent*** !");
		#endif		
		status = TRUE;
	}
	return status;
}


/**  @fn BOOL msuSendCcmMsg( UINT32 totalChnksExpected  )
 *   @details msuSendCcmMsg( ) function is used send CCM message to server, it calls msuCcmMsgCreate( ) to 
 *            construct CCM message format and calls msuSocketSendTo( ) to send to the server
 *   @param[in] : UINT32 totalChnksExpected     total number of chunks in the transfered file
 *   @return    : TRUE if transfered successful, otherwise return FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */
BOOL msuSendCcmMsg( UINT32 totalChnksExpected  )
{
   SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
	BOOL status = FALSE;
	UINT32 missedChnkCnt = 0;
	MSUCCMPdu *ccmMsgPdu = (MSUCCMPdu *)gMsgBuffer;

	msuSockinfo destAddr;

	msuCcmMsgCreate( ccmMsgPdu, totalChnksExpected, &missedChnkCnt );

	destAddr.port = msuBroadcastPdu.msuSyncHeaderformat.multicastPort;
	destAddr.ipAddress = msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv4;
	sentStatus = msuSocketSendTo( msuUpdateSocket, (UINT8*)ccmMsgPdu, sizeof(MSUCCMPdu) + (sizeof(UINT32) * missedChnkCnt ),
																													&destAddr );

	if(sentStatus == MSU_SOCKET_ERROR )
	{
		#ifdef DEBUG_LEVEL2	
		msuLogMsg( ERRMSG,"\n Failed to send CCM Unicast");
		#endif
      MsuGlobalError = MSU_UPDATE_SOCKET_TX_FAILED;		
	}
	else
	{
		#ifdef DEBUG_LEVEL2	
		msuLogMsg( ERRMSG,"\n****CCM Unicast Sent*** !");
		#endif		
		status = TRUE;
	}
	return status;
}



/**  @fn void msuSendGroupCreationAckMsg( UINT8 *gMsg  )
 *   @details msuSendGroupCreationAckMsg( ) function is used send group creation Ack message to the server,
 *            it calls msuGroupAckMsgCreate( ) f'n to construct Group creation Ack message format and calls msuSocketSendTo( ) to send to the server
 *   @param[in] : UINT32 totalChnksExpected     total number of chunks in the transfered file
 *   @return    : TRUE if transfered successful, otherwise return FALSE
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */

void msuSendGroupCreationAckMsg( UINT8 *gMsg  )
{
   SINT32 sentStatus =  MSU_SOCKET_SUCCESS;
	MSUGroupCreationPdu *gAckMsgPdu = NULL;
	MSUGroupCreationPdu *gReqPdu    = (MSUGroupCreationPdu *)gMsg;

	gAckMsgPdu = msuGroupAckMsgCreate( gReqPdu );
	sentStatus = msuSocketSendTo( msuDefaultSocket, (UINT8*)gAckMsgPdu, sizeof(MSUGroupCreationPdu), &destDeviceAddr );
	if(sentStatus == MSU_SOCKET_ERROR )
	{
		#ifdef DEBUG_LEVEL2	
		msuLogMsg( ERRMSG,"\n Failed to send Group Creation ACK Message ");
		#endif
	}
}




#endif /*#ifndef __MSU_SOCKET_CPP__*/
