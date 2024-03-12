/**
    @file

    @brief
        Definitions for the DM35424 DMA Library.

    $Id: dm35424_dma_library.h 105018 2016-12-07 15:47:31Z rgroner $
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


#ifndef __DM35424_DMA_LIBRARY_H__
#define __DM35424_DMA_LIBRARY_H__


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*=============================================================================
Public library constants
 =============================================================================*/


 /**
  * @defgroup DM35424_Dma_Library_Constants DM35424 DMA Public Library Constants
  * @{
 */

/**
 * @brief
 *    Register value for DMA clear action.
 */
#define DM35424_DMA_ACTION_CLEAR	0x00

/**
 * @brief
 *    Register value for DMA go action.
 */
#define DM35424_DMA_ACTION_GO		0x01

/**
 * @brief
 *    Register value for DMA pause action.
 */
#define DM35424_DMA_ACTION_PAUSE	0x02

/**
 * @brief
 *    Register value for DMA halt action.
 */
#define DM35424_DMA_ACTION_HALT		0x03

/**
 * @brief
 *    Register value to set DMA to READ direction.
 */
#define DM35424_DMA_SETUP_DIRECTION_READ	0x04

/**
 * @brief
 *    Register value to set DMA to WRITE direction.
 */
#define DM35424_DMA_SETUP_DIRECTION_WRITE	0x00

/**
 * @brief
 *    Register value to set DMA to READ direction.
 */
#define DM35424_DMA_SETUP_DIRECTION_MASK	0x04

/**
 * @brief
 *    Register value to tell DMA to ignore used buffers.
 */
#define DM35424_DMA_SETUP_IGNORE_USED	0x08

/**
 * @brief
 *    Register value to tell DMA to not ignore used buffers.
 */
#define DM35424_DMA_SETUP_NOT_IGNORE_USED	0x00

/**
 * @brief
 *    Bit mask for Ignore Used bit in setup register.
 */
#define DM35424_DMA_SETUP_IGNORE_USED_MASK	0x08

/**
 * @brief
 *    Register value to enabled interrupts in the setup register.
 */
#define DM35424_DMA_SETUP_INT_ENABLE	0x01

/**
 * @brief
 *    Register value to disable interrupts in the setup register.
 */
#define DM35424_DMA_SETUP_INT_DISABLE	0x00

/**
 * @brief
 *    Bit mask for the interrupt bit in the setup register.
 */
#define DM35424_DMA_SETUP_INT_MASK	0x01

/**
 * @brief
 *    Register value to enable the error interrupt.
 */
#define DM35424_DMA_SETUP_ERR_INT_ENABLE	0x02

/**
 * @brief
 *    Register value to disable the error interrupt
 */
#define DM35424_DMA_SETUP_ERR_INT_DISABLE	0x00

/**
 * @brief
 *    Bit mask for the error interrupt bit in the setup register.
 */
#define DM35424_DMA_SETUP_ERR_INT_MASK		0x02

/**
 * @brief
 *    Register value to write to status registers to clear them.
 */
#define DM35424_DMA_STATUS_CLEAR		0x00

/**
 * @brief
 *    Register value to write to control register to clear it.
 */
#define DM35424_DMA_CTRL_CLEAR		0x00

/**
 * @brief
 *    Register value to write to the buffer status register
 *    to clear it.
 */
#define DM35424_DMA_BUFFER_STATUS_CLEAR		0x00

/**
 * @brief
 *    Register value to write to the buffer control register
 *    to clear it.
 */
#define DM35424_DMA_BUFFER_CTRL_CLEAR		0x00

/**
 * @brief
 *    Bit mask for the used buffer bit in the buffer status register.
 */
#define DM35424_DMA_BUFFER_STATUS_USED_MASK		0x01

/**
 * @brief
 *    Bit mask for the terminated buffer bit in the buffer status register.
 */
#define DM35424_DMA_BUFFER_STATUS_TERM_MASK		0x02

/**
 * @brief
 *    Register value to write to buffer control register to mark it as valid.
 */
