/**	@file complainMode.h
 *	<TABLE>
 *	@brief Implementation of MSU complainMode header
 *
 *	@copy
 *	<TABLE>
 *	<TR><TD> Copyright </TD></TR>
 *	<TR>
 *	<TD>Schneider Electric India (Pvt) Ltd.
 *	Copyright (c) 2011 - All rights reserved.
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
 *	<TD>Author</TD>			<TD>Date</TD>			<TD>Description</TD>
 *	</TR>
 *	<TR>
 *	<TD>Deepak G</TD>	<TD>03-Oct-2011</TD>	<TD>First creation</TD>
 *	</TR>
 *
 *	</TABLE>
 *	</TABLE>
 */

#ifndef _MSU_COMPLAIN_H__
#define _MSU_COMPLAIN_H__





#include "MsuPlatform.h"

/**  @def MSU_MAX_NUM_OF_SEQ 
*    @brief Macro to be defined for Max number of sequences in each chunk
*/
#define MSU_MAX_NUM_OF_SEQ 32

/**  @def MAX_NUM_CHNKS 
*    @brief Macro to be defined for Max number of chunks value 128
*/
#define MAX_NUM_CHNKS 128

/**  @def MIN_NUM_CHNKS 
*    @brief Macro to be defined for Min Chunk number value 1
*/
#define MIN_NUM_CHNKS 1

/**  @def MAX_NUM_CNUKS_SUPPPORTED 
*    @brief Macro to be defined for Max number of chunks supported value 3968
*/
#define MAX_NUM_CHUNKS_SUPPORTED 3968


/**  @fn void setRecvdSeqBitMap(UINT8 index)
*    @brief This function is used to set the received sequence bit map 
*/ 
void setRecvdSeqBit(UINT8 index);


/**  @fn UINT32 getMissedSeqBitMap(UINT8 numOfSeqns);
*    @brief This function is used to get the missed sequence bit maps in the current chunk
*/ 
UINT32 getMissedSeqBitMap(UINT8 numOfSeqns);


/**  @fn void clearSeqBitMap(void)
*    @brief Create a socket with specified port number and register with specified multicast IP Address
*/ 
void clearSeqBitMap(void);

/**  @fn BOOL isSeqBitMapSet(UINT8 seqNum)
*    @brief This function is used to check for particulat sequence number is already received or not
*/ 
BOOL isSeqRecvd(UINT8 seqNum);

/**  @fn void complainModeInit(void)
*    @brief This function is used to initialize complain mode global variables
*/ 
void complainModeInit(void);

/**  @fn BOOL setRecvdChnkBit(UINT32 chnkNum)
*    @brief This function is used to set the chunk bit for the particular chunk number
*/ 
BOOL setRecvdChnkBit(UINT32 chnkNum);

/**  @fn mBOOL clrRecvdChnkBit(UINT32 chnkNum)
*    @brief This function is used to clear the particular chunk number bit
*/ 
BOOL clrRecvdChnkBit(UINT32 chnkNum);

/**  @fn void clrRecvdChnkBitMap(void)
*    @brief This function is used to clear entire chunk bit map arrey
*/ 
void clrRecvdChnkBitMap(void);

/**  @fn BOOL isChnRecvd(UINT32 chnkNum)
*    @brief This function is used to check particulat chunk number is received or not
*/ 
BOOL isChnkRecvd(UINT32 chnkNum);

/**  @fn UINT32 getMissedChnkBitMap(UINT32 totalChnksExpected)
*    @brief This function is used to return total number of missed chunks in the bit map arrey
*/ 
UINT32 getMissedChnkBitMap(UINT32 totalChnksExpected);

/**  @fn UINT32* getMissedChnkNumsArry(UINT32 totalChnksExpected,UINT32 *totalNumOfChnksMissed  )
*    @brief This function returns missed chunk numbers arrey pointer, which points to missed chunk numbers
*/ 
UINT32* getMissedChnkNumsArry(UINT32 totalChnksExpected,UINT32 *totalNumOfChnksMissed  );

/**  @fn void clrMissedChnkBitMap(void)
*    @brief This function is used to clear missed chunk bitmap arrey
*/ 
void clrMissedChnkBitMap(void);

/**  @fn UINT32 getSeqBitMap( )
*    @brief This function is used to get the current chunk sequence bit map value
*/ 
UINT32 getSeqBitMap( );

/**  @fn void setSeqBitMap(UINT32 seqbitmapVal )
*    @brief This function is used to set the current chunk sequence bit map value
*/ 
void setSeqBitMap(UINT32 seqbitmapVal );


UINT8 getMissedSeqCount(UINT32 seqBitmapVal );



#endif //_MSU_COMPLAIN_H__



