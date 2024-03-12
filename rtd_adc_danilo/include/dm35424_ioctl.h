/**
	@file

	@brief
		DM35424 Low level ioctl() request descriptor structure
		 and request code definitions

	$Id: dm35424_ioctl.h 80523 2014-07-17 18:38:59Z rgroner $
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

#ifndef __DM35424_IOCTL_H__
#define __DM35424_IOCTL_H__

#include <linux/types.h>
#include <linux/ioctl.h>



/*=============================================================================
Macros
 =============================================================================*/

/**
 * @defgroup DM35424_Ioctl_Macros DM35424 Ioctl macros
 * @{
 */

/**
 * @brief
 *	  Unique 8-bit value used to generate unique ioctl() request codes
 */

#define DM35424_IOCTL_MAGIC	  'D'

/**
 * @brief
 *	  First ioctl() request number
 */

#define DM35424_IOCTL_REQUEST_BASE   0x00

/**
 * @brief
 *	  ioctl() request code for reading from a PCI region
 */

#define DM35424_IOCTL_REGION_READ \
	_IOR( \
	DM35424_IOCTL_MAGIC, \
	(DM35424_IOCTL_REQUEST_BASE + 1), \
	union dm35424_ioctl_argument)

/**
 * @brief
 *	  ioctl() request code for writing to a PCI region
 */

#define DM35424_IOCTL_REGION_WRITE \
	_IOW( \
	DM35424_IOCTL_MAGIC, \
	(DM35424_IOCTL_REQUEST_BASE + 2), \
	union dm35424_ioctl_argument)

/**
 * @brief
 *	  ioctl() request code for PCI region read/modify/write
 */

#define DM35424_IOCTL_REGION_MODIFY \
	_IOWR( \
	DM35424_IOCTL_MAGIC, \
	(DM35424_IOCTL_REQUEST_BASE + 3), \
	union dm35424_ioctl_argument)


/**
 * @brief
 *	  ioctl() request code for DMA function
 */

#define DM35424_IOCTL_DMA_FUNCTION \
	_IOW( \
	DM35424_IOCTL_MAGIC, \
	(DM35424_IOCTL_REQUEST_BASE + 4), \
	union dm35424_ioctl_argument)

/**
 * @brief
 *	  ioctl() request code for User ISR thread wake up
 */

#define DM35424_IOCTL_WAKEUP \
	_IOW( \
	DM35424_IOCTL_MAGIC, \
	(DM35424_IOCTL_REQUEST_BASE + 5), \
	union dm35424_ioctl_argument)

/**
 * @brief
 *	  ioctl() request code to retrieve interrupt status information
 */

#define DM35424_IOCTL_INTERRUPT_GET \
	_IOWR( \
	DM35424_IOCTL_MAGIC, \
	(DM35424_IOCTL_REQUEST_BASE + 6), \
	union dm35424_ioctl_argument)

/**
 * @} DM35424_Ioctl_Macros
 */


#endif /* __dm35424_ioctl_h__ */
