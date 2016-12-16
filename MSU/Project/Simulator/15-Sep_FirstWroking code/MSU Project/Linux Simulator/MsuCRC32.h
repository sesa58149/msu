/**	@file MsuCRC32.hpp
 *	<TABLE>
 *	@brief Implementation of MSU CRC32 header
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

#ifndef CRC32_H_
#define CRC32_H_

typedef UINT32 CRC32;


/**  @fn void crcInit(void)
*    @brief This function Initialize crc table
*/ 
void crcInit(void);


/**  @fn CRC32 crcFast(UINT8 const message[], Sint16 nBytes)
*    @brief This function calculate crc for each bye
*/ 
CRC32 crcFast(UINT8 const message[], SINT16 nBytes);

#endif /* CRC32HPP */
