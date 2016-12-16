/**	@file MsuErrNo.h
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
 *	@author Rajinikanth Reddy
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

#ifndef _MSU_ERRNO_H__
#define _MSU_ERRNO_H__



/**  @def MSU_FILE_OPEN_FAILED 
*    @brief This macro defines MSU file open failed Error value 1
*/
#define MSU_FILE_OPEN_FAILED            1

/**  @def MSU_UPDATE_SOCKET_OPEN_FAILED 
*    @brief Thos Macro defines MSU Update socket creation failed Error value 2
*/
#define MSU_UPDATE_SOCKET_OPEN_FAILED   2

/**  @def MSU_UPDATE_SOCKET_RX_FAILED 
*    @brief This Macro defines Reading data from MSU Update Socket failed value 3
*/
#define MSU_UPDATE_SOCKET_RX_FAILED     3

/**  @def MSU_UPDATE_SOCKET_TX_FAILED 
*    @brief This Macro defines Transmitting data from MSU Update Socket failed value 4
*/
#define MSU_UPDATE_SOCKET_TX_FAILED     4

/**  @def MSU_UPDATE_SCM_FAILED 
*    @brief This Macro defines Client data Upgrade failed in Sequence complain mode Error value is 5
*/
#define MSU_UPDATE_SCM_FAILED           5

/**  @def MSU_UPDATE_CCM_FAILED 
*    @brief This Macro defines Client data Upgrade failed in Chunk complain mode Error value is 6
*/
#define MSU_UPDATE_CCM_FAILED           6  

/**  @def MSU_UPDATE_TIMEOUT 
*    @brief This Macro defines timeout happend on Upgrade process Error value is 7
*/
#define MSU_UPDATE_TIMEOUT              7 

/**  @def MSU_FILE_READ_FAILED 
*    @brief This macro defines File read failed Error value is 8
*/
#define MSU_FILE_READ_FAILED    			 8

/**  @def MSU_FILE_WRITE_FAILED 
*    @brief This Macro defines File write failed Error value is 9
*/
#define MSU_FILE_WRITE_FAILED           9

/**  @def MSU_UPDATE_FILE_CRC_FAILED 
*    @brief This Macro defines File CRC failed Error value is 10
*/
#define MSU_UPDATE_FILE_CRC_FAILED      10 

/**  @def MSU_UPDATE_EXCEED_CCM_RETRY 
*    @brief Macro to be defines Exceed CCM retry failed Error value is 11
*/
#define MSU_UPDATE_EXCEED_CCM_RETRY     11

/**  @def MSU_UPDATE_STATUS_ABORT 
*    @brief This Macro defines Status Abort Error value is 0x80
*/
#define MSU_UPDATE_STATUS_ABORT         0x80  

/**  @def MSU_UPDATE_STATUS_FILE_VALUE_NULL 
*    @brief This Macro defines Status File Value Null Error value 
*/
#define MSU_UPDATE_STATUS_FILE_VALUE_NULL   12  

/**  @def MSU_UPDATE_STATUS_FILE_SIZE_ZERO 
*    @brief This Macro defines Status File Size Zero 
*/
#define MSU_UPDATE_STATUS_FILE_SIZE_ZERO   13  

/**  @def MSU_UPDATE_STATUS_CHUNKCOUNT_ZERO 
*    @brief This Macro defines Status File Size Zero 
*/
#define MSU_UPDATE_STATUS_CHUNKCOUNT_ZERO   14

/**  @def MSU_UPDATE_STATUS_SEQLIMIT_ZERO 
*    @brief This Macro defines Sequence Number limit Zero error 
*/
#define MSU_UPDATE_STATUS_SEQLIMIT_ZERO   15

/**  @def MSU_UPDATE_STATUS_SEQSIZE_ZERO 
*    @brief This Macro defines Sequence size Zero error 
*/
#define MSU_UPDATE_STATUS_SEQSIZE_ZERO   16

/**  @def MSU_UPDATE_STATUS_SERVER_IP_ZERO 
*    @brief This Macro defines Server IP size Zero error 
*/
#define MSU_UPDATE_STATUS_SERVER_IP_ZERO   17


/**  @def MSU_UPDATE_STATUS_SERVER_CM_IP_ZERO 
*    @brief This Macro defines Server complain mode IP size Zero error 
*/
#define MSU_UPDATE_STATUS_SERVER_CM_IP_ZERO   18

/**  @def MSU_UPDATE_STATUS_PORT_ZERO 
*    @brief This Macro defines multicast port size Zero error 
*/
#define MSU_UPDATE_STATUS_PORT_ZERO   19

/**  @def MSU_UPDATE_STATUS_CM_PORT_ZERO 
*    @brief This Macro defines complain mode multicast port size Zero error 
*/
#define MSU_UPDATE_STATUS_CM_PORT_ZERO   20

/**  @def MSU_UPDATE_STATUS_TRANSACTIONID_ZERO 
*    @brief This Macro defines transaction id  size Zero error 
*/
#define MSU_UPDATE_STATUS_TRANSACTIONID_ZERO    21

/**  @def MSU_UPDATE_STATUS_CRC_ZERO 
*    @brief This Macro defines transaction id  size Zero error 
*/
#define MSU_UPDATE_STATUS_CRC_ZERO    22

/**  @def MSU_UPDATE_STATUS_CHUNKCOUNT_ZERO 
*    @brief This Macro defines Status File Size Zero 
*/
#define MSU_UPDATE_STATUS_SEQLIMIT_EXCEEDED   23

/**  @def MSU_UPDATE_STATUS_SEQSIZE_ZERO 
*    @brief This Macro defines Sequence size Zero error 
*/
#define MSU_UPDATE_STATUS_SEQSIZE_EXCEEDED   24

/*   @struct MSU_AUTHENTICATION_ERRCODE
*    @brief Macro defining MSU error code for authenitcation response 
*/

#define MSU_AUTHENTICATION_ERRCODE 1

/*   @struct MSU_AUTHENTICATION_ERRSUBCODE_IP
*    @brief Macro defining MSU error sub code for server ip mismatch with socketip address 
*/

#define MSU_AUTHENTICATION_ERRSUBCODE_IP 2


/*   @struct MSU_AUTHENTICATION_ERRSUBCODE_LOGININFO
*    @brief Macro defining MSU error sub code for login info mismatch 
*/

#define MSU_AUTHENTICATION_ERRSUBCODE_LOGININFO 3

/*   @struct MSU_AUTHENTICATION_ERRSUBCODE_FILENAME
*    @brief Macro defining MSU error sub code for file name mismatch 
*/
#define MSU_AUTHENTICATION_ERRSUBCODE_FILENAME 4

/*   @struct MSU_AUTHENTICATION_ERRSUBCODE_HARDWAREID
*    @brief Macro defining MSU error sub code for file name mismatch 
*/

#define MSU_AUTHENTICATION_ERRSUBCODE_HARDWAREID 5

#endif