#define DM35424_DMA_BUFFER_CTRL_VALID		0x01

/**
 * @brief
 *    Register value to write to buffer control register to tell DMA to halt
 *    after processing this buffer.
 */
#define DM35424_DMA_BUFFER_CTRL_HALT		0x02

/**
 * @brief
 *    Register value to write to buffer control register to tell DMA to loop
 *    back to buffer 0 after using this buffer.
 */
#define DM35424_DMA_BUFFER_CTRL_LOOP		0x04

/**
 * @brief
 *    Register value to write to buffer control register to tell DMA to
 *    issue an interrupt after using this buffer.
 */
#define DM35424_DMA_BUFFER_CTRL_INTR		0x08

/**
 * @brief
 *    Register value to write to buffer control register to tell DMA to pause
 *    after processing this buffer.
 */
#define DM35424_DMA_BUFFER_CTRL_PAUSE		0x10

/**
 * @brief
 *    Constant value indicating DMA control block size.
 */
#define DM35424_DMA_CTRL_BLOCK_SIZE		0x10

/**
 * @brief
 *    Constant value indicating DMA buffer control block size.
 */
#define DM35424_DMA_BUFFER_CTRL_BLOCK_SIZE	0x10

/**
 * @brief
 *    Bit mask for the DMA buffer size, since it is 24-bits of
 *    a 32-bit register.
 */
#define DM35424_BIT_MASK_DMA_BUFFER_SIZE	0x0FFFFFF

 /**
  * @brief
  *    Descriptions of the possible states the FIFO might be in.
  */
enum DM35424_Fifo_States {

	/**
	 * State of FIFO is unknown.
	 */
	DM35424_FIFO_UNKNOWN,

	/**
	 * FIFO is empty.
	 */
	DM35424_FIFO_EMPTY,

	/**
	 * FIFO is full
	 */
	DM35424_FIFO_FULL,

	/**
	 * FIFO is between empty and full
	 */
	DM35424_FIFO_HAS_DATA
};

/**
 * @} DM35424_Dma_Library_Constants
 */

/*=============================================================================
Public library functions
 =============================================================================*/

 /**
  * @defgroup DM35424_Dma_Library_Functions DM35424 DMA Public Library Functions
  * @{
 */


/**
*******************************************************************************
@brief
    Start the DMA.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to start.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EBUSY	The action was not executed before timeout.
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int
DM35424_Dma_Start(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel);


/**
*******************************************************************************
@brief
    Stop the DMA.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to stop.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EBUSY	The action was not executed before timeout.
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int
DM35424_Dma_Stop(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel);



/**
*******************************************************************************
@brief
    Pause the DMA.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to pause.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EBUSY	The action was not executed before timeout.
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int
DM35424_Dma_Pause(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel);


/**
*******************************************************************************
@brief
    Clear the DMA.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to clear.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EBUSY	The action was not executed before timeout.
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int
DM35424_Dma_Clear(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel);


/**
*******************************************************************************
@brief
    Get the Read and Write FIFO count values

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to get counts for.

@param
    write_count

    Pointer to the returned number of bytes of space available in the FIFO.

@param
    read_count

    Pointer to the returned number of bytes of data available in the FIFO.

@note
    These counts are valid regardless of the direction of the DMA.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Dma_Get_Fifo_Counts(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				uint16_t *write_count,
				uint16_t *read_count);


/**
*******************************************************************************
@brief
    Get the state of the FIFO

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to get state for.

@param
    state

    Pointer to the returned FIFO state enumeration.

@note
    This is just a convenience function that infers a FIFO state from
    the FIFO counts.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Dma_Get_Fifo_State(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				enum DM35424_Fifo_States *state);



/**
*******************************************************************************
@brief
    Configure the interrupts for the DMA channel.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to configure interrupts for.

@param
    enable

    Boolean value indicating if interrupt is to be enabled or disabled.

@param
    error_enable

    Boolean value indicating if interrupts for error conditions are to be
    enabled or disabled.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Dma_Configure_Interrupts(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int enable,
				int error_enable);


/**
*******************************************************************************
@brief
    Get the configuration of the interrupts for the DMA channel.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to configure interrupts for.

@param
    enable

    Pointer to returned value indicating if interrupt is enabled or disabled.

@param
    error_enable

    Pointer to returned boolean value indicating if interrupts for error
    conditions are enabled or disabled.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Dma_Get_Interrupt_Configuration(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int *enable,
				int *error_enable);


/**
*******************************************************************************
@brief
    Setup the DMA channel, specifically the direction and if used buffers
    are ignored

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to set.

@param
    direction

    Direction for DMA.  See the DMA constants for possible values.

@param
    ignore_used

    Boolean value indicating if used buffers should be ignored.

@note
    This is a convenience function that accomplishes two of the setup
    steps in one function.  This function is identical to calling
    the direction and ignore used library functions separately.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested, or wrong direction
            		requested for function block type.
 */
