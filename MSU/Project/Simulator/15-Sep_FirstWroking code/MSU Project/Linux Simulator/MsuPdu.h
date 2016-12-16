/**	@file MsuPdu.hpp
 *	<TABLE>
 *	@brief Implementation of MSU PDU header
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

#ifndef MSUPDU_H
#define MSUPDU_H

#include "MsuPlatform.h"
#include "MsuFilePlatform.h"
#include "MsuSocketInterface.h"

/*   @struct MSU_OPCODE_UPGRADE
*    @brief Macro defining MSU Opcode for UPGRADE 1
*/
#define MSU_OPCODE_UPGRADE 1

/*   @struct MSU_OPCODE_DEVICE_DISCOVER
*    @brief Macro defining MSU Opcode for DEVICE_DISCOVER 2
*/
#define MSU_OPCODE_DEVICE_DISCOVER 2

/*   @struct MSU_OPCODE_DEVICE_GROUP
*    @brief Macro defining MSU Opcode for DEVICE_GROUP 3
*/
#define MSU_OPCODE_DEVICE_GROUP 3

/*   @struct MSU_OPCODE_USER_CMD
*    @brief Macro defining MSU Opcode for USER_CMD 4
*/
#define MSU_OPCODE_USER_CMD 4

/*   @struct MSU_SUBCODE_UPGRADE
*    @brief Macro defining MSU Sub code for UPGRADE 1
*/
#define MSU_SUBCODE_UPGRADE 1

/*   @struct MSU_SUBCODE_DOWNGRADE
*    @brief Macro defining MSU Sub code for DOWNGRADE 2
*/
#define MSU_SUBCODE_DOWNGRADE 2

/*   @struct MSU_SUBCODE_FORCE_UPGRADE
*    @brief Macro defining MSU Sub code for FORCE_UPGRAD 3
*/
#define MSU_SUBCODE_FORCE_UPGRADE 3

/*   @struct MSU_SUBCODE_DATA_TRANSFER
*    @brief Macro defining MSU Sub code for DATA_TRANSFER 4
*/
#define MSU_SUBCODE_DATA_TRANSFER 4

/*   @struct MSU_SUBCODE_SCM
*    @brief Macro defining MSU Sub code for SCM 5
*/
#define MSU_SUBCODE_SCM 5

/*   @struct MSU_SUBCODE_CCM
*    @brief Macro defining MSU Sub code for CCM 6
*/
#define MSU_SUBCODE_CCM 6

/*   @struct MSU_SUBCODE_TRANSFER_COMPLETE
*    @brief Macro defining MSU Sub code for TRANSFER_COMPLETE 7
*/
#define MSU_SUBCODE_TRANSFER_COMPLETE 7

/*   @struct MSU_SUBCODE_ABORT_TRANSFERT
*    @brief Macro defining MSU Sub code for ABORT_TRANSFERT 8
*/
#define MSU_SUBCODE_ABORT_TRANSFERT 8

/*   @struct MSU_SUBCODE_CCM_TRANSFER_COMPLETED
*    @brief Macro defining MSU Sub code for TRANSFER_COMPLETED 9
*/
#define MSU_SUBCODE_CCM_TRANSFER_COMPLETED 9


#define MSU_SUBCODE_SCM_TRANSFER_COMPLETED 10

/*   @struct MSU_SUBCODE_CLIENT_STATUS_REQUEST
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_REQUEST 10
*/
#define MSU_SUBCODE_CLIENT_STATUS_REQUEST 11

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_CLIENT_STATUS_RESPONSE 12


/*   @struct MSU_OPCODE_AUTHENTICATION
*    @brief Macro defining MSU Opcode for authentication 4
*/
#define MSU_OPCODE_AUTHENTICATION 4


/*   @struct MSU_SUBCODE_DEVICE_DISCOVERY_REQUEST
*    @brief Macro defining MSU Sub code for DEVICE_DISCOVERY_REQUEST 1
*/
#define MSU_SUBCODE_DEVICE_DISCOVERY_REQUEST 1

