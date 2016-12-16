/**	@file MsuPdu.cpp
 *	<TABLE>
 *	@brief Implementation of MSU Pdu
 *
 *	@copy
 *	<TABLE>
 *	<TR><TD> Copyright </TD></TR>
 *	<TR>
 *	<TD>Schneider Electric India (Pvt) Ltd.
 *	Copyright (c) 2009 - All rights reserved.
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
 *	<TD>Author</TD>				<TD>Date</TD>		<TD>Description</TD>
 *	</TR>
 *	<TR>
 *	<TD>Deepak G</TD>	<TD>04-Oct-2011</TD>	<TD>First creation</TD>
 *	</TR>
 *
 * 	</TABLE>
 *	</TABLE>
 */

#include <stdio.h>
#include <string.h>
#include "MsuPlatform.h"
#include "MsuPdu.h"
#include "MsuLeech.h"
#include "complainMode.h"
#include "msuErrNo.h"

MSUSyncMessagePdu msuBroadcastPdu;
MSUMulticastPdu msuMcastPdu;
MsuDeviceInfo msuDeviceInfo;
SecurityInfo securityData;
login_info loginVariable;
extern UINT32 g_transactionId;
/***************************/
UINT8 *pbuffer=NULL;
UINT16 location_size;
/***************************/
UINT8 gGroupID = 0;

#ifdef ETHERBRICK_PLATFORM
  extern UINT32 clientIP;
#else
  extern char clientIP[40]; // Increased to 40 to support IPV6

#endif

UINT32 serverIP = 0;
static char privatekey[]="text1";

extern UINT8   MsuGlobalError; 
MSUBasicDeviceIdentification deviceidentification;
static void msuGetDeviceInfo(void);

/**  @fn msuPduInit(void)
 *   @brief msuPduInit() is used to Intialize pdu structures
 *   @details  msuPduInit() function is used Initialize pdu sync message and multicast message structures
 *             and variables. 
 *   @param[in] : None
 *   @return None
 *   @retval None
 */
void msuPduInit(void)
{
 memset(&msuBroadcastPdu, 0, sizeof(MSUSyncMessagePdu));
 memset(&msuMcastPdu, 0, sizeof(MSUMulticastPdu));
 serverIP = 0;
 gGroupID = 0;
 msuGetDeviceInfo();
}

/**  @fn isMsuVersion( MSUHeader *header)
 *   @brief isMsuVersion() is used to validate Msu Version
 *   @details  isMsuVersion() function is used to validate MSU IP version and Msu protocol version
 *   @param[in] : header - Is a pointer type of  MSUHeader structure which contains version information
 *   @return TRUE ot FALSE
 *           Error code if the operation is failed.
 *   @retval socket descriptor ( integer value) if created successfully
 *   @retval -1 if failed to create socket
 */
BOOL isMsuVersion( MSUHeader *header)
{
	BOOL result = FALSE;
	if( (header->versionInfo.versionIP == MSU_IPV4_VERSION) &&
	    (header->versionInfo.versionProtocol == MSU_PROTOCOL_VERSION))
	{
		result = TRUE;
	}
	return result;
}


SINT32 msuDecryption(UINT8 *inputstring ,char *privatekeydecrypt,UINT32 lengthstring, UINT32 length_privatekey)
{
	
	int i=0,j=0;
	UINT8 *stringtoascii = inputstring;
	UINT8 *privatekeytoascii = (UINT8*)privatekeydecrypt;
		
	for (i=0;i<lengthstring; i++) 
	{		
		if (stringtoascii[i]<privatekeytoascii[j])
		{
			stringtoascii[i]=(UINT8)(stringtoascii[i]+(UINT8)128);
			stringtoascii[i]=(UINT8)(stringtoascii[i]-privatekeytoascii[j]);
		}		
		else
		{			
			stringtoascii[i] = (UINT8) (stringtoascii[i]-privatekeytoascii[j]);
		
		}		
	
		if(++j == length_privatekey )
			j = 0;
		
	}
	return 0 ;			
}


/**  @fn void msuGetDeviceInfo(void)
 *   @brief msuGetDeviceInfo() is used to get the device information
 *   @details  msuGetDeviceInfo() function reads the device information from msu.bin file,
 *             msu.bin contains information like product name, hardware id,product Id,model name,
 *             vendor Id, firmware version number and device location information. If msu.bin is 
 *             not available in the device then it reads default configuration from device.
 *   @param[in] : None
 *   @return    : None
 */
void msuGetDeviceInfo(void)
{
  SINT32 fileDes = -1;

  UINT16 bytesToRead = 0;
  UINT16 dataOffset = 0;
  BOOL fileConfigStatus = FALSE;
  UINT8 numberofiterations = 0;
  UINT8 dataoffset=0;

  //Initialize the device information
  memset(&msuDeviceInfo, 0, sizeof(MsuDeviceInfo));

  //Try to get the device information from the device information binary
  fileDes = msuFileOpen(MSU_DEVICE_INFO_FILE_NAME, MSU_FILE_R);
  if(fileDes != -1)
  { //File opened successfully, try to read the header information
	  bytesToRead = bytesToRead =  sizeof(MsuDeviceInfo);

	  if( msuFileRead(fileDes,(SINT8 *)&msuDeviceInfo, bytesToRead) == bytesToRead )
	  {       
		  fileConfigStatus = TRUE;	
	  }
	  msuFileClose(fileDes );
  }

  //Load the default configuration in case the configuration extraction from the file failed
  if(fileConfigStatus == FALSE)
  {
	  //Initialize the device information
	  memset(&msuDeviceInfo, 0, sizeof(MsuDeviceInfo));
		msuDeviceInfo.Client = MSU_CLIENT_STATUS;
//Fill the Master status
		
		msuDeviceInfo.Master = MSU_MASTER_STATUS;
		
	    	
		//Fill the AutheticationTag status
		
		msuDeviceInfo.AutheticationTag = MSU_AUTHENTICATION_TAG;
		
		//Fill the AuthenticationLevelTag status
		
		msuDeviceInfo.AuthenticationLevelTag = MSU_AUTHETICATION_LEVEL_TAG;
		
		//Fill the MaxIPSupported status
		
		msuDeviceInfo.NumberOfIPSupported = MSU_MAX_IP_FIELDS;
		
		//Fill the MaxIPSupported status
		
		msuDeviceInfo.noOfFileSupported = MSU_MAX_FILES_SUPPORTED;
		
	    //Fill in only one IP fields
	    #ifdef AF_INET
	    	msuDeviceInfo.IP[0].ServerIPv4=MSU_DEFAULT_SERVER_IP;
	    #else
	    	memcpy(&msuDeviceInfo.IP[0].ServerIPv6[0],MSU_DEFAULT_SERVER_IP,strlen(MSU_DEFAULT_SERVER_IP));
	    #endif
	    	    
	    //Fill MAC fields	    
	    memset(msuDeviceInfo.MAC,0,sizeof(msuDeviceInfo.MAC));
	    	
	    
	    
	    //Fill the Username status
		memcpy(&msuDeviceInfo.Username,(SINT8 *)MSU_DEFAULT_USERNAME,strlen(MSU_DEFAULT_USERNAME));
	
		
		//Fill the Password status
	    memcpy(&msuDeviceInfo.Password,(SINT8 *)MSU_DEFAULT_PASSWORD,strlen(MSU_DEFAULT_PASSWORD));
	    
	    //Fill the Filename1 
	     memcpy(&msuDeviceInfo.filename[0],(SINT8 *)MSU_DEFAULT_FILENAME,strlen(MSU_DEFAULT_FILENAME));
	     
	     //Fill the Filename2
	     memcpy(&msuDeviceInfo.filename[1],(SINT8 *)MSU_DEFAULT_FILENAME1,strlen(MSU_DEFAULT_FILENAME1));
	     
	     //Fill the Filename3
	     
	      memcpy(&msuDeviceInfo.filename[2],(SINT8 *)MSU_DEFAULT_FILENAME2,strlen(MSU_DEFAULT_FILENAME2));  
	     

     }//if(fileConfigStatus == FALSE)
}