DM35424LIB_API
int DM35424_Dma_Setup(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int direction,
				int ignore_used);


/**
*******************************************************************************
@brief
    Set the direction of the DMA, read or write.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to set.

@param
    direction

    Direction for DMA.  See the DMA constants for possible values.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested, or wrong direction
            		requested for function block type.
 */
DM35424LIB_API
int DM35424_Dma_Setup_Set_Direction(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int direction);


/**
*******************************************************************************
@brief
    Set the DMA channel to ignore or not ignore a used buffer.  Ignoring used
    buffers is mostly useful when outputting a repeating data cycle.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to set.

@param
    ignore_used

    Boolean value indicating if used buffers should be ignored.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Dma_Setup_Set_Used(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int ignore_used);


/**
*******************************************************************************
@brief
    Get the current value of the DMA channel error registers.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to configure interrupts for.

@param
    stat_overflow

    Pointer to the returned boolean indicating if overflow has occurred.

@param
    stat_underflow

    Pointer to the returned boolean indicating if underflow has occurred.

@param
    stat_used

    Pointer to the returned boolean indicating if the DMA attempted to use
    an already used buffer.

@param
    stat_invalid

    Pointer to the returned boolean indicating if the DMA attempted to use
    an invalid buffer.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Dma_Get_Errors(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int *stat_overflow,
				int *stat_underflow,
				int *stat_used,
				int *stat_invalid);

/**
*******************************************************************************
@brief
    Get the current status of the DMA channel.  Determine which buffer it is
    using, what its current action is, and the state of all error conditions
    and normal interrupt conditions.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to configure interrupts for.

@param
    current_buffer

    Pointer to the returned current buffer the DMA is using.

@param
    current_count

    Pointer to the returned count for the current buffer.  This indicates how
    far into the buffer the DMA is.

@param
    current_action

    Pointer to the returned action the DMA is currently taking.

@param
    stat_overflow

    Pointer to the returned boolean indicating if overflow has occurred.

@param
    stat_underflow

    Pointer to the returned boolean indicating if underflow has occurred.

@param
    stat_used

    Pointer to the returned boolean indicating if the DMA attempted to use
    an already used buffer.

@param
    stat_invalid

    Pointer to the returned boolean indicating if the DMA attempted to use
    an invalid buffer.

@param
    stat_complete

    Pointer to the returned boolean indicating if the DMA has completed using
    a buffer that had an interrupt set.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Dma_Status(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				uint32_t *current_buffer,
				uint32_t *current_count,
				int *current_action,
				int *stat_overflow,
				int *stat_underflow,
				int *stat_used,
				int *stat_invalid,
				int *stat_complete);


/**
*******************************************************************************
@brief
    Get the current buffer and buffer count in use by the DMA.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    Channel to get buffer info from.

@param
    current_buffer

    Pointer to the returned current buffer the DMA is using.

@param
    current_count

    Pointer to the returned count for the current buffer.  This indicates how
    far into the buffer the DMA is.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dma_Get_Current_Buffer_Count(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					uint32_t *current_buffer,
					uint32_t *current_count);


/**
*******************************************************************************
@brief
    Check the DMA channel for any error conditions.  This just returns a simple
    boolean as quickly as possible.  If there is an error condition, you will
    have to query the DMA again to determine what the error is.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to set.

@param
    has_error

    Pointer to returned boolean value indicating if the DMA channel has an
    error condition.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Dma_Check_For_Error(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int *has_error);



/**
*******************************************************************************
@brief
    Setup the DMA buffer for use.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to set.

@param
    buffer

    DMA buffer to set.

@param
    ctrl
 * 
 *  Unsigned short containing control bits. Will be written to control register.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel or buffer requested.
 */
