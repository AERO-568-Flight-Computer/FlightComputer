/**
    @file

    @brief
        Function declarations for the DM35424 that are Linux specific.

    $Id: dm35424_os.h 109114 2017-06-09 07:13:20Z prucz $
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


#ifndef _DM35424_BOARD_OS__H_
#define _DM35424_BOARD_OS__H_

#include <pthread.h>

// This forward declaration is made so that board_access.h 
// does not need to be included, which would causes circular dependencies.
struct DM35424_Function_Block;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DM35424_Board_Access_Library DM35424 Board Access Public Library Functions
 * @{
 */

/**
  @brief
  DM35424 board descriptor.  This structure holds information about
  the board as a whole.  It holds the file descriptor and ISR callback
  function, if applicable.
 */

struct DM35424_Board_Descriptor {

	/**
	 * File descriptor for device returned from open()
	 */
	int file_descriptor;

	/**
	 * Function pointer to the user ISR callback function.
	 */
	void (*isr) ();

	/**
	 * Process ID of the child process which will monitor DMA done interrupts.
	 */
	pthread_t pid;
};


/**
*******************************************************************************
@brief
    Initialize the DMA channel and prepare it for data.  Interrupts are disabled,
    error conditions are cleared, buffers are allocated in kernel space and
    their status and controls are cleared.

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
    num_buffers

    Number of DMA buffers to allocate and initialize.

@param
    buffer_size

    The size in bytes to allocate for each buffer.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel or buffer requested.
            ENOMEM	Memory could not be allocated for the DMA buffers.

 */
int
DM35424_Dma_Initialize(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						unsigned int channel,
						unsigned int num_buffers,
						uint32_t buffer_size);


/**
*******************************************************************************
@brief
    Read data from the DMA buffer.  Data is copied from kernel buffers to local
    user-space buffers.

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

    DMA channel containing the buffer to be read.

@param
    buffer_to_read_from

    Buffer in channel to read.

@param
    buffer_size

    Number of bytes to read from the DMA buffer.  In most cases, this should
    be equal to the allocated size of the buffer.

@param
    local_buffer_ptr

    Pointer to local memory buffer already allocated that data will be
    copied into.

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
int
DM35424_Dma_Read(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer_to_read_from,
				uint32_t buffer_size,
				void *local_buffer_ptr);



/**
*******************************************************************************
@brief
    Write data to the DMA buffer.  Data is copied from local user buffers to
    kernel buffers.

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

    DMA channel containing the buffer to be written to.

@param
    buffer_to_write_to

    Buffer in channel to write to.

@param
    buffer_size

    Number of bytes to write to the DMA buffer.  In most cases, this should
    be equal to the allocated size of the buffer.

@param
    local_buffer_ptr

    Pointer to local memory buffer already allocated where data will come
    from.

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
int
DM35424_Dma_Write(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer_to_write_to,
				uint32_t buffer_size,
				void *local_buffer_ptr);



/**
*******************************************************************************
@brief
    Remove the ISR from the system interrupt.

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EFAULT	User ISR was already removed.
 */
int DM35424_General_RemoveISR(struct DM35424_Board_Descriptor *handle);


/**
*******************************************************************************
@brief
    Loop/Poll and wait for an interrupt to happen, then take action.

@param
    ptr

    A void pointer for the board descriptor.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            ENODATA	File descriptor is missing or unreadable.
            EIO		There was no interrupt allocated to this device.
 */
void *DM35424_General_WaitForInterrupt(void *ptr);


/**
*******************************************************************************
@brief
    Start a thread that will sit and wait for an interrupt from the board, and
    call the user ISR when it happens.

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@param
    isr_fnct

    Pointer to the user ISR function that will be executed when an interrupt happens.

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EFAULT	Could not create thread.
 */
int
DM35424_General_InstallISR(struct DM35424_Board_Descriptor *handle, void (*isr_fnct));


/**
*******************************************************************************
@brief
    Set the priority of the user ISR thread

@param
    handle

    Pointer to the device descriptor, which contains the open file id.

@param
    priority

    Attempt to set the priority of the user ISR thread.

@note
    This may require root priviledges

@retval
    0

    Success.

@retval
    -1

    Failure.
    errno may be set as follows:
        @arg \c
            EFAULT	User ISR did not exist.
 */
	int DM35424_General_SetISRPriority(struct DM35424_Board_Descriptor
					   *handle, int priority);

/**
 * @} DM35424_Board_Access_Library
 */

#ifdef __cplusplus
}
#endif

#endif
