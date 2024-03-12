/**
    @file

    @brief
        Definitions for the DM35424 Board Library, a library for accessing
        the board registers


    $Id: dm35424_gbc_library.h 103741 2016-10-17 20:35:58Z rgroner $
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


#ifndef _DM35424_BOARD_LIBRARY__H_
#define _DM35424_BOARD_LIBRARY__H_


#include <stdint.h>
#include <time.h>

#include "dm35424_board_access.h"


#ifdef __cplusplus
extern "C" {
#endif


 /**
  * @defgroup DM35424_Board_Macros DM35424 Board Macros
  * @{
 */

#define	CLOSE_TO_40MHZ(x)	((x) <= 42000000 && (x) >= 38000000)

/**
 * This is the standard clock of the DM35x18 boards
 */
#define CLK_40MHZ		40000000

#define	CLOSE_TO_54MHZ(x)	((x) <= 56700000 && (x) >= 51300000)

#define CLK_54MHZ		54000000

#define	CLOSE_TO_100MHZ(x)	((x) <= 105000000 && (x) >= 95000000)

#define CLK_100MHZ		100000000

#define	CLOSE_TO_57_6MHZ(x)	((x) <= 63360000 && (x) >= 51840000)

#define CLK_57_6MHZ		57600000

#define CLK_50MHZ		50000000
	
#define CLOSE_TO_50MHZ(x)	((x) <= 52500000 && (x) >= 47500000)
/**
 * @} DM35424_Board_Macros
 */


 /**
  * @defgroup DM35424_Board_Library_Functions DM35424 Board Library Public Functions
  * @{
 */

/********************************************************************
* Board Level Functions
*********************************************************************/


/**
**************************************************************************
@brief
    Write the reset value to the correct register to initiate a board-level
    reset.

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Gbc_Board_Reset(struct DM35424_Board_Descriptor *handle);


/**
**************************************************************************
@brief
    Send an End-Of-Interrupt acknowledgement to the board.  This will cause
    any pending interrupts to re-issue.  This is a protection against missing
    interrupts while in the interrupt handler.

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Gbc_Ack_Interrupt(struct DM35424_Board_Descriptor *handle);



/**
**************************************************************************
@brief
    Open a specific function block. Nothing is opened in a file sense, but
    the memory location for the function block is read and certain important
    values are read.  A function block descriptor is allocated to hold the
    data that will be used every time this function block is accessed.

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@param
    number

    Which function block to open.  The first function block on the board
    is at number 0.

@param
    func_block

    Pointer to the function block descriptor.  When the function block info
    is successfully read from the device, then this descriptor will be
    allocated to hold the data.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Function_Block_Open(struct DM35424_Board_Descriptor *handle,
                            unsigned int number,
                            struct DM35424_Function_Block *func_block);


/**
**************************************************************************
@brief
    Open a specific function block module. This is the same as opening a
    function block, except we are looking for a function block with a specific
    type.  This is the method you would use to open the 2nd ADC, for example.

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@param
    fb_type

    Type of function block you want to open.  ADC, DAC, DIO, etc.  The constant
    values are in the dm35424_types.h file.

@param
    number_of_type

    Ordinal number of that particular type of function block that you
    wish to access.  The first instance of that type is 0th.

@param
    func_block

    Pointer to the function block descriptor.  When the function block info
    is successfully read from the device, then this descriptor will be
    allocated to hold the data.
@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Function_Block_Open_Module(struct DM35424_Board_Descriptor *handle,
                            uint32_t fb_type,
                            unsigned int number_of_type,
                            struct DM35424_Function_Block *func_block);


/**
**************************************************************************
@brief
    Get the format ID of the board

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@param
    format_id

    Pointer to the returned format ID value.


@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Gbc_Get_Format(struct DM35424_Board_Descriptor *handle,
				uint8_t *format_id);


/**
**************************************************************************
@brief
    Get the PDP revision number of the board

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@param
    rev

    Pointer to the returned revision value.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Gbc_Get_Revision(struct DM35424_Board_Descriptor *handle,
				uint8_t *rev);


/**
**************************************************************************
@brief
    Get PDP Number of the board

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@param
    pdp_num

    Pointer to the returned PDP Number.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Gbc_Get_Pdp_Number(struct DM35424_Board_Descriptor *handle,
				uint32_t *pdp_num);


/**
**************************************************************************
@brief
    Get the FPGA Build number of the board

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@param
    fpga_build

    Pointer to the returned FPGA Build number.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Gbc_Get_Fpga_Build(struct DM35424_Board_Descriptor *handle,
				uint32_t *fpga_build);


/**
**************************************************************************
@brief
    Get the measured frequency of the system clock of the board.

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@param
    clock_freq

    Pointer to the returned system clock frequency (in Hz)

@param
	is_std_clk

	Boolean value indicating if the clock read is a standard value.  If true,
	then this function will always return the same value upon every call.  If
	false, then this function will return the clock frequency actually read
	from the register.  Note: If the value read from the GBC register is not
	a standard clock, then the clock frequency returned can change from read
	to read by slight variations.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Gbc_Get_Sys_Clock_Freq(struct DM35424_Board_Descriptor *handle,
				uint32_t *clock_freq, int *is_std_clk);



/**
 * @} DM35424_Board_Library_Functions
 */

#ifdef __cplusplus
}
#endif

#endif