/*   @struct MSU_SUBCODE_DEVICE_DISCOVERY_RESPONSE
*    @brief Macro defining MSU Sub code for DEVICE_DISCOVERY_RESPONSE 2
*/
#define MSU_SUBCODE_DEVICE_DISCOVERY_RESPONSE 2

/*   @struct MSU_SUBCODE_SET_GROUP
*    @brief Macro defining MSU Sub code for SET_GROUP 1
*/
#define MSU_SUBCODE_SET_GROUP 1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_RESET_GROUP 2

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_ACK_GROUP 3

/*   @struct MSU_SUBCODE_AUTHENTICATION_REQUEST
*    @brief Macro defining MSU Sub code for authentication request 
*/
#define MSU_SUBCODE_AUTHENTICATION_REQUEST 1

/*   @struct MSU_SUBCODE_AUTHENTICATION_RESPONSE
*    @brief Macro defining MSU Sub code for authentication response 
*/
#define MSU_SUBCODE_AUTHENTICATION_RESPONSE 2


/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_USER_CMD_STOP 1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_USER_CMD_INIT 2

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_SUBCODE_USER_CMD_START 3

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_UPGRADE_STATUS_PASS 0

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_UPGRADE_STATUS_FAIL 1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_IPV4_VERSION 0

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_IPV6_VERSION 1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_PROTOCOL_VERSION 2

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MIN_SEQ_NUMBER  1

/*   @struct MSU_MAX_SEQ_NUMBER
*    @brief Macro defining MAX sequence Number 32
*/
#define MSU_MAX_SEQ_NUMBER  32

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MIN_CHUNK_NUMBER  1

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_CCM_CHUNK_NUMBER  256  

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_FIELD_NAME_LENGTH 127

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_FIELD_PRESENT 1

/*   @struct MSU_MAX_PACKET_SIZE
*    @brief Macro defining MAX Packet size 1500 bytes
*/
#define MSU_MAX_PACKET_SIZE 1500

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_TAG_COUNT 2

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_DEVICE_INFO_TAG_COUNT 8

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_TAG_SIZE 127 //127 bytes excluding NULL character
#define MSU_AUTHENTICATION_TAG_COUNT 10
#define MAX_BYTES_IPv6_ADDRESS 40

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAX_IP_FIELDS 1 //10

#define MSU_CLIENT_STATUS 1 

#define MSU_MASTER_STATUS 0 

#define MSU_AUTHENTICATION_TAG 1 

#define MSU_AUTHETICATION_LEVEL_TAG  1

#define MSU_DEFAULT_SERVER_IP 3232236043 //10

#define MSU_VENDORNAME_LENGTH 20

#define MSU_PRODUCTCODE_LENGTH 14

#define MSU_MAJOR_MINOR_REVISION_LENGTH 4

#define MSU_MAX_FILES_SUPPORTED 3
#define MAC_LEN 6
#define IP_LEN 40				 
#define MSU_MAX_TAG_BUFFER_SIZE (MSU_MAX_TAG_COUNT * MSU_MAX_TAG_SIZE)

/*   @struct MSU_MAX_SEQ_SIZE
*    @brief Macro defining MAX sequence size 1460 bytes
*/
#define MSU_MAX_SEQ_SIZE 1460    

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define I_AM_VARIABLE_DATA_INFO 8 + 4 



/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_HARDWARE_ID "1"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_PRODUCT_ID "2"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_PRODUCT_NAME "MSU Node"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MODEL_NAME "MSU Client"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_VENDOR_ID "243"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_PRODUCT_FIRMWARE_VERSION "1.0"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_MAJOR_MINOR_REVISION "1.01"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU Sub code for CLIENT_STATUS_RESPONSE 11
*/
#define MSU_DEVICE_LOCATION MSU_FILE_DEFAULT_PATH

/*   @struct MSU_DEFAULT_PASSWORD
*    @brief Macro defining MSU default password
*/

#define MSU_DEFAULT_PASSWORD "admin"

