/**	@file complainMode.cpp
 *	<TABLE>
 *	@brief Implementation of MSU Complain Mode
 *
 *	@copy
 *	<TABLE>
 *	<TR><TD> Copyright </TD></TR>
 *	<TR>
 *	<TD>Schneider Electric India (Pvt) Ltd.
 *	Copyright (c) 2009 - All rights reserved.
 * India Patent Application Number:1448/CHE/2010
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
 
#ifndef _MSU_COMPLAIN_CPP__
#define _MSU_COMPLAIN_CPP__


#include "limits.h"
#include "MsuComplainMode.hpp"
#include <stdio.h>
#include <string.h>
#include "LogMsg.h"


UINT32 missedSeqBitMap = 0;
static UINT32 recvdSeqBitMap = 0;
static UINT32 recvdChnkArr[MAX_NUM_CHNKS];
static UINT32 missedChnkArr[MAX_NUM_CHNKS];
static UINT32 missedChnkNums[MAX_NUM_CHNKS];

#define SEQUENCE_MASK 0xFFFFFFFF
#define CHUNK_NUM_DIV 32

/**  @fn void complainModeInit(void)
 *   @brief This function is used to initialize complain mode global variables
 *   @details  This function is used to initialize reeived chunk arrey, missed chunk arrey and
 *             missed chunk numbers arrey's with 0 
 *   @param[in] : None
 *   @return    : None
 */
void complainModeInit(void)
{
   missedSeqBitMap = 0;
	recvdSeqBitMap = 0;

	memset(recvdChnkArr, 0, sizeof(UINT32) * MAX_NUM_CHNKS);
	memset(missedChnkArr, 0, sizeof(UINT32) * MAX_NUM_CHNKS);
   memset(missedChnkNums, 0, sizeof(UINT32) * MAX_NUM_CHNKS);
  
}


/**  @fn BOOL setRecvdChnkBit(UINT32 chnkNum)
 *   @brief This function is used to set the chunk bit for the particular chunk number
 *   @details  MSU server divides the file into number of chunks, each chunk contains n number sequences,
 *             when all the sequences received in the partucular chunk , we set the particular chunk number
 *             in chunk bit arrey
 *   @param[in] : Chunk Number
 *   @return True if the operation is successful
 *           False if the operation is failed.
 *   @retval 1 if chunk bit set successfully
 *   @retval 0 if failed to set chunk bit 
 */

BOOL setRecvdChnkBit(UINT32 chnkNum)
{
   UINT16 index = 0;
   BOOL retVal = 1;

 	if( (MIN_NUM_CHNKS > chnkNum) || (chnkNum > MAX_NUM_CHUNKS_SUPPPORTED) )
 	{
		#ifdef DEBUG_LEVEL3 
		LogMsg( ERRMSG,"\nOut of proper chunk range\n");
		#endif	 
	 	return 0; 
 	}

 	index = (UINT16)((chnkNum -1)/32);

 	recvdChnkArr[index] |=  (0x01 << (((chnkNum-1)%32) ));
	#ifdef DEBUG_LEVEL2
	LogMsg( ERRMSG,"\nSet Recvd Chunk Bit for CN %ld\n", chnkNum);
	#endif 

 	return retVal;
}



/**  @fn UINT32* getMissedChnkNumsArry(UINT32 totalChnksExpected,UINT32 *totalNumOfChnksMissed )
 *   @brief This function returns missed chunk numbers arrey pointer, which points to missed chunk numbers
 *   @details   Msu client once receive the chunks from server, it checks all the chunks recieved or not,
 *              if any chunks are missed then client uses this function to get the missed chunk numbers and
 *              sends message to server to get the missed chunk packets from server 
 *   @param[in] : UINT32 totalChnksExpected        total number of chunks sent from server      
 *   @param[in] : UINT32 *totalNumOfChnksMissed    total number of chunks missed in client updated by function
 *   @return    Uint32* pointer which points to missed chunk numbers arrey
 *   @retval    pointer to UINT32* arrey
 */
