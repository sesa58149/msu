/**	@file MsuLeech.c
 *	<TABLE>
 *	@brief Implementation of MSU Leech
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
 *	<TD>Deepak G</TD>	<TD>03-Oct-2011</TD>	<TD>First creation</TD>
 *	</TR>
 *
 * 	</TABLE>
 *	</TABLE>
 */

#ifndef MSU_LEECH_C
#define MSU_LEECH_C



#include <stdio.h>
#include <string.h>
#include "MsuPlatform.h"
#include "MsuFilePlatform.h"
//#include "MsuFileInterface.h"
#include "MsuPdu.h"
#include "MsuLeech.h"
#include "MsuCRC32.h"
#include "complainMode.h"
#include "MsuSocket.h"
#include "msuErrNo.h"

extern UINT8 State;
#pragma optimization_level 0



extern UINT32 missedSeqBitMap; //recvdSeqBitMap
#ifdef CHUNK_DATA_STORE_RAM
	static UINT8 *pChunkBuffer;
#endif
static UINT16 localSeqNumber;
static UINT32 localChunkNumber;
static UINT16 sequenceBitMap;
static UINT32 partialChunkSize;
static EMSUErr eMsuStatus;
static UINT32 chunkDataSize;
static UINT32 lastChunkSize;
static UINT32 noOfSeqInLastChunk;
static SINT8 filePath[MSU_MAX_FILE_PATH_SIZE];
static MsuInfo info_array[MSU_MAX_SEQ_NUMBER];
static UINT16 ChunkSize =0;

static EMSUState eMsuState     = eMsuStateInitialize;
static EMSUState eMsuPrevState = eMsuStateRunning;
static EMSUUpgradeState deviceUpgadeStatus = eMsuUpgradeFailed;
/******************************************************************/
static char upgrade_filepath[MSU_MAX_FILE_PATH_SIZE];
static EMSUUpgradeState UpgadeStatus = eMsuUpgradeFailed;
/******************************************************************/
static UINT8 firstTime = 1;
static BOOL ccmSent = FALSE;

static UINT8 ccmRetryCount = 0;
static UINT8 scmRetryCount = 0;

static UINT32 ccm_seqVal[MSU_MAX_CCM_CHUNK_NUMBER];
static SINT32 pMsuFileDes;

UINT8   MsuGlobalError = 0;


static void msuLeechInit(void);
static SINT32 msuLeechConfigure(void);
static void msuHandleMcastPacket(void);
static void msuHandleScmMcastPacket(void);
static void msuHandleCcmMcastPacket(void);
static void msuReleaseResources(void);
SINT32 msuDefaultSocketConfig();



/****************************************************************/
SINT32 msuDefaultSocketConfig()
{
   return msuDefaultSocketCreate();
}
void msuLeechInit(void)
{
 //Initialize the PDU module
 msuPduInit();

 //Initialize the socket module
 msuSocketInit( );
 // Register call back function if any
 msuRegisterCallBackMethod(0,&msuChangeDeviceState,NULL);

 //Initialize the CRC module
 crcInit();

 /* initialize the complain mode*/
 complainModeInit();
 localSeqNumber = 1;
 localChunkNumber = 1;
 sequenceBitMap = 0;
 partialChunkSize = 0;
 chunkDataSize = 0;
 lastChunkSize = 0;

 memset(filePath, 0, MSU_MAX_FILE_PATH_SIZE);

#ifdef CHUNK_DATA_STORE_RAM  
 pChunkBuffer = (UINT8 *) msuMalloc(sizeof(UINT8) * MSU_MAX_SEQ_NUMBER * MSU_MAX_SEQ_SIZE);
 if( pChunkBuffer == NULL)
 {
#ifdef	Debug_level1
	 printf("\n Malloc failed to allocate Chunk Buffer\n");
#endif
 }

 
 memset(pChunkBuffer, 0, ( sizeof(UINT8) * MSU_MAX_SEQ_NUMBER * MSU_MAX_SEQ_SIZE));
#endif

 eMsuStatus = EMSUErrSuccess;

}


/**  @fn SINT32 msuLeechConfigure(void)
 *   @brief msuLeechConfigure() is used to configure MSU client with the configuration received in Sync message
 *   @details  When MSU client receives Sync message from server, sync message contains configuration information 
 *             like File Number, Number of chunks,sequence number limit, Multicast port and IP address,file name,
 *             dest path name and CRC. using this parameter msuLeechConfigure( ) open a file with the specified 
 *             file name and creates a updated socket with specified Multicast IP address and port number. 
 *   @param[in] : None 
 *   @return MSU_SOCKET_SUCCESS, if the operation is successful
 *           MSU_SOCKET_ERROR code if the operation is failed.
 *   @retval 0 if created successfully
 *   @retval -1 if failed 
 */