/**  @fn BOOL msuParseDiscoverRequest(UINT8 *pMsg , UINT32 msgSize, MSUDiscoveryRequestPdu *pDiscoverRequest)
 *   @brief msuParseDiscoverRequest() is used to validate device discovery request
 *   @details  When client receives Msu discovery request from server, client calls msuParseDiscoverRequest()
 *             function to validate the request and checks this device falls between the starting and ending 
 *             IP address range, if it falls then this function return true.
 *   @param[in] : UINT8 *pMsg   Device discovery request message
 *   @param[in] : UINT32 msgSize  size of discovery request message
 *   @param[in] : MSUDiscoveryRequestPdu *pDiscoverRequest    pointer to   MSUDiscoveryRequestPdu which will be
 *                updated with pMsg data  
 *   @return    : TRUE, if device falls in b/w starting and ending IP range, otherwise 0
 *                 
 *   @retval  1  if successfu
 *   @retval  0 if failed 
 */
BOOL msuParseDiscoverRequest(UINT8 *pMsg , UINT32 msgSize, MSUDiscoveryRequestPdu *pDiscoverRequest)
{
 BOOL result = FALSE;
 
   	msuMemcpy(pDiscoverRequest, pMsg, sizeof(MSUDiscoveryRequestPdu));
   	
   	if (!pDiscoverRequest->versionInfo.versionIP ) // IPv4 ==0 IPV6 =1
   	{
		#ifdef ETHERBRICK_PLATFORM
		 UINT32  clientIPAddress = clientIP;
		#else
		 UINT32  clientIPAddress = msuNtoHl(((UINT32)inet_addr((char *)clientIP)));//Conversion is purposefully done
		#endif

		 printf("ClientIP = %X\n",clientIPAddress);
		 if((pMsg != NULL) && (msgSize == sizeof(MSUDiscoveryRequestPdu)) && (pDiscoverRequest != NULL))
		 {

		  pDiscoverRequest->startRange.ServerIPv4 = msuNtoHl (pDiscoverRequest->startRange.ServerIPv4);
		  pDiscoverRequest->endRange.ServerIPv4 = msuNtoHl (pDiscoverRequest->endRange.ServerIPv4);


		  if( ( (clientIPAddress >= pDiscoverRequest->startRange.ServerIPv4 ) && (clientIPAddress <= pDiscoverRequest->endRange.ServerIPv4) ) ||
			(clientIPAddress == pDiscoverRequest->startRange.ServerIPv4))
		  {
			  result = TRUE;
		  }
	 }

 }
 return result;
}


/**  @fn BOOL msuHnadleDeviceStatusUpdateRequest(char *gMsgBuffer,  UINT32 nByte)
 *   @brief msuHnadleDeviceStatusUpdateRequest() is 
 *   @details  msuHnadleDeviceStatusUpdateRequest() function is used to validate Device 
 *             status Update request by checking the transaction Id of the MSU cycle received in sync message
 *             with received transaction Id received in Update Request, if both are equal then this function returns true.
 *             Transaction ID : Randomly generated number which uniquely represents one MSU cycle
 *   @param[in] : char *gMsgBuffer     Update request message buffer 
 *   @param[in] : UINT32 nByte         size of the message buffer 
 *   @return TRUE if successful , otherwise FALSE
 *   @retval 1 successful
 *   @retval 0 fail 
 */
BOOL msuHnadleDeviceStatusUpdateRequest(char *gMsgBuffer,  UINT32 nByte)
{
	MSUUpgradeStatusPdu *statusMsg = NULL;

	if(gMsgBuffer == NULL) return FALSE;

	statusMsg = (MSUUpgradeStatusPdu *)gMsgBuffer;
	if( (msuBroadcastPdu.msuSyncHeaderformat.transactionID) == (msuHtoNl( statusMsg->transactionID)) )
		return TRUE;
	return FALSE;
}


/**  @fn BOOL msuHandleDeviceGroupMsg(UINT8 *pMsg , UINT32 msgSize)
 *   @brief msuHandleDeviceGroupMsg() is used to handle group creation request message
 *   @details  When client receives group creation message, it calls msuHandleDeviceGroupMsg( ) function
 *             which validates the message, if message is valid then it create a Group Id in the client device.
 *   @param[in] : UINT8 *pMsg         group creation request message buffer
 *   @param[in] : UINT32 msgSize      size of the message buffer
 *   @return TRUE, if group created successful, otherwise FALSE
 *   @retval 1  if successful
 *   @retval 0  if failed
 */
BOOL msuHandleDeviceGroupMsg(UINT8 *pMsg , UINT32 msgSize)
{
 BOOL result = TRUE;
 MSUGroupCreationPdu *msuGroupPdu = NULL;

 if((pMsg != NULL) && (msgSize == sizeof(MSUGroupCreationPdu)))
 {
	 msuGroupPdu = (MSUGroupCreationPdu*)pMsg;
	 if(msuGroupPdu->command.subcode == MSU_SUBCODE_SET_GROUP)
	 {
		 gGroupID = msuGroupPdu->groupID;
	 }
	 else if(msuGroupPdu->command.subcode == MSU_SUBCODE_RESET_GROUP)
	 {
		 gGroupID = 0;
	 }
	 else
	 {
		 result =FALSE;
	 }
 }
 return result;
}


/**  @fn BOOL msuSocketProcessSyncMessage(UINT8 *pMsg , UINT32 msgSize)
 *   @brief msuSocketProcessSyncMessage() is used process sync message received from server
 *   @details  When server sends Sync message with the information like file number, total number chunks, 
 *             sequence number limit,port number and multicast IP address and other parameters, this function
 *             stores all the information in local Sync message PDU and validates the pdu, if parameters are 
 *             valid then it returns TRUE 
 *   @param[in] : UINT8 *pMsg     Notification message ( Sync Message ) request message buffer
 *   @param[in] : UINT32 msgSize  size of the message buffer
 *   @return      TRUE, if validation is success, otherwise FALSE 
 *   @retval      1  for success
 *   @retval      0  for fail
 */
