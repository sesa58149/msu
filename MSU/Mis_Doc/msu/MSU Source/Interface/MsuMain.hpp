#ifndef MSU_MAIN_HPP
#define MSU_MAIN_HPP

/**	@file msuMain.hpp
 *	<TABLE>
 *	@brief This file implements the MSU Protocol class
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
 *	@author Deepak Yadav
 *	@par HISTORY
 *
 *	<TABLE>
 *	<TR>
 *	<TD>Author</TD>		<TD>Date</TD>		<TD>Description</TD>
 *	</TR>
 *	<TR>
 *	<TD>Deepak Yadav</TD>	<TD>10-Oct-2010</TD>	<TD>First creation</TD>
 *	</TR>
 *
 *	</TABLE>
 *	</TABLE>
 *
 */
  
//#include "LCE_Brick_settings.h"
//#include "trTypes.h"
#include "BrickComponent.hpp"
#include "EventHandler.h"

/**  @def MSU_PROTOCOL_TASK_NAME  
 *   @brief Msu Protocol Task Name
 */
#define MSU_PROTOCOL_TASK_NAME            "MsuProtocol"

/**  @def HOST_FIRMWARE_TASK_PRIORITY  
 *   @brief Host Firmware Task Priority
 */
#define MSU_PROTOCOL_TASK_PRIORITY        35    //45  //59

/**  @def HOST_FIRMWARE_TASK_STACK_SIZE  
 *   @brief Host Firmware Task Size
 */
//#define MSU_PROTOCOL_TASK_STACK_SIZE      4096
#define MSU_PROTOCOL_TASK_STACK_SIZE       4096 //2048


/**
 * @class MSU Protocol
 * @brief Mass Software Upgrade Protocol Class
 * 
 * @details Defines a class MsuProtocol which is derived from the BrickComponent. 
 *          This class implements all the MSU Protocol Functionality.
 */
class MsuProtocol : public BrickComponent,public EventHandler
{

public:

	/**	
    *  @brief Default Constructor for the class MsuProtocol
    */
   MsuProtocol();

	/**	
    *  @brief Destructor for the class MsuProtocol
    */
   ~MsuProtocol();

   int initialize();

   int configure();

   int start();
   
   int monitor();

   static int msuTask();

	Sint32 getIpAddrFromFDR();
	
private:	
	
	 void processEvent( Event * const evnt );	
	
	
//   HFU_STATE_INFO stateInfo_;

//   void hfuLogMsg();

//   TRSEMID hfuTriggerSemId_;

//   TRSEMID  hfuMdBusSemId_;

//   void callback( MODBUS_TRANSACTION    *   info, MODBUS_LUPDU * pResponseMsg, BOOL exceptionReturn );
   
//   char hostFwFileNamePath_[HFU_FILE_NAME_PATH_SIZE];


private:
	Sint32 MsuProtocolTaskId_;
	Uint32 msuTaskId_;
	Uint32 ipAddress_;
	Uint32 subnetmask_;
};

/**  @enum HFU_FP
 *   @brief Host Firmware Upgrade Function Pointer
 */ 
typedef void (MsuProtocol::* MSU_PROTOCOL)(void);

#endif /*#ifndef MSU_MAIN_HPP */