SINT32 msuLeechConfigure(void)
{
	SINT32 status = MSU_SOCKET_ERROR;
	UINT8 pathOffset = 0;

#ifdef CHUNK_DATA_STORE_RAM 
	if(pChunkBuffer != NULL)
	{
#endif	
	   if((msuBroadcastPdu.msuSyncHeaderformat.destPathTag.isFieldPresent) &&
		   (msuBroadcastPdu.msuSyncHeaderformat.destPathTag.fieldLength != 0))
		{
		   //Copy the filepath
			strncpy(filePath,(SINT8 *) msuBroadcastPdu.msuSyncMessageVariableData.destPath, msuBroadcastPdu.msuSyncHeaderformat.destPathTag.fieldLength);
			pathOffset += msuBroadcastPdu.msuSyncHeaderformat.destPathTag.fieldLength;
		}
		else
		{
			//Copy the default path
			strncpy(filePath,(SINT8 *) MSU_FILE_DEFAULT_PATH, strlen(MSU_FILE_DEFAULT_PATH));
			pathOffset += strlen(MSU_FILE_DEFAULT_PATH);
		}
		
		if((msuBroadcastPdu.msuSyncHeaderformat.fileNameTag.isFieldPresent) &&
		   (msuBroadcastPdu.msuSyncHeaderformat.fileNameTag.fieldLength != 0))
		{
		   //Copy the filename
			msuMemcpy(&filePath[pathOffset], msuBroadcastPdu.msuSyncMessageVariableData.fileName,msuBroadcastPdu.msuSyncHeaderformat.fileNameTag.fieldLength);
		}
		else
		{
			//Copy the default filename
			msuMemcpy(&filePath[pathOffset], MSU_FILE_DEFAULT_NAME,strlen(MSU_FILE_DEFAULT_NAME));
		}
#ifdef	Debug_level1
		printf("\nOpening file %s\n", filePath);
#endif

		pMsuFileDes = msuFileOpen(filePath, MSU_FILE_RWC);
		if( pMsuFileDes != -1)
		{
#ifdef	Debug_level1
			printf("\nFile opened successfully at %s\n", filePath);
#endif
		}
		else if( pMsuFileDes == -1)
		{
		   MsuGlobalError = MSU_FILE_OPEN_FAILED;
           return( MSU_SOCKET_ERROR);			
		}

		//Calculate the last chunk size
		lastChunkSize = (UINT32) (msuBroadcastPdu.msuSyncHeaderformat.fileSize % ((UINT32)(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * msuBroadcastPdu.msuSyncHeaderformat.seqSize)));
#ifdef	Debug_level1
		printf("\nLast chunk size  = %ld\n", lastChunkSize);
#endif
		if(lastChunkSize%msuBroadcastPdu.msuSyncHeaderformat.seqSize)
		{
			noOfSeqInLastChunk = lastChunkSize/msuBroadcastPdu.msuSyncHeaderformat.seqSize + 1;
		}
		else
		{
			noOfSeqInLastChunk = lastChunkSize/msuBroadcastPdu.msuSyncHeaderformat.seqSize;
		}
		/* Setup socket for update process */
		localChunkNumber = 1;		
		status = msuUpdateSocketCreate(msuBroadcastPdu.msuSyncHeaderformat.multicastPort, msuNtoHl(msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv4) );
		if(status == MSU_SOCKET_ERROR)
		{
		 MsuGlobalError = MSU_UPDATE_SOCKET_OPEN_FAILED;
#ifdef	Debug_level1
		 printf("\nSetting up multicast socket failed for port = %d and ip = %lx\n",
				 msuBroadcastPdu.msuSyncHeaderformat.multicastPort,
				 msuBroadcastPdu.msuSyncHeaderformat.multicastIP);
#endif
  		return (status);
		}
#ifdef CHUNK_DATA_STORE_RAM 
	}
	else
	{
#ifdef	Debug_level1
		printf("\nMemory allocation failed for chunk buffer\n");
#endif
	}
#endif
	
	memset( ccm_seqVal,0, sizeof( UINT32) * MSU_MAX_CCM_CHUNK_NUMBER );
	ccmRetryCount = 0;
	MsuGlobalError = 0;
	ccmSent = FALSE;	
	eMsuPrevState = eMsuStateRunning;
	return status;
}

/**  @fn void msuReleaseResources(void)
 *   @brief msuReleaseResources() is used to release resources
 *   @details  msuReleaseResources() function releases the resources like closing the existing sockets 
 *             and closing the opened file and initialize the chunk buffer with 0
 *   @param[in] : None
 *   @return      None
 *   @retval  None
 */