BOOL msuSocketProcessSyncMessage(UINT8 *pMsg , UINT32 msgSize)
{
	BOOL result = FALSE;
	UINT8 *pBuf = NULL;
    UINT32 tempPacketSize = 0;
    UINT16 tagDataSize = 0;   	
	MSUSyncHeaderformat *localmsuSyncHeaderformat;
  	MSUSyncMessageVariableData *localmsuSyncMessageVariableData;
    /*The message pointer should not be NULL and a minimum received bytes should be
       atleast equal to the fixed size of the broadcast message pdu*/
    if((pMsg == NULL) && (msgSize < sizeof(MSUSyncHeaderformat) ) )
    	return FALSE;
		
  	pBuf = pMsg;

  /*Initialize the Broadcast PDU*/
	localmsuSyncHeaderformat = (MSUSyncHeaderformat*)pBuf;
   
	localmsuSyncMessageVariableData=(MSUSyncMessageVariableData*)(pBuf+sizeof(MSUSyncHeaderformat));
   
   memset(&msuBroadcastPdu, 0, sizeof(MSUSyncMessagePdu));
   
  
   msuBroadcastPdu.msuSyncHeaderformat.msuHeader.command =(MSUCmd)(localmsuSyncHeaderformat->msuHeader.command);
	   
   
   msuBroadcastPdu.msuSyncHeaderformat.msuHeader.fileNumber = localmsuSyncHeaderformat->msuHeader.fileNumber;
   
   if(msuBroadcastPdu.msuSyncHeaderformat.msuHeader.fileNumber==0)
   	 
   	{
   		MsuGlobalError = MSU_UPDATE_STATUS_FILE_VALUE_NULL;
   		return result;	 
    }
	    	
   
   msuBroadcastPdu.msuSyncHeaderformat.msuHeader.versionInfo  = (localmsuSyncHeaderformat->msuHeader.versionInfo);
   	      	      
   
   msuBroadcastPdu.msuSyncHeaderformat.fileSize = msuNtoHl(localmsuSyncHeaderformat->fileSize);
   
   if(msuBroadcastPdu.msuSyncHeaderformat.fileSize==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_FILE_SIZE_ZERO;
   	    return result;	   		
   }
	      
	      
   msuBroadcastPdu.msuSyncHeaderformat.chunkCount = msuNtoHl(localmsuSyncHeaderformat->chunkCount);
   
      if(msuBroadcastPdu.msuSyncHeaderformat.chunkCount==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_CHUNKCOUNT_ZERO;
   	    return result;	   		
   }
  	  
	         
   msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit = msuNtoHs(localmsuSyncHeaderformat->seqNumberLimit);
   
         if(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_SEQLIMIT_ZERO;
   	    return result;	   		
   }
   
   		if(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit>MSU_MAX_SEQ_NUMBER)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_SEQLIMIT_EXCEEDED;
   	    return result;	   		
   }
   
	      
   msuBroadcastPdu.msuSyncHeaderformat.seqSize = msuNtoHs(localmsuSyncHeaderformat->seqSize);
   
          if(msuBroadcastPdu.msuSyncHeaderformat.seqSize==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_SEQSIZE_ZERO;
   	    return result;	   		
   }
        
          if(msuBroadcastPdu.msuSyncHeaderformat.seqSize>MSU_MAX_PACKET_SIZE )
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_SEQSIZE_EXCEEDED;
   	    return result;	   		
   }
	 
	  
	if(!msuBroadcastPdu.msuSyncHeaderformat.msuHeader.versionInfo.versionIP)
	{
		
		 msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv4=msuNtoHl(localmsuSyncHeaderformat->multicastIP.ServerIPv4);
   
          if(msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv4==0)
  		 {
   	    	MsuGlobalError = MSU_UPDATE_STATUS_SERVER_IP_ZERO;
   	    	return result;	   		
   	     }
	}
	else
	{
		memcpy(&msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv6[0],&localmsuSyncHeaderformat->multicastIP.ServerIPv6[0],MAX_BYTES_IPv6_ADDRESS);
		  if(msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv6[0]==0)
  		 {
   	    	MsuGlobalError = MSU_UPDATE_STATUS_SERVER_IP_ZERO;
   	    	return result;	   		
   	     }	
	}
	
		if(!msuBroadcastPdu.msuSyncHeaderformat.msuHeader.versionInfo.versionIP)
	{
		
		 msuBroadcastPdu.msuSyncHeaderformat.multicastCmIP.ServerIPv4=msuNtoHl(localmsuSyncHeaderformat->multicastCmIP.ServerIPv4);
   
          if(msuBroadcastPdu.msuSyncHeaderformat.multicastCmIP.ServerIPv4==0)
  		 {
   	    	MsuGlobalError = MSU_UPDATE_STATUS_SERVER_CM_IP_ZERO;
   	    	return result;	   		
   	     }
	}
	else
	{
		memcpy(&msuBroadcastPdu.msuSyncHeaderformat.multicastCmIP.ServerIPv6[0],&localmsuSyncHeaderformat->multicastCmIP.ServerIPv6[0],MAX_BYTES_IPv6_ADDRESS);
		  if(msuBroadcastPdu.msuSyncHeaderformat.multicastCmIP.ServerIPv6[0]==0)
  		 {
   	    	MsuGlobalError = MSU_UPDATE_STATUS_SERVER_CM_IP_ZERO;
   	    	return result;	   		
   	     }	
	}
  

	     
   msuBroadcastPdu.msuSyncHeaderformat.multicastPort = msuNtoHs(localmsuSyncHeaderformat->multicastPort);
   
         if(msuBroadcastPdu.msuSyncHeaderformat.multicastPort==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_PORT_ZERO;
   	    return result;	   		
   }
     
   msuBroadcastPdu.msuSyncHeaderformat.multicastCmPort = msuNtoHs(localmsuSyncHeaderformat->multicastCmPort);
    
   /*       if(msuBroadcastPdu.msuSyncHeaderformat.multicastCmPort==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_CM_PORT_ZERO;
   	    return result;	   		
   }
   */  
   msuBroadcastPdu.msuSyncHeaderformat.transactionID = msuNtoHl(localmsuSyncHeaderformat->transactionID);
   
          if(msuBroadcastPdu.msuSyncHeaderformat.transactionID==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_TRANSACTIONID_ZERO;
   	    return result;	   		
   }
     
   msuBroadcastPdu.msuSyncHeaderformat.fileChecksum = msuNtoHl(localmsuSyncHeaderformat->fileChecksum);
   
          if(msuBroadcastPdu.msuSyncHeaderformat.fileChecksum==0)
   {
   	    MsuGlobalError = MSU_UPDATE_STATUS_CRC_ZERO;
   	    return result;	   		
   }

   msuBroadcastPdu.msuSyncHeaderformat.fileNameTag = (localmsuSyncHeaderformat->fileNameTag);
    
   msuBroadcastPdu.msuSyncHeaderformat.destPathTag = (localmsuSyncHeaderformat->destPathTag);
  
   msuBroadcastPdu.msuSyncHeaderformat.updateTimeout = localmsuSyncHeaderformat->updateTimeout;
   
   /* Populate the filename and the destination path fields*/
   if(localmsuSyncHeaderformat->fileNameTag.isFieldPresent)
   {
   	  memcpy(msuBroadcastPdu.msuSyncMessageVariableData.fileName,localmsuSyncMessageVariableData->fileName,localmsuSyncHeaderformat->fileNameTag.fieldLength);
   }
   
   
   if(localmsuSyncHeaderformat->destPathTag.isFieldPresent)
   {
   	  memcpy(msuBroadcastPdu.msuSyncMessageVariableData.destPath,localmsuSyncMessageVariableData->destPath,localmsuSyncHeaderformat->destPathTag.fieldLength);
   }
   
 
	/*Validate the PDU fields*/
    result = TRUE;
    return result;
}


