/**
	@file

	@brief
		DM35424 DMA library source code


	$Id: librtd-dm35424_dma.c 105018 2016-12-07 15:47:31Z rgroner $
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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "dm35424_registers.h"
#include "dm35424_board_access.h"
#include "dm35424_dma_library.h"
#include "dm35424_util_library.h"
#include "dm35424_board_access_structs.h"

#define DM35424_DMA_ACTION_LOOPS	10
#define DM35424_LAST_ACTION_SLEEP_USEC	1000


/***********************************************************************************
 Private DMA Library functions
 ***********************************************************************************/

static int
DM35424_Validate_Dma_Channel_Setup_Args(int dma_channel_direction)
{

	if (dma_channel_direction != DM35424_DMA_SETUP_DIRECTION_READ &&
	    dma_channel_direction != DM35424_DMA_SETUP_DIRECTION_WRITE) {
		return -1;
	}

	return 0;
}


static int
DM35424_Dma_Channel_Get_Last_Action(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					uint8_t *last_action)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;


	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_LAST_ACTION;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_code = DM35424_Read(handle, &ioctl_request);

	*last_action = ioctl_request.readwrite.access.data.data8;

	return return_code;

}


/***********************************************************************************
 Public DMA Library functions
 ***********************************************************************************/
DM35424LIB_API
int
DM35424_Dma_Start(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel)
{

	int return_code;
	union dm35424_ioctl_argument ioctl_request;
	unsigned int loop_count = 0;
	uint8_t last_action = 5;


	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_ACTION;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_DMA_ACTION_GO;

	return_code = DM35424_Write(handle, &ioctl_request);

	if (return_code != 0) {
		return -1;
	}

	/**
	 * Before returning, we need to check that the action has been
	 * taken by the DMA.  We do this by comparing the action and
	 * last action register.  By comparing them directly (instead
	 * of comparing the last action to the intended action),
	 * we handle any cases where the intended action results in a
	 * different action (usually HALT) due to system setup.
	 */
	return_code = DM35424_Dma_Channel_Get_Last_Action(handle,
							func_block,
							channel,
							&last_action);

	while ((last_action != ioctl_request.readwrite.access.data.data8) &&
		(loop_count < DM35424_DMA_ACTION_LOOPS) &&
		(return_code == 0)) {

		DM35424_Micro_Sleep(DM35424_LAST_ACTION_SLEEP_USEC);

		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return -1;
		}

		return_code = DM35424_Dma_Channel_Get_Last_Action(handle,
								func_block,
								channel,
								&last_action);

		loop_count ++;
	}

	if (last_action != ioctl_request.readwrite.access.data.data8) {
		if (return_code == 0) {
			errno = EBUSY;
		}
		return -1;
	}

	return return_code;

}

DM35424LIB_API
int
DM35424_Dma_Stop(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel)
{

	int return_code;
	union dm35424_ioctl_argument ioctl_request;
	unsigned int loop_count = 0;
	uint8_t last_action = 5;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_ACTION;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_DMA_ACTION_HALT;


	return_code = DM35424_Write(handle, &ioctl_request);

	if (return_code != 0) {
		return -1;
	}

	/**
	 * Before returning, we need to check that the action has been
	 * taken by the DMA.  We do this by comparing the action and
	 * last action register.  By comparing them directly (instead
	 * of comparing the last action to the intended action),
	 * we handle any cases where the action results in a
	 * different action (usually HALT) due to system setup.
	 */
	return_code = DM35424_Dma_Channel_Get_Last_Action(handle,
							func_block,
							channel,
							&last_action);

	while ((last_action != ioctl_request.readwrite.access.data.data8) &&
		(loop_count < DM35424_DMA_ACTION_LOOPS) &&
		(return_code == 0)) {

		DM35424_Micro_Sleep(DM35424_LAST_ACTION_SLEEP_USEC);

		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return -1;
		}

		return_code = DM35424_Dma_Channel_Get_Last_Action(handle,
								func_block,
								channel,
								&last_action);

		loop_count ++;
	}

	if (last_action != ioctl_request.readwrite.access.data.data8) {
		if (return_code == 0) {
			errno = EBUSY;
		}
		return -1;
	}

	return return_code;

}


