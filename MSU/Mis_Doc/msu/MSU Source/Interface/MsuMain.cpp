/**	@file MsuMain.cpp
 *	<TABLE>
 *	@brief Implementation of MSU Main
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
 *	<TD>Deepak G</TD>	<TD>04-Oct-2011</TD>	<TD>First creation</TD>
 *	</TR>
 *
 * 	</TABLE>
 *	</TABLE>
 */

#ifndef MSU_MAIN_CPP
#define MSU_MAIN_CPP

#include "MsuMain.hpp"
#include "FileSystemImpl.hpp"
#include "trTypes.h"
#include "fsf.h"
#include "trErrno.h"
#include "LogMsg.h"
#include "MsuLeech.hpp"
#include "MsuSocket.hpp"
#include "FdrClient.hpp"
#include "SystemControlManager.hpp"

/**  @var  g_msuProtocol
 *   @brief MsuProtocol Object Pointer
 */
#ifdef ETHERBRICK_PLATFORM
  extern UINT32 clientIP;
#else
  extern char clientIP[20];
#endif 
 
MsuProtocol * g_msuProtocol     = NULL;


/**  @fn  MsuProtocol()
 *   @brief Msu Protocol Constructor
 *   @details The constructor initializes some of the class member variables.
 */
MsuProtocol::MsuProtocol()
{
	MsuProtocolTaskId_ = NULL;
}

/**  @fn  ~MsuProtocol()
 *   @brief Msu Protocol Destructor
 *   @details The destructor  frees some of the resources.
 */
MsuProtocol::~MsuProtocol()
{
	
}
 
/**  @fn  MsuProtocol::initialize()
 *   @brief Msu Protocol initialization function
 *   @details This function initializes the Msu Protocol Component.
 *   @param[in] None
 *   @return Success on successful initialization, error otherwise.
 */
int MsuProtocol::initialize()
{
	// Msu Protocol Component Must dependen on FDR....
   dependencyList_[TR_FDR_BASIC_ID] = true;   
   
   g_msuProtocol = this;
   
//    registerEvent( TR_EVT_FDR_IPCONFIG_READY );
      
	return(BrickComponent::initialize());
}

/**  @fn  MsuProtocol::configure()
 *   @brief Msu Protocol configuration function
 *   @details This function configures the Msu Protocol Component.
 *   @param[in] None
 *   @return Success on successful initialization, error otherwise.
 *   @retval TR_SUCCESS on success
 *   @retval TR_ERROR on Failure
 */
int MsuProtocol::configure()
{
	return(BrickComponent::configure());
}

/**  @fn  MsuProtocol::monitor()
 *   @brief Msu Protocol monitor function
 *   @details This function monitors the Msu Protocol Component.
 *   @param[in] None
 *   @retval TR_SUCCESS Always
 */						  
int MsuProtocol::monitor()
{
    return(TR_SUCCESS);
}

/**  @fn  MsuProtocol::start()
 *   @brief Msu Protocol Start function
 *   @details This function creates the Host Firmware Upgrade Task
 *   @param[in] None
 *   @return Success on successful initialization, error otherwise.
 *   @retval TR_SUCCESS on success 
 *   @retval TR_ERROR on failure
 */
int MsuProtocol::start()
{
  getIpAddrFromFDR();	

  clientIP = ipAddress_;
  int retCode;
  /*
   * Create the Host Firmware Upgrade task
   */
  retCode = trTaskCreate( (unsigned char*)MSU_PROTOCOL_TASK_NAME,  // Name for the Task
                             (void*)(&this->msuTask),                 // Task Entry Point
                             (Uint8)MSU_PROTOCOL_TASK_PRIORITY,       // The Task Priority (1-126)
                             MSU_PROTOCOL_TASK_STACK_SIZE,            // Normal Stack Size
                             0,                                       // Arguments, if any
                             TASK_CREATE_AND_EXECUTE,                 // Create and run
                             &msuTaskId_							  // Task Id		
                             );

  if (retCode == TR_ERROR)
  {
      g_msuProtocol = NULL;
#ifdef DEBUG_LEVEL2      
		LOGMSG( IPCL_METHOD_MSU,LOG_MSG_PRIORITY_ERROR,
				IPCL_LABEL_FAIL,
				"Unable to create task : MsuProtocol::start" );
      
#endif      
      return( TR_ERROR );
  }

  state_ = COMPONENT_STATE_READY;

  return( TR_SUCCESS );
}

/**  @fn  MsuProtocol::hfuTask()
 *   @brief Msu Protocol  Task
 *   @details This implements the Msu Protocol Task 
 *   @param[in] None
 *   @return Error as this infinite task is normally not supposed to return
 *   @retval TR_ERROR on failure
 */
int MsuProtocol::msuTask()
{
    if (g_msuProtocol == NULL)
    {
       return TR_ERROR;
    }
    
    while(1)
    {
	    msuLeechStateMachine();    	
    }


    return TR_ERROR;
}

/*-----------------------------------------------------------------------------
*  FUNCTION:    getIpAddrFromFDR()
*               
*  PARAMETERS:  None
*               
*  DESCRIPTION: 
*				
*  RETURN:      
*				TR_SUCCESS	:  	 If successfully completed
*			otherwise
*				TR_ERROR	
*----------------------------------------------------------------------------*/
Sint32 MsuProtocol::getIpAddrFromFDR()
{
	FdrClient *fdr = NULL;
	unsigned long net_addr, /*_bcast_addr, */*myIPAddr, *mySubnetMask;

	fdr = (FdrClient *)SystemControlManager::findComponent( TR_FDR_BASIC_ID,0 );
	IP_PARAMETERS myIPParams = fdr->getOperatingIpConfiguration();

	myIPAddr= (unsigned long *)&(myIPParams.ip_address[0]);
	mySubnetMask=(unsigned long *)&(myIPParams.subnet_mask[0]);

	net_addr = *myIPAddr & *mySubnetMask;

	memcpy(&ipAddress_, &myIPParams.ip_address[0], 4);

	memcpy(&subnetmask_, &myIPParams.subnet_mask[0], 4);

	return (TR_SUCCESS);
}



void MsuProtocol::processEvent(Event * const evnt)
{
  
    switch ( evnt->id )
    {
    case TR_EVT_FDR_IPCONFIG_READY: 
    
           getIpAddrFromFDR();	
  			  clientIP = ipAddress_;
        break;

    default: 
        break;
    }
    
}



#endif /*#ifndef MSU_MAIN_CPP */