/**  @fn BOOL msuPduParseMcastRequest(UINT8 *pMsg , UINT32 msgSize)
 *   @brief msuPduParseMcastRequest() function is used to validate multicast data packet
 *   @details  When client receives multicast data packet, it calls msuPduParseMcastRequest( ) function
 *             and validates the message and assign the message buffer to multicast pdu buffer.
 *             function returns true if message is valid message.
 *   @param[in] : UINT8 *pMsg     data transfer message buffer
 *   @param[in] : UINT32 msgSize  size of message buffer
 *   @return  TRUE, if message is valid, otherwise FALSE
 *   @retval 1 if success
 *   @retval 0 if fails
 */
BOOL msuPduParseMcastRequest(UINT8 *pMsg , UINT32 msgSize)
{
 BOOL result = FALSE;
 UINT8 *pBuf = NULL;
 UINT32 tempMcastPacketSize = 0;

 //Get the pdu size for fixed section of the frame
 tempMcastPacketSize = (UINT32) (sizeof(MSUMulticastPdu) - sizeof(UINT8 *));

 /*The message pointer should not be NULL and a minimum received bytes should be
    atleast equal to the fixed size of the multicast message pdu or Upgrade complete pdu*/
 if((pMsg != NULL) && (msgSize >= sizeof(MSUCompletePdu)))
 {
    pBuf = pMsg;

   //Initialize the Multicast PDU
    memset(&msuMcastPdu, 0, sizeof(MSUMulticastPdu));

    /* Copy header*/
    msuMemcpy(&msuMcastPdu,pBuf,sizeof(MSUMulticastPdu));
    /*Extract the members*/
    msuMcastPdu.msuHeader.fileNumber = msuNtoHs(msuMcastPdu.msuHeader.fileNumber);
    if ( (msuMcastPdu.msuHeader.command.subcode == MSU_SUBCODE_TRANSFER_COMPLETE) &&
    		(msuCurrentState()== eMsuStateRunning))
    {
      /*The packet received was upgrade complete message from the server. Extract the remaining
        fields.*/
    	msuMcastPdu.fileTransferInfo = (*(FileTransferInfo *) (pBuf+4));

    	result = TRUE;
#ifdef Debug_level1
    	printf("\nUpgrade Complete Msg Received..Validation OK\n");
#endif
    }
    else if(msgSize > tempMcastPacketSize)
    {
      /*The packet received was a normal multicast message from the server. Extract the remaining
          fields.*/
    	msuMcastPdu.chunkNumber = msuNtoHl(msuMcastPdu.chunkNumber);
    	msuMcastPdu.dataLength = msuNtoHs(msuMcastPdu.dataLength);

    	if(msgSize  == ((UINT32) (tempMcastPacketSize + msuMcastPdu.dataLength)))
    	{
    		//Point the data to the location in the buffer
    		msuMcastPdu.pData = pBuf+tempMcastPacketSize;
    		result = TRUE;
    	}
    }
 }
 return result;
}
/**  @fn BOOL msuAuthenticationRequest(UINT8 *pMsg,UINT32 msgSize,MSUAuthenticationRequestPdu *pAuthenticationRequest,
		MSUAuthenticationResponsePdu *msuAuthenticationResponse)
 *   @brief msuAuthenticationRequest parses the authentication request and constructs the response
 *   @details  When client receives the authentication request it parses the request,reads and 
 *             stores the data in the buffer.It then validates the data against the data from MSU.bin
 *             It then sends the response to the server
 *   @param[in] : UINT8 *pMsg     data transfer message buffer
 *   @param[in] : UINT32 msgSize  size of message buffer
 *   @param[in] : MSUAuthenticationRequestPdu pAuthenticationRequest: Authentication Request Pdu
 *   @param[in] : MSUAuthenticationResponsePdu msuAuthenticationResponse: Authentication Response Pdu 
 *   @return  TRUE, if message is valid and credentials valid, otherwise FALSE
 *   @retval 1 if success
 *   @retval 0 if fails
 */