DM35424LIB_API
int
DM35424_Dma_Pause(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel)
{

	int return_code;
	union dm35424_ioctl_argument ioctl_request;
	unsigned int loop_count = 0;
	uint8_t last_action = 5;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_ACTION;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_DMA_ACTION_PAUSE;


	return_code = DM35424_Write(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	/**
	 * Before returning, we need to check that the action has been
	 * taken by the DMA.  We do this by comparing the action and
	 * last action register.  By comparing them directly (instead
	 * of comparing the last action to the intended action),
	 * we handle any cases where the action results in a
	 * different action (usually HALT) due to system setup.
	 */
	return_code = DM35424_Dma_Channel_Get_Last_Action(handle,
							func_block,
							channel,
							&last_action);

	while ((last_action != ioctl_request.readwrite.access.data.data8) &&
		(loop_count < DM35424_DMA_ACTION_LOOPS) &&
		(return_code == 0)) {

		DM35424_Micro_Sleep(DM35424_LAST_ACTION_SLEEP_USEC);

		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return -1;
		}

		return_code = DM35424_Dma_Channel_Get_Last_Action(handle,
								func_block,
								channel,
								&last_action);

		loop_count ++;
	}

	if (last_action != ioctl_request.readwrite.access.data.data8) {
		if (return_code == 0) {
			errno = EBUSY;
		}
		return -1;
	}

	return return_code;
}


DM35424LIB_API
int
DM35424_Dma_Clear(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel)

{
	int return_code;
	union dm35424_ioctl_argument ioctl_request;
	unsigned int loop_count = 0;
	uint8_t last_action = 5;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}
	/**
	 * Stop DMA, and clear it
	 */
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_ACTION;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_DMA_ACTION_CLEAR;


	return_code = DM35424_Write(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	/**
	 * Due to the way the DMA works, it's possible that after we tell the DMA
	 * to clear, that the controller came back and set it to Halt instead.
	 * So we need to readback the DMA action and make sure it is still set
	 * to clear.  If it is not, then we give it one more try before throwing
	 * an error.
	 */
	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	if (ioctl_request.readwrite.access.data.data8 != DM35424_DMA_ACTION_CLEAR) {
		ioctl_request.readwrite.access.data.data8 = DM35424_DMA_ACTION_CLEAR;
		return_code = DM35424_Write(handle, &ioctl_request);

		if (return_code != 0) {
			return return_code;
		}

		return_code = DM35424_Read(handle, &ioctl_request);

		if (ioctl_request.readwrite.access.data.data8 != DM35424_DMA_ACTION_CLEAR) {
			errno = EBUSY;
			return_code = -1;
		}
	}

	if (return_code != 0) {
		return return_code;
	}

	/**
	 * Before returning, we need to check that the action has been
	 * taken by the DMA.  We do this by comparing the action and
	 * last action register.  By comparing them directly (instead
	 * of comparing the last action to the intended action),
	 * we handle any cases where the action results in a
	 * different action (usually HALT) due to system setup.
	 */
	return_code = DM35424_Dma_Channel_Get_Last_Action(handle,
							func_block,
							channel,
							&last_action);

	while ((last_action != ioctl_request.readwrite.access.data.data8) &&
		(loop_count < DM35424_DMA_ACTION_LOOPS) &&
		(return_code == 0)) {

		DM35424_Micro_Sleep(DM35424_LAST_ACTION_SLEEP_USEC);

		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return -1;
		}

		return_code = DM35424_Dma_Channel_Get_Last_Action(handle,
								func_block,
								channel,
								&last_action);

		loop_count ++;
	}

	if (last_action != ioctl_request.readwrite.access.data.data8) {
		if (return_code == 0) {
			errno = EBUSY;
		}
		return -1;
	}

	return return_code;


}




