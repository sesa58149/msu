#ifndef MSUEXTERNALCALLS_CPP
#define MSUEXTERNALCALLS_CPP

#include "MsuExternalCalls.h"
#include "ModbusDeviceManager.hpp"


void BasicIdentification(void*ptr)
{


	Uint8 dataoffset=0;
	Uint8 hardwareid='1';
	BasicDeviceIdentification *basicdeviceidentification= (BasicDeviceIdentification*)ptr; 


	#ifdef MODBUS_IDENTIFICATION

	
    MODBUS_LUPDU dummyMsg = { 0 };
	ModbusDeviceManager DeviceManager;



	/*Request Buildup*/
	dummyMsg.unitId=0xFF;
    dummyMsg.pdu.functionCode =0x2b;
	dummyMsg.pdu.readDevIdReq .meiType =0x0e;
	dummyMsg.pdu.readDevIdReq .readDevIDCode =0x01;
	dummyMsg.pdu.readDevIdReq .objectID = 0x00;

    /*Call the handler*/
	 DeviceManager.handleMessage(&dummyMsg);

	/*traverse the buffer*/
	/*Set those fields to false that have not been incorporated*/
	basicdeviceidentification->hardwareIdTag.isFieldPresent = FALSE;
    basicdeviceidentification->productIdTag.isFieldPresent = FALSE;
    basicdeviceidentification->modelNameTag.isFieldPresent = FALSE;
    basicdeviceidentification->deviceLocTag.isFieldPresent = FALSE;

    /*copy the Vendor name*/
    basicdeviceidentification->vendorIdTag.isFieldPresent = TRUE;
	basicdeviceidentification->vendorIdTag.fieldLength  = VENDORNAME_LENGTH-2;
	memcpy(&basicdeviceidentification->vendorname,&dummyMsg.pdu.readDevIdRsp.devIDObjs[0].objValue[0],VENDORNAME_LENGTH-2);
	dataoffset+=VENDORNAME_LENGTH ;

    /*copy the Product name*/
    basicdeviceidentification->productNameTag.isFieldPresent = TRUE;
	basicdeviceidentification->productNameTag.fieldLength  = PRODUCTCODE_LENGTH-2;
    memcpy(&basicdeviceidentification->productName,&dummyMsg.pdu.readDevIdRsp.devIDObjs[0].objValue[dataoffset],PRODUCTCODE_LENGTH-2 );
    dataoffset+=PRODUCTCODE_LENGTH ;

	/*copy the Version Info*/
	basicdeviceidentification->firmwareVersionTag.isFieldPresent = TRUE;
    basicdeviceidentification->firmwareVersionTag.fieldLength  = 2;
	memcpy(&basicdeviceidentification->firmwareVersion,&dummyMsg.pdu.readDevIdRsp.devIDObjs[0].objValue[dataoffset],2 );
	dataoffset+=3 ;

	/*copy the revision Info*/
	basicdeviceidentification->revisionTag.isFieldPresent = TRUE;
    basicdeviceidentification->revisionTag.fieldLength  = 1;
    memcpy(&basicdeviceidentification->revision,&dummyMsg.pdu.readDevIdRsp.devIDObjs[0].objValue[dataoffset],1 );//Build the 43-14 command and parse the response
		    

#else
    /*traverse the buffer*/
    /*Set those fields to false that have not been incorporated*/
    /*copy the Hardwareid name*/
    basicdeviceidentification->hardwareIdTag.isFieldPresent = TRUE;
    basicdeviceidentification->hardwareIdTag.fieldLength  = strlen("1");
    memcpy(&basicdeviceidentification->hardwareId,"1", strlen("1"));
    dataoffset+= strlen("1") ;


 
    /*copy the productId*/
    basicdeviceidentification->productIdTag.isFieldPresent = TRUE;
    basicdeviceidentification->productIdTag.fieldLength  = strlen("EtherBrickAS");
    memcpy(&basicdeviceidentification->productId,"EtherBrickAS", strlen("EtherBrickAS"));
    dataoffset+= strlen("EtherBrickAS") ;
    
    /*copy the deviceLoc name*/
    basicdeviceidentification->deviceLocTag.isFieldPresent = TRUE;
    basicdeviceidentification->deviceLocTag.fieldLength  = strlen("1234");
    memcpy(&basicdeviceidentification->deviceLoc,"1234", strlen("1234"));
    dataoffset+= strlen("1234") ;
    
    /*copy the modelName name*/
    basicdeviceidentification->modelNameTag.isFieldPresent = FALSE;
    
    
    
     /*copy the Vendor name*/
    basicdeviceidentification->vendorIdTag.isFieldPresent = TRUE;
    basicdeviceidentification->vendorIdTag.fieldLength  = strlen("Schneider");
    memcpy(&basicdeviceidentification->vendorname,"Schneider", strlen("Schneider"));
    dataoffset+= strlen("Schneider") ;
    
    /*copy the Product name*/
    basicdeviceidentification->productNameTag.isFieldPresent = TRUE;
    basicdeviceidentification->productNameTag.fieldLength  = strlen("MassSoftwareUpgrade");
    memcpy(&basicdeviceidentification->productName,"MassSoftwareUpgrade", strlen("MassSoftwareUpgrade"));
    dataoffset+= strlen("MassSoftwareUpgrade") ;
    
    /*copy the Version Info*/
    basicdeviceidentification->firmwareVersionTag.isFieldPresent = TRUE;
    basicdeviceidentification->firmwareVersionTag.fieldLength  = strlen("V3");
    memcpy(&basicdeviceidentification->firmwareVersion,"V3", strlen("V3"));
    dataoffset+= strlen("V3") ;

    
    /*copy the revision Info*/
    basicdeviceidentification->revisionTag.isFieldPresent = TRUE;
    basicdeviceidentification->revisionTag.fieldLength  = strlen("1");
    memcpy(&basicdeviceidentification->revision,"1", strlen("1"));
    dataoffset+= strlen("1") ;
#endif
	
   
}

#endif