BOOL msuAuthenticationRequest(UINT8 *pMsg,UINT32 msgSize,MSUAuthenticationRequestPdu *pAuthenticationRequest,
		MSUAuthenticationResponsePdu *msuAuthenticationResponse,msuSockinfo *socketip )
{
   BOOL 	result = FALSE;
   UINT8 	*pBuf;  ;  
   UINT8 	errcode=0;
   UINT8 	errsubcode=0;
   MSUAuthenticationHeader 	*Header;
   MSUtransactionid			decryptedtransactonid;	   
   int 		i=0,j=0;
   UINT8 	tagSize = 0;
   BOOL 	EntryFound = FALSE;
   SINT32 	retCode = 0;
   UINT32	length_privatekey=0;
   UINT32   transactionidtest=0;      
   
   msuMemset(pAuthenticationRequest,0,sizeof(MSUAuthenticationRequestPdu));   
  
   /* Get MSU Authentication message header*/
   Header = (MSUAuthenticationHeader*)pMsg;
   
   
   /*The message pointer should not be NULL and a minimum received bytes should be
      atleast equal to MSU Authentication message header*/
   if((pMsg != NULL) && (msgSize >= sizeof(MSUAuthenticationHeader)))
   {	
   			
   	  pBuf= (UINT8 *)(pMsg+sizeof(MSUAuthenticationHeader));
      //Header->transactionid = (Header->transactionid);
      
      /*if (Header->AuthenticationLevel.Level==msuDeviceInfo.AuthenticationLevelTag)
   	  {
   	   	
   	   	result=TRUE;
   	  }
   	  else
   	  {
   	  	errcode = TRUE;
   	   	errsubcode = MSU_AUTHENTICATION_ERRCODE;
   	   	result = FALSE;
   	  }*/
   	  
     /* if (result)
      {*/
      	/*Decrypting transaction ID */
      length_privatekey = msuStrlen(privatekey);
      //msuDecryption((UINT8*)Header->transactionid,privatekey,sizeof(Header->transactionid),length_privatekey);
      /*For testing */
      transactionidtest=msuNtoHl(Header->transactionid.transactionid);
      msuDecryption((UINT8*)Header->transactionid.transactionencrypt,privatekey,sizeof(Header->transactionid),length_privatekey);
      
      decryptedtransactonid.transactionid=(Header->transactionid.transactionid);
      
      /*Compute the actual Tag Data Length*/
      /*File Name Tag*/
     if(Header->FileNameTag.isFieldPresent)
      {
      	   if(Header->FileNameTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->FileNameTag.fieldLength;
      	   }
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.filename,pBuf,tagSize);
    	   pBuf+=Header->FileNameTag.fieldLength; 	   
      }
	
      /*Hardware ID Tag*/
     if(Header->hardwareIdTag.isFieldPresent)
      {
      	   if(Header->hardwareIdTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->hardwareIdTag.fieldLength;
      	   }
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.HardwareId,pBuf,tagSize);
    	   pBuf+=Header->hardwareIdTag.fieldLength; 	   
      }
	
  	/*Product ID Tag*/
      if(Header->productIdTag.isFieldPresent)
      {
      	   if(Header->productIdTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->productIdTag.fieldLength;
      	   }
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.ProductId,pBuf,tagSize);
    	   pBuf+=Header->productIdTag.fieldLength; 	   
      }
      

		/*modelname Tag*/
            
      if(Header->modelnameTag.isFieldPresent)
      {
      	   if(Header->modelnameTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->modelnameTag.fieldLength;
      	   }
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.modelname,pBuf,tagSize);
    	   pBuf+=Header->modelnameTag.fieldLength; 	   
      }
      
	
		/*password Tag*/
      
		if(Header->passwordTag.isFieldPresent)
      	{
      	   if(Header->passwordTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->passwordTag.fieldLength;
      	   }		
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.password,pBuf,tagSize);
    	    pBuf+=Header->passwordTag.fieldLength; 	   
      	}
	
	
		/*username Tag*/
     
      if(Header->usernameTag.isFieldPresent)
      	{
      	   if(Header->usernameTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->usernameTag.fieldLength;
      	   }
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.username,pBuf,tagSize);
    	   pBuf+=Header->usernameTag.fieldLength; 	   
      	}
	
	
		/*firmware version Tag*/
     	
	if(Header->firmwareversionTag.isFieldPresent)
      	{
      	   if(Header->firmwareversionTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->firmwareversionTag.fieldLength;
      	   }
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.firmwareversion,pBuf,tagSize);
    	    pBuf+=Header->firmwareversionTag.fieldLength; 	   
      	}
	
	
		/*software version Tag*/
         
      
      if(Header->softwareversionTag.isFieldPresent)
      	{
      	   if(Header->softwareversionTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->softwareversionTag.fieldLength;
      	   }
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.softwareversion,pBuf,tagSize);
    	   pBuf+=Header->softwareversionTag.fieldLength; 	   
      	}

		/*vendorid Tag*/
     
	
	if(Header->vendorIdTag.isFieldPresent)
      	{
      	   if(Header->vendorIdTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->vendorIdTag.fieldLength;
      	   }
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.vendorId,pBuf,tagSize);
    	    pBuf+=Header->vendorIdTag.fieldLength; 	   
      	}
	
		/*product name Tag*/
     	   
		if(Header->productnameTag.isFieldPresent)
      	{
      	   if(Header->productnameTag.fieldLength > MSU_MAX_TAG_SIZE)
      	   {
      	   		tagSize = MSU_MAX_TAG_SIZE;		
      	   }
      	   else
      	   {
      	   	tagSize = Header->productnameTag.fieldLength;
      	   }
      	   		
    	   msuMemcpy(&pAuthenticationRequest->authPayload.productname,pBuf,tagSize);
    	   pBuf+=Header->productnameTag.fieldLength; 	   
      	}
	
	
	/* Validating the server IP with socket ip address and aginst the Whitelisted IPs */	
	
	if (!Header->versionInfo.versionIP) 
	{	
	
		if ((Header->msuServerIP.ServerIPv4)==(socketip->ipAddress))
		{
			/* Retrieving IP whitelist and checking whether the Server IP matches with the list */	
			if(msuDeviceInfo.NumberOfIPSupported <= 10)
			{
				for (i=0;i<msuDeviceInfo.NumberOfIPSupported;i++)
				{				
					if ((Header->msuServerIP.ServerIPv4)==(msuDeviceInfo.IP[i].ServerIPv4))
					{
						EntryFound = TRUE;
						result=TRUE;
						break;
					}							
				}				
			}						
		}
		
		if (EntryFound == FALSE)
		{
			msuAuthenticationResponse->ErrCode=TRUE;
			msuAuthenticationResponse->ErrSubCode=MSU_AUTHENTICATION_ERRSUBCODE_IP;
			return result;
		}	
			
	}//if (!Header->versionInfo.versionIP) 		
		
	
	
	/* User name and password decryption algorithm */
	if (Header->usernameTag.isFieldPresent)
	{
		
		msuDecryption((UINT8*)&pAuthenticationRequest->authPayload.username,privatekey,
								Header->usernameTag.fieldLength,length_privatekey);
	}
		
	if (msuStrncmp((char*)&pAuthenticationRequest->authPayload.username,(SINT8*)&msuDeviceInfo.Username,sizeof(pAuthenticationRequest->authPayload.username))!=0)
	{
		errcode=MSU_AUTHENTICATION_ERRCODE;
		errsubcode=MSU_AUTHENTICATION_ERRSUBCODE_LOGININFO;
		result=FALSE;
	}
	else
	{
		result=TRUE;
	}
	
	
	if (result)
	{
		if (Header->passwordTag.isFieldPresent  )
		{
			msuDecryption((UINT8*)&pAuthenticationRequest->authPayload.password,privatekey,
							Header->passwordTag.fieldLength,length_privatekey);
		}
	
	
		if (msuStrncmp((char*)&pAuthenticationRequest->authPayload.password,(SINT8*)&msuDeviceInfo.Password,sizeof(pAuthenticationRequest->authPayload.password))!=0)
		{
			errcode=MSU_AUTHENTICATION_ERRCODE;
			errsubcode=MSU_AUTHENTICATION_ERRSUBCODE_LOGININFO;
			result=FALSE;
		}
		else
		{
			 result=TRUE;
		}			
	}
	
	
	
	EntryFound = FALSE;
	
	
	if (result)
	{
	  	if(Header->FileNameTag.isFieldPresent )
    	  {	
			if(msuDeviceInfo.noOfFileSupported <= 3)
			{
				for (i=0;i<msuDeviceInfo.noOfFileSupported;i++)
				{
								
					if(msuStrncmp((SINT8*)&pAuthenticationRequest->authPayload.filename,(SINT8*)&msuDeviceInfo.filename[i],MAX_FILENAME_LEN) !=0)			
					{
						EntryFound = TRUE;
						result=TRUE;
						break;
				}							
			}				
		}
      }
      
		if (!EntryFound)
		{
			errcode=MSU_AUTHENTICATION_ERRCODE;
			errsubcode=MSU_AUTHENTICATION_ERRSUBCODE_FILENAME;
			result=FALSE;
		}	
	}
	
	
	if(result)
	{
		if(Header->hardwareIdTag.isFieldPresent && deviceidentification.hardwareIdTag.isFieldPresent  ) 
		{
			if (msuStrncmp((SINT8*)&pAuthenticationRequest->authPayload.HardwareId[0],(SINT8*)&deviceidentification.hardwareId[0],
							Header->hardwareIdTag.fieldLength)!=0)
			{
				errcode=MSU_AUTHENTICATION_ERRCODE;
				errsubcode=MSU_AUTHENTICATION_ERRSUBCODE_HARDWAREID;
				result=FALSE;
			}
			else
			{
				result = TRUE;
			}
		}
	
	}
    
    //}
      	
		
	/* Load the data to authentication response buffer */
	
	msuAuthenticationResponse->command.opcode=MSU_OPCODE_AUTHENTICATION;
	msuAuthenticationResponse->command.subcode=MSU_SUBCODE_AUTHENTICATION_RESPONSE;
	msuAuthenticationResponse->ErrCode=errcode;
	msuAuthenticationResponse->ErrSubCode=errsubcode;		
	msuAuthenticationResponse->versionInfo.versionIP = Header->versionInfo.versionIP;
	msuAuthenticationResponse->versionInfo.versionProtocol=MSU_PROTOCOL_VERSION;	
      
    }
   return result;
}