DM35424LIB_API
int DM35424_Dma_Get_Fifo_Counts(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				uint16_t *write_count,
				uint16_t *read_count)
{

	int return_code = 1;
	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
	if (write_count != NULL) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
							DM35424_OFFSET_DMA_WR_FIFO_CNT;


		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return return_code;
		}
		*write_count = ioctl_request.readwrite.access.data.data16;

	}


	if (read_count != NULL) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
							DM35424_OFFSET_DMA_RD_FIFO_CNT;

		return_code = DM35424_Read(handle, &ioctl_request);

		*read_count = ioctl_request.readwrite.access.data.data16;
	}

	return return_code;

}


DM35424LIB_API
int DM35424_Dma_Get_Fifo_State(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				enum DM35424_Fifo_States *state)
{

	uint16_t read_count = 0, write_count = 0;
	int return_code = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	*state = DM35424_FIFO_UNKNOWN;

	return_code = DM35424_Dma_Get_Fifo_Counts(handle,
							func_block,
							channel,
							&write_count,
							&read_count);

	if (return_code != 0) {
		return return_code;
	}

	if (read_count & 0x8000) {
		*state = DM35424_FIFO_EMPTY;
	}
	else if (write_count & 0x8000) {
		*state = DM35424_FIFO_FULL;
	}
	else {
		*state = DM35424_FIFO_HAS_DATA;
	}

	return return_code;


}


DM35424LIB_API
int DM35424_Dma_Configure_Interrupts(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int enable,
				int error_enable)
{

	uint8_t val8b = 0;
	uint8_t mask8b = 0;
	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	/**
	  * Set the DMA interrupt enable
	  */
	ioctl_request.modify.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_SETUP;
	ioctl_request.modify.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.modify.access.size = DM35424_PCI_REGION_ACCESS_8;

	if (enable) {
		val8b = DM35424_DMA_SETUP_INT_ENABLE;
	}
	if (error_enable) {
		val8b |= DM35424_DMA_SETUP_ERR_INT_ENABLE;
	}

	mask8b = DM35424_DMA_SETUP_INT_MASK;
	mask8b |= DM35424_DMA_SETUP_ERR_INT_MASK;

	ioctl_request.modify.access.data.data8 = val8b;
	ioctl_request.modify.mask.mask8 = mask8b;

	return DM35424_Modify(handle, &ioctl_request);

}


DM35424LIB_API
int DM35424_Dma_Get_Interrupt_Configuration(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int *enable,
				int *error_enable)
{


	int return_code;
	union dm35424_ioctl_argument ioctl_request;
	uint8_t setup_value;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_SETUP;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	return_code = DM35424_Read(handle, &ioctl_request);

	setup_value = ioctl_request.readwrite.access.data.data8;

	*enable = setup_value & 0x01;

	setup_value >>= 1;

	*error_enable = setup_value & 0x01;

	return return_code;


}



DM35424LIB_API
int DM35424_Dma_Setup(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int direction,
				int ignore_used)
{


	union dm35424_ioctl_argument ioctl_request;
	int return_code;
	uint8_t val = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	return_code = DM35424_Validate_Dma_Channel_Setup_Args(direction);

	if (return_code != 0) {
		return return_code;
	}

	val = (uint8_t) direction;

	if (ignore_used) {
		val |= DM35424_DMA_SETUP_IGNORE_USED;

	}


	ioctl_request.modify.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.modify.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.modify.mask.mask8 = DM35424_DMA_SETUP_IGNORE_USED_MASK |
					  DM35424_DMA_SETUP_DIRECTION_MASK;
	ioctl_request.modify.access.data.data8 = val;
	ioctl_request.modify.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_SETUP;

	return DM35424_Modify(handle, &ioctl_request);


}