/*   @struct MSU_SUBCODE_CLIENT_STATUS_RESPONSE
*    @brief Macro defining MSU default username
*/
#define MSU_DEFAULT_USERNAME "admin"

#define MSU_DEFAULT_FILENAME "App2.out"

#define MSU_DEFAULT_FILENAME1 "text1.txt"

#define MSU_DEFAULT_FILENAME2 "text2.txt"

#define MSU_USERPASS_LEN 10

#define MAX_FILENAME_LEN 32

#define MAX_FILE_SUPPORTED	3

#define MSU_VENDORNAME_LENGTH 20

#define MSU_PRODUCTCODE_LENGTH 14

#define MSU_MAJOR_MINOR_REVISION_LENGTH 4

/*   @struct MSUCmd
*    @brief Structure defining MSU opcode and subCode
*    @details Defines structure with holds opcode and subcode variables, 
*              opcode occupies 4 bits and subcode occupies 4 bits 
*/
typedef struct
{
#ifdef MSU_PLATFORM_BIG_ENDIAN
 UINT8 opcode : 4;
 UINT8 subcode : 4;
#else
 UINT8 subcode : 4;
 UINT8 opcode: 4;
#endif
}MSU_STRUCT_PACK MSUCmd;

/*   @struct VerInfo
*    @brief Structure defining MSU versionIP, versionProtocol and reserved fields.
*    @details Defines structure which holds versionIP, versionProtocol and reserved fields, 
*              versionIP occupies 1 bit, versionProtocol occupies 3 bits and reserved occupies 1 bit
*/
typedef struct
{
#ifdef MSU_PLATFORM_BIG_ENDIAN
 UINT8 versionIP : 1;
 UINT8 versionProtocol : 3;
 UINT8 reserved : 4;
#else
 UINT8 reserved : 4;
 UINT8 versionProtocol : 3;
 UINT8 versionIP : 1;
#endif
}MSU_STRUCT_PACK VerInfo;

/*   @struct FieldNameTag
*    @brief Structure defining MSU isFieldPresent, fieldLength fields.
*    @details Defines structure which holds isFieldPresent and fieldLength fields, 
*              isFieldPresent occupies 1 bit, fieldLength occupies 73 bits 
*/
typedef struct
{
#ifdef MSU_PLATFORM_BIG_ENDIAN
 UINT8 isFieldPresent : 1;
 UINT8 fieldLength : 7;
#else
 UINT8 fieldLength : 7;
 UINT8 isFieldPresent : 1;
#endif
}MSU_STRUCT_PACK FieldNameTag;


/*   @struct FileTransferInfo
*    @brief Structure defining MSU fileTransferState, chunkTransferState and reserved fields.
*    @details Defines structure which holds fileTransferState,chunkTransferState and reserved fields, 
*              fileTransferState occupies 1 bit, chunkTransferState occupies 1 bit and reserved occupies 6 bits 
*/
typedef struct
{
#ifdef MSU_PLATFORM_BIG_ENDIAN
 UINT8 fileTransferState : 1;
 UINT8 chunkTransferState : 1; 
 UINT8 reserved : 6;
#else
 UINT8 reserved :6; 
 UINT8 chunkTransferState : 1;
 UINT8 fileTransferState : 1;
#endif
}MSU_STRUCT_PACK FileTransferInfo;


/*   @struct MSUHeader
*    @brief Structure defining MSU command, fileNumber and versionInfo fields.
*    @details Defines structure which holds command, fileNumber and versionInfo fields, 
*             command is a type of MSUCmd structure variable which holds opcode and subcode,
*             fileNumber which occupies UINT16 bits and versionInfo is type of VerInfo structure variable 
*             holds versionIP, versionProtocol and reserved fields
*/
typedef struct
{
 MSUCmd command;
 UINT16 fileNumber;
 VerInfo versionInfo;
}MSU_STRUCT_PACK MSUHeader;
typedef union
{
	UINT32 ServerIPv4;
	UINT8  ServerIPv6[MAX_BYTES_IPv6_ADDRESS];
} MSU_STRUCT_PACK MSUIpAddressFormat;

