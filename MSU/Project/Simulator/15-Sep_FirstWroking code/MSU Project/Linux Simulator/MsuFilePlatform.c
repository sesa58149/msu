#ifndef __MSU_FILE_PLATFORM_C__
#define __MSU_FILE_PLATFORM_C__

#include <stdio.h>
#include <string.h>
#include "MsuFilePlatform.h"
#include "MsuCRC32.h"
#include <errno.h>


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



SINT32 msuFileOpen(SINT8 *filePath, MSU_FILE_OPEN_MODE mode)
{
 BOOL status = FALSE;
 SINT8 fileMode[MSU_MAX_FILE_MODE_LENGTH] = {0};

 MSU_FILE *fds = NULL;

 if(filePath != NULL)
 {
	 if(mode == MSU_FILE_R)
	 {
		 memcpy(fileMode, MSU_FILE_READ, 1);
	 }
	 else if(mode == MSU_FILE_W)
	 {
		 memcpy(fileMode, MSU_FILE_WRITE, 1);
	 }
	 else if(mode == MSU_FILE_RW)
	 {
		 memcpy(fileMode, MSU_FILE_READ_WRITE, MSU_MAX_FILE_MODE_LENGTH);
	 }
	 else if(mode == MSU_FILE_RWC)
	 {
		 memcpy(fileMode, MSU_FILE_WRITE_CREATE, MSU_MAX_FILE_MODE_LENGTH);
	 }
	 else if(mode == MSU_FILE_APND)
	 {
		 memcpy(fileMode, MSU_FILE_APPEND, 1);
	 }
	 else if(mode == MSU_FILE_APNDC)
	 {
		 memcpy(fileMode, MSU_FILE_READ_APPEND, MSU_MAX_FILE_MODE_LENGTH);
	 }

	 //Open the file at the given location
	 fds = fopen(filePath, (SINT8 *) fileMode);
     if(fds == NULL)
     {
#ifdef Debug_level1
    	 printf("\nOpening file at the path %s with mode %s failed\n",filePath, fileMode);
#endif
    	 return (-1);
     }
     else
    	 return ( (SINT32)fds);
 }
 else
	 return(-1);

 return status;
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
  UINT16 retVal = 0;

  MSU_FILE *ffds = NULL;

  ffds = ((MSU_FILE *)fds);

  if((pData != NULL) && (ffds != NULL) && (size != 0))
  {
	  //Write the data into the already opened file
	  retVal = fwrite((SINT8 *)pData, sizeof(SINT8), size, ffds);

	  return (retVal);
#ifdef Debug_level1
	  printf("\nWrite operation: written %d bytes of data, FP = %ld\n", retVal, ftell(ffds));
#endif
  }
  else
	  return(TR_ERROR);
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
  UINT16 retVal = 0;
  MSU_FILE *ffds = NULL;

  ffds = ((MSU_FILE *)fds);


  if((pBuf != NULL) && (ffds != NULL) && (size != 0))
  {
	  //Write the data into the already opened file
	  retVal = fread((SINT8 *)pBuf, sizeof(SINT8), size, ffds);

	  return(retVal);
#ifdef Debug_level1
	  printf("\nRead operation: read %d bytes of data\n", retVal);
#endif
  }
  else
	  return(TR_ERROR);
}

BOOL msuFileSeek(SINT32 *fds, UINT32 position)
{
	BOOL result = FALSE;
	MSU_FILE *ffds = NULL;

	ffds = ((MSU_FILE *)(*fds));
#ifdef Debug_level1
    printf("\nSeeking to position %ld\n", position);
#endif

	if(ffds != NULL)
	{
     if(fseek(ffds, position, MSU_FILE_SEEK_START) == -1)
     {
#ifdef Debug_level1
       printf("\nSeek operation failed with errror: %d\n", errno);
#endif
       return(-1);
     }
     else
     {
#ifdef Debug_level1
       printf("\nSeek operation Successful\n");
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
   MSU_FILE *ffds = NULL;

   ffds = ((MSU_FILE *)fds);


   if(ffds != NULL)
   {
      retVal = fclose(ffds);
      return(retVal);
   }
   else
      return(-1);
}


#endif/*#ifndef MSU_FILE_PLATFORM_CPP */
