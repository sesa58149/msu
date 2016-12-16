/**	@file MsuFilePlatform.hpp
 *	<TABLE>
 *	@brief Implementation of MSU File Platform header
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

#ifndef MSUFILEPLATFORM_H
#define MSUFILEPLATFORM_H

#include "MsuPlatform.h"

#define MSU_FILE  FILE

#define MSU_MAX_FILE_PATH_SIZE (MSU_MAX_TAG_SIZE * 2 + 1)
#define MSU_FILE_CRC_BUFFER_SIZE 255
#define MSU_MAX_FILE_MODE_LENGTH 2

#define MSU_FILE_READ "r"
#define MSU_FILE_WRITE "w"
#define MSU_FILE_READ_WRITE "r+"
#define MSU_FILE_WRITE_CREATE "w+"
#define MSU_FILE_APPEND "a"
#define MSU_FILE_READ_APPEND "a+"


/**  @def MSU_FILE_SEEK_START 
*    @brief Macro to be defined for file reading from Start
*/
#define MSU_FILE_SEEK_START SEEK_SET

/**  @def MSU_FILE_SEEK_CURRENT 
*    @brief Macro to be defined for file reading from current position
*/
#define MSU_FILE_SEEK_CURRENT SEEK_CUR

#ifdef LINUX_SIMULATOR
	/**  @def MSU_FILE_SEEK_END
	*    @brief Macro to be defined for file reading from end of the file position
	*/
	#define MSU_FILE_SEEK_END SEEK_END
	#define MSU_FILE_DEFAULT_PATH "/home"
	#define MSU_FILE_DEFAULT_NAME "MsuFile.txt"
	#define MSU_DEVICE_INFO_FILE_NAME "/home/msu.bin"
#else
	/**  @def MSU_SOCKET_ERROR
	*    @brief Macro to be defined for Error value (-1)
	*/
	#define MSU_FILE_DEFAULT_PATH "flash0:/fw/"

	/**  @def MSU_SOCKET_ERROR
	*    @brief Macro to be defined for Error value (-1)
	*/
	#define MSU_FILE_DEFAULT_NAME "MsuFile.txt"

	/**  @def MSU_SOCKET_ERROR
	*    @brief Macro to be defined for Error value (-1)
	*/
	#define MSU_DEVICE_INFO_FILE_NAME "flash0:/fw/msu.bin"
#endif

/*   @struct msuSockinfo
*    @brief Structure defining UINT32 IP Address and UINT16 port number
*    @details Defines structure with two variables, one for the 32 bit IP Address 
*             and other for 16 bit port number
*/
typedef enum
{
 MSU_FILE_R = 0,
 MSU_FILE_W = 1,
 MSU_FILE_RW = 2,
 MSU_FILE_RWC = 3,
 MSU_FILE_APND = 4,
 MSU_FILE_APNDC = 5,
}MSU_FILE_OPEN_MODE;

/**  @fn setTimeoutVal( Sint32 socketfd, UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 

SINT32 msuFileOpen(SINT8 * filePath, MSU_FILE_OPEN_MODE  mode);

/**  @fn setTimeoutVal( Sint32 socketfd, UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
SINT16 msuFileWrite(SINT32 fds, SINT8 * pData, UINT16 size);


/**  @fn setTimeoutVal( Sint32 socketfd, UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
SINT16 msuFileRead(SINT32 fds, SINT8 * pBuf, UINT16 size);


/**  @fn setTimeoutVal( Sint32 socketfd, UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
BOOL msuFileSeek( SINT32 *fds, UINT32 position);


/**  @fn setTimeoutVal( Sint32 socketfd, UINT8 timeout_sec )
*    @brief Stores the timeout value, which will be used by select system call
*/ 
SINT16 msuFileClose(SINT32 fds);



#endif/*#ifndef MSUFILEPLATFORMHPP */



