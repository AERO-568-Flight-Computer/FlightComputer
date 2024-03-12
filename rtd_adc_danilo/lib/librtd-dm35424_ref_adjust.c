/**
	@file

	@brief
		DM35424 Reference Adjustment library source code


	$Id: librtd-dm35424_ref_adjust.c 84708 2015-01-05 21:57:01Z mmcintire $
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


#include "dm35424_ref_adjust_library.h"
#include "dm35424_registers.h"
#include "dm35424_gbc_library.h"
#include "dm35424_board_access_structs.h"
#include "dm35424_board_access.h"



int
DM35424_Ref_Adjust_Validate_Copy(struct DM35424_Function_Block *fb,
					enum DM35424_Copy_Directions direction)
{

	if (fb->type != DM35424_FUNC_BLOCK_REF_ADJUST) {
		errno = EINVAL;
		return -1;
	}

	switch (direction) {
	/* Breaks intentionally omitted */
	case DM35424_ADC_VOL_TO_NON_VOL:
	case DM35424_DAC_VOL_TO_NON_VOL:
	case DM35424_BOTH_VOL_TO_NON_VOL:
	case DM35424_ADC_NON_VOL_TO_VOL:
	case DM35424_DAC_NON_VOL_TO_VOL:
	case DM35424_BOTH_NON_VOL_TO_VOL:
		break;

	default:
		errno = EINVAL;
		return -1;
	}

	return 0;

}



int
DM35424_Is_Spi_Busy(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *fb)
{
	union dm35424_ioctl_argument ioctl_request;
	int result;

	ioctl_request.readwrite.access.offset = fb->control_offset + DM35424_OFFSET_REF_ADJUST_GO_BUSY;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	result = DM35424_Read(handle, &ioctl_request);

	if (result != 0) {
		return 1;
	}

	return (ioctl_request.readwrite.access.data.data8 ==
		DM35424_REF_ADJUST_SPI_BUSY);

}


int
DM35424_Start_Spi_Trans(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *fb)
{

	union dm35424_ioctl_argument ioctl_request;

	if (DM35424_Is_Spi_Busy(handle, fb)) {
		errno = EBUSY;
		return -1;
	}

	ioctl_request.readwrite.access.offset = fb->control_offset +
						DM35424_OFFSET_REF_ADJUST_GO_BUSY;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_REF_ADJUST_START_TRANS;

	return DM35424_Write(handle,
					&ioctl_request);

}




/******************************************************************************
 * Reference Adjust Library Functions
 *****************************************************************************/
DM35424LIB_API
int DM35424_Ref_Adjust_Open(struct DM35424_Board_Descriptor *handle,
				unsigned int ordinal_to_open,
				struct DM35424_Function_Block *fb)
{

	return DM35424_Function_Block_Open_Module(handle,
							DM35424_FUNC_BLOCK_REF_ADJUST,
							ordinal_to_open,
							fb);


}



DM35424LIB_API
int DM35424_Ref_Adjust_Write_Adc_To_Volatile(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					uint8_t adjustment)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_value = 0;

	if (fb->type != DM35424_FUNC_BLOCK_REF_ADJUST) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = fb->control_offset +
						DM35424_OFFSET_REF_OUTPUT_LATCH;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
	ioctl_request.readwrite.access.data.data16 = DM35424_REF_ADJUST_WRITE_ADC_VOLATILE |
							adjustment;

	return_value = DM35424_Write(handle, &ioctl_request);

	if (return_value) {
		return return_value;
	}

	return DM35424_Start_Spi_Trans(handle, fb);

}


DM35424LIB_API
int DM35424_Ref_Adjust_Write_Adc_To_NonVolatile(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					uint8_t adjustment)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_value = 0;

	if (fb->type != DM35424_FUNC_BLOCK_REF_ADJUST) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = fb->control_offset +
						DM35424_OFFSET_REF_OUTPUT_LATCH;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
	ioctl_request.readwrite.access.data.data16 = DM35424_REF_ADJUST_WRITE_ADC_NON_VOLATILE |
							adjustment;

	return_value = DM35424_Write(handle, &ioctl_request);

	if (return_value) {
		return return_value;
	}

	return DM35424_Start_Spi_Trans(handle, fb);

}


DM35424LIB_API
int DM35424_Ref_Adjust_Write_Dac_To_Volatile(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					uint8_t adjustment)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_value = 0;

	if (fb->type != DM35424_FUNC_BLOCK_REF_ADJUST) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = fb->control_offset +
						DM35424_OFFSET_REF_OUTPUT_LATCH;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
	ioctl_request.readwrite.access.data.data16 = DM35424_REF_ADJUST_WRITE_DAC_VOLATILE |
							adjustment;

	return_value = DM35424_Write(handle, &ioctl_request);

	if (return_value) {
		return return_value;
	}

	return DM35424_Start_Spi_Trans(handle, fb);
}


DM35424LIB_API
int DM35424_Ref_Adjust_Write_Dac_To_NonVolatile(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					uint8_t adjustment)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_value = 0;

	if (fb->type != DM35424_FUNC_BLOCK_REF_ADJUST) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = fb->control_offset +
						DM35424_OFFSET_REF_OUTPUT_LATCH;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
	ioctl_request.readwrite.access.data.data16 = DM35424_REF_ADJUST_WRITE_DAC_NON_VOLATILE |
							adjustment;

	return_value = DM35424_Write(handle, &ioctl_request);

	if (return_value) {
		return return_value;
	}

	return DM35424_Start_Spi_Trans(handle, fb);
}


DM35424LIB_API
int DM35424_Ref_Adjust_Copy_Data(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					enum DM35424_Copy_Directions direction)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_value = 0;

	if (DM35424_Ref_Adjust_Validate_Copy(fb,
						direction)) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = fb->control_offset +
						DM35424_OFFSET_REF_OUTPUT_LATCH;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
	ioctl_request.readwrite.access.data.data16 = direction;

	return_value = DM35424_Write(handle, &ioctl_request);

	if (return_value) {
		return return_value;
	}

	return DM35424_Start_Spi_Trans(handle, fb);
}

