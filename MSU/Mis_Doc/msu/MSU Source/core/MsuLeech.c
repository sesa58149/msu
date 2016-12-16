/**	@file MsuLeech.cpp
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

#ifndef MSU_LEECH_CPP
#define MSU_LEECH_CPP


#include "fsf.h"               
#include "fsm.h"               
#include "fsmf.h"

#include <stdio.h>
#include <string.h>
#include "MsuPlatform.hpp"
#include "MsuFilePlatform.hpp"
#include "MsuPdu.hpp"
#include "MsuSocket.hpp"
#include "MsuCRC32.hpp"
#include "MsuComplainMode.hpp"
#include "LogMsg.h"
#include "MsuLeech.hpp"
#include "FileSystemImpl.hpp"

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



SINT32 msuDefaultSocketConfig()
{
   return msuDefaultSocketCreate();
}


/**  @fn void msuLeechInit(void)
 *   @brief msuLeechInit() is used to initialize a variables with default values  
 *   @details  msuLeechInit() function is used intialize socket related variables, complain mode variables, 
 *             protocol data unit variables and global varables with default values.
 *             This function also creates a chunk buffer dynamically which is used to store chunk information
 *             before writing to a file.   
 *   @param[in] : None
 *   @return    : None 
 *   @retval    : None
 */            
void msuLeechInit(void)
{
 
   msuPduInit(); 
 	msuSocketInit( );
 	msuRegisterCallBackMethod(0,&msuChangeDeviceState,NULL);
	crcInit();
	complainModeInit();
	
	localSeqNumber   = 1;
	localChunkNumber = 1;
	sequenceBitMap   = 0;
	partialChunkSize = 0;
	chunkDataSize    = 0;
	lastChunkSize    = 0;

   msuMemset(filePath, 0, MSU_MAX_FILE_PATH_SIZE);

#ifdef CHUNK_DATA_STORE_RAM 
   pChunkBuffer = (UINT8 *) msuMalloc(sizeof(UINT8) * MSU_MAX_SEQ_NUMBER * MSU_MAX_SEQ_SIZE);
   if( pChunkBuffer == NULL)
   {
		#ifdef DEBUG_LEVEL3
		msuLogMsg( IPCL_METHOD_MSU,LOG_MSG_PRIORITY_ERROR,
				IPCL_LABEL_FAIL,
				"\n Malloc failed to allocate Chunk Buffer\n" );		
		#endif	
	}
   msuMemset(pChunkBuffer, 0, ( sizeof(UINT8) * MSU_MAX_SEQ_NUMBER * MSU_MAX_SEQ_SIZE));
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
			msuMemcpy(filePath,(SINT8 *) msuBroadcastPdu.msuSyncMessageVariableData.destPath, msuBroadcastPdu.msuSyncHeaderformat.destPathTag.fieldLength);
			pathOffset += msuBroadcastPdu.msuSyncHeaderformat.destPathTag.fieldLength;
		}
		else
		{
			//Copy the default path
			msuMemcpy(filePath,(SINT8 *) MSU_FILE_DEFAULT_PATH, msuStrlen(MSU_FILE_DEFAULT_PATH));
			pathOffset += msuStrlen(MSU_FILE_DEFAULT_PATH);
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
			msuMemcpy(&filePath[pathOffset], MSU_FILE_DEFAULT_NAME,msuStrlen(MSU_FILE_DEFAULT_NAME));
		}
		
		#ifdef DEBUG_LEVEL3
		msuLogMsg( STATMSG,"\nOpening file %s\n", filePath);
		#endif		

		pMsuFileDes = msuFileOpen(filePath, eMsuFileW);
	
		if( pMsuFileDes != -1)
		{
			#ifdef DEBUG_LEVEL2		
			msuLogMsg( STATMSG,"\nFile opened successfully at %s\n", filePath);
			#endif	
			
		
					
		}
		else if( pMsuFileDes == -1)
		{
		   MsuGlobalError = MSU_FILE_OPEN_FAILED;
			#ifdef DEBUG_LEVEL2		
			msuLogMsg( STATMSG,"\nFile open failed at %s\n", filePath);
			#endif	
         return( MSU_SOCKET_ERROR);			
		}

		//Calculate the last chunk size
		lastChunkSize = (UINT32) (msuBroadcastPdu.msuSyncHeaderformat.fileSize % ((UINT32)(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * msuBroadcastPdu.msuSyncHeaderformat.seqSize)));
			
		#ifdef DEBUG_LEVEL2
		msuLogMsg( STATMSG,"\nLast chunk size  = %ld\n", lastChunkSize);
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
		
		if(!msuBroadcastPdu.msuSyncHeaderformat.msuHeader.versionInfo.versionIP)
		{
				status = msuUpdateSocketCreate(msuBroadcastPdu.msuSyncHeaderformat.multicastPort, trNtoHl(msuBroadcastPdu.msuSyncHeaderformat.multicastIP.ServerIPv4) );
		}
		else
		{
			//Handle IPv6 address
		}
	
		if(status == MSU_SOCKET_ERROR)
		{
		   MsuGlobalError = MSU_UPDATE_SOCKET_OPEN_FAILED;
			#ifdef DEBUG_LEVEL2		
		 	msuLogMsg( STATMSG,"\nSetting up multicast socket failed for port = %d and ip = %lx\n",
				 msuBroadcastPdu.msuSyncHeaderformat.multicastPort,
				 msuBroadcastPdu.msuSyncHeaderformat.multicastIP);
			#endif	
       	return (status);			 
		}