UINT32* getMissedChnkNumsArry(UINT32 totalChnksExpected,UINT32 *totalNumOfChnksMissed )
{
   UINT32 chnkNum;
	UINT32 numofMissedChnks = 0;
	UINT16 i = 0;

	numofMissedChnks = getMissedChnkBitMap(totalChnksExpected);
	*totalNumOfChnksMissed = numofMissedChnks;

	for( chnkNum = 1; chnkNum <= totalChnksExpected;  chnkNum++ )
	{
		if ( isChnkRecvd(chnkNum) == 0)
		{
			missedChnkNums[i] = trHtonl(chnkNum);
			#ifdef DEBUG_LEVEL2				
			LogMsg( ERRMSG,"\nCN missed = %ld\n", chnkNum);
			#endif				
			i++;
		}
	}
	return missedChnkNums;
}



/**  @fn UINT32* UINT32 getMissedChnkBitMap(UINT32 totalChnksExpected)
 *   @brief This function is used to return total number of missed chunks in the bit map arrey
 *   @details   This f'n checks all the chunks recieved or not, if any chunks are missed in the client, 
 *              this function returns the missed chunks count 
 *   @param[in] : UINT32 totalChnksExpected        total number of chunks sent from server      
 *   @return    Uint32   missed chunk numbers count
 */
UINT32 getMissedChnkBitMap(UINT32 totalChnksExpected)
{
	UINT16 chnkNum ;
	UINT16 index = 0;
	UINT32 numofMissedChnks =0;

	for( chnkNum = 1; chnkNum <= totalChnksExpected;  chnkNum++ )
	{
		if ( isChnkRecvd(chnkNum) == 0)
		{
			index = (UINT16)((chnkNum-1)/32 );
			missedChnkArr[index] |=  ( 1 << ((chnkNum-1)%32));
			numofMissedChnks++;
		}
	}
	#ifdef DEBUG_LEVEL2
	LogMsg( ERRMSG,"\nMissed chunk count = %ld\n", numofMissedChnks);
	#endif	

	return numofMissedChnks;
}


/**  @fn void clrRecvdChnkBitMap(void)
 *   @brief This function is used to clear entire chunk bit map arrey
 *   @param[in] : None
 *   @return    : None
 */
void clrRecvdChnkBitMap(void)
{
	memset (recvdChnkArr,0,sizeof(recvdChnkArr));
}


/**  @fn void clrMissedChnkBitMap(void)
 *   @brief This function is used to clear missed chunk bit map arrey
 *   @param[in] : None
 *   @return    : None
 */
void clrMissedChnkBitMap(void)
{
	memset (missedChnkArr,0,sizeof(missedChnkArr));
}


/**  @fn BOOL isChnRecvd(UINT32 chnkNum)
 *   @brief This function is used to check particulat chunk number is received or not
 *   @param[in] : UINT32 Chunk Number
 *   @return    : TRUE if particular chunk received bit set in the received chunk arrey
 *                FALSE if particular chunk received bit is not set in the received chunk arrey 
 *   @retval    : 1 if success, 0 if failure

 */
BOOL isChnkRecvd(UINT32 chnkNum)
{
   UINT16 index = 0;
   UINT32 mask = 0;
   BOOL retVal = 0;

 	if( (MIN_NUM_CHNKS > chnkNum) || (chnkNum > MAX_NUM_CHUNKS_SUPPPORTED) )
	   return retVal; 

 	index = (UINT16)( (chnkNum -1)/CHUNK_NUM_DIV );
 	mask = 1ul << ((chnkNum-1)%CHUNK_NUM_DIV);
 	if(recvdChnkArr[index] & mask)
 	{
		 retVal = 1;
 	}
 	return retVal;
}


/**  @fn BOOL clrRecvdChnkBit(UINT32 chnkNum)
 *   @brief This function is used to clear particulat chunk number in the received chunk bit arrey
 *   @param[in] : UINT32 Chunk Number
 *   @return    : TRUE if particular chunk received bit is cleared in the received chunk arrey
 *                FALSE if particular chunk received bit is not cleared in the received chunk arrey 
 *   @retval    : 1 if success, 0 if failure

 */
BOOL clrRecvdChnkBit(UINT32 chnkNum)
{
   UINT16 index = 0;
   BOOL retVal = 1;

 	if( (MIN_NUM_CHNKS > chnkNum) || (chnkNum > MAX_NUM_CHUNKS_SUPPPORTED) )
		return 0; 

 	index = (UINT16) ((chnkNum -1)/CHUNK_NUM_DIV);
 	recvdChnkArr[index] &=  ~(0x01 << ((chnkNum - 1)%CHUNK_NUM_DIV));
 	return retVal;
}





