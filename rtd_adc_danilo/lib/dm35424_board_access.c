/**
	@file

	@brief
		DM35424 Board Access library source code

	$Id: dm35424_board_access.c 104885 2016-12-01 21:29:08Z rgroner $
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


#include "dm35424_gbc_library.h"
#include "dm35424_ioctl.h"
#include "dm35424_board_access.h"

#define DEVICE_NAME_PATH_PREFIX "/dev/rtd-dm35424"

int
DM35424_Board_Open(uint8_t dev_num, struct DM35424_Board_Descriptor **handle)
{
	char device_name[25];
	int descriptor;

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Form the device file name and attempt to open the file
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	(void)snprintf(&(device_name[0]), sizeof(device_name),
			   DEVICE_NAME_PATH_PREFIX "-%u", dev_num);

	descriptor = open(&(device_name[0]), O_RDWR);
	if (descriptor == -1) {
		if (errno != EBUSY) {
			*handle = NULL;
		}
		return -1;
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Allocate and initialize memory for the library device descriptor
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	*handle = (struct DM35424_Board_Descriptor *)
			   malloc(sizeof(struct DM35424_Board_Descriptor));
	if (*handle == NULL) {
		errno = ENOMEM;
		return -1;
	}

	(void)memset(*handle, 0x00, sizeof(struct DM35424_Board_Descriptor));

	(*handle)->file_descriptor = descriptor;
	(*handle)->isr = NULL;
	return 0;
}


int DM35424_Board_Close(struct DM35424_Board_Descriptor *handle)
{
	if (handle == NULL) {
		errno = ENODATA;
		return -1;
	}

	if (close(handle->file_descriptor) == -1) {
		free(handle);
		return -1;
	}

	free(handle);
	return 0;
}

int
DM35424_Read(struct DM35424_Board_Descriptor *handle,
	union dm35424_ioctl_argument *ioctl_request)
{

	return ioctl(handle->file_descriptor, DM35424_IOCTL_REGION_READ,
			ioctl_request);

}

int
DM35424_Write(struct DM35424_Board_Descriptor *handle,
	union dm35424_ioctl_argument *ioctl_request)
{
	return ioctl(handle->file_descriptor, DM35424_IOCTL_REGION_WRITE,
			ioctl_request);

}

int
DM35424_Modify(struct DM35424_Board_Descriptor *handle,
	union dm35424_ioctl_argument *ioctl_request)
{

	return ioctl(handle->file_descriptor, DM35424_IOCTL_REGION_MODIFY,
			ioctl_request);

}

int
DM35424_Dma(struct DM35424_Board_Descriptor *handle,
    union dm35424_ioctl_argument *ioctl_request)
{
    return ioctl(handle->file_descriptor, DM35424_IOCTL_DMA_FUNCTION,
        ioctl_request);
}