DM35424LIB_API
int DM35424_Dma_Setup_Set_Direction(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int direction)
{


	union dm35424_ioctl_argument ioctl_request;
	int return_code;
	uint8_t val = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	return_code = DM35424_Validate_Dma_Channel_Setup_Args(direction);

	if (return_code != 0) {
		return return_code;
	}

	val = (uint8_t) direction;


	ioctl_request.modify.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.modify.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.modify.mask.mask8 = DM35424_DMA_SETUP_DIRECTION_MASK;
	ioctl_request.modify.access.data.data8 = val;
	ioctl_request.modify.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_SETUP;

	return DM35424_Modify(handle, &ioctl_request);


}




DM35424LIB_API
int DM35424_Dma_Setup_Set_Used(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int ignore_used)
{


	union dm35424_ioctl_argument ioctl_request;
	uint8_t val = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	if (ignore_used) {
		val |= DM35424_DMA_SETUP_IGNORE_USED;

	}

	ioctl_request.modify.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.modify.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.modify.mask.mask8 = DM35424_DMA_SETUP_IGNORE_USED_MASK;
	ioctl_request.modify.access.data.data8 = val;
	ioctl_request.modify.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_SETUP;

	return DM35424_Modify(handle, &ioctl_request);


}


DM35424LIB_API
int DM35424_Dma_Get_Errors(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int *stat_overflow,
				int *stat_underflow,
				int *stat_used,
				int *stat_invalid)
{

	uint32_t value;

	union dm35424_ioctl_argument ioctl_request;
	int return_code = 1;

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	if (stat_overflow != NULL || stat_underflow != NULL) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset + DM35424_OFFSET_DMA_STAT_OVERFLOW;
		ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;

		return_code = DM35424_Read(handle, &ioctl_request);

		value = ioctl_request.readwrite.access.data.data16;

		if (stat_overflow != NULL) {
			*stat_overflow = (value & 0xFF);
		}

		if (stat_underflow != NULL) {
			*stat_underflow = value >> 8;
		}

	}

	if (stat_used != NULL || stat_invalid != NULL) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset + DM35424_OFFSET_DMA_STAT_USED;
		ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;

		return_code = DM35424_Read(handle, &ioctl_request);

		value = ioctl_request.readwrite.access.data.data16;

		if (stat_used != NULL) {
			*stat_used = (value & 0xFF);
		}

		if (stat_invalid != NULL) {
			*stat_invalid = value >> 8;
		}

	}


	return return_code;

}


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
				int *stat_complete)
{

	uint32_t value;

	union dm35424_ioctl_argument ioctl_request;
	int return_code = 1;

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	if (current_buffer != NULL || current_count != NULL) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset + DM35424_OFFSET_DMA_CURRENT_COUNT;

		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return return_code;
		}

		value = ioctl_request.readwrite.access.data.data32;

		if (current_count != NULL) {
			*current_count = (value & 0x00FFFFFF);
		}

		if (current_buffer != NULL) {
			*current_buffer = (value >> 24);
		}
	}

	if (current_action != NULL) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset + DM35424_OFFSET_DMA_ACTION;
		ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return return_code;
		}

		*current_action = ioctl_request.readwrite.access.data.data8;
	}

	if (stat_overflow != NULL || stat_underflow != NULL || stat_used != NULL || stat_invalid != NULL) {
        return_code = DM35424_Dma_Get_Errors(handle,
                            func_block,
                            channel,
                            stat_overflow,
                            stat_underflow,
                            stat_used,
                            stat_invalid);

        if (return_code != 0) {
            return return_code;
        }
    }

	if (stat_complete != NULL) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset + DM35424_OFFSET_DMA_STAT_COMPLETE;
		ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return -1;
		}

		*stat_complete = ioctl_request.readwrite.access.data.data8;
	}
	return return_code;

}



DM35424LIB_API
int DM35424_Dma_Get_Current_Buffer_Count(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					uint32_t *current_buffer,
					uint32_t *current_count)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;


	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
						DM35424_OFFSET_DMA_CURRENT_COUNT;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	return_code = DM35424_Read(handle, &ioctl_request);

	*current_count = (ioctl_request.readwrite.access.data.data32 & 0x00FFFFFF);

	*current_buffer = (ioctl_request.readwrite.access.data.data32 >> 24);

	return return_code;

}




