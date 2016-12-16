/**	@file MsuLeech.hpp
 *	<TABLE>
 *	@brief Implementation of MSU Leech header
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
 
#ifndef MSULEECH_H
#define MSULEECH_H

#include "MsuPlatform.h"
#include "MsuPdu.h"
#include "msuErrNo.h"

extern MSUSyncMessagePdu msuBroadcastPdu;
extern MSUMulticastPdu msuMcastPdu;

/*   @enum EMSUState
*    @brief Enumeration defining MSU state machine different states
*/
typedef enum
{
   eMsuStateUndefined = 0,    /**< MSU undefined state  eMsuStateUndefined */
	eMsuStateInitialize,       /**< MSU Intialized state  eMsuStateInitialize */
	eMsuStateConfigure,			/**< MSU Configured state  eMsuStateConfigure */
	eMsuStateListen,				/**< MSU Listen state eMsuStateListen */
   eMsuStateReady,				/**< MSU Ready state  eMsuStateReady */
   eMsuStateStart,				/**< MSU start state  eMsuStateStart */
   eMsuStateRunning,				/**< MSU Running state  eMsuStateRunning */
   eMsuStateSCM,					/**< MSU Sequence complain mode state  eMsuStateSCM */
	eMsuStateCCM,					/**< MSU Chunk complain mode state  eMsuStateCCM */
	eMsuStateAbort,				/**< MSU Abort state  eMsuStateAbort */
	eMsuStateDone,					/**< MSU Done state  eMsuStateDone */
	eMsuStateLoopBack				/**< MSU Loopback state  eMsuStateLoopBack */
}EMSUState;

/*   @enum MSUCmd
*    @brief Enumeration defining MSU Upgrade different states
*/
typedef enum
{
	eMsuUpgradePassed = 0, 		/**< MSU Upgrade Passed state  eMsuUpgradePassed */
	eMsuUpgradeFailed,			/**< MSU upgrade Failed state  eMsuUpgradeFailed */
	eMsuUpgradeInProgress		/**< MSU Upgrade InProgress state  eMsuUpgradeInProgress */
	
}EMSUUpgradeState;

/*   @enum MSUCmd
*    @brief Enumeration defining MSU Error states
*/
typedef enum
{
	EMSUErrSuccess = 0,     /**< MSU Error success state  EMSUErrSuccess */
	EMSUErrPassiveSocket,	/**< MSU Error passive socket state  EMSUErrPassiveSocket */
   EMSUErrMcastSocket,		/**< MSU Error Multicast socket state  EMSUErrMcastSocket */
	EMSUErrUnicastSocket,	/**< MSU Error Unicast socket   state  EMSUErrUnicastSocket */
	EMSUErrFile,				/**< MSU Error File state  EMSUErrFile */
	EMSUErrFileCrc,			/**< MSU Error File CRS state  EMSUErrFileCrc */
	EMSUErrUndefinedState	/**< MSU Error Undefined state  EMSUErrUndefinedState */
}EMSUErr;


/**  @fn void msuLeechStateMachine(void)
*    @brief This function executes the MSU state machine
*/ 
void msuLeechStateMachine(void);

/**  @fn void  msuChangeDeviceState( EMSUState presentState)
*    @brief This function is used to change the state of the state machine
*/ 
void  msuChangeDeviceState( EMSUState presentState);

/**  @fn BOOL msuSocketGetDataPackets()
*    @brief This function is used to receive MSU data packets
*/ 
BOOL msuSocketGetDataPackets();

/**  @fn EMSUState msuCurrentState()
*    @brief This function is used to get the current state of the state machine
*/ 
EMSUState msuCurrentState();

/**  @fn EMSUUpgradeState  msuGetUpgradeProcessState()
*    @brief This function is used to get the upgrade status of the device after completion of data transfer
*/ 
EMSUUpgradeState  msuGetUpgradeProcessState();

/**  @fn BOOL msuPduHandleTransferCompleteRequest()
*    @brief This function is used to handle the transfer completed request received from server
*/ 
BOOL msuPduHandleTransferCompleteRequest();//UINT8 *pMsg , UINT32 msgSize

/**  @fn BOOL msuPduHandleCCMCompleteRequest(UINT8 *pMsg , UINT32 msgSize)
*    @brief This function is used to handle CCM transfer complete request  received from server
*/ 
BOOL msuPduHandleCCMCompleteRequest(UINT8 *pMsg , UINT32 msgSize);

/**  @fn UINT32 getCcmRetryCount()
*    @brief This function is used to get the CCM retry count
*/ 
UINT32 getCcmRetryCount();

/**  @fn EMSUState getCurrentMsuState( )
*    @brief This function is used to get the current MSU state
*/ 
EMSUState getCurrentMsuState( );

/**  @fn BOOL msuPlatformComputeCheckSum(SINT8 * filePath, UINT32 fileCheckSum)
*    @brief This function is used to calculate the checksum of the file
*/ 
BOOL msuPlatformComputeCheckSum(SINT8 * filePath, UINT32 fileCheckSum);


/**  @fn BOOL msuPduHandleCCMCompleteRequest(UINT8 *pMsg , UINT32 msgSize)
*    @brief This function is used to handle SCM transfer complete request  received from server
*/ 
BOOL msuPduHandleSCMCompleteRequest(UINT8 *pMsg , UINT32 msgSize);

#endif /*#ifndef __MSU_LEECH_H__*/