/**  @fn void msuCcmMsgCreate( MSUCCMPdu *ccmMsgPdu, UINT32 totalChnksExpected,UINT32 *missedChnkCnt)
 *   @brief msuCcmMsgCreate() is used to construct CCM message format
 *   @details msuCcmMsgCreate()  function is used to construct CCM message format, client will use this message   
 *            and send to server to re transmit missed chunks  
 *   @param[in] : MSUCCMPdu *ccmMsgPdu   pointer to CCM pdu which will be updated inside the function     
 *   @param[in] : UINT32 totalChnksExpected  total chunk number in the file
 *   @param[in] : UINT32 *missedChnkCnt   pointer which will be updated with missed chunk count inside the function
 *   @return    : None  
 *   @retval    : None 
 */
void msuCcmMsgCreate( MSUCCMPdu *ccmMsgPdu, UINT32 totalChnksExpected,UINT32 *missedChnkCnt)
{
    UINT32 *missedChunkNums = NULL;
	ccmMsgPdu->msuHeader.command.opcode = MSU_OPCODE_UPGRADE;
	ccmMsgPdu->msuHeader.command.subcode = MSU_SUBCODE_CCM;
	ccmMsgPdu->msuHeader.fileNumber = msuHtoNs(msuBroadcastPdu.msuSyncHeaderformat.msuHeader.fileNumber);
	ccmMsgPdu->msuHeader.versionInfo.versionIP = MSU_IPV4_VERSION;
	ccmMsgPdu->msuHeader.versionInfo.versionProtocol=MSU_PROTOCOL_VERSION;

	missedChunkNums = getMissedChnkNumsArry(totalChnksExpected, missedChnkCnt);

	ccmMsgPdu->chunkCount = msuHtoNl(*missedChnkCnt);

	ccmMsgPdu++;

	msuMemcpy(ccmMsgPdu, missedChunkNums, (sizeof(UINT32) * *missedChnkCnt));
}

/**  @fn void msuUpgrateMsgCreate( MSUUpgradeStatusPdu *msuUpgradeStatus, SINT32 upgradeStatus, UINT8 ccmRetryVal )
 *   @brief msuUpgrateMsgCreate() is used to construct a Upgrade status message format
 *   @param[in] : MSUUpgradeStatusPdu *msuUpgradeStatus    pointer to upgrade stauts pdu, pdu data updated inside the function 
 *   @param[in] : SINT32 upgradeStatus  status of the data upgrade, it may fail or pass or in progress 
 *   @param[in] : UINT8 ccmRetryVal   Number of times CCM retrys happend to receive data 
 *   @return    : None 
 *   @retval    : None 
 */
void msuUpgrateMsgCreate( MSUUpgradeStatusPdu *msuUpgradeStatus, SINT32 upgradeStatus, UINT8 ccmRetryVal )
{
    memset(msuUpgradeStatus, 0, sizeof(MSUUpgradeStatusPdu));

    msuUpgradeStatus->command.opcode = MSU_OPCODE_UPGRADE;
    msuUpgradeStatus->command.subcode = MSU_SUBCODE_CLIENT_STATUS_RESPONSE;
    msuUpgradeStatus->status = (UINT8)upgradeStatus;
    
    msuUpgradeStatus->errorCode = MsuGlobalError;
    
    msuUpgradeStatus->versionInfo.versionIP = MSU_IPV4_VERSION;
    msuUpgradeStatus->versionInfo.versionProtocol = MSU_PROTOCOL_VERSION;
    msuUpgradeStatus->transactionID = msuHtoNl(msuBroadcastPdu.msuSyncHeaderformat.transactionID);
#ifdef ETHERBRICK_PLATFORM
  msuUpgradeStatus->deviceID = (UINT32)clientIP;
#else
  msuUpgradeStatus->deviceID = ((UINT32)inet_addr((char *)clientIP));
#endif 
    
    msuUpgradeStatus->ccmRetryCount = ccmRetryVal;
    

}

/**  @fn void msuDiscoveryResponseMsgCreate( MSUDiscoveryRequestPdu *discoveryRequestPdu,
					MSUDiscoveryResponsePdu *msuDiscoveryResponse,UINT16 *dataPayloadSize )
 *   @brief msuDiscoveryResponseMsgCreate() is used to construct Discovery response message format
 *   @param[in] : MSUDiscoveryRequestPdu *discoveryRequestPdu    passing msu discovery request pdu pointer
 *   @param[in] : MSUDiscoveryResponsePdu *msuDiscoveryResponse  msu discovery response pdu data will be updated inside the function
 *   @param[in] : UINT16 *dataPayloadSize  dat payload size will be update by the function
 *   @return    : None 
 *   @retval None
 */
