/**
    @file

    @brief
        Structures for the DM35424 Board Access Library.

    $Id: dm35424_board_access_structs.h 80523 2014-07-17 18:38:59Z rgroner $
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


#ifndef _DM35424_BOARD_ACCESS_STRUCTS__H_
#define _DM35424_BOARD_ACCESS_STRUCTS__H_


/**
 * @defgroup DM35424_PCI_Region_Structures DM35424 PCI Region Structures
 * @{
 */


/**
 * @brief
 * Standard PCI region number
 */

enum dm35424_pci_region_num {

	/**
	 * General Board Control Registers (BAR0)
	 */

	DM35424_PCI_REGION_GBC = 0,

	/**
	 * General Board Control Registers (64-bit) (BAR1)
	 */

	DM35424_PCI_REGION_GBC2,

	/**
	 * Functional Blocks Registers (BAR2)
	 */

	DM35424_PCI_REGION_FB
};



/**
 * @brief
 *	  Desired size in bits of access to standard PCI region
 */

enum dm35424_pci_region_access_size {

	/**
	 * 8-bit access
	 */

	DM35424_PCI_REGION_ACCESS_8 = 0,

	/**
	 * 16-bit access
	 */

	DM35424_PCI_REGION_ACCESS_16,

	/**
	 * 32-bit access
	 */

	DM35424_PCI_REGION_ACCESS_32,

};


/**
 * @brief
 *
 *       Enumeration for DMA functions that can be requested for the driver to perform.
 */
enum DM35424_DMA_FUNCTIONS {

	/**
	 * Initialize the DMA buffers
	 */
	DM35424_DMA_INITIALIZE,

	/**
	 * Read from the DMA buffers (transfer to user space)
	 */
	DM35424_DMA_READ,

	/**
	 * Write to the DMA buffers (transfer from user space)
	 */
	DM35424_DMA_WRITE
};


/**
 * @brief
 *	  PCI region access request descriptor.  This structure holds information
 *	  about a request to read data from or write data to one of a device's
 *	  PCI regions.
 */

struct dm35424_pci_access_request {

	/**
	 * Size of access in bits
	 */

	enum dm35424_pci_region_access_size size;

	/**
	 * The PCI region to access
	 */

	enum dm35424_pci_region_num region;

	/**
	 * Offset within region to access
	 */

	uint16_t offset;

	/**
	 * Data to write or the data read
	 */

	union {

	/**
	 * 8-bit value
	 */

		uint8_t data8;

	/**
	 * 16-bit value
	 */

		uint16_t data16;

	/**
	 * 32-bit value
	 */

		uint32_t data32;
	} data;
};


/**
 * @brief
 *	  ioctl() request structure for read from or write to PCI region
 */

struct dm35424_ioctl_region_readwrite {

	/**
	 * PCI region access request
	 */

	struct dm35424_pci_access_request access;
};


/**
 * @brief
 *	  ioctl() request structure for PCI region read/modify/write
 */

struct dm35424_ioctl_region_modify {

	/**
	 * PCI region access request
	 */
	struct dm35424_pci_access_request access;

	/**
	 * Bit mask that controls which bits can be modified.  A zero in a bit
	 * position means that the corresponding register bit should not be
	 * modified.  A one in a bit position means that the corresponding register
	 * bit should be modified.
	 *
	 * Note that it's possible to set bits outside of the mask depending upon
	 * the register value before modification.  When processing the associated
	 * request code, the driver will silently prevent this from happening but
	 * will not return an indication that the mask or new value was incorrect.
	 */

	union {

	/**
	 * Mask for 8-bit operations
	 */

		uint8_t mask8;

	/**
	 * Mask for 16-bit operations
	 */

		uint16_t mask16;

	/**
	 * Mask for 32-bit operations
	 */

		uint32_t mask32;
	} mask;
};




/**
 * @brief
 *	  ioctl() request structure for interrupt
 */

struct dm35424_ioctl_interrupt_info_request {


	/**
	 * Count of interrupts remaining in the driver queue.
	 */
	int interrupts_remaining;

	/**
	 * Boolean of if interrupt is valid or not.
	 */
	int valid_interrupt;

	/**
	 * Boolean if error occurred during interrupt
	 */
	int error_occurred;

	/**
	 * Function block that had interrupt.  The MSB indicates if this
	 * was a DMA interrupt or not.  (0 = Not DMA, 1 = DMA)
	 */
	int interrupt_fb;

};


/**
 * @brief
 *	  ioctl() request structure for DMA
 */
struct dm35424_ioctl_dma {

	/**
	 * Requested DMA function to perform.
	 */
	enum DM35424_DMA_FUNCTIONS function;

	/**
	 * Number of buffers to initialize for DMA
	 */
	int num_buffers;

	/**
	 * Size (in bytes) to allocate for buffers
	 */
	uint32_t buffer_size;

	/**
	 * Function Block DMA is for.
	 */
	uint32_t fb_num;

	/**
	 * Channel in function with DMA operation is for.
	 */
	int channel;

	/**
	 * Buffer in DMA channel that DMA is meant for.
	 */
	int buffer;

	/**
	 * PCI Address of DMA registers for this operation
	 */
	struct dm35424_pci_access_request pci;

	/**
	 * Pointer to user-space buffer for read or write.
	 */
	void *buffer_ptr;

};


/**
 * @brief
 *	  ioctl() request structure encapsulating all possible requests.  This is
 *	  what gets passed into the kernel from user space on the ioctl() call.
 */

union dm35424_ioctl_argument {

	/**
	 * PCI region read and write
	 */

	struct dm35424_ioctl_region_readwrite readwrite;

	/**
	 * PCI region read/modify/write
	 */

	struct dm35424_ioctl_region_modify modify;

	/**
	 * Interrupt request structure
	 */

	struct dm35424_ioctl_interrupt_info_request interrupt;
	/**
	 * DMA Configuration and Control
	 */

	struct dm35424_ioctl_dma dma;


};


/**
 * @} DM35424_Board_Access_Structures
 */

#endif

