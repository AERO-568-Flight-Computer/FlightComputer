/**
    @file

    @brief
        Definitions for the DM35424 Temperature Library.

    $Id: dm35424_temperature_library.h 60276 2012-06-05 16:04:15Z rgroner $
*/

//----------------------------------------------------------------------------
//  COPYRIGHT (C) RTD EMBEDDED TECHNOLOGIES, INC.  ALL RIGHTS RESERVED.
//
//  This software package is dual-licensed.  Source code that is compiled for
//  kernel mode execution is licensed under the GNU General Public License
//  version 2.  For a copy of this license, refer to the file
//  LICENSE_GPLv2.TXT (which should be included with this software) or contact
//  the Free Software Foundation.  Source code that is compiled for user mode
//  execution is licensed under the RTD End-User Software License Agreement.
//  For a copy of this license, refer to LICENSE.TXT or contact RTD Embedded
//  Technologies, Inc.  Using this software indicates agreement with the
//  license terms listed above.
//----------------------------------------------------------------------------


#ifndef _DM35424_TEMPERATURE_LIBRARY__H_
#define _DM35424_TEMPERATURE_LIBRARY__H_


#include "dm35424_gbc_library.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DM35424_Temperature_Library_Functions DM35424 Temperature
 * Public Library Functions
 * @{
*/

/**
*******************************************************************************
@brief
   Open the temperature function block, getting address values that will be
   used later by other library functions.

@param
   handle

   Address of the handle pointer, which will contain the device
   descriptor.

@param
   ordinal_to_open

   Which function block on the board to open (0th, 1st, 2nd, etc)

@param
   fb_temp

   Pointer to function block structure that will hold register offset values.

@retval
   0

   Success.

@retval
   Non-Zero

   Failure.
*/
DM35424LIB_API
int DM35424_Temperature_Open(struct DM35424_Board_Descriptor *handle,
				unsigned int ordinal_to_open,
				struct DM35424_Function_Block *fb_temp);


/**
*******************************************************************************
@brief
   Read the temperature of the board, in Celsius degrees.

@param
   handle

   Address of the handle pointer, which will contain the device
   descriptor.

@param
   temp_fb

   Pointer to function block we want to read.

@param
   temperature

   Pointer to a preallocated float. On output holds the temperature in Celsius degree.

@retval
   0

   Success.

@retval
   Non-Zero

   Failure.
*/
DM35424LIB_API
int DM35424_Temperature_Read(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *temp_fb,
							float *temperature);

/**
 * @} DM35424_Temperature_Library_Functions
 */

#ifdef __cplusplus
}
#endif

#endif
