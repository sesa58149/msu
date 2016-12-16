/**	@file MsuPlatform.hpp
 *	<TABLE>
 *	@brief Implementation of MSU Platform header
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
 
#ifndef MSUPLATFORM_H
#define MSUPLATFORM_H


#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>

/*Type declarations*/
typedef  unsigned char UINT8;
typedef  char SINT8;
typedef  unsigned short UINT16;
typedef  signed short SINT16;
typedef  unsigned long UINT32;
typedef  long SINT32;
typedef  unsigned char BOOL;

/*Boolean defines*/
#define TRUE 1
#define FALSE 0

/*Socket defines*/
#define MSU_SOCKET int
#define MSU_INVALID_SOCKET -1
#define MSU_SOCKET_ERROR -1
#define MSU_SOCKET_TIMEOUT_ERROR  -2
#define MSU_SOCKET_RECEIVE_ERROR  -3

#define MSU_SOCKET_SUCCESS 0
#define TR_SUCCESS  0

#define MSU_FILE_ERROR -2

#define TR_ERROR  -1

/*Byte ordering macros*/
#ifdef ETHERBRICK_PLATFORM
	
	#define msuHtoNs(x)      trHtons(x)
	#define msuNtoHs(x)      trNtohs(x)
	#define msuHtoNl(x)      trHtonl(x)
	#define msuNtoHl(x)      trNtohl(x)
#else  
	#define msuHtoNs(x)      htons(x)
	#define msuHtoNl(x)      htonl(x)
	#ifdef MSU_PLATFORM_BIG_ENDIAN
		#define msuNtoHs(x)      (x)
		#define msuNtoHl(x)      (x)
	#else
		#define msuNtoHs(x)      ntohs(x)
		#define msuNtoHl(x)      ntohl(x)
	#endif
#endif

#define UINT16_GET(a,b) (memcpy((UINT8 *)b, (UINT8 *) a, sizeof(UINT16)))
#define UINT32_GET(a,b) (memcpy((UINT8 *)b, (UINT8 *) a, sizeof(UINT32)))

typedef union
{
	UINT8 byte[2];
	UINT16 data;
}U16Data;

typedef union
{
	UINT8 byte[4];
	UINT32 data;
}U32Data;

#define MSU_STRUCT_PACK __attribute__((packed))

#define msuMalloc(size)  malloc(size)
#define msuFree(ptr)     free(ptr)
#define msuMemcpy(d,s,l) memcpy(d,s,l)
#define msuStrlen(str)	strlen(str)
#define msuMemset(src,val,len) memset(src,val,len)
#define msuStrncmp(src,dst,len) strncmp(src,dst,len)

#define MSU_BIT_PER_BYTE 8

#define LINUX_SIMULATOR
#define Debug_level0
#define  Debug_level1



#endif/*#ifndef __MSU_PLATFORM_H__*/