typedef union
{
	UINT32 transactionid;
	UINT8  transactionencrypt[4];
} MSU_STRUCT_PACK MSUtransactionid;



/*   @struct AuthLevel
*    @brief Structure defining Authentication.
*    @details Defines structure which holds IsClient,IsMaster,Is Authentication required,Level.
*/
typedef struct
{
 #ifdef MSU_PLATFORM_BIG_ENDIAN
 	UINT8 IsClient:1;
 	UINT8 IsMaster:1;
 	UINT8 IsAuthRequired:1;
 	UINT8 Level:1;
 	UINT8 Reserved:4;
 #else
 	UINT8 Reserved:4;
 	UINT8 Level:1;
 	UINT8 IsAuthRequired:1;
 	UINT8 IsMaster:1;
 	UINT8 IsClient:1;
 #endif	
 } AuthLevel;


/************************************************************/
	   	//Adding on Dec 6//Security Info struct
/************************************************************/
typedef struct
{
	char username[9];
	char password[9];
	char IP[1][IP_LEN];//only for testing
}SecurityInfo;

	//	char MAC[1][MAC_LEN];//only for testing
	//  char IP[MAX_NO_DEVICES][IP_LEN];//
	//  char MAC[MAX_NO_DEVICES][MAC_LEN];
 


/*   @struct MsuDeviceInfo
*    @brief Structure defining MSU Device information fields
*    @details Defines structure which holds hardwareIdTag,productIdTag,productNameTag,
*             modelNameTag,vendorIdTag,firmwareVersionTag,revisionTag,deviceLocTag information which are type of
*             FieldNameTag structure which holds isFieldPresent and fieldLength
*             it also contains deviceInfoData char arrey which holds information
*/
typedef struct
{
	UINT8 Client;
	UINT8 Master;
	UINT8 AutheticationTag;
	UINT8 AuthenticationLevelTag;
	UINT8 NumberOfIPSupported;
	UINT8 noOfFileSupported;
	MSUIpAddressFormat IP[10];
	UINT8 MAC[6];
	SINT8 Username[MSU_USERPASS_LEN];
	SINT8 Password[MSU_USERPASS_LEN];
	SINT8 filename[MAX_FILE_SUPPORTED][MAX_FILENAME_LEN];
	
}MSU_STRUCT_PACK MsuDeviceInfo;
//typedef struct SecurityInfo var;//adding on Dec 09-Security Info;
// SecurityInfo var;
typedef struct //this would not be used,SecurityInfo is enough//
{
	char username[15];
	char password[15];
}MSU_STRUCT_PACK login_info;


/*   @struct MSUSyncHeaderformat
*    @brief Structure defining MSU Sync message fixed frame format
*    @details Defines structure which holds MSU SyYnc Message fixed frame information, sync message contains 
*             msuHeader,fileSize,chunkCount,seqNumberLimit,seqSize,serverIP,multicastIP,multicastCmIP
*             multicastPort,multicastCmPort,transactionID,fileChecksum,fileNameTag,destPathTag,groupIDTag,
*             updateTimeout,fileName and  destPath. 
*/

typedef struct
{
 MSUHeader msuHeader;
 UINT32 fileSize;
 UINT32 chunkCount;
 UINT16 seqNumberLimit;
 UINT16 seqSize;
 MSUIpAddressFormat serverIP;
 MSUIpAddressFormat multicastIP;
 MSUIpAddressFormat multicastCmIP;
 UINT16 multicastPort;
 UINT16 multicastCmPort;
 UINT32 transactionID;
 UINT32 fileChecksum;
 FieldNameTag fileNameTag;
 FieldNameTag destPathTag;
 UINT8 reserveFeild;
 UINT8 updateTimeout;
 
} MSU_STRUCT_PACK MSUSyncHeaderformat;