DM35424LIB_API
int DM35424_Dma_Check_For_Error(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				int *has_error)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code = 1;

	*has_error = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
							DM35424_OFFSET_DMA_STAT_OVERFLOW;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	if (ioctl_request.readwrite.access.data.data16) {
		*has_error = 1;
		return 0;
	}

	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
							DM35424_OFFSET_DMA_STAT_USED;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (ioctl_request.readwrite.access.data.data16) {
		*has_error = 1;
		return 0;
	}

	return return_code;

}



DM35424LIB_API
int DM35424_Dma_Buffer_Setup(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer,
				uint8_t ctrl)
{

	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels ||
	    buffer >= func_block->num_dma_buffers) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;

	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.offset =
			func_block->dma_channel[channel].buffer_start_offset[buffer] +
			DM35424_OFFSET_DMA_BUFFER_CTRL;
	ioctl_request.readwrite.access.data.data8 = ctrl;

	return DM35424_Write(handle, &ioctl_request);


}



DM35424LIB_API
int DM35424_Dma_Buffer_Status(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer,
				uint8_t *status,
				uint8_t *control,
				uint32_t *size)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	if (channel >= func_block->num_dma_channels ||
	    buffer >= func_block->num_dma_buffers) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.offset =
			func_block->dma_channel[channel].buffer_start_offset[buffer] +
			DM35424_OFFSET_DMA_BUFFER_STAT;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	*status = ioctl_request.readwrite.access.data.data8;

	ioctl_request.readwrite.access.offset =
			func_block->dma_channel[channel].buffer_start_offset[buffer] +
			DM35424_OFFSET_DMA_BUFFER_CTRL;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	*control = ioctl_request.readwrite.access.data.data8;

	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.offset =
			func_block->dma_channel[channel].buffer_start_offset[buffer] +
			DM35424_OFFSET_DMA_BUFFER_SIZE;

	return_code = DM35424_Read(handle, &ioctl_request);

	*size = DM35424_BIT_MASK_DMA_BUFFER_SIZE & ioctl_request.readwrite.access.data.data32;

	return return_code;
}



DM35424LIB_API
int DM35424_Dma_Check_Buffer_Used(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer_num,
				int *is_used)
{


	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	*is_used = 0;

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].buffer_start_offset[buffer_num] +
										DM35424_OFFSET_DMA_BUFFER_STAT;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (ioctl_request.readwrite.access.data.data8 & DM35424_DMA_BUFFER_STATUS_USED_MASK) {
		*is_used = 1;
	}

	return return_code;
}


int
DM35424_Dma_Find_Interrupt(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int *channel,
				int *channel_complete,
				int *channel_error)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	uint32_t stat_over_under;
	uint32_t stat_used_inv_comp;
    uint8_t  ignore_used;

	*channel_complete = 0;
	*channel = 0;
	*channel_error = 0;

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;

	do {
		ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
		ioctl_request.readwrite.access.offset = func_block->dma_channel[*channel].control_offset +
							DM35424_OFFSET_DMA_STAT_OVERFLOW;

		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return return_code;
		}

		stat_over_under = ioctl_request.readwrite.access.data.data16;

		if (stat_over_under) {
			*channel_error = 1;
		}

		ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
		ioctl_request.readwrite.access.offset = func_block->dma_channel[*channel].control_offset +
							DM35424_OFFSET_DMA_STAT_USED;

		return_code = DM35424_Read(handle, &ioctl_request);

		if (return_code != 0) {
			return return_code;
		}

		stat_used_inv_comp = ioctl_request.readwrite.access.data.data32 & 0xFFFFFF;

		if (stat_used_inv_comp & 0xFF00) {
            // Invalid
			*channel_error = 1;
		}

		if (stat_used_inv_comp & 0x00FF) {
            // Used
            // See if the "Ignore Used" bit is set.  If so, ignore the "used" bit
            ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
            ioctl_request.readwrite.access.offset = func_block->dma_channel[*channel].control_offset +
                            DM35424_OFFSET_DMA_SETUP;

            return_code = DM35424_Read(handle, &ioctl_request);

            if (return_code != 0) {
                return return_code;
            }

            ignore_used = ioctl_request.readwrite.access.data.data8 & DM35424_DMA_SETUP_IGNORE_USED;

            if (!ignore_used) {
                *channel_error = 1;
            }
		}

		if (stat_used_inv_comp & 0x00FF0000) {

			*channel_complete = 1;
		}

        	if( !*channel_complete && !*channel_error) {

			(*channel)++;
		}


	} while (*channel_complete == 0 && *channel_error == 0 &&
			*channel < func_block->num_dma_channels);

	return 0;

}