/**  @fn void setRecvdSeqBitMap(UINT8 seqNum)
 *   @brief This function is used to set particul sequence number bit in the received sequence bit map arrey
 *   @param[in] : UINT8 Sequence Number
 *   @return    : None
 *   @retval    : None
 */
void setRecvdSeqBit(UINT8 seqNum)
{
   UINT8 offSet = 32;

	recvdSeqBitMap |= (UINT32) (1L << (offSet -seqNum ));
	#ifdef DEBUG_LEVEL3
	LogMsg( ERRMSG,"\n!!!! Set Bit Called for SN %d and Rx Bit Map = %lx\n",seqNum,  recvdSeqBitMap);
	#endif	
}


/**  @fn UINT32 getSeqBitMap( )
 *   @brief This function is used to get the current chunk sequence bit map value
 *   @param[in] : None
 *   @return    : UINT32 received sequence bit map value
 *   @retval    : UINT32
 */
UINT32 getSeqBitMap( )
{
   return recvdSeqBitMap;

}


/**  @fn void setSeqBitMap(UINT32 seqbitmapVal )
 *   @brief This function is used to set the current chunk sequence bit map value
 *   @param[in] : UINT32 seqbitmapVal     sequence bit map value  
 *   @return    : None
 *   @retval    : None
 */
void setSeqBitMap(UINT32 seqbitmapVal )
{
   recvdSeqBitMap = seqbitmapVal;

}



/**  @fn UINT32 getMissedSeqBitMap(UINT8 numOfSeqns)
 *   @brief This function is used to get the current chunk missed sequence bit map value
 *   @param[in] : UINT8 numOfSeqns      total number of sequences in each chunk value       
 *   @return    : missed sequence bit map value
 *   @retval    : UINT32
 */

UINT32 getMissedSeqBitMap(UINT8 numOfSeqns)
{
   static UINT32 tempMissed = 0;
	UINT8 index=0 ;
	UINT32 mask = 0;

	for(index = 1; index <= numOfSeqns; index++ )
	{
		mask |= 1<<(32-index);
	}
	if((recvdSeqBitMap & mask) != 0)
	{
		tempMissed = (recvdSeqBitMap ^ SEQUENCE_MASK);
		tempMissed &= mask;
	}
	#ifdef DEBUG_LEVEL3
   LogMsg( ERRMSG,"\nMissed sequence bitmap = %lx\n",(UINT32) trNtoHl(tempMissed));
	#endif    
	return tempMissed;
}


/**  @fn BOOL isSeqBitMapSet(UINT8 seqNum)
 *   @brief This function is used to verify is particular sequence number is received or not
 *   @param[in] :UINT8 seqNum    sequence number      
 *   @return    : TRUE, if bit map set for passed sequence number
                  FALSE, if bit map not set for passed sequence number  
 *   @retval    : UINT32
 */

BOOL isSeqRecvd(UINT8 seqNum)
{
   BOOL result = FALSE;

	#ifdef DEBUG_LEVEL3
 	LogMsg( ERRMSG,"\n****Is Set Called for SN %d and Rx Bit Map = %lx\n",seqNum,  recvdSeqBitMap);
	#endif 

 	if(recvdSeqBitMap & (((UINT32)1L) << (32 - seqNum)) )
 	{
		result = TRUE;
 	}
 	return result;
}



/**  @fn void clearSeqBitMap(void)
 *   @brief This function is used to clear the current chunk sequence bit map value to 0
 *   @param[in] : None
 *   @return    : None
 *   @retval    : None
 */
void clearSeqBitMap(void)
{
	recvdSeqBitMap = 0;
}


UINT8 getMissedSeqCount(UINT32 seqBitmapVal )
{
	UINT8 ctr=0; 
	for( ; seqBitmapVal!=0; seqBitmapVal>>=1 )
	{ 
		if( seqBitmapVal & 1 ) 
		{
			 ctr++;
		}
	}
	return (ctr);
}


#endif //_MSU_COMPLAIN_CPP__