/*   @struct MSUSyncMessagePdu
*    @brief Structure defining MSU Sync message PDU
*    @details Defines structure which holds MSU SyYnc Message information, sync message contains 
*    msuHeader,fileSize,chunkCount,seqNumberLimit,seqSize,serverIP,multicastIP,multicastCmIP
*    multicastPort,multicastCmPort,transactionID,fileChecksum,fileNameTag,destPathTag,groupIDTag,
*    updateTimeout,fileName and  destPath. 
*/

typedef struct
{
 UINT8 fileName[MSU_MAX_TAG_SIZE];
 UINT8 destPath[MSU_MAX_TAG_SIZE]; 
} MSU_STRUCT_PACK MSUSyncMessageVariableData;



/*   @struct MSUSyncMessagePdu
*    @brief Structure defining MSU Sync message PDU
*    @details Defines structure which holds MSU SyYnc Message information, sync message contains 
*    msuHeader,fileSize,chunkCount,seqNumberLimit,seqSize,serverIP,multicastIP,multicastCmIP
*    multicastPort,multicastCmPort,transactionID,fileChecksum,fileNameTag,destPathTag,groupIDTag,
*    updateTimeout,fileName and  destPath. 
*/

typedef struct  
{
  MSUSyncHeaderformat msuSyncHeaderformat;
  MSUSyncMessageVariableData msuSyncMessageVariableData;
} MSU_STRUCT_PACK MSUSyncMessagePdu;



/*   @struct MSUMulticastPdu
*    @brief Structure defining MSU Multicasr PDU
*    @details Defines structure which holds MSU msuHeader,chunkNumber,seqNumber,dataLength,fileTransferInfo
*             and data pointer.
*/
typedef struct
{
 MSUHeader msuHeader;
 UINT32 chunkNumber;
 UINT8 seqNumber;
 UINT16 dataLength;
 FileTransferInfo fileTransferInfo;
 UINT8 *pData;
}MSU_STRUCT_PACK MSUMulticastPdu;

typedef struct 
{
char Info[1460];
//UINT16 seqsize;	
}MsuInfo;

/*   @struct MSUSCMPdu
*    @brief Structure defining MSU SCM PDU
*    @details Defines structure which holds MSU msuHeader,chunkNumber,seqCount,reserved and seqBitMap
*/
typedef struct
{
 MSUHeader msuHeader;
 UINT32 chunkNumber;
 UINT8 seqCount;
 UINT8 reserved[3];
 UINT32 seqBitMap;
}MSU_STRUCT_PACK MSUSCMPdu;

/*   @struct MSUCCMPdu
*    @brief Structure defining MSU CCM PDU
*    @details Defines structure which holds MSU msuHeader,chunkNumber
*/
typedef struct
{
 MSUHeader msuHeader;
 UINT32 chunkCount;
}MSU_STRUCT_PACK MSUCCMPdu;

/*   @struct MSUDiscoveryRequestPdu
*    @brief Structure defining MSU Discovery Request PDU
*    @details Defines structure which holds MSU command,isRange,versionInfo,IP address startRange and endRange
*/
typedef struct
{
 MSUCmd command;
 UINT8 isRange;
 UINT8 MsgType;
 VerInfo versionInfo;
 MSUIpAddressFormat startRange;
 MSUIpAddressFormat endRange;
}MSU_STRUCT_PACK MSUDiscoveryRequestPdu;

typedef struct
{	
UINT8 bufferdiscoveryresponse[MSU_MAX_TAG_SIZE];	
} MSUDiscoveryResponseVarlength;

