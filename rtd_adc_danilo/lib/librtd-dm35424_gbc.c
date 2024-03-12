/**
	@file

	@brief
		DM35424 Board library source code


	$Id: librtd-dm35424_gbc.c 104760 2016-11-28 20:17:52Z rgroner $
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
#include <assert.h>

#include "dm35424_registers.h"
#include "dm35424_board_access.h"
#include "dm35424_gbc_library.h"
#include "dm35424_util_library.h"
#include "dm35424_dma_library.h"
#include "dm35424_board_access_structs.h"


#define DM35424_RESET_DELAY_MICRO_SEC	1000

DM35424LIB_API
int DM35424_Gbc_Board_Reset(struct DM35424_Board_Descriptor *handle)
{
	union dm35424_ioctl_argument ioctl_request;
	int return_value;

	ioctl_request.readwrite.access.data.data8 = DM35424_BOARD_RESET_VALUE;
	ioctl_request.readwrite.access.offset = DM35424_OFFSET_GBC_BOARD_RESET;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_value = DM35424_Write(handle, &ioctl_request);

	if (return_value == 0) {

		/* Sleep for 1 millisecond, which gives the board time to
		 * come back up from the reset.
		 */
		DM35424_Micro_Sleep(DM35424_RESET_DELAY_MICRO_SEC);
	}

	return return_value;
}


DM35424LIB_API
int DM35424_Gbc_Ack_Interrupt(struct DM35424_Board_Descriptor *handle)
{
	union dm35424_ioctl_argument ioctl_request;

	ioctl_request.readwrite.access.data.data8 = DM35424_BOARD_ACK_INTERRUPT;
	ioctl_request.readwrite.access.offset = DM35424_OFFSET_GBC_END_INTERRUPT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return DM35424_Write(handle, &ioctl_request);

}


DM35424LIB_API
int DM35424_Gbc_Get_Format(struct DM35424_Board_Descriptor *handle,
				uint8_t *format_id)
{
	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = DM35424_OFFSET_GBC_FORMAT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	return_code = DM35424_Read(handle, &ioctl_request);

	*format_id = ioctl_request.readwrite.access.data.data8;

	return return_code;

}


DM35424LIB_API
int DM35424_Gbc_Get_Revision(struct DM35424_Board_Descriptor *handle,
				uint8_t *rev)
{
	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = DM35424_OFFSET_GBC_REV;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	return_code = DM35424_Read(handle, &ioctl_request);

	*rev = ioctl_request.readwrite.access.data.data8;

	return return_code;

}


DM35424LIB_API
int DM35424_Gbc_Get_Pdp_Number(struct DM35424_Board_Descriptor *handle,
				uint32_t *pdp_num)
{
	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = DM35424_OFFSET_GBC_PDP_NUMBER;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	return_code = DM35424_Read(handle, &ioctl_request);

	*pdp_num = ioctl_request.readwrite.access.data.data32;

	return return_code;

}


DM35424LIB_API
int DM35424_Gbc_Get_Fpga_Build(struct DM35424_Board_Descriptor *handle,
				uint32_t *fpga_build)
{
	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = DM35424_OFFSET_GBC_FPGA_BUILD;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	return_code = DM35424_Read(handle, &ioctl_request);

	*fpga_build = ioctl_request.readwrite.access.data.data32;

	return return_code;

}


DM35424LIB_API
int DM35424_Gbc_Get_Sys_Clock_Freq(struct DM35424_Board_Descriptor *handle,
				uint32_t *clock_freq, int *is_std_clk)
{
    union dm35424_ioctl_argument ioctl_request;
	int return_code;
	uint32_t clk;

	ioctl_request.readwrite.access.offset = DM35424_OFFSET_GBC_SYS_CLK_FREQ;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
	return_code = DM35424_Read(handle, &ioctl_request);

	/* The value in the register is in 10kHz units, per hardware manual. */
	clk = ioctl_request.readwrite.access.data.data16 * (uint32_t) 10000;

	*is_std_clk = 1;

	if (CLOSE_TO_40MHZ(clk)) {
		*clock_freq = CLK_40MHZ;
	}
	else if (CLOSE_TO_54MHZ(clk)) {
		*clock_freq = CLK_54MHZ;
	}
	else if (CLOSE_TO_100MHZ(clk)) {
		*clock_freq = CLK_100MHZ;
	}
	else if (CLOSE_TO_57_6MHZ(clk)) {
		*clock_freq = CLK_57_6MHZ;
	}
	else if (CLOSE_TO_50MHZ(clk)) {
		*clock_freq = CLK_50MHZ;
	}
	else {
		*clock_freq = clk;
		*is_std_clk = 0;
	}

	return return_code;


}