int
DM35424_Dma_Clear_Interrupt(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					int clear_overflow,
					int clear_underflow,
					int clear_used,
					int clear_invalid,
					int clear_complete)
{

	union dm35424_ioctl_argument ioctl_request;
	int result;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_DMA_STATUS_CLEAR;

	if (clear_overflow) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
							DM35424_OFFSET_DMA_STAT_OVERFLOW;

		result = DM35424_Write(handle, &ioctl_request);

		if (result != 0) {
			return result;
		}

	}

	if (clear_underflow) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
							DM35424_OFFSET_DMA_STAT_UNDERFLOW;

		result = DM35424_Write(handle, &ioctl_request);

		if (result != 0) {
			return result;
		}

	}
	if (clear_used) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
							DM35424_OFFSET_DMA_STAT_USED;

		result = DM35424_Write(handle, &ioctl_request);

		if (result != 0) {
			return result;
		}

	}
	if (clear_invalid) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
							DM35424_OFFSET_DMA_STAT_INVALID;

		result = DM35424_Write(handle, &ioctl_request);

		if (result != 0) {
			return result;
		}

	}
	if (clear_complete) {
		ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].control_offset +
							DM35424_OFFSET_DMA_STAT_COMPLETE;

		result = DM35424_Write(handle, &ioctl_request);

		if (result != 0) {
			return result;
		}

	}

	return result;

}


int
DM35424_Dma_Reset_Buffer(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer)
{
	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels ||
	    buffer >= func_block->num_dma_buffers) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].buffer_start_offset[buffer] +
						DM35424_OFFSET_DMA_BUFFER_STAT;
	ioctl_request.readwrite.access.data.data8 = DM35424_DMA_STATUS_CLEAR;

	return DM35424_Write(handle, &ioctl_request);

}


DM35424LIB_API
int DM35424_Dma_Buffer_Get_Size(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					unsigned int buffer,
					uint32_t *buffer_size)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.offset = func_block->dma_channel[channel].buffer_start_offset[buffer] +
						DM35424_OFFSET_DMA_BUFFER_SIZE;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	return_code = DM35424_Read(handle, &ioctl_request);

	*buffer_size = (ioctl_request.readwrite.access.data.data32 & DM35424_BIT_MASK_DMA_BUFFER_SIZE);

	return return_code;
}


int DM35424_Dma_Buffer_Set_Size(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				unsigned int buffer,
				uint32_t buffer_size)
{


	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels ||
	    buffer >= func_block->num_dma_buffers) {
		errno = EINVAL;
		return -1;
	}

	if ((buffer_size & 0x3) || (buffer_size > DM35424_BIT_MASK_DMA_BUFFER_SIZE)) {
		errno = EINVAL;
		return -1;
	}
	
	/**
	 * Set the buffer size
	 */
	ioctl_request.modify.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.modify.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.modify.mask.mask32 = DM35424_BIT_MASK_DMA_BUFFER_SIZE;
	ioctl_request.modify.access.data.data32 = buffer_size;
	ioctl_request.modify.access.offset = func_block->dma_channel[channel].buffer_start_offset[buffer] +
						DM35424_OFFSET_DMA_BUFFER_SIZE;

	return DM35424_Modify(handle, &ioctl_request);
	
}
