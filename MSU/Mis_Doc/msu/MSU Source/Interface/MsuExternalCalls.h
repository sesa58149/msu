#ifndef MSUEXTERNALCALLS_HPP
#define MSUEXTERNALCALLS_HPP

#include <string.h>
#include <stdlib.h>
#include "trTypes.h"  


#define Uint8 unsigned char
//#define MODBUS_IDENTIFICATION

START_STRUCT_PACK

typedef struct
{
 
#ifdef TR_BIG_ENDIAN 
Uint8 isFieldPresent : 1;
Uint8 fieldLength : 7;
#else
 Uint8 fieldLength : 7;
 Uint8 isFieldPresent : 1;
#endif	
}FieldTag; 

typedef struct
{
 FieldTag hardwareIdTag;
 FieldTag productIdTag;
 FieldTag productNameTag;
 FieldTag modelNameTag;
 FieldTag vendorIdTag;
 FieldTag firmwareVersionTag;
 FieldTag revisionTag;
 FieldTag deviceLocTag;
 Uint8 hardwareId[20];
 Uint8 productId[20];
 Uint8 productName[20];
 Uint8 vendorname[20];
 Uint8 modelName[20];
 Uint8 firmwareVersion[20];
 Uint8 revision[20];
 Uint8 deviceLoc[20];  
} BasicDeviceIdentification;

END_STRUCT_PACK

#define VENDORNAME_LENGTH 20

#define HARDWAREID_LENGTH 1

#define PRODUCTCODE_LENGTH 14



/*=================================================================
  DESCRIPTION :Used to retrieve Basicidentification information .
  RETURNS:  
                    NONE
                    
==================================================================*/
void BasicIdentification(void*ptr);


#endif