/*   @struct BasicDeviceIdentification
*    @brief Structure defining BasicDeviceIdentification fields
*    @details Defines structure which holds MSU command,isRange,versionInfo,IP address startRange and endRange
*/
typedef struct
{
 FieldNameTag hardwareIdTag;
 FieldNameTag productIdTag;
 FieldNameTag productNameTag;
 FieldNameTag modelNameTag;
 FieldNameTag vendorIdTag;
 FieldNameTag firmwareVersionTag;
 FieldNameTag revisionTag;
 FieldNameTag deviceLocTag;
 UINT8 hardwareId[20];
 UINT8 productId[20];
 UINT8 productName[20];
 UINT8 vendorname[20];
 UINT8 modelName[20];
 UINT8 firmwareVersion[20];
 UINT8 revision[20];
 UINT8 deviceLoc[20];  
} MSU_STRUCT_PACK MSUBasicDeviceIdentification;
/*   @struct MSUDiscoveryResponsePdu
*    @brief Structure defining MSU Discovery Response PDU
*    @details Defines structure which holds MSU command,GroupId,versionInfo,deviceID,hardwareIdTag,productIdTag,
*             productNameTag,modelNameTag,vendorIdTag,firmwareVersionTag,revisionTag,deviceLocTag and reservedUser.
*/
typedef struct
{
 MSUCmd command;
 UINT8 GroupId;
 AuthLevel AuthenticationLevel;
 VerInfo versionInfo;
 MSUIpAddressFormat deviceID;
 FieldNameTag hardwareIdTag;
 FieldNameTag productIdTag;
 FieldNameTag productNameTag;
 FieldNameTag modelNameTag;
 FieldNameTag vendorIdTag;
 FieldNameTag firmwareVersionTag;
 FieldNameTag revisionTag;
 FieldNameTag deviceLocTag;
  FieldNameTag devMsuParam; 
 UINT8 reservedUser[3];
 UINT8 varlengthdata[MSU_MAX_TAG_SIZE];
}MSU_STRUCT_PACK MSUDiscoveryResponsePdu;

/*    @struct MSUAuthenticationRequestPdu
 *    @brief Structure defining MSU Authentication Request PDU
 *    @details Defines structure which holds MSU command,ipversion,serverip,transaction id,filename,hardwareid,
 *    productid,modelname,username,password,FWVersion,SWVersion,vendorid,productname
 */
typedef struct
{
 MSUCmd command;
 AuthLevel AuthenticationLevel;
 UINT8 reserved;
 VerInfo versionInfo;
 MSUIpAddressFormat msuServerIP;
 MSUtransactionid transactionid;
 UINT8  MACAddr[6];
 FieldNameTag FileNameTag;
 FieldNameTag hardwareIdTag;
 FieldNameTag productIdTag;
 FieldNameTag modelnameTag;
 FieldNameTag passwordTag;
 FieldNameTag usernameTag;
 FieldNameTag firmwareversionTag;
 FieldNameTag softwareversionTag;
 FieldNameTag vendorIdTag;
 FieldNameTag productnameTag;
 } MSU_STRUCT_PACK MSUAuthenticationHeader;
 
 /*    @struct MSUAuthenticationRequestPdu
 *    @brief Structure defining MSU Authentication Request PDU
 *    @details Defines structure which holds MSU command,ipversion,serverip,transaction id,filename,hardwareid,
 *    productid,modelname,username,password,FWVersion,SWVersion,vendorid,productname
 */
typedef struct
{

 UINT8 filename[MSU_MAX_TAG_SIZE];
 UINT8 HardwareId[MSU_MAX_TAG_SIZE];
 UINT8 ProductId[MSU_MAX_TAG_SIZE];
 UINT8 modelname[MSU_MAX_TAG_SIZE];
 UINT8 password[MSU_MAX_TAG_SIZE];
 UINT8 username[MSU_MAX_TAG_SIZE];
 UINT8 firmwareversion[MSU_MAX_TAG_SIZE];
 UINT8 softwareversion[MSU_MAX_TAG_SIZE];
 UINT8 vendorId[MSU_MAX_TAG_SIZE];
 UINT8 productname[MSU_MAX_TAG_SIZE];
 
 } MSU_STRUCT_PACK MSUAuthenticationPayload;
 
/*    @struct MSUAuthenticationRequestPdu
 *    @brief Structure defining MSU Authentication Request PDU
 *    @details Defines structure which holds MSU command,ipversion,serverip,transaction id,filename,hardwareid,
 *    productid,modelname,username,password,FWVersion,SWVersion,vendorid,productname
 */
