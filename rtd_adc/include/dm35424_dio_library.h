/**
    @file

    @brief
        Definitions for the DM35424 DIO Library

    $Id: dm35424_dio_library.h 84663 2014-12-19 22:03:35Z mmcintire $
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


#ifndef _DM35424_DIO_LIBRARY__H_
#define _DM35424_DIO_LIBRARY__H_


#include "dm35424_gbc_library.h"

#ifdef __cplusplus
extern "C" {
#endif

 /**
  * @defgroup DM35424_Dio_Library_Functions DM35424 DIO Public
  * Library Functions
  * @{
 */


/**
*******************************************************************************
@brief
    Open the DIO indicated, and determine register locations of control
    blocks needed to control it.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    number_of_type

    Which DIO to open.  The first DIO on the board will be 0.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dio_Open(struct DM35424_Board_Descriptor *handle,
			unsigned int number_of_type,
			struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Set the direction of the DIO pins.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block representing the DIO.

@param
    direction

    Bitmask representing the directions to set the pins.  (0 = input,
    1 = output)

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dio_Set_Direction(struct DM35424_Board_Descriptor *handle,
			const struct DM35424_Function_Block *func_block,
			uint32_t direction);


/**
*******************************************************************************
@brief
    Get the direction of the DIO pins.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block representing the DIO.

@param
    direction

    Bitmask representing the directions of the pins.  (0 = input, 1 = output)

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dio_Get_Direction(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t *direction);


/**
*******************************************************************************
@brief
    Get the input value of the DIO.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block representing the DIO.

@param
    value

    Pointer to returned value.

@note
    The value of pins that are set to output will be zero.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API 
int DM35424_Dio_Get_Input_Value(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t *value);

/**
*******************************************************************************
@brief
    Get the current value of the output register.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block representing the DIO.

@param
    value

    Pointer to returned value.

@note
    The value of pins that are set to input will be zero.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dio_Get_Output_Value(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t *value);


/**
*******************************************************************************
@brief
    Set the value to be put on output pins.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block representing the DIO.

@param
    value

    Value to be written to output pins.

@note
    Writing a bit to a pin set to input will have no effect.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dio_Set_Output_Value(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t value);


/**
 * @} DM35424_Dio_Library_Functions
 */

#ifdef __cplusplus
}
#endif

#endif