void msuReleaseResources(void)
{
  /*Release Leech resources*/
#ifdef CHUNK_DATA_STORE_RAM
	if(pChunkBuffer != NULL)
	{
#ifdef ETHERBRICK_PLATFORM
	memset(pChunkBuffer, 0, ( sizeof(UINT8) * MSU_MAX_SEQ_NUMBER * MSU_MAX_SEQ_SIZE));
#else
	msuFree(pChunkBuffer);
	pChunkBuffer = NULL;
#endif	
	}
#endif
	
	/*Release Socket resources*/
	msuSocketRelease();

	complainModeInit( );

	/*Release file resources*/
   if( pMsuFileDes != -1)
	   msuFileClose(pMsuFileDes);

	localSeqNumber   = 0;
	localChunkNumber = 0;
	sequenceBitMap   = 0;
	partialChunkSize = 0;
	chunkDataSize    = 0;
	lastChunkSize    = 0;

	 memset(filePath, 0, MSU_MAX_FILE_PATH_SIZE);

	firstTime = 1;	
}


/**  @fn void  msuChangeDeviceState( EMSUState presentState )
 *   @brief msuChangeDeviceState() is used to change device state machine state
 *   @param[in] : EMSUState presentState   
 *   @return   None
 *   @retval socket descriptor ( integer value) if created successfully
 */
void  msuChangeDeviceState( EMSUState presentState )
{
	eMsuState = presentState;
}

/**  @fn EMSUState msuCurrentState()
 *   @brief msuCurrentState() is used to return the current state of the MSU state machine
 *   @param[in] : None
 *   @return returns EMSUState
 */

EMSUState msuCurrentState()
{
	return eMsuState;
}

/**  @fn EMSUUpgradeState  msuGetUpgradeProcessState()
 *   @brief msuGetUpgradeProcessState() is used to get the upgrade process state
 *   @param[in] : None 
 *   @return EMSUUpgradeState   upgrade state
 *   @retval eMsuUpgradePassed or eMsuUpgradeFailed or  eMsuUpgradeInProgress
 */
	
EMSUUpgradeState  msuGetUpgradeProcessState()
{
	return deviceUpgadeStatus;
}

/**  @fn void msuHandleScmMcastPacket( )
 *   @brief msuHandleScmMcastPacket() function is used to receive missed sequence packets in that particular chunk
 *   @details  After reception of each chunk, client checks whether any sequences missed in that chunk, if missed 
 *             then client enters into SCM mode and waits to received missed sequence packets from server. 
 *             This function receives missed sequence packets and write into a file. If all missed sequences are received
 *             or sequence packet received from different current chunk number other than current chunk then write the current
 *             chunk in file and changes the state to Running or CCM state depends on previous contained state
 *   @param[in] : None
 *   @return None
 */