void msuDiscoveryResponseMsgCreate( MSUDiscoveryRequestPdu *discoveryRequestPdu,
					MSUDiscoveryResponsePdu *msuDiscoveryResponse,UINT16 *dataPayloadSize )
{
	DEV_MSU_PARAM   msuDevParam;
   UINT8 pbufvarlen=0;    
   
#ifdef ETHERBRICK_PLATFORM
  UINT32 clientIPAddress = (UINT32)clientIP;
#else
  UINT32 clientIPAddress = ((UINT32)inet_addr((SINT8 *)clientIP));
#endif 

	 msuBasicIdentification(&deviceidentification);
	 memset(msuDiscoveryResponse, 0, sizeof(MSUDiscoveryResponsePdu));

    msuDiscoveryResponse->command.opcode = MSU_OPCODE_DEVICE_DISCOVER;
    msuDiscoveryResponse->command.subcode = MSU_SUBCODE_DEVICE_DISCOVERY_RESPONSE;
    msuDiscoveryResponse->GroupId =gGroupID;
    msuDiscoveryResponse->versionInfo = discoveryRequestPdu->versionInfo;
   if (!discoveryRequestPdu->versionInfo.versionIP)
   {
   	msuMemcpy(&msuDiscoveryResponse->deviceID.ServerIPv6[0],&clientIPAddress, 40 );
   }
   else
   {
   	msuMemcpy(&msuDiscoveryResponse->deviceID.ServerIPv4,&clientIPAddress,sizeof(UINT32));
   }
   // Authentication field
   	msuDiscoveryResponse->AuthenticationLevel.IsClient=msuDeviceInfo.Client;
   	msuDiscoveryResponse->AuthenticationLevel.IsMaster=msuDeviceInfo.Master;
	msuDiscoveryResponse->AuthenticationLevel.IsAuthRequired=msuDeviceInfo.AutheticationTag;
	msuDiscoveryResponse->AuthenticationLevel.Level=msuDeviceInfo.AuthenticationLevelTag;
   
   if(deviceidentification.hardwareIdTag.isFieldPresent)
   {
   		msuDiscoveryResponse->hardwareIdTag.isFieldPresent=TRUE;
   		msuDiscoveryResponse->hardwareIdTag.fieldLength=deviceidentification.hardwareIdTag.fieldLength;
   		msuMemcpy(&msuDiscoveryResponse->varlengthdata[pbufvarlen],&deviceidentification.hardwareId,msuDiscoveryResponse->hardwareIdTag.fieldLength);
   		*dataPayloadSize +=msuDiscoveryResponse->hardwareIdTag.fieldLength;
   		pbufvarlen=(UINT8)*dataPayloadSize;	
   }
    //Product ID field
	if(deviceidentification.productIdTag.isFieldPresent)
	{
		msuDiscoveryResponse->productIdTag.isFieldPresent=TRUE;
   		msuDiscoveryResponse->productIdTag.fieldLength=deviceidentification.productIdTag.fieldLength;
   		msuMemcpy(&msuDiscoveryResponse->varlengthdata,&deviceidentification.productId,
   		msuDiscoveryResponse->productIdTag.fieldLength);
   		*dataPayloadSize +=msuDiscoveryResponse->productIdTag.fieldLength;
   		pbufvarlen=(UINT8)*dataPayloadSize;
    }
    //Product Name field
   	if(deviceidentification.productNameTag.isFieldPresent)
   	{
   		msuDiscoveryResponse->productNameTag.isFieldPresent=TRUE;
   		msuDiscoveryResponse->productNameTag.fieldLength=deviceidentification.productNameTag.fieldLength;
		msuMemcpy(&msuDiscoveryResponse->varlengthdata[pbufvarlen],&deviceidentification.productName,
					msuDiscoveryResponse->productNameTag.fieldLength);   
   		*dataPayloadSize +=msuDiscoveryResponse->productNameTag.fieldLength;
   		pbufvarlen=(UINT8)*dataPayloadSize;	
   	}
    //Model Name field
   	if(deviceidentification.modelNameTag.isFieldPresent)
   	{
   		msuDiscoveryResponse->modelNameTag.isFieldPresent=TRUE;
   		msuDiscoveryResponse->modelNameTag.fieldLength=deviceidentification.modelNameTag.fieldLength;
		msuMemcpy(&msuDiscoveryResponse->varlengthdata[pbufvarlen],&deviceidentification.modelName,
					msuDiscoveryResponse->modelNameTag.fieldLength);      
   		*dataPayloadSize +=msuDiscoveryResponse->modelNameTag.fieldLength;
   		pbufvarlen=(UINT8)*dataPayloadSize;	
   	}

    //Vendor ID field
   	if(deviceidentification.vendorIdTag.isFieldPresent)
   	{
   		msuDiscoveryResponse->vendorIdTag.isFieldPresent=TRUE;
   		msuDiscoveryResponse->vendorIdTag.fieldLength=deviceidentification.vendorIdTag.fieldLength;
   		msuMemcpy(&msuDiscoveryResponse->varlengthdata[pbufvarlen],&deviceidentification.vendorname,
		msuDiscoveryResponse->vendorIdTag.fieldLength);
   		*dataPayloadSize +=msuDiscoveryResponse->vendorIdTag.fieldLength;
   		pbufvarlen=(UINT8)*dataPayloadSize;
    }

    //Firmware version field
   	if(deviceidentification.firmwareVersionTag.isFieldPresent)
   	{
   		msuDiscoveryResponse->firmwareVersionTag.isFieldPresent=TRUE;
   		msuDiscoveryResponse->firmwareVersionTag.fieldLength=deviceidentification.firmwareVersionTag.fieldLength;
   		msuMemcpy(&msuDiscoveryResponse->varlengthdata[pbufvarlen],&deviceidentification.firmwareVersion,
					msuDiscoveryResponse->firmwareVersionTag.fieldLength);
   		*dataPayloadSize +=msuDiscoveryResponse->firmwareVersionTag.fieldLength;
   		pbufvarlen=(UINT8)*dataPayloadSize;	
   	}

    //Revision field
   	if(deviceidentification.revisionTag.isFieldPresent)
   	{
   		msuDiscoveryResponse->revisionTag.isFieldPresent=TRUE;
   		msuDiscoveryResponse->revisionTag.fieldLength=deviceidentification.revisionTag.fieldLength;
   		msuMemcpy(&msuDiscoveryResponse->varlengthdata[pbufvarlen],&deviceidentification.revision,
					msuDiscoveryResponse->firmwareVersionTag.fieldLength);
   		*dataPayloadSize +=msuDiscoveryResponse->revisionTag.fieldLength;
   		pbufvarlen=(UINT8)*dataPayloadSize;	
   	}

    //Device location field
   	if(deviceidentification.deviceLocTag.isFieldPresent)
   	{
   		msuDiscoveryResponse->deviceLocTag.isFieldPresent=TRUE;
   		msuDiscoveryResponse->deviceLocTag.fieldLength=deviceidentification.deviceLocTag.fieldLength;
   		msuMemcpy(&msuDiscoveryResponse->varlengthdata[pbufvarlen],&deviceidentification.deviceLoc,
					msuDiscoveryResponse->deviceLocTag.fieldLength);
   		*dataPayloadSize +=msuDiscoveryResponse->deviceLocTag.fieldLength;
   		pbufvarlen=(UINT8)*dataPayloadSize;
   	}  
	#ifdef ETHERBRICK_PLATFORM
		msuDevParam.seqDelay = 20;	 
    	msuDevParam.scmDelay = 1000;
    	msuDevParam.ccmDelay = 1000;
    	msuDevParam.ccmRetry = 1; 
    	msuDevParam.scmRetry = 3; 
    	msuDevParam.commTimeout_sec = 10;
	#else
	 	msuDevParam.seqDelay = msuHtoNs(0);
    	msuDevParam.scmDelay = msuHtoNs(5);
    	msuDevParam.ccmDelay = msuHtoNs(100);
    	msuDevParam.ccmRetry = 1; 
    	msuDevParam.scmRetry = 1; 
	 	msuDevParam.commTimeout_sec = 5;
	#endif    

    msuDiscoveryResponse->devMsuParam.isFieldPresent = TRUE;
    msuDiscoveryResponse->devMsuParam.fieldLength    = sizeof(DEV_MSU_PARAM);
    
    
   //Copy the device information header
   msuMemcpy((&msuDiscoveryResponse->varlengthdata[pbufvarlen] ), &msuDevParam,sizeof(DEV_MSU_PARAM) );
    
    *dataPayloadSize += msuDiscoveryResponse->devMsuParam.fieldLength;


}

