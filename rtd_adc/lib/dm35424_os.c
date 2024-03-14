/**
	@file

	@brief
		DM35424 Board Access library source code

	$Id: dm35424_os.c 119521 2019-04-26 10:32:22Z prucz $
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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "dm35424_ioctl.h"
#include "dm35424_board_access.h"
#include "dm35424_dma_library.h"
#include "dm35424_registers.h"
#include "dm35424_util_library.h"

#define DEVICE_NAME_PATH_PREFIX "/dev/rtd-dm35424"


int DM35424_Dma_Initialize(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int num_buffers,
				uint32_t buffer_size)
{


	int result = 0;
	int buff = 0;
	union dm35424_ioctl_argument ioctl_request;


	/**
	 * Disable interrupts
	 */
	result = DM35424_Dma_Configure_Interrupts(handle,
						func_block,
						channel,
						0,
						0);

	check_result(result, "Error disabling DMA interrupts.");


	result = DM35424_Dma_Clear(handle,
					func_block,
					channel);

	check_result(result, "Error clearing DMA.");


	result = DM35424_Dma_Clear_Interrupt(handle,
						func_block,
						channel,
						1,
						1,
						1,
						1,
						1);

	check_result(result, "Error clearing DMA interrupts.");


	/**
	 * Tell the driver to allocate the space required and to
	 * write the buffer address to the PCI location given.
	 */
	for (buff = 0; buff < num_buffers; buff++) {

		/**
		 * Allocate buffer
		 */
		ioctl_request.dma.pci.region = DM35424_PCI_REGION_FB;
		ioctl_request.dma.pci.size = DM35424_PCI_REGION_ACCESS_32;
		ioctl_request.dma.function = DM35424_DMA_INITIALIZE;
		ioctl_request.dma.channel = channel;
		ioctl_request.dma.fb_num = func_block->fb_num;
		ioctl_request.dma.buffer_size = buffer_size;
		ioctl_request.dma.pci.offset = func_block->dma_channel[channel].buffer_start_offset[buff] +
						DM35424_OFFSET_DMA_BUFFER_ADDRESS;
		ioctl_request.dma.buffer = buff;
        	result = DM35424_Dma(handle, &ioctl_request);
		check_result(result, "Error initializing DMA via ioctl");

		result = DM35424_Dma_Buffer_Set_Size(handle,
							func_block,
							channel,
							buff,
							(buffer_size & DM35424_BIT_MASK_DMA_BUFFER_SIZE));

		check_result(result, "Error setting DMA Buffer size");


		/**
		 * Clear the status
		 */
		result = DM35424_Dma_Reset_Buffer(handle,
							func_block,
							channel,
							buff);

		check_result(result, "Error resetting DMA buffer.");

		/**
		 * Clear the buffer control
		 */
		result = DM35424_Dma_Buffer_Setup(handle,
							func_block,
							channel,
							buff,
							DM35424_DMA_BUFFER_CTRL_CLEAR);

		check_result(result, "Error clearing buffer control register.");


	}

	return 0;

}



int
DM35424_Dma_Read(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer_to_get,
				uint32_t buffer_size,
				void *local_buffer_ptr)
{


	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels ||
		buffer_to_get >= func_block->num_dma_buffers) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.dma.function = DM35424_DMA_READ;
	ioctl_request.dma.channel = channel;
	ioctl_request.dma.fb_num = func_block->fb_num;
	ioctl_request.dma.buffer_size = buffer_size;
	ioctl_request.dma.buffer_ptr = local_buffer_ptr;
	ioctl_request.dma.buffer = buffer_to_get;

	return DM35424_Dma(handle, &ioctl_request);


}


int
DM35424_Dma_Write(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer_to_write_to,
				uint32_t buffer_size,
				void *local_buffer_ptr)
{


	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels ||
	    buffer_to_write_to >= func_block->num_dma_buffers) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.dma.function = DM35424_DMA_WRITE;
	ioctl_request.dma.channel = channel;
	ioctl_request.dma.fb_num = func_block->fb_num;
	ioctl_request.dma.buffer_size = buffer_size;
	ioctl_request.dma.buffer_ptr = local_buffer_ptr;
	ioctl_request.dma.buffer = buffer_to_write_to;

	return DM35424_Dma(handle, &ioctl_request);


}


int DM35424_General_RemoveISR(struct DM35424_Board_Descriptor *handle)
{

	/*
	 * Check to make sure there exists an ISR to remove
	 */

	if (!handle->isr) {
		return -EFAULT;
	}

	/*
	 * Make ISR pointer NULL this will be seen by the thread and is a signal
	 * for it to quit.
	 */

	handle->isr = NULL;
	/*
	 * Join back up with ISR thread
	 */

	ioctl(handle->file_descriptor, DM35424_IOCTL_WAKEUP);

	return pthread_join(handle->pid, NULL);
}