typedef struct
{
 	MSUAuthenticationHeader 	authHeader;
 	MSUAuthenticationPayload 	authPayload;
 } MSU_STRUCT_PACK MSUAuthenticationRequestPdu;

/*    @struct MSUAuthenticationResponsePdu
 *    @brief Structure defining MSU Authentication Response PDU
 *    @details Defines structure which holds MSU command,ipversion,error code and error subcode
 */
 typedef struct
 {
  MSUCmd command;
  UINT8  ErrCode;
  UINT8  ErrSubCode;
  VerInfo versionInfo; 	
 }MSU_STRUCT_PACK MSUAuthenticationResponsePdu;


/*   @struct MSUGroupCreationPdu
*    @brief Structure defining MSU Group creation PDU
*    @details Defines structure which holds MSU command,GroupId and versionInfo
*/
typedef struct
{
 MSUCmd command;
 UINT8 groupID;
 UINT8 reserved;
 VerInfo versionInfo;
 UINT32  deviceId;
}MSU_STRUCT_PACK MSUGroupCreationPdu;

/*   @struct MSUUpgradeStatusPdu
*    @brief Structure defining MSU Upgrade status PDU
*    @details Defines structure which holds MSU command,status,errorCode,versionInfo,transactionID and deviceID
*/
typedef struct
{
 MSUCmd command;
 UINT8  status;
 UINT8 errorCode;
 VerInfo versionInfo;
 UINT32 transactionID;
 UINT32 deviceID;
 UINT8 ccmRetryCount;
 UINT8 reserve1[3];
}MSU_STRUCT_PACK MSUUpgradeStatusPdu;


/*   @struct MSUGroupCreationAckPdu
*    @brief Structure defining MSU Group creation Ack PDU
*    @details Defines structure which holds MSU command,groupID,reserved and versionInfo.
*/
typedef struct
{
 MSUCmd command;
 UINT8 groupID;
 UINT8 reserved;
 VerInfo versionInfo;
 UINT32  deviceId;
}MSU_STRUCT_PACK MSUGroupCreationAckPdu;


/*   @struct MSUCompletePdu
*    @brief Structure defining MSU Complete PDU
*    @details Defines structure which holds MSU msuHeader.
*/
typedef struct
{
 MSUHeader msuHeader;
}MSU_STRUCT_PACK MSUCompletePdu;




/*   @struct MSUCCMCompletePdu
*    @brief Structure defining MSU CCM Complete PDU
*    @details Defines structure which holds MSU msuHeader.
*/
typedef struct
{
  MSUHeader msuHeader;
  BOOL ccmRetry;
  UINT8 reserved[3];

}MSU_STRUCT_PACK MSUCCMCompletePdu;

typedef struct
{
  MSUHeader msuHeader;

}MSU_STRUCT_PACK MSUAbortMsgPdu;


typedef struct
{
  MSUHeader msuHeader;
  BOOL scmRetry;
  UINT8 reserved[3];
}MSU_STRUCT_PACK MSUSCMCompletePdu;



typedef struct
{
   UINT16 seqDelay;
   UINT16 scmDelay;
   UINT16 ccmDelay;
   UINT8  ccmRetry;
   UINT8  scmRetry;
   UINT8  commTimeout_sec; 
}MSU_STRUCT_PACK DEV_MSU_PARAM;


/**  @fn msuPduInit(void)
*    @brief This function Initializes the PDU data with 
*/ 
void msuPduInit(void);
/**  @fn msuSocketProcessSyncMessage(UINT8 *pMsg , UINT32 msgSize)
*    @brief This function process the sync message received from server
*/
BOOL msuSocketProcessSyncMessage(UINT8 *pMsg , UINT32 msgSize);
/**  @fn msuParseDiscoverRequest(UINT8 *pMsg , UINT32 msgSize, MSUDiscoveryRequestPdu *pDiscoverRequest)
*    @brief This function parses the discovery request received from server
*/ 
BOOL msuPduParseMcastRequest(UINT8 *pMsg , UINT32 msgSize);