DM35424LIB_API
int DM35424_Dma_Buffer_Setup(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer,
				uint8_t ctrl);


/**
*******************************************************************************
@brief
    Get the status of the buffer.  This gets the status, control, and size
    registers.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to get.

@param
    buffer

    DMA buffer to get.

@param
    status

    Pointer to returned DMA buffer status register value.

@param
    control

    Pointer to returned DMA buffer control register value.

@param
    size

    Pointer to returned DMA buffer size (in bytes).

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel or buffer requested.
 */
DM35424LIB_API
int DM35424_Dma_Buffer_Status(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer,
				uint8_t *status,
				uint8_t *control,
				uint32_t *size);


/**
*******************************************************************************
@brief
    Check if the indicated buffer has the "Used" flag set

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA Channel to get.

@param
    buffer_num

    Which buffer in the DMA channel to check.

@param
    is_used

    Pointer to returned boolean indicating if the buffer has the Used
    flag set.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */

DM35424LIB_API
int DM35424_Dma_Check_Buffer_Used(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer_num,
				int *is_used);



/**
*******************************************************************************
@brief
    Find which DMA channel has an interrupt condition, whether from using a
    buffer with interrupt set, or from an error.  DMA channels are evaluated
    starting at Channel 0.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    Pointer to returned DMA channel with interrupt, if any.

@param
    channel_complete

    Pointer to returned boolean indicating that the interrupt on this channel
    was from using a buffer with the interrupt bit set.

@param
    channel_error

    Pointer to returned boolean indicating that the interrupt on this channel
    was from an error condition.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.

@warning
    This function is not compatible with the Windows driver package and is
    therefore not included in the Windows DLL.
 */
int
DM35424_Dma_Find_Interrupt(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int *channel,
				int *channel_complete,
				int *channel_error);


/**
*******************************************************************************
@brief
    Clear the interrupt flag from a DMA channel.  Clearing the flags will allow
    another interrupt of the same type to occur again, and is the normal
    operation after handling the interrupt itself.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA channel to clear.

@param
    clear_overflow

    Boolean indicating whether or not to clear the overflow interrupt status.

@param
    clear_underflow

    Boolean indicating whether or not to clear the underflow interrupt status.

@param
    clear_used

    Boolean indicating whether or not to clear the used interrupt status.

@param
    clear_invalid

    Boolean indicating whether or not to clear the invalid buffer interrupt
    status.

@param
    clear_complete

    Boolean indicating whether or not to clear the buffer completed interrupt
    status.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.

@warning
    This function is not compatible with the Windows driver package and is
    therefore not included in the Windows DLL.
 */
int
DM35424_Dma_Clear_Interrupt(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					int clear_overflow,
					int clear_underflow,
					int clear_used,
					int clear_invalid,
					int clear_complete);


/**
*******************************************************************************
@brief
    Reset the DMA buffer, preparing it to be used again by the DMA engine.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DMA channel containing the buffer.

@param
    buffer

    Buffer in channel to clear.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel or buffer requested.

@warning
    This function is not compatible with the Windows driver package and is
    therefore not included in the Windows DLL.
 */
int
DM35424_Dma_Reset_Buffer(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer);


DM35424LIB_API
int DM35424_Dma_Buffer_Get_Size(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					unsigned int buffer,
					uint32_t *buffer_size);


int DM35424_Dma_Buffer_Set_Size(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer,
				uint32_t buffer_size);

/**
 * @} DM35424_Dma_Library_Functions
 */


#ifdef __cplusplus
}
#endif

#endif