DM35424LIB_API
int DM35424_Function_Block_Open(struct DM35424_Board_Descriptor *handle,
				unsigned int number,
				struct DM35424_Function_Block *func_block)
{

	int return_code = 0;
	int chan = 0;
	int buff = 0;
	union dm35424_ioctl_argument ioctl_request;
	uint32_t id_value;
	uint32_t fb_offset_in_gbc;

	if (number >= DM35424_MAX_FB) {
		return -1;
	}

	fb_offset_in_gbc = DM35424_OFFSET_GBC_FB_START +
				(number * DM35424_GBC_FB_BLK_SIZE) +
				DM35424_OFFSET_GBC_FB_ID;

	ioctl_request.readwrite.access.offset = fb_offset_in_gbc;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	id_value = ioctl_request.readwrite.access.data.data32;

	func_block->type = id_value & DM35424_FB_ID_TYPE_MASK;
	func_block->sub_type = (id_value & DM35424_FB_ID_SUBTYPE_MASK) >> 16;
	func_block->type_revision = (id_value & DM35424_FB_ID_TYPE_REV_MASK) >> 24;

	/*
	 * If the type is INVALID, then we're done here...no other data
	 * obtained would be worth anything.  Note that an invalid function
	 * block is NOT an error condition.
	 */

	if (func_block->type == DM35424_FUNC_BLOCK_INVALID ||
	    func_block->type == DM35424_FUNC_BLOCK_INVALID2) {
		return 0;
	}

	ioctl_request.readwrite.access.offset = fb_offset_in_gbc + DM35424_OFFSET_GBC_FB_OFFSET;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	func_block->fb_offset = ioctl_request.readwrite.access.data.data32;

	/*
	 * Acquire the DMA offset in the function block region
	 */
	ioctl_request.readwrite.access.offset = fb_offset_in_gbc + DM35424_OFFSET_GBC_FB_DMA_OFFSET;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_GBC;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	func_block->dma_offset = ioctl_request.readwrite.access.data.data32;

	/*
	 * Sanity check that the value at the FB offset
	 * is the same as what we had at the GBC.
	 */
	ioctl_request.readwrite.access.offset = func_block->fb_offset;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	if ((ioctl_request.readwrite.access.data.data32 & DM35424_FB_ID_TYPE_MASK) != func_block->type) {
		errno = EFAULT;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->fb_offset + DM35424_OFFSET_FB_DMA_CHANNELS;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	func_block->num_dma_channels = ioctl_request.readwrite.access.data.data8;

	assert(func_block->num_dma_channels <= MAX_DMA_CHANNELS);

	ioctl_request.readwrite.access.offset = func_block->fb_offset + DM35424_OFFSET_FB_DMA_BUFFERS;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}

	func_block->num_dma_buffers = ioctl_request.readwrite.access.data.data8;

	assert(func_block->num_dma_buffers <= MAX_DMA_BUFFERS);

	func_block->control_offset = func_block->fb_offset + DM35424_OFFSET_FB_CTRL_START;

	for (chan = 0; chan < func_block->num_dma_channels; chan ++) {

		func_block->dma_channel[chan].control_offset = func_block->dma_offset +
				(DM35424_DMA_CTRL_BLOCK_SIZE +
				(DM35424_DMA_BUFFER_CTRL_BLOCK_SIZE * func_block->num_dma_buffers)) * chan;

		func_block->dma_channel[chan].num_buffers = func_block->num_dma_buffers;

		for (buff = 0; buff < func_block->num_dma_buffers; buff++) {

			func_block->dma_channel[chan].buffer_start_offset[buff] =
				func_block->dma_channel[chan].control_offset +
				DM35424_DMA_CTRL_BLOCK_SIZE +
				(DM35424_DMA_BUFFER_CTRL_BLOCK_SIZE * buff);

		}


	}


	func_block->fb_num = number;

	return 0;


}


DM35424LIB_API
int DM35424_Function_Block_Open_Module(struct DM35424_Board_Descriptor *handle,
				uint32_t fb_type,
				unsigned int number_of_type,
				struct DM35424_Function_Block *func_block)
{

	int return_code = 0;
	int func_block_count = 0;
	int found = 0;

	func_block->ordinal_fb_type_num = number_of_type;

	while (found == 0 && func_block_count < DM35424_MAX_FB) {

		return_code = DM35424_Function_Block_Open(handle,
							func_block_count,
							func_block);

		if (return_code != 0) {
			return return_code;
		}

		if (func_block->type == fb_type) {
			if (number_of_type == 0) {
				found = 1;
			}
			else {
				number_of_type--;
			}
		}

		func_block_count++;

	}

	if (found == 0) {
		errno = ENODEV;
		return_code = -1;
	}

	return return_code;


}