/**  @fn msuParseDiscoverRequest(UINT8 *pMsg , UINT32 msgSize, MSUDiscoveryRequestPdu *pDiscoverRequest)
*    @brief This function parses the discovery request received from server
*/
BOOL msuParseDiscoverRequest(UINT8 *pMsg , UINT32 msgSize, MSUDiscoveryRequestPdu *pDiscoverRequest);
/**  @fn msuHnadleDeviceStatusUpdateRequest(char *gMsgBuffer,  UINT32 nByte)
*    @brief This function handles device status update request received from server
*/
BOOL msuHandleDeviceGroupMsg(UINT8 *pMsg , UINT32 msgSize);
/**  @fn isMsuVersion( MSUHeader *header)
*    @brief This function validates the MSU version
*/
BOOL isMsuVersion( MSUHeader *header);
/**  @fn msuHnadleDeviceStatusUpdateRequest(char *gMsgBuffer,  UINT32 nByte)
*    @brief This function handles device status update request received from server
*/
BOOL msuHnadleDeviceStatusUpdateRequest(char *gMsgBuffer,  UINT32 nByte);

/**  @fn msuCcmMsgCreate( MSUCCMPdu *ccmMsgPdu, UINT32 totalChnksExpected, UINT32 *missedChnkCnt)
*    @brief This function constructs the CCM message format
*/
void msuCcmMsgCreate( MSUCCMPdu *ccmMsgPdu, UINT32 totalChnksExpected, UINT32 *missedChnkCnt);

/**  @fn msuUpgrateMsgCreate( MSUUpgradeStatusPdu *msuUpgradeStatus, SINT32 upgradeStatus )
*    @brief This function constructs the Upgrade message format
*/ 
void msuUpgrateMsgCreate( MSUUpgradeStatusPdu *msuUpgradeStatus, SINT32 upgradeStatus,  UINT8 ccmRetryVal );
/**  @fn msuDiscoveryResponseMsgCreate(MSUDiscoveryRequestPdu *discoveryRequestPdu,
		MSUDiscoveryResponsePdu *msuDiscoveryResponse, UINT16 *dataPayloadSize )
*    @brief This function constructs the Discovery response message format
*/ 
void msuDiscoveryResponseMsgCreate(MSUDiscoveryRequestPdu *discoveryRequestPdu,
		MSUDiscoveryResponsePdu *msuDiscoveryResponse, UINT16 *dataPayloadSize );

/**  @fn msuScmMsgCreate( MSUSCMPdu *scmMsgPdu,UINT32 bitMap, UINT32 chunkNum, UINT8 missedSeqCount)
*    @brief This function constructs the SCM message format
*/
void msuScmMsgCreate( MSUSCMPdu *scmMsgPdu,UINT32 bitMap, UINT32 chunkNum, UINT8 missedSeqCount);

/**  @fn msuGroupAckMsgCreate( MSUGroupCreationPdu *gReqPdu )
*    @brief This function constructs the Group creation Ack message format
*/
MSUGroupCreationPdu * msuGroupAckMsgCreate( MSUGroupCreationPdu *gReqPdu );


/**  @fn msuAuthenticationRequest(UINT8 *pMsg,MSUAuthenticationRequestPdu *pAuthenticationRequest,
		MSUAuthenticationResponsePdu *msuAuthenticationResponse, UINT16 *dataPayloadSize )
*    @brief This function parses the authentication request and constructs the response
*/ 
BOOL msuAuthenticationRequest(UINT8 *pMsg,UINT32 msgSize,MSUAuthenticationRequestPdu *pAuthenticationRequest,
							  MSUAuthenticationResponsePdu *msuAuthenticationResponse, msuSockinfo *socketip );


SINT32 msuDecryption(UINT8 *inputstring ,char *privatekeydecrypt,UINT32 lengthstring, UINT32 length_privatekey);

void msuBasicIdentification(void *ptr );

#endif/*#ifndef MSUPDUHPP */
