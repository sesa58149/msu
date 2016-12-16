/**	@file MsuFilePlatform.cpp
 *	<TABLE>
 *	@brief Implementation of MSU File Platform
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

#ifndef MSU_FILE_PLATFORM_CPP
#define MSU_FILE_PLATFORM_CPP


#include "FileSystemImpl.hpp" 
#include "MsuPlatform.hpp"
#include "MsuFilePlatform.hpp"
#include "MsuCRC32.hpp"
#include "LogMsg.h"
#include <stdio.h>

/**  @fn msuFileOpen(char * filePath, EMsuFileOpenMode eMode)
 *   @brief This function is used to open a file in different modes at specified path
 *   @details  msuFileOpen() function is used to open a file in read mode or write mode or readwrite mode
 *             or append mode. if the file is not availale in write or readwrite or append then it will 
 *             create a new file at specified location
 *   @param[in] : filePath - specified location of the file
 *   @param[in] : eMode - is a enum of EMsuFileOpenMode type, different enum values are  eMsuFileR,
 *                eMsuFileW,eMsuFileRW and  eMsuFileApnd.
 *   @return returns file descriptor if success or -1 if error.
 *           Error code if the operation is failed.
 *   @retval File descriptor of type Sint32, if success
 *   @retval -1 if failed to open file
  */

SINT32 msuFileOpen(char * filePath, EMsuFileOpenMode eMode)
{
	BOOL status = FALSE;
	Uint8 operationMode = 0;
	SINT32 *fds = NULL;

	if(filePath != NULL)
	{
		if(eMode == eMsuFileR)
		{
			operationMode = FILE_OPERATING_TYPE_R;
		}
		else if(eMode == eMsuFileW)
		{
			operationMode = FILE_OPERATING_TYPE_W;
		}
		else if(eMode == eMsuFileRW)
		{
			operationMode = FILE_OPERATING_TYPE_RW;  
		}	
		else if(eMode == eMsuFileApnd)
		{
			operationMode = FILE_OPERATING_TYPE_A;
		}
	
		fds = (SINT32 *)trfOpen(filePath, operationMode);
		if(fds == NULL)
		{
			#ifdef DEBUG_LEVEL3		
    	 	msuLogMsg(DEBUGMSG,"\nOpening file at the path %s with mode %s failed\n",filePath, operationMode);
			#endif
    	 	return (-1);
		}
		else
		{
    	   return ( (SINT32)fds);
		}
	}
	else
	{
		return(-1);
	}
}



/**  @fn msuFileWrite(SINT32 fds, SINT8 * pData, UINT16 size)
 *   @brief This function is used to write data into a file
 *   @details  msuFileWrite() function is used to write a data into a file by using the specified file descriptor.
 *   @param[in] : fds - File descriptor
 *   @param[in] : pData - Data buffer which we need to write into the data
 *   @param[in] : size - size of the data to write
 *   @return returns number of bytes written successfully 
 *           Error code if the operation is failed.
 *   @retval bytes value written into the file successfully
 *   @retval TR_ERROR if failed to write into file
  */

SINT16 msuFileWrite(SINT32 fds, SINT8 * pData, UINT16 size)
{
   Sint16 retVal = TR_SUCCESS;
   SINT32 *ffds  = NULL;

  	ffds = ((SINT32 *)fds);
  	if((pData != NULL) && (ffds != NULL) && (size != 0))
  	{
		//Write the data into the already opened file
	  	retVal = (SINT16)trfWrite((SINT32*)ffds, (UINT8 *)pData, (UINT32)size);
	  return (retVal);
	}
  	else
  	{
  		return(TR_ERROR);
  	}
}

/**  @fn msuFileRead(SINT32 fds, SINT8 * pBuf, UINT16 size)
 *   @brief This function is used to read data into a file
 *   @details  msuFileRead() function is used to read a data into a file by using the specified file descriptor.
 *   @param[in] : fds - File descriptor
 *   @param[in] : pBuf - empty data buffer used to receive data
 *   @param[in] : size - size of the data to read
 *   @return returns number of bytes read successfully 
 *           Error code if the operation is failed.
 *   @retval bytes value read from the file successfully
 *   @retval TR_ERROR if failed to write into file
  */

SINT16 msuFileRead(SINT32 fds, SINT8 * pBuf, UINT16 size)
{
   Sint16 retVal = 0;
   Sint32 *ffds = NULL;

   ffds = ((SINT32 *)fds);
	if((pBuf != NULL) && (ffds != NULL) && (size != 0))
  	{
      retVal = (SINT16)trfRead((SINT32 *)ffds, (UINT8 *)pBuf, (UINT32)size);
	   return(retVal);
		#ifdef DEBUG_LEVEL3	  
	  	msuLogMsg( ERRMSG,"\nRead operation: read %d bytes of data\n", retVal);
		#endif	  
  	}
  	else
  	{
  		return(TR_ERROR);
  	}
}



/**  @fn msuFileSeek(SINT32 *fds, UINT32 position)
 *   @brief This function is used to move the file pointer to the specified position
 *   @details  msuFileRead() function is used to move the file pointer to the specified location from the 
 *             starting position of the file.  
 *   @param[in] : fds - File descriptor, updates the descriptor if moved successfully
 *   @param[in] : position - number of bytes want to move from the file starting position
 *   @return returns true if file seek successful 
 *           Error code if the operation is failed.
 *   @retval TRUE if file seek successful
 *   @retval FALSE if failed to seek
  */

BOOL msuFileSeek(SINT32 *fds, UINT32 position)
{
   BOOL result = FALSE;
	SINT32 *ffds = NULL;

	ffds = ((SINT32 *)(*fds));

	#ifdef DEBUG_LEVEL3
   msuLogMsg( ERRMSG,"\nSeeking to position %ld\n", position);
	#endif    

	if(ffds != NULL)
	{
   	if(trfSeek((SINT32 *)ffds, (long)position, (long)MSU_FILE_SEEK_START) == -1)
      {
			#ifdef DEBUG_LEVEL2     
      	msuLogMsg( ERRMSG,"\nSeek operation failed\n" );
			#endif 
       	return(FALSE);
     	}
     	else
     	{
			#ifdef DEBUG_LEVEL2     
       	msuLogMsg( ERRMSG,"\nSeek operation Successful\n");
			#endif       
	      
	      *fds = (SINT32)ffds;
	      result = TRUE;
     	}
	}
	return result;
}





/**  @fn msuFileClose(SINT32 fds )
 *   @brief This function is used to close the file 
 *   @details  This function is used to close specified file 
 *   @param[in] : fds - file descriptor
 *   @return returns 0 if file closed successfully or -1
 *   @retval 0 if  successful
 *   @retval -1 if failed 
  */

SINT16 msuFileClose(SINT32 fds )
{
   SINT16 retVal = 0;
   SINT32 *ffds = NULL;

   ffds = ((SINT32 *)fds);
   if(ffds != NULL)
   {
      retVal = (SINT16)trfClose((Sint32*)ffds);
		#ifdef DEBUG_LEVEL2      
      msuLogMsg( ERRMSG,"\nFile close Error Code= %d\n", retVal);
		#endif      
      return(retVal);
   }
   else
   {
   	return(-1);
   }
}


#endif/*#ifndef MSU_FILE_PLATFORM_CPP */