void *DM35424_General_WaitForInterrupt(void *ptr)
{

	fd_set exception_fds;
	fd_set read_fds;
	int status;
	struct DM35424_Board_Descriptor *handle;
	handle = (struct DM35424_Board_Descriptor *) ptr;
	union dm35424_ioctl_argument ioctl_arg;

	while (1) {

		/*
		 * Set up the set of file descriptors that will be watched for input
		 * activity.  Only the DM35424 device file descriptor is of interest.
		 */

		FD_ZERO(&read_fds);
		FD_SET(handle->file_descriptor, &read_fds);

		/*
		 * Set up the set of file descriptors that will be watched for exception
		 * activity.  Only the DM35424 file descriptor is of interest.
		 */

		FD_ZERO(&exception_fds);
		FD_SET(handle->file_descriptor, &exception_fds);

		/*
		 * Wait for the interrupt to happen.  No timeout is given, which means
		 * the process will not be woken up until either an interrupt occurs or
		 * a signal is delivered
		 */

		status = select((handle->file_descriptor) + 1,
				&read_fds, NULL, &exception_fds, NULL);

		/*
		 * The isr should be a null pointer if RemoveISR has been called
		 * This checks if the user has asked the thread to quit.
		 */
		if (handle->isr == NULL) {
			break;
		}

		/*
		 * Check select() error status
		 */

		if (status == -1) {

			/*
			 * Some error occurred.
			 */
			ioctl_arg.interrupt.error_occurred = 2;
			ioctl_arg.interrupt.valid_interrupt = 0;
			(*(handle->isr)) (ioctl_arg.interrupt);
			break;
		}

		if (status == 0) {

			/*
			 * No file descriptors have data available.  Something is broken in the
			 * driver.
			 */

			errno = -ENODATA;
			ioctl_arg.interrupt.error_occurred = 3;
			ioctl_arg.interrupt.valid_interrupt = 0;
			(*(handle->isr)) (ioctl_arg.interrupt);
			break;
		}
		/*
		 * An exception occured, this means that no IRQ line was allocated to
		 * the device when the driver was loaded.
		 */

		if (FD_ISSET(handle->file_descriptor, &exception_fds)) {
			errno = -EIO;
			ioctl_arg.interrupt.error_occurred = 4;
			ioctl_arg.interrupt.valid_interrupt = 0;
			(*(handle->isr)) (ioctl_arg.interrupt);
			break;
		}

		/*
		 * At least one file descriptor has data available and no exception
		 * occured.  Check the device file descriptor to see if it is readable.
		 */

		if (!FD_ISSET(handle->file_descriptor, &read_fds)) {

			/*
			 * The device file is not readable.  This means something is broken.
			 */
			errno = -ENODATA;
			ioctl_arg.interrupt.error_occurred = 5;
			ioctl_arg.interrupt.valid_interrupt = 0;
			(*(handle->isr)) (ioctl_arg.interrupt);
			break;
		}

		status = ioctl(handle->file_descriptor, DM35424_IOCTL_INTERRUPT_GET,
			  &ioctl_arg);

		if (status != 0) {
			ioctl_arg.interrupt.error_occurred = 6;
			ioctl_arg.interrupt.valid_interrupt = 0;
			(*(handle->isr)) (ioctl_arg.interrupt);
			return handle;
		}

		/*
         * As an ioctl call can occur, one more check is needed before calling 
         * the ISR 
         */
		if (handle->isr == NULL) {
            break;
        }
		(*(handle->isr)) (ioctl_arg.interrupt);

		while (ioctl_arg.interrupt.interrupts_remaining > 0) {

			/*
			* Get Next Status
			*/

			status = ioctl(handle->file_descriptor,
					DM35424_IOCTL_INTERRUPT_GET,
					&ioctl_arg);

			if (status != 0) {
				ioctl_arg.interrupt.error_occurred = 7;
				ioctl_arg.interrupt.valid_interrupt = 0;
				ioctl_arg.interrupt.interrupts_remaining = 0;

			}

			/*
             * As an ioctl call can occur, one more check is needed before 
             * calling the ISR 
             */
        
            if (handle->isr == NULL) {
                break;
            }
			
			/*
			* Call the Interrupt Service Routine and pass through the status
			*/
			(*(handle->isr)) (ioctl_arg.interrupt);
		}
	}

	/*
	 * Terminate waiting thread
	 */

	return 0;
}



int
DM35424_General_InstallISR(struct DM35424_Board_Descriptor *handle, void (*isr_fnct))
{
	/*
	 * Check for ISR already installed
	 */

	if (handle->isr != NULL) {
		return -EBUSY;
	}

	/*
	 * Set devices isr to the passed userspace isr
	 */

	handle->isr = isr_fnct;

	/*
	 * Start the thread to wait for the interrupt
	 */

	if (pthread_create
		(&(handle->pid), NULL, DM35424_General_WaitForInterrupt,
		 handle) != 0) {
		errno = EFAULT;
		return -1;
	}

	return 0;
}

int 
DM35424_General_SetISRPriority(struct DM35424_Board_Descriptor *handle,
				   int priority)
{
	struct sched_param param;

	param.sched_priority = priority;
	if (handle->isr == NULL) {
		errno = -EFAULT;
		return -1;
	}

	if (getuid() != 0) {
		return 0;
	}

	return pthread_setschedparam(handle->pid, SCHED_FIFO, &param);
}
