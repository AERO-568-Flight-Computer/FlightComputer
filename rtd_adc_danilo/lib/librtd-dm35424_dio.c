/**
	@file

	@brief
		DM35424 DIO library source code

	$Id: librtd-dm35424_dio.c 84663 2014-12-19 22:03:35Z mmcintire $
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

#include <stdint.h>

#include "dm35424_registers.h"
#include "dm35424_gbc_library.h"
#include "dm35424_board_access.h"
#include "dm35424_board_access_structs.h"

/******************************************************************************
 * DIO Library Functions
 *****************************************************************************/


DM35424LIB_API
int DM35424_Dio_Open(struct DM35424_Board_Descriptor *handle,
		unsigned int number_of_type,
		struct DM35424_Function_Block *func_block)
{

	return DM35424_Function_Block_Open_Module(handle,
						DM35424_FUNC_BLOCK_DIO,
						number_of_type,
						func_block);
}



DM35424LIB_API
int DM35424_Dio_Set_Direction(struct DM35424_Board_Descriptor *handle,
			const struct DM35424_Function_Block *func_block,
			uint32_t direction)
{

	union dm35424_ioctl_argument ioctl_request;


	ioctl_request.readwrite.access.offset = func_block->control_offset +
						DM35424_OFFSET_DIO_DIRECTION;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = direction;

	return DM35424_Write(handle, &ioctl_request);

}



DM35424LIB_API
int DM35424_Dio_Get_Direction(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t *direction)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = func_block->control_offset +
						DM35424_OFFSET_DIO_DIRECTION;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;


	return_code = DM35424_Read(handle, &ioctl_request);

	*direction = ioctl_request.readwrite.access.data.data32;

	return return_code;

}


DM35424LIB_API
int DM35424_Dio_Get_Input_Value(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t *value)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = func_block->control_offset +
						DM35424_OFFSET_DIO_INPUT_VAL;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;


	return_code = DM35424_Read(handle, &ioctl_request);

	*value = ioctl_request.readwrite.access.data.data32;

	return return_code;

}


DM35424LIB_API
int DM35424_Dio_Get_Output_Value(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t *value)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = func_block->control_offset +
						DM35424_OFFSET_DIO_OUTPUT_VAL;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;


	return_code = DM35424_Read(handle, &ioctl_request);

	*value = ioctl_request.readwrite.access.data.data32;

	return return_code;

}



DM35424LIB_API
int DM35424_Dio_Set_Output_Value(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t value)
{

	union dm35424_ioctl_argument ioctl_request;


	ioctl_request.readwrite.access.offset = func_block->control_offset +
						DM35424_OFFSET_DIO_OUTPUT_VAL;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = value;

	return DM35424_Write(handle, &ioctl_request);

}