#ifdef CHUNK_DATA_STORE_RAM 		
   }
	else
	{
		#ifdef DEBUG_LEVEL2	
	 	msuLogMsg(DEBUGMSG,"\nMemory allocation failed for chunk buffer\n");
		#endif	 
	}
#endif	
	
	msuMemset( ccm_seqVal,0, sizeof( UINT32) * MSU_MAX_CCM_CHUNK_NUMBER );
	ccmRetryCount  = 0;
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
#ifdef CHUNK_DATA_STORE_RAM 
   if(pChunkBuffer != NULL)
	{
		#ifdef ETHERBRICK_PLATFORM
			msuMemset(pChunkBuffer, 0, ( sizeof(UINT8) * MSU_MAX_SEQ_NUMBER * MSU_MAX_SEQ_SIZE));
		#else
			msuFree(pChunkBuffer);
			pChunkBuffer = NULL;
		#endif	
	}
#endif	
	
	msuSocketRelease();
	complainModeInit( );

   if( pMsuFileDes != -1)
	   msuFileClose(pMsuFileDes);

	localSeqNumber   = 0;
	localChunkNumber = 0;
	sequenceBitMap   = 0;
	partialChunkSize = 0;
	chunkDataSize    = 0;
	lastChunkSize    = 0;

	msuMemset(filePath, 0, MSU_MAX_FILE_PATH_SIZE);

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
   
   UINT8 result=TR_ERROR;

	/* We are in SCM mode: check whether we have received the packet within the same chunk number*/
	if(localChunkNumber == msuMcastPdu.chunkNumber)
	{	 
	   if(isSeqRecvd(msuMcastPdu.seqNumber) == FALSE)
	   {
		   /*Copy the data into the buffer/File*/ //dataLength
 #ifdef CHUNK_IN_RAM
 	  
 	    	msuMemcpy(info_array[msuMcastPdu.seqNumber - 1].Info,(UINT8*)msuMcastPdu.pData,msuMcastPdu.dataLength);
 		
 			ChunkSize+=msuMcastPdu.dataLength;
 			
 		
 #else			
         if( msuFileSeek(&pMsuFileDes,(UINT32)( (	(msuMcastPdu.chunkNumber - 1) * 
         	(msuBroadcastPdu.seqNumberLimit * msuBroadcastPdu.seqSize) ) +
          	( (msuMcastPdu.seqNumber - 1) * ( msuBroadcastPdu.seqSize) ) ) )	== TRUE )			   
         {
         
         	if( msuFileWrite(pMsuFileDes,(SINT8 *)msuMcastPdu.pData, msuMcastPdu.dataLength) == TR_SUCCESS)
         	{
					#ifdef DEBUG_LEVEL0         	
         	   msuLogMsg( STATMSG,"\n SCM CN %ld and SN %d Writing at location from SCM %ld\n",
         	   		msuMcastPdu.chunkNumber, msuMcastPdu.seqNumber,f_tell((FS_FILE*)pMsuFileDes));				
					#endif 
			}
		
		else
		{
         		MsuGlobalError = MSU_FILE_WRITE_FAILED;
         		#ifdef DEBUG_LEVEL0 
      			msuLogMsg( STATMSG,"\nSCM =====>>FILE write error\n");
      			#endif
				//added return
		#ifdef CHUNK_IN_RAM
				return;
		#endif
        }
		}
		
 #endif  					
				  
				   localSeqNumber = msuMcastPdu.seqNumber;
				  
				   setRecvdSeqBit(localSeqNumber);
				  
				   if( msuBroadcastPdu.msuSyncHeaderformat.chunkCount == localChunkNumber)
				   	noOfSeq = noOfSeqInLastChunk;
					else
				   	noOfSeq = msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit;

				  
				   if( getMissedSeqBitMap(noOfSeq) == 0 )
				  
				   {

					
					   setRecvdChnkBit(localChunkNumber);
	#ifdef CHUNK_IN_RAM
					
					
					#ifdef DEBUG_LEVEL0
							msuLogMsg( STATMSG,"\nSCM pckt CN %ld and SEQBITS %x\n",localChunkNumber,getSeqBitMap( ));
					#endif
						
					
				result=msuFileWrite(pMsuFileDes,(SINT8 *)&info_array,(UINT16)ChunkSize) ;
					
			
					
						
				   
						if(result!=TR_SUCCESS)
						{
					
				
						MsuGlobalError = MSU_FILE_WRITE_FAILED;
						#ifdef DEBUG_LEVEL0
						msuLogMsg( STATMSG,"\n File write operation Fail\n"); 
						#endif			
						}
							
					
						msuMemset(info_array,0,MSU_MAX_SEQ_NUMBER*sizeof(struct MsuInfo));
					
						ChunkSize=0;
						
    #endif	
					   
					   localSeqNumber = 0;
					   clearSeqBitMap();
					   chunkDataSize=0;
					   localChunkNumber++;
					   if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
					   {
					   							
							eMsuState = eMsuStateDone;	
					   }
					   else 
					   	eMsuState = eMsuPrevState; 
				   }
         	
			
			
         }
	   }
   
   else /* Receive wrong chunk*/
   {
	   /*Reset the parameters for new chunk*/
		#ifdef DEBUG_LEVEL1
		msuLogMsg( STATMSG,"\n chunk Jump over SCM pckt CN %ld and SEQBITS %x\n",localChunkNumber,getSeqBitMap( ));
		#endif	
		   
		ccm_seqVal[localChunkNumber -1] = getSeqBitMap( );
		   
		localSeqNumber = 0;
		chunkDataSize  = 0;
		clearSeqBitMap();
		localChunkNumber = msuMcastPdu.chunkNumber;

		if(isSeqRecvd(msuMcastPdu.seqNumber) == FALSE)
		{
	#ifdef CHUNK_IN_RAM
	
 			msuMemcpy(info_array[msuMcastPdu.seqNumber-1].Info,(UINT8*)msuMcastPdu.pData,((sizeof(UINT8))*msuMcastPdu.dataLength));
 		
 				ChunkSize+=msuMcastPdu.dataLength;
 		
    
	#else
	      if( msuFileSeek(&pMsuFileDes,(UINT32)( (	(msuMcastPdu.chunkNumber - 1) * 
	        	(msuBroadcastPdu.seqNumberLimit * msuBroadcastPdu.seqSize) ) +
	         ( (msuMcastPdu.seqNumber - 1) * ( msuBroadcastPdu.seqSize) ) ) )	== TRUE )			   
	      {
	         		
	        	if( msuFileWrite(pMsuFileDes,(SINT8 *)msuMcastPdu.pData, msuMcastPdu.dataLength) == TR_SUCCESS)
	         {
					#ifdef DEBUG_LEVEL0	         	
	         	msuLogMsg( STATMSG,"\n Jump Over SCM CN %ld and SN %d Writing at location from SCM %ld\n",
	         			msuMcastPdu.chunkNumber, msuMcastPdu.seqNumber,f_tell((FS_FILE*)pMsuFileDes));	         	
					#endif				
					/*Set the sequence bitmask*/
					setRecvdSeqBit(msuMcastPdu.seqNumber);														   
					eMsuState = eMsuPrevState;									   					
	         }
	         else
	         {
	         	MsuGlobalError = MSU_FILE_WRITE_FAILED;
	        		#ifdef DEBUG_LEVEL0 
      			msuLogMsg( STATMSG,"\nSCM =====>>FILE write error\n");
      			#endif
	         }
	            
	      }
	  #endif // #ifdef CHUNK_IN_RAM    
   	} // if(isSeqRecvd(msuMcastPdu.seqNumber) == FALSE)
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
   Uint32  missedSeqBitMap = 0;
   UINT8 missedSeqCount = 0;
   UINT8 res=0;
   

	#ifdef DEBUG_LEVEL2
	msuLogMsg( STATMSG,"\nRx CCM CN %ld and SN %d \n",msuMcastPdu.chunkNumber, msuMcastPdu.seqNumber);
	#endif	
	
	if(isChnkRecvd(msuMcastPdu.chunkNumber) == FALSE)
	{
	   if(firstTime)
		{
			firstTime=0;
			localChunkNumber = msuMcastPdu.chunkNumber;
			/*Reset the received sequence bit map*/
			clearSeqBitMap();
			setSeqBitMap(ccm_seqVal[localChunkNumber -1] );
		}

	 	if(localChunkNumber != msuMcastPdu.chunkNumber)
		{	
		   /*Save the sequences information received for current chunk*/			
			ccm_seqVal[localChunkNumber -1] = getSeqBitMap( );	
			localChunkNumber = msuMcastPdu.chunkNumber;
			
			/*Reset the received sequence bit map*/
			clearSeqBitMap();
			setSeqBitMap(ccm_seqVal[localChunkNumber -1] );
			
		#ifdef CHUNK_IN_RAM
 
			ChunkSize=0;
		msuFileSeek(&pMsuFileDes,(UINT32)( ((msuMcastPdu.chunkNumber - 1) * 
		(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * msuBroadcastPdu.msuSyncHeaderformat.seqSize) )) );
		
		#endif

		}
		if( isSeqRecvd( msuMcastPdu.seqNumber) == TRUE)
		{
#ifdef DEBUG_LEVEL2
			msuLogMsg( STATMSG,"\nCCm Already received %ld and SN %d \n",msuMcastPdu.chunkNumber, msuMcastPdu.seqNumber);
#endif
#ifdef CHUNK_IN_RAM //adding only for testing//get checked with Deepak/Pawan
	
		msuMemcpy(info_array[msuMcastPdu.seqNumber -1].Info,(SINT8 *)msuMcastPdu.pData,msuMcastPdu.dataLength);
		
		ChunkSize += msuMcastPdu.dataLength;
 #endif   
			return;
		 }
		 

#ifdef CHUNK_IN_RAM
	
		msuMemcpy(info_array[msuMcastPdu.seqNumber -1].Info,(SINT8 *)msuMcastPdu.pData,msuMcastPdu.dataLength);
		
		ChunkSize += msuMcastPdu.dataLength;
    
        #ifdef DEBUG_LEVEL0
      		msuLogMsg( STATMSG,"\nCCM pckt CN %ld and SEQBITS %x  and the chunksize %ld \n",msuMcastPdu.chunkNumber,getSeqBitMap( ),ChunkSize);
		#endif
	
#else
		
		 msuFileSeek(&pMsuFileDes,(UINT32)( ((msuMcastPdu.chunkNumber - 1) *(msuBroadcastPdu.seqNumberLimit * msuBroadcastPdu.seqSize) ) ) ); 

		res=msuFileWrite(pMsuFileDes,(SINT8 *)msuMcastPdu.pData, msuMcastPdu.dataLength);
		 
		 		
				    if(res!=TR_SUCCESS)
				 {
					
						MsuGlobalError = MSU_FILE_WRITE_FAILED;
						#ifdef DEBUG_LEVEL0 
						msuLogMsg( STATMSG,"\nCCM =====>>FILE write error\n");
						#endif
					
				
				 }	
#endif 

      	/*Set the sequence bit mask*/
      	setRecvdSeqBit(msuMcastPdu.seqNumber);
      	if(msuMcastPdu.fileTransferInfo.chunkTransferState == 1)
      	{
      		missedSeqBitMap = getMissedSeqBitMap(msuMcastPdu.seqNumber);
      		if(missedSeqBitMap == 0)
      		{
#ifdef DEBUG_LEVEL3
      			msuLogMsg( STATMSG,"\nCCM Writing CN %ld of size %ld into file\n",localChunkNumber, chunkDataSize);
#endif
#ifdef DEBUG_LEVEL0
      			msuLogMsg( STATMSG,"\nCCM pckt CN %ld and SEQBITS %x\n",msuMcastPdu.chunkNumber,getSeqBitMap( ));
#endif


#ifdef CHUNK_IN_RAM	
      			/*Set chunk bit*/
				/* Essentially for regular files only*/
				msuFileSeek(&pMsuFileDes,(UINT32)( ((msuMcastPdu.chunkNumber - 1) *(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * msuBroadcastPdu.msuSyncHeaderformat.seqSize) ) ) ); 

						
				
				res=msuFileWrite(pMsuFileDes,(SINT8 *)&info_array, (UINT16)ChunkSize);
				
				msuMemset(info_array,0,MSU_MAX_SEQ_NUMBER*sizeof(struct MsuInfo));	
						
				ChunkSize=0;
				#ifdef DEBUG_LEVEL0
						msuLogMsg( STATMSG,"\n In-CCM  CCM CN%ld and Writing at location%ld\n",msuMcastPdu.chunkNumber, f_tell((FS_FILE*)pMsuFileDes));
				#endif
				
				
				    if(res!=TR_SUCCESS)
				 {
					
						MsuGlobalError = MSU_FILE_WRITE_FAILED;
						#ifdef DEBUG_LEVEL0 
						msuLogMsg( STATMSG,"\nCCM =====>>FILE write error\n");
						#endif
					
				
				 }	
      			
#endif			
				
					
						setRecvdChnkBit(localChunkNumber);
					
					
      				
      		
      			
      			/* Check All the the missed chunk received*/
      			if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
      				eMsuState = eMsuStateDone;
      			else
      				firstTime =1;
      		}
      		else /* Go SCM there is some sequence missed in current chunk*/
      		{
#ifdef DEBUG_LEVEL0
      			msuLogMsg( STATMSG,"\nCCM pckt CN %ld and SEQBITS %x\n",msuMcastPdu.chunkNumber,getSeqBitMap( ));
#endif
      			ccm_seqVal[localChunkNumber -1] = getSeqBitMap( );
      			missedSeqCount = getMissedSeqCount(missedSeqBitMap );
      			if(msuSendScmMsg(missedSeqBitMap,msuMcastPdu.chunkNumber,missedSeqCount) == TRUE)
      			{
#ifdef DEBUG_LEVEL3
      				msuLogMsg( STATMSG,"\nUnicast SCM Message Sent\n");
#endif
      				eMsuState = eMsuStateSCM;
#ifdef DEBUG_LEVEL3
      				msuLogMsg( STATMSG,"\nCCM =====>> SCM\n");
#endif
      				eMsuPrevState = eMsuStateCCM;
      				return;
      			}
      		}
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
   UINT16 ChunkSize_local=0;
   UINT8 result=TR_ERROR;
   UINT8 i;
   
   #ifdef DEBUG_LEVEL2
	msuLogMsg( STATMSG,"\nRx Update Mcast pckt CN %ld and SN %d\n",msuMcastPdu.chunkNumber,msuMcastPdu.seqNumber);
	#endif	

   if( localChunkNumber > msuMcastPdu.chunkNumber)
   {
   #ifdef DEBUG_LEVEL0
   	msuLogMsg( STATMSG,"\n MCast CN %ld and SN %d Return\n",
   			msuMcastPdu.chunkNumber, msuMcastPdu.seqNumber);
   #endif			
   	return;
   }
   
   if(msuMcastPdu.chunkNumber==1 && msuMcastPdu.seqNumber==1)
   {
   	ChunkSize=0;   	
   }
  	
   
	if(localChunkNumber != msuMcastPdu.chunkNumber)
	{
	   ccm_seqVal[localChunkNumber -1] = getSeqBitMap( );		
		clearSeqBitMap();
		localChunkNumber = msuMcastPdu.chunkNumber;
		#ifdef DEBUG_LEVEL0
   			msuLogMsg( STATMSG,"\nChunk num not right not right\n");
   		#endif
			
	#ifdef CHUNK_IN_RAM
		msuFileSeek(&pMsuFileDes,(UINT32)( (	(msuMcastPdu.chunkNumber - 1) * 
	        	(msuBroadcastPdu.msuSyncHeaderformat.seqNumberLimit * msuBroadcastPdu.msuSyncHeaderformat.seqSize) )));
		ChunkSize=0;
	#endif
		
	}
	

	
#ifdef CHUNK_IN_RAM 
	
            
 			msuMemcpy(info_array[msuMcastPdu.seqNumber - 1].Info,(UINT8*)msuMcastPdu.pData, msuMcastPdu.dataLength );
 		
 			ChunkSize  += msuMcastPdu.dataLength;
 			
 	
#else	
	if(msuFileSeek(&pMsuFileDes,(UINT32)( ((msuMcastPdu.chunkNumber - 1) * 
		(msuBroadcastPdu.seqNumberLimit * msuBroadcastPdu.seqSize) ) + 
		( (msuMcastPdu.seqNumber -1) * ( msuBroadcastPdu.seqSize) ) ) ) == FALSE)
	{
		#ifdef DEBUG_LEVEL0		
		msuLogMsg( STATMSG,"Fseek fail in Sequence jump over\n");
		#endif			
	
	}

	if( msuFileWrite(pMsuFileDes,(SINT8 *)msuMcastPdu.pData, msuMcastPdu.dataLength) == TR_SUCCESS )
	{

		#ifdef DEBUG_LEVEL0	
		msuLogMsg( STATMSG,"\n MCast CN %ld and SN %d Writing at location%ld\n",msuMcastPdu.chunkNumber,msuMcastPdu.seqNumber,f_tell((FS_FILE*)pMsuFileDes));
		#endif
	}
		
		else
	{
	   MsuGlobalError = MSU_FILE_WRITE_FAILED;
		#ifdef DEBUG_LEVEL0
		msuLogMsg( STATMSG,"\n File write operation Fail\n"); 
		#endif			
	}

#endif		
		/*Set the sequence bitmask*/
		setRecvdSeqBit(msuMcastPdu.seqNumber);
		
		/*Check for chunk end state*/
		if(msuMcastPdu.fileTransferInfo.chunkTransferState == 1)
		{
		   /*End of chunk, check whether we need to go into SCM*/
			missedSeqBitMap = getMissedSeqBitMap(msuMcastPdu.seqNumber);
			if(missedSeqBitMap != 0)
			{	
			#ifdef DEBUG_LEVEL0	
     		msuLogMsg( STATMSG,"\n missedSeqBitMap = %lx\n", missedSeqBitMap ); 
			#endif
			   missedSeqCount = getMissedSeqCount(missedSeqBitMap );			
				if(msuSendScmMsg(missedSeqBitMap,msuMcastPdu.chunkNumber,missedSeqCount) == TRUE)
				{
				    #ifdef DEBUG_LEVEL0
				    msuLogMsg( STATMSG,"\n missedSeqCount = %ld\n", missedSeqCount ); 
								
					msuLogMsg( STATMSG,"\nUnicast SCM Message Sent CN %ld and SN %ld\n",msuMcastPdu.chunkNumber,msuMcastPdu.seqNumber);
					#endif			
					eMsuState = eMsuStateSCM;
					#ifdef DEBUG_LEVEL3				
					msuLogMsg( STATMSG,"\nRUNNING =====>> SCM\n");
					#endif				
					eMsuPrevState = eMsuStateRunning;
					return;
				}//if(msuSendScmMsg(missedSeqBitMap,msuMcastPdu.chunkNumber,1) == TRUE)
			}//if(missedSeqBitMap != 0)
			else
			{
			
			
				#ifdef DEBUG_LEVEL0
				msuLogMsg( STATMSG,"\nMCAST pckt CN %ld and SEQBITS %x\n",msuMcastPdu.chunkNumber,getSeqBitMap( ));
				#endif	
		
		
			#ifdef CHUNK_IN_RAM
		
								result=msuFileWrite(pMsuFileDes,(char*)&info_array,(UINT16)ChunkSize) ;
							
		
					
					
			
						
					
								
									if(result!=TR_SUCCESS)
									{
										
									MsuGlobalError = MSU_FILE_WRITE_FAILED;
								
								#ifdef DEBUG_LEVEL0
									msuLogMsg( STATMSG,"\n File write operation Fail\n"); 
										
										
									msuLogMsg( STATMSG,"\n MCast CN %ld and SN %d Writing at location %ld\n",msuMcastPdu.chunkNumber,msuMcastPdu.seqNumber,f_tell((FS_FILE*)pMsuFileDes));
								#endif									
											
									}
							

						
		//added memset for info array
			
			
				msuMemset(info_array,0,MSU_MAX_SEQ_NUMBER*sizeof(struct MsuInfo));
			
			
				ChunkSize=0;
				
			#endif	
		
				/*set appropriate CN bit mask as received*/
				setRecvdChnkBit(localChunkNumber);				
				localChunkNumber = localChunkNumber + 1;
				
				clearSeqBitMap();			
			}
			
			if(msuMcastPdu.fileTransferInfo.fileTransferState == 1)
			{
				/*Check whether we need to go into CCM mode : This check is performed on the last packet itself
				 * instead of waiting for upgrade complete message*/
				if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
				{	/*All chunks received, proceed to CRC verification*/
					eMsuState = eMsuStateDone;
					#ifdef DEBUG_LEVEL2				
					msuLogMsg( STATMSG,"\nREADY =====>> DONE\n");
					#endif
					#ifdef DEBUG_LEVEL3				
					msuLogMsg( STATMSG,"All chunks received, proceed to CRC verification");
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
						msuLogMsg( STATMSG,"\nREADY =====>> CCM\n");
						#endif			
						eMsuState = eMsuStateCCM;
						/* Make prevState to CCM so SCM can now jump to CCM*/
						eMsuPrevState = eMsuStateCCM;
					}	
				}					
			}//if(msuMcastPdu.fileTransferInfo.fileTransferState == 1)
		}//if(msuMcastPdu.fileTransferInfo.chunkTransferState == 1)	

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
				#ifdef DEBUG_LEVEL2			
				msuLogMsg( STATMSG,"\nMSU Leech Initialized\n");
				#endif			
				eMsuState = eMsuStateConfigure;
				break;

			case  eMsuStateConfigure:
				if (msuDefaultSocketConfig() == MSU_SOCKET_SUCCESS )
					eMsuState = eMsuStateListen;
				break;

			case  eMsuStateListen:
				/*Listen to the sockets until sync message is received*/
				#ifdef DEBUG_LEVEL1			
				msuLogMsg( STATMSG,"\nMsu listening on default socket\n");
				#endif			
				if(msuSocketCheckActivity() == TRUE)
				{
					eMsuState = eMsuStateStart;
				}
				break;

			case eMsuStateStart:
				#ifdef DEBUG_LEVEL3			
				msuLogMsg( STATMSG,"\nConfigurating MSU Leech\n");
				#endif			
				/*Set up the MSU Leech according to the sync message paramaters*/
				if(msuLeechConfigure() == MSU_SOCKET_SUCCESS)
				{
					/*Update the configuration state to configured*/
					eMsuState = eMsuStateRunning;
					#ifdef DEBUG_LEVEL2				
					msuLogMsg( STATMSG,"\nMSU Ready to receive file from server\n");
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
				#ifdef DEBUG_LEVEL0			
				msuLogMsg( STATMSG,"\nAborting file transfer\n");
				#endif			
				eMsuState = eMsuStateLoopBack;
				deviceUpgadeStatus = eMsuUpgradeFailed;
				MsuGlobalError = MsuGlobalError | MSU_UPDATE_STATUS_ABORT; 
				break;

			case eMsuStateDone:
				#ifdef DEBUG_LEVEL1		
				msuLogMsg( STATMSG,"\nClosing file\n");
				#endif	
				/*Close the previously opened file*/
				msuFileClose(pMsuFileDes);				
				if(getMissedChnkBitMap(msuBroadcastPdu.msuSyncHeaderformat.chunkCount) == 0)
				{
					#ifdef DEBUG_LEVEL0			
					msuLogMsg( STATMSG,"\nFile received Successful\n");
					#endif				
               deviceUpgadeStatus = eMsuUpgradePassed;

#ifdef CRC_VALIDATION_REQUIRED			
					/*Perform checksum validation of the file*/
					if(msuPlatformComputeCheckSum(filePath, msuBroadcastPdu.msuSyncHeaderformat.fileChecksum) == TRUE)
					{
						#ifdef DEBUG_LEVEL0			
						msuLogMsg( STATMSG,"\nChecksum Verification Successful\n");
						#endif				
               	deviceUpgadeStatus = eMsuUpgradePassed;
               	UpgadeStatus = eMsuUpgradePassed;
  	
					}
					else
					{
						#ifdef DEBUG_LEVEL0//changed from 1 to 0			
						msuLogMsg( STATMSG,"\n**!!**!!Checksum Verification Failed **!!**!!\n");
						
						#endif				
				   	MsuGlobalError = MSU_UPDATE_FILE_CRC_FAILED;
						deviceUpgadeStatus = eMsuUpgradeFailed;
						/*Discard the file here*/
					}				
#endif				
				}			
				else
				{
					#ifdef DEBUG_LEVEL0			
					msuLogMsg( STATMSG,"\n Failed To Receive Complete File \n");
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
 *   @return    always FALSE because int the state machine TRUE only if data packet
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
			#ifdef DEBUG_LEVEL2			
			msuLogMsg( STATMSG,"\nREADY =====>> CCM\n");
			#endif			
			eMsuState = eMsuStateCCM;
			eMsuPrevState = eMsuStateCCM;
			ccmSent = TRUE;
		}
	}
	else
	{
		eMsuState = eMsuStateDone;
		#ifdef DEBUG_LEVEL2		
		msuLogMsg( STATMSG,"\nREADY =====>> DONE\n");
		#endif
		#ifdef DEBUG_LEVEL3		
		msuLogMsg( STATMSG,"All chunks received, proceed to CRC verification");
		#endif		
   }
	return FALSE;
}
/**  @fn BOOL msuPduHandleCCMCompleteRequest(UINT8 *pMsg , UINT32 msgSize)
 *   @brief msuPduHandleCCMCompleteRequest( ) is used to handle CCM transfer complete request
 *   @details  When client receives CCM transfer complete request, first it checks if CCM retry is set in the request packet, 
 *             if not then it enters into abort state. if CCM retry is set in the request packet then it checks for missed chunks in the 
 *             file, if any then it sends CCM request message to server and enters into CCM state.
 *             If all the chunks received then enters into done state.
 *   @param[in] : UINT8 *pMsg           CCM transfer complete request message
 *   @param[in] : UINT32 msgSize        message size
 *   @return    always FALSE because int the state machine TRUE only if data packet
 *   @retval   returns 1 or 0
  */

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
			#ifdef DEBUG_LEVEL2			
			msuLogMsg( STATMSG,"\nREADY =====>> CCM\n");
			#endif			
			eMsuState = eMsuStateCCM;
		}
	}
	else
	{
		eMsuState = eMsuStateDone;
		#ifdef DEBUG_LEVEL2		
		msuLogMsg( STATMSG,"\nREADY =====>> DONE\n");
		#endif
		#ifdef DEBUG_LEVEL3		
		msuLogMsg( STATMSG,"All chunks received, proceed to CRC verification");
		#endif		
	}
	return FALSE;
}