/**  @fn void msuScmMsgCreate( MSUSCMPdu *scmMsgPdu,UINT32 bitMap, UINT32 chunkNum, UINT8 missedSeqCount)
 *   @brief msuScmMsgCreate() function is used to construct SCM message format
 *   @param[in] : MSUSCMPdu *scmMsgPdu   SCM pdu data will be updated inside fuction
 *   @param[in] : UINT32 bitMap          sequence bit map value for particular chunk
 *   @param[in] : UINT32 chunkNum        chunk number
 *   @param[in] : UINT8 missedSeqCount   total number of missed sequence count in this particular chunk
 *   @return    : None
 *   @retval    : None
 */
void msuScmMsgCreate( MSUSCMPdu *scmMsgPdu,UINT32 bitMap, UINT32 chunkNum, UINT8 missedSeqCount)
{
	scmMsgPdu->msuHeader.command.opcode = MSU_OPCODE_UPGRADE;
	scmMsgPdu->msuHeader.command.subcode = MSU_SUBCODE_SCM;
	scmMsgPdu->msuHeader.fileNumber = msuHtoNs( msuBroadcastPdu.msuSyncHeaderformat.msuHeader.fileNumber);
	scmMsgPdu->msuHeader.versionInfo.versionIP = MSU_IPV4_VERSION;
	scmMsgPdu->msuHeader.versionInfo.versionProtocol=MSU_PROTOCOL_VERSION;
	scmMsgPdu->chunkNumber = msuHtoNl(chunkNum);
	scmMsgPdu->seqCount = missedSeqCount;
	scmMsgPdu->seqBitMap = msuHtoNl(bitMap);
}


/**  @fn MSUGroupCreationPdu * msuGroupAckMsgCreate( MSUGroupCreationPdu *gReqPdu )
 *   @brief msuGroupAckMsgCreate() is used to construct group creation ack message format
 *   @param[in] : MSUGroupCreationPdu *gReqPdu   group creation pointer pdu data will be updated inside the function.   
 *   @return    : returns group creation ack pdu pointer
 *   @retval    : PDU pointer of type MSUGroupCreationPdu *
 */
MSUGroupCreationPdu * msuGroupAckMsgCreate( MSUGroupCreationPdu *gReqPdu )
{
	gReqPdu->command.subcode   = MSU_SUBCODE_ACK_GROUP;
	gReqPdu->groupID           = gGroupID;
	return(gReqPdu);
}

void msuBasicIdentification(void*ptr )
{

	UINT32 numberofiteration = 0;
	UINT8 dataoffset=0;
	MSUBasicDeviceIdentification *basicdeviceidentification= (MSUBasicDeviceIdentification*)ptr; 
	
	
    
    /*traverse the buffer*/
    /*Set those fields to false that have not been incorporated*/
    basicdeviceidentification->hardwareIdTag.isFieldPresent = FALSE;
    basicdeviceidentification->productIdTag.isFieldPresent = FALSE;
    basicdeviceidentification->modelNameTag.isFieldPresent = FALSE;
    basicdeviceidentification->deviceLocTag.isFieldPresent = FALSE;
    
     /*copy the Vendor name*/
    basicdeviceidentification->vendorIdTag.isFieldPresent = TRUE;
    basicdeviceidentification->vendorIdTag.fieldLength  = msuStrlen("Schneider");
    msuMemcpy(&basicdeviceidentification->vendorname,"Schneider", msuStrlen("Schneider"));
    dataoffset+= msuStrlen("Schneider") ;
    
    /*copy the Product name*/
    basicdeviceidentification->productNameTag.isFieldPresent = FALSE;

    
    /*copy the Version Info*/
    basicdeviceidentification->firmwareVersionTag.isFieldPresent = FALSE;

    
    /*copy the revision Info*/
    basicdeviceidentification->revisionTag.isFieldPresent = FALSE;

}

/**  @fn void msuDisconnectMsg( )
 *   @brief msuScmMsgCreate() function is used to construct SCM message format
 *   @param[in] : MSUSCMPdu *scmMsgPdu   SCM pdu data will be updated inside fuction
 *   @param[in] : UINT32 bitMap          sequence bit map value for particular chunk
 *   @param[in] : UINT32 chunkNum        chunk number
 *   @param[in] : UINT8 missedSeqCount   total number of missed sequence count in this particular chunk
 *   @return    : None
 *   @retval    : None
 */
/* UINT8 msuDisconnectMsg(UINT8 *gMsg,UINT32 msgsize )
{


	UINT32 localmsgsize =0;
	UINT32 result =FALSE;
	MSUDisconnectPdu *localDisconnectpdu=NULL;

	
	localmsgsize = sizeof(MSUDisconnectPdu);
	
	if(localmsgsize==msgsize)
	{
		
	localDisconnectpdu =(MSUDisconnectPdu*) gMsg;
	
		if(	localDisconnectpdu->authenticationlevel.Level!=msuDeviceInfo.AuthenticationLevelTag)
		{
			MsuGlobalError=MSU_AUTHENTICATION_LEVEL_FAILURE;
		}
		if(!localDisconnectpdu->versionInfo.versionIP)
		{
			if(localDisconnectpdu->serverIp.ServerIPv4!=pAuthenticationRequestPdu.authHeader.msuServerIP.ServerIPv4)
			{
				MsuGlobalError=MSU_SERVER_IP_MISMATCH;
			}
				
		}
		else
		{
			if(!msuStrncmp((SINT8*)&localDisconnectpdu->serverIp.ServerIPv6[0],(SINT8*)&pAuthenticationRequestPdu.authHeader.msuServerIP.ServerIPv6[0],(UINT32)MAX_BYTES_IPv6_ADDRESS))
			{
				MsuGlobalError=MSU_SERVER_IP_MISMATCH;
			}
		}
		if(localDisconnectpdu->transactionId!=pAuthenticationRequestPdu.authHeader.transactionid)
		{
				MsuGlobalError=MSU_TRANSACTION_ID_FAILURE;
				
		}
	
	}
	
	return result;
	
}*/