void msuHandleScmMcastPacket( )
{
	UINT8 noOfSeq = 0;
#ifdef	Debug_level1
	printf("\nRx SCM CN %ld and SN %d for parsing\n",msuMcastPdu.chunkNumber, msuMcastPdu.seqNumber);
	printf("\n^^^^^^SCM Mode^^^^^^^\n");
#endif
	/* We are in SCM mode: check whether we have received the packet within the same chunk number*/
	if(localChunkNumber == msuMcastPdu.chunkNumber)
	{	 
	   if(isSeqRecvd(msuMcastPdu.seqNumber) == FALSE)
	   {
		   /*Copy the data into the buffer/File*/
         if( msuFileSeek(&pMsuFileDes,(UINT32)( (	(msuMcastPdu.chunkNumber - 1) * 
         	(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) +
          ( (msuMcastPdu.seqNumber - 1) * ( msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) )	== TRUE )
         {
         		
         	if( msuFileWrite(pMsuFileDes,(SINT8 *)msuMcastPdu.pData, msuMcastPdu.dataLength) == TR_SUCCESS)
         	{
//         	   printf("\n Writing at location from SCM %ld\n",f_tell((FS_FILE*)pMsuFileDes));
         		/*Update sequence number*/
				   localSeqNumber = msuMcastPdu.seqNumber;
				  
				   setRecvdSeqBit(localSeqNumber);
				  
				   if( msuBroadcastPdu.msuSyncHeaderformat.chunkCount == localChunkNumber)
				   	noOfSeq = noOfSeqInLastChunk;
			   	else
				   	noOfSeq = msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit;

				   if( getMissedSeqBitMap(noOfSeq) == 0 )
				   {
#ifdef	Debug_level0
						printf("\nSCM pckt CN %ld and SEQBITS %x\n",localChunkNumber,getSeqBitMap( ));
#endif
				   
					   setRecvdChnkBit(localChunkNumber);
					   localSeqNumber = 0;
					   clearSeqBitMap();
					   chunkDataSize=0;
					   localChunkNumber++;
					   if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
							eMsuState = eMsuStateDone;	
					   else 
					   	eMsuState = eMsuPrevState; 
				   }

         	}
         	else
         	   MsuGlobalError = MSU_FILE_WRITE_FAILED;
#ifdef Debug_level0 
      			printf( "\nSCM =====>>FILE write error\n");
#endif
         }
	   }
   }
   else /* Receive wrong chunk*/
   {
		   /*Reset the parameters for new chunk*/
#ifdef	Debug_level1
	printf("\nSCM pckt CN %ld and SEQBITS %x\n",localChunkNumber,getSeqBitMap( ));
#endif		   
		   ccm_seqVal[localChunkNumber -1] = getSeqBitMap( );		   
		   localSeqNumber = 0;
		   chunkDataSize  = 0;
		   clearSeqBitMap();
		   localChunkNumber = msuMcastPdu.chunkNumber;

		if(isSeqRecvd(msuMcastPdu.seqNumber) == FALSE)
		{
	      if( msuFileSeek(&pMsuFileDes,(UINT32)( (	(msuMcastPdu.chunkNumber - 1) * 
	        	(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) +
	         ( (msuMcastPdu.seqNumber - 1) * ( msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) )	== TRUE )
	      {
	         		
	         	if( msuFileWrite(pMsuFileDes,(SINT8 *)msuMcastPdu.pData, msuMcastPdu.dataLength) == TR_SUCCESS)
	         	{
//	         	   printf("\n Writing at location from SCM %ld\n",f_tell((FS_FILE*)pMsuFileDes));
						printf( "\n Jump Over SCM CN %ld and SN %d Writing at location from SCM %ld\n",
									msuMcastPdu.chunkNumber, msuMcastPdu.seqNumber);
					   /*Set the sequence bitmask*/
					   setRecvdSeqBit(msuMcastPdu.seqNumber);
					   eMsuState = eMsuPrevState;
	         	}
	         	else
	         	   MsuGlobalError = MSU_FILE_WRITE_FAILED;
#ifdef Debug_level0 
      			printf( "\nSCM =====>>FILE write error\n");
#endif
	         }
   		}/*if(isSeqBitMapSet)*/
   }
}



/**  @fn void msuHandleCcmMcastPacket(void)
 *   @brief void msuHandleCcmMcastPacket(void) function is used to receive missed chunks from the server
 *   @details  After file transfer completes, client checks whether any chunks are missed in that file, if chunks missed 
 *             then client enters into CCM mode and waits to received missed chunks from server. 
 *             After receiving each chunk it checks for missed sequences in the chunk, if any then it enters into SCM state,
 *             after receiving missed sequences it comes back to CCM state to receive other missed chunks  
 *   @param[in] : None
 *   @return    : None
 */
void msuHandleCcmMcastPacket(void)
{
   UINT32  missedSeqBitMap = 0;
   UINT8 missedSeqCount = 0;
#ifdef	Debug_level1
	printf("\nRx CCM CN %ld and SN %d \n",msuMcastPdu.chunkNumber, msuMcastPdu.seqNumber);
#endif
	if(isChnkRecvd(msuMcastPdu.chunkNumber) == FALSE)
	{
		
	
		if(firstTime)
		{
			firstTime=0;
			localChunkNumber = msuMcastPdu.chunkNumber;			
			localSeqNumber = msuMcastPdu.seqNumber;
			/*Reset the received sequence bit map*/
			clearSeqBitMap();
			setSeqBitMap(ccm_seqVal[localChunkNumber -1] );        
		}

	 	if(localChunkNumber != msuMcastPdu.chunkNumber)
		{				
		  ccm_seqVal[localChunkNumber -1] = getSeqBitMap( );	
		  localChunkNumber = msuMcastPdu.chunkNumber;		
		  /*Reset the received sequence bit map*/
		  clearSeqBitMap();
		  setSeqBitMap(ccm_seqVal[localChunkNumber -1] );		
		}      
		if( isSeqRecvd( msuMcastPdu.seqNumber) == TRUE)
		{
#ifdef	Debug_level1
			printf("\nCCm Already received %ld and SN %d \n",msuMcastPdu.chunkNumber, msuMcastPdu.seqNumber);
#endif
			return;
		}     
		msuFileSeek(&pMsuFileDes,(UINT32)( ((msuMcastPdu.chunkNumber - 1) *
           (msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) +
           ( (msuMcastPdu.seqNumber -1) * ( msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) );
		   
      if( msuFileWrite(pMsuFileDes,(SINT8 *)msuMcastPdu.pData, msuMcastPdu.dataLength) == TR_SUCCESS )
      {
#ifdef	Debug_level0
      	printf("\n CCM packet Writing to file\n");
#endif      	
      	setRecvdSeqBit(msuMcastPdu.seqNumber);
		if(msuMcastPdu.fileTransferInfo.chunkTransferState == 1)
		{			
			missedSeqBitMap = getMissedSeqBitMap(msuMcastPdu.seqNumber);		
			if(missedSeqBitMap == 0)
			{
#ifdef	Debug_level1
				printf("\nCCM Writing CN %ld of size %ld into file\n",localChunkNumber, chunkDataSize);
#endif
#ifdef	Debug_level0
				printf("\nCCM pckt CN %ld and SEQBITS %x\n",msuMcastPdu.chunkNumber,getSeqBitMap( ));
#endif			/*Set chunk bit*/
				setRecvdChnkBit(localChunkNumber);
				/* Check All the the missed chunk received*/
				if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
					eMsuState = eMsuStateDone;
				else
					firstTime =1;
			}
			else /* Go SCM there is some sequence missed in current chunk*/
			{
#ifdef	Debug_level0
				printf("\nCCM pckt CN %ld and SEQBITS %x\n",msuMcastPdu.chunkNumber,getSeqBitMap( ));
#endif
				ccm_seqVal[localChunkNumber -1] = getSeqBitMap( );
				missedSeqCount = getMissedSeqCount(missedSeqBitMap );		
				if(msuSendScmMsg(missedSeqBitMap,msuMcastPdu.chunkNumber,missedSeqCount) == TRUE)
				{
#ifdef	Debug_level1
					printf("\nUnicast SCM Message Sent\n");
#endif
					eMsuState = eMsuStateSCM;
#ifdef	Debug_level1
					printf("\nCCM =====>> SCM\n");
#endif
					eMsuPrevState = eMsuStateCCM;
					return;
				}
			}
		}	
      }
      else
      {
      	MsuGlobalError = MSU_FILE_WRITE_FAILED;
      }
	}
}

/**  @fn void msuHandleMcastPacket(void)
 *   @brief void msuHandleMcastPacket(void) function is used to receive all the chunks from server
 *   @details  When server Intiate data transfer, msuHandleMcastPacket( ) receives all the sequences for each chunk
 *             after reciving each chunk it checks for missed sequences in the chunk, if any then it sends SCM message to 
 *             server with missed missed sequence numbers then enters ito SCM mode. After coming back from SCM mode it set the 
 *             chunk number bit map arrey if all sequences received for that particulat chunk.
 *             Then it again starts to receive next chunk number from server till the end of file reached    
 *   @param[in] : None
 *   @return    : None
 */

void msuHandleMcastPacket(void)
{
	UINT8 missedSeqCount = 0;
#ifdef	Debug_level1
	printf("\nRx Update Mcast pckt CN %ld and SN %d\n",msuMcastPdu.chunkNumber,msuMcastPdu.seqNumber);
#endif

   if( localChunkNumber > msuMcastPdu.chunkNumber)
      return;
 
	if(localChunkNumber != msuMcastPdu.chunkNumber)
	{
		ccm_seqVal[localChunkNumber -1] = getSeqBitMap( );		
		clearSeqBitMap();
		localChunkNumber = msuMcastPdu.chunkNumber;
	}	
	if( msuFileSeek(&pMsuFileDes,(UINT32)( (	(msuMcastPdu.chunkNumber - 1) * 
		(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) +
		( (msuMcastPdu.seqNumber - 1) * ( msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) )	== TRUE )
	{
#ifdef Debug_level2		
		printf( "Fseek fail in Sequence jump over\n");
#endif
	}
	

	if( msuFileWrite(pMsuFileDes,(SINT8 *)msuMcastPdu.pData, msuMcastPdu.dataLength) > 0)
	{
		printf("\n MCast CN %ld and SN %d writen to file \n",msuMcastPdu.chunkNumber,
				msuMcastPdu.seqNumber);      
		setRecvdSeqBit(msuMcastPdu.seqNumber);

		/*Check for chunk end state*/
		if(msuMcastPdu.fileTransferInfo.chunkTransferState == 1)
		{
			//End of chunk, check whether we need to go into SCM
			missedSeqBitMap = getMissedSeqBitMap(msuMcastPdu.seqNumber);
			if(missedSeqBitMap != 0)
			{	
			   missedSeqCount = getMissedSeqCount(missedSeqBitMap );			
				if(msuSendScmMsg(missedSeqBitMap,msuMcastPdu.chunkNumber,missedSeqCount) == TRUE)
				{
#ifdef	Debug_level1
					printf("\nUnicast SCM Message Sent CN %ld\n",msuMcastPdu.chunkNumber);
#endif
					eMsuState = eMsuStateSCM;
					eMsuPrevState = eMsuStateRunning;
					return;
				}//if(msuSendScmMsg(missedSeqBitMap,msuMcastPdu.chunkNumber,1) == TRUE)
			}//if(missedSeqBitMap != 0)
			else
			{
#ifdef	Debug_level0
	printf("\nMCAST pckt CN %ld and SEQBITS %x\n",msuMcastPdu.chunkNumber,getSeqBitMap( ));
#endif
			
				/*set appropriate CN bit mask as received*/
				setRecvdChnkBit(localChunkNumber);				
				localChunkNumber = localChunkNumber + 1;
				/*Reset the parameters for next chunk*/
				clearSeqBitMap();				
			}
			if(msuMcastPdu.fileTransferInfo.fileTransferState == 1)
			{
				/*Check whether we need to go into CCM mode : This check is performed on the last packet itself
				 * instead of waiting for upgrade complete message*/
				if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
				{	/*All chunks received, proceed to CRC verification*/
					eMsuState = eMsuStateDone;
#ifdef	Debug_level1
					printf("\nREADY =====>> DONE\n");
					printf("All chunks received, proceed to CRC verification");
#endif
				}
				else
				{					
					if(msuSendCcmMsg(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == TRUE)
					{
						/* reset the parameter for chunk retransmission */
						localChunkNumber = 0;
						chunkDataSize = 0;
						ccmRetryCount++;
						#ifdef DEBUG_LEVEL2			
						LogMsg( STATMSG,"\nREADY =====>> CCM\n");
						#endif			
						eMsuState = eMsuStateCCM;
						/* Make prevState to CCM so SCM can now jump to CCM*/
						eMsuPrevState = eMsuStateCCM;
					}	
				
				}
			}
		}	
	}
	else
	{
 		MsuGlobalError = MSU_FILE_WRITE_FAILED;
#ifdef	Debug_level0
		printf("\n File write operation Fail\n");
#endif
	}
}

/**  @fn void msuLeechStateMachine(void)
 *   @brief void msuLeechStateMachine(void) function is the main state machine function to handle MSU client
 *   @details  This is the main MSU client function , which follows state machine, initially it enters into initialize state
 *             to initalize all the variables related to client then it enters into configure state where it creates default
 *             multicast socket and enters into listen state where it blocks to receive multicast packet it may be discovery
 *             messag request ot update status request or sync message request, once it receives sync message it enters into 
 *             eMsuStateStart state and creates update server socket with the specified configuration received from sync
 *             message then it enters into eMsuStateRunning state to receive all chunks, depends on complain mode it enters 
 *             into SCM or CCM states to receive missed sequences or chunks. if everything is received it enters into done state
 *             and does check sum validation on the received file, sends status message to server, then it enters into loopback 
 *             state and releases all resources and goes back to configure state. if any problem occured then it enters into
 *             abort state from there it enter into loopback then reeases resources then enters into configure state
 *
 *             
 *   @param[in] : None
 *   @return    : None
 */

void msuLeechStateMachine(void)
{
	BOOL status = FALSE;

	while(eMsuStatus == EMSUErrSuccess)
	{
		switch(eMsuState)
		{
		case eMsuStateInitialize:
			/*Initialize MSU Leech at start of each cycle*/
			msuLeechInit();
#ifdef Debug_level1
			printf("\nMSU Leech Initialized\n");
#endif
			eMsuState = eMsuStateConfigure;
			break;

		case  eMsuStateConfigure:
			if (msuDefaultSocketConfig() == MSU_SOCKET_SUCCESS )
				eMsuState = eMsuStateListen;
			break;

		case  eMsuStateListen:
			/*Listen to the sockets until sync message is received*/
#ifdef Debug_level0
			printf("\nMsu listening on default socket\n");
#endif
			if(msuSocketCheckActivity() == TRUE)
			{
				eMsuState = eMsuStateStart;
			}
			break;

		case eMsuStateStart:
			//printf("\nConfigurating MSU Leech\n");
			/*Set up the MSU Leech according to the sync message paramaters*/
			if(msuLeechConfigure() == MSU_SOCKET_SUCCESS)
			{	/*Update the configuration state to configured*/
				eMsuState = eMsuStateRunning;
#ifdef Debug_level1
				printf("\nMSU Ready to receive file from server\n");
#endif
			}
			else
			{
				eMsuState = eMsuStateListen;
			}
			   
          break;

		case eMsuStateRunning:
			/*Listen for Multicast packets from the server*/
			status = msuSocketGetDataPackets();
			if(status == TRUE)
			{
				msuHandleMcastPacket();
			}
            break;

		case eMsuStateSCM:
			status = msuSocketGetDataPackets();
			if(status == TRUE)
			{
				msuHandleScmMcastPacket();
			}
			break;

		case eMsuStateCCM:
			status = msuSocketGetDataPackets();
			if(status == TRUE)
			{
				msuHandleCcmMcastPacket();
			}
			break;

		case eMsuStateAbort:
			/*Discard the file here*/
#ifdef Debug_level0
			printf("\nAborting file transfer\n");
#endif
			eMsuState = eMsuStateLoopBack;
			deviceUpgadeStatus = eMsuUpgradeFailed;
			MsuGlobalError = MsuGlobalError | MSU_UPDATE_STATUS_ABORT;
			break;

		case eMsuStateDone:
#ifdef Debug_level1
			printf("\nClosing file\n");
#endif
			/*Close the previously opened file*/
			msuFileClose(pMsuFileDes);
			
			pMsuFileDes = -1;
			if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
			{
#ifdef Debug_level1
					printf("\nFile received Successful\n");
#endif
               deviceUpgadeStatus = eMsuUpgradePassed;

				/*Perform checksum validation of the file*/
				if(msuPlatformComputeCheckSum(filePath, msuBroadcastPdu.msuSyncHeaderformat.fileChecksum) == TRUE)
				{
#ifdef Debug_level0
					printf("\nChecksum Verification Successful\n");
#endif
					deviceUpgadeStatus = eMsuUpgradePassed;
				}
				else
				{
#ifdef Debug_level0
					printf("\nChecksum Verification Failed\n");
#endif
                    MsuGlobalError = MSU_UPDATE_FILE_CRC_FAILED; 
					deviceUpgadeStatus = eMsuUpgradeFailed;
				}				
			}			
			else
			{
#ifdef Debug_level1
				printf("\n Failed To Receive Complete File \n");
#endif
				deviceUpgadeStatus = eMsuUpgradeFailed;

				/*Discard the file here*/
			}
			/*Loop back and keep listening*/
			msuSendUpgradeStatusMsg(FALSE);
			eMsuState = eMsuStateLoopBack;
			break;

		case eMsuStateLoopBack:
			/*Release All resources*/
			msuReleaseResources();

			/*Loop back and keep listening*/
	      eMsuState = eMsuStateConfigure;
			break;

		default :
			eMsuStatus = EMSUErrUndefinedState;
			break;

		}//switch(eMsuState)

	}//while(eMsuStatus == EMSUErrSuccess)

	if(eMsuStatus != EMSUErrSuccess)
	{
		/*Release all resources*/
		msuReleaseResources();
	}//if(eMsuStatus != EMSUErrSuccess)
}

/**  @fn BOOL msuPduHandleTransferCompleteRequest( )
 *   @brief msuPduHandleTransferCompleteRequest() is used to handle transfer complete request
 *   @details  When client receives transfer complete request, it calls msuPduHandleTransferCompleteRequest() function,
 *             it checks whether all chunks received successfully or not , if some chunks are missing then it sends 
 *             CCM message with missed chunk numbers to server and chnges state to CCM state.
 *             if all the chunks received then it enters to done state. 
 *   @return   TRUE, if it sends CCM message, otherwise FALSE
 *   @retval   returns 1 or 0
  */

BOOL msuPduHandleTransferCompleteRequest( )//UINT8 *pMsg , UINT32 msgSize
{
   if( ccmSent == TRUE)
      return FALSE;
   
	if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) != 0)
	{
		if(msuSendCcmMsg(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == TRUE)
		{
			/* reset the parameter for chunk retransmission */
			localChunkNumber = 0;
			chunkDataSize = 0;
			localSeqNumber = 0;
			ccmRetryCount++; 
#ifdef Debug_level1
			printf("\nREADY =====>> CCM\n");
#endif
			eMsuState = eMsuStateCCM;
			eMsuPrevState = eMsuStateCCM;
			ccmSent = TRUE;
		}
	}
	else
	{
		eMsuState = eMsuStateDone;
#ifdef Debug_level1
		printf("\nREADY =====>> DONE\n");
		printf("All chunks received, proceed to CRC verification");
#endif
	}
return FALSE;
}
/*
 * Return always false becase this is not a data packet. So next cycle may have data packet
 * */
BOOL msuPduHandleCCMCompleteRequest(UINT8 *pMsg , UINT32 msgSize)
{

   MSUCCMCompletePdu  *ccmCompletedReq;
  
   ccmCompletedReq = (MSUCCMCompletePdu  *)pMsg;
  
   eMsuPrevState = eMsuStateCCM;
   if( ccmCompletedReq->ccmRetry == FALSE)
   {
     	eMsuState = eMsuStateAbort;
     	if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) != 0)
     		MsuGlobalError = MSU_UPDATE_EXCEED_CCM_RETRY;
     	
   	return FALSE;
   }
   
	if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) != 0)
	{
		if(msuSendCcmMsg(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == TRUE)
		{
			/* reset the parameter for chunk retransmission */
			localChunkNumber = 0;
			chunkDataSize = 0;
			ccmRetryCount++;
#ifdef Debug_level1
			printf("\nREADY =====>> CCM\n");
#endif
			eMsuState = eMsuStateCCM;
		}
	}
	else
	{
		eMsuState = eMsuStateDone;
#ifdef Debug_level1
		printf("\nREADY =====>> DONE\n");
#endif
		printf("All chunks received, proceed to CRC verification");

	}
return FALSE;
}

BOOL msuPduHandleSCMCompleteRequest(UINT8 *pMsg , UINT32 msgSize)
{
   UINT8 missedSeqCount = 0;
   UINT32 missedSepBitmap = 0;
   MSUSCMCompletePdu  *scmCompletedReq;
  
   scmCompletedReq = (MSUSCMCompletePdu  *)pMsg;
  
   if( scmCompletedReq->scmRetry == FALSE)
   {
      eMsuState = eMsuPrevState ;
     	   
     	#ifdef DEBUG_LEVEL1		
		LogMsg( ERRMSG,"\n SCM retry 0  In SCM Complete request \n");
		#endif

   	return FALSE;
   }

   missedSepBitmap = getMissedSeqBitMap(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit);
	if( missedSepBitmap != 0 )
	{
	   missedSeqCount = getMissedSeqCount(missedSepBitmap );	
		if(msuSendScmMsg(missedSepBitmap,msuMcastPdu.chunkNumber,missedSeqCount) == TRUE)
		{
			scmRetryCount++;
			#ifdef DEBUG_LEVEL1			
				LogMsg( ERRMSG,"\nREADY =====>> SCM from SCM Complete Request \n");
			#endif	
			
			eMsuState = eMsuStateSCM;		
		}
	}
	else
	{
		eMsuState = eMsuPrevState ;
      
		#ifdef DEBUG_LEVEL1		
		LogMsg( ERRMSG,"\n No Seq missed In SCM Complete request \n");
		#endif
	}
	return FALSE;
}

/**  @fn UINT32 getCcmRetryCount()
 *   @brief getCcmRetryCount() is used to get the CCM Retry count
 *   @details  This function used to get the CCM retry count value, i,e how many times client sends the CCM Request 
 *             to receive missed chunks from server
 *   @param[in] : None
 *   @return UINT32  CCM retry count value
 *   @retval UINT32 value
  */

UINT32 getCcmRetryCount()
{
	return(ccmRetryCount);
}

/**  @fn EMSUState getCurrentMsuState( )
 *   @brief getCurrentMsuState() is used to get the current state machine state
 *   @param[in] : None
 *   @return EMSUState   msu state machine current state
 *   @retval EMSUState type
  */

EMSUState getCurrentMsuState( )
{
	return (eMsuState);
}

/**  @fn msuPlatformComputeCheckSum(SINT8 * filePath, UINT32 fileCheckSum)
 *   @brief This function is used to calculate the checksum
 *   @details  This function is used to calculate checksum of the specified file and compare the checksum
 *             with received check sum to confirm whether we received complete file successfully or not
 *   @param[in] : filePath - complete location of the file
 *   @param[in] : fileCheckSum - checksum value received from server
 *   @return returns true if file received successfully
 *   @retval TRUE if  successful
 *   @retval FALSE if failed 
  */

BOOL msuPlatformComputeCheckSum(SINT8 * filePath, UINT32 fileCheckSum)
{
	BOOL result = FALSE;
	UINT8 fileBuff[1460];//MSU_FILE_CRC_BUFFER_SIZE
	SINT16 bytesRead = 0;
	UINT32 tempCheckSum = 0;

	SINT32 fileDes = 0;

	if( (filePath != NULL) && (fileCheckSum != 0))
	{
      //Open the file in read only mode
		fileDes = msuFileOpen((char *)filePath,  MSU_FILE_R);
		if( fileDes != -1)
		{
#ifdef Debug_level1
		  // LogMsg( ERRMSG,"\nFile opened in read mode\n");
#endif		   

		   do
		   {
			   //Read from the file
			   bytesRead = (UINT16)msuFileRead(fileDes,(SINT8 *)fileBuff, 1460);	
			   if(bytesRead > 0)
			   {
			      tempCheckSum += crcFast(fileBuff, (SINT16)bytesRead);
			   }

		   }while(bytesRead > 0);
#ifdef Debug_level1
	//	   LogMsg( ERRMSG,"\nCalculated checksum = %lx\n", tempCheckSum);
#endif		   

           //Close the file
           msuFileClose(fileDes);

		   //Compare the checksums calculated
           if(tempCheckSum == fileCheckSum)
           {
        	 		result = TRUE;
           }
		}
		else
		   MsuGlobalError = MSU_FILE_OPEN_FAILED;
	}

	return result;
}




#endif /*#ifndef MSU_LEECH_CPP */