/**  @fn BOOL msuPduHandleSCMCompleteRequest(UINT8 *pMsg , UINT32 msgSize)
 *   @brief msuPduHandleSCMCompleteRequest( ) is used to handle SCM transfer complete request
 *   @details  When client receives SCM transfer complete request, first it checks if SCM retry is 
 *					set in the request packet, if not then it will not do any thing device will go as
 *					per state machine .
 *					if SCM retry is set in the request packet then it checks for missed sequence of current chunk 
 *             if any then it sends SCM request message to server and enters into SCM state.
            
 *   @param[in] : UINT8 *pMsg           SCM transfer complete request message
 *   @param[in] : UINT32 msgSize        message size
 *   @return    always FALSE because int the state machine TRUE only if data packet
 *   @retval   returns 1 or 0
  */
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
		msuLogMsg( STATMSG,"\n SCM retry 0  In SCM Complete request \n");
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
				msuLogMsg( STATMSG,"\nREADY =====>> SCM from SCM Complete Request \n");
				/*****added*****/
				msuLogMsg( STATMSG,"\n sequence count %d \n",scmRetryCount);
			#endif	
			
			eMsuState = eMsuStateSCM;		
		}
	}
	else
	{
		eMsuState = eMsuPrevState ;
      
		#ifdef DEBUG_LEVEL1		
		msuLogMsg( STATMSG,"\n No Seq missed In SCM Complete request \n");
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

/**  @fn BOOL msuPlatformComputeCheckSum(SINT8 * filePath, UINT32 fileCheckSum)
 *   @brief msuPlatformComputeCheckSum( ) function is used to calculate the checksum
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
	UINT8 fileBuff[1460];   //MSU_FILE_CRC_BUFFER_SIZE
	SINT16 bytesRead = 0;
	UINT32 tempCheckSum = 0;

	SINT32 fileDes = 0;

	if( (filePath != NULL) && (fileCheckSum != 0))
	{
      //Open the file in read only mode
		fileDes = msuFileOpen((char *)filePath, eMsuFileR);
		if( fileDes != -1)
		{
			#ifdef DEBUG_LEVEL2		
		   msuLogMsg( STATMSG,"\nFile opened in read mode\n");
			#endif		   

		   do
		   {
			   //Read from the file
			   bytesRead = (UINT16)msuFileRead(fileDes,(SINT8 *)fileBuff, 1460);	
			   if(bytesRead > 0)
			   {
			      tempCheckSum += crcFast(fileBuff, (Sint16)bytesRead);
			   }

		   }while(bytesRead > 0);
			
			#ifdef DEBUG_LEVEL2
			msuLogMsg( STATMSG,"\nCalculated checksum = %lx\n", tempCheckSum);
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
		{
			MsuGlobalError = MSU_FILE_OPEN_FAILED;
		}
	}

	return result;
}


#endif /*#ifndef MSU_LEECH_CPP */
