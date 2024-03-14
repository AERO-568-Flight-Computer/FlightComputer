
/**
    @file

    @brief
        DM35424 driver source code

    $Id: rtd-dm35424.c 130734 2021-06-29 18:14:09Z zcovone $
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


#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/version.h>


#include "dm35424_driver.h"
#include "dm35424_ioctl.h"
#include "dm35424_board_access_structs.h"
#include "dm35424_registers.h"
#include "dm35424.h"
#include "dm35424_types.h"

/*===============================================================
Driver identification
 ===============================================================*/

#define DRIVER_NAME "rtd-dm35424"
#define DRIVER_VERSION "04.00.01"
#define DRIVER_DESCRIPTION "Device driver for the DM35424"
#define DRIVER_COPYRIGHT "Copyright (C), RTD Embedded Technologies, Inc.  All Rights Reserved."

/*===============================================================
Debug Flags.
 ===============================================================*/
#if defined(DEBUG_INT) || defined(DEBUG_ALL)
	#define DM35424_DEBUG_INTERRUPTS
#endif

#if defined(DEBUG_DMA) || defined(DEBUG_ALL)
	#define DM35424_DEBUG_DMA
#endif

#if defined(DEBUG) || defined(DEBUG_ALL)
	#define DM35424_DEBUG
#endif


/*===============================================================
Linux Kernel Version specific defines.
 ===============================================================*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 9)

#define IO_MEMORY_READ8     readb
#define IO_MEMORY_READ16    readw
#define IO_MEMORY_READ32    readl
#define IO_MEMORY_WRITE8    writeb
#define IO_MEMORY_WRITE16   writew
#define IO_MEMORY_WRITE32   writel

#else

#define IO_MEMORY_READ8     ioread8
#define IO_MEMORY_READ16    ioread16
#define IO_MEMORY_READ32    ioread32
#define IO_MEMORY_WRITE8    iowrite8
#define IO_MEMORY_WRITE16   iowrite16
#define IO_MEMORY_WRITE32   iowrite32

#endif


#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
#define IRQF_SHARED     SA_SHIRQ
#endif


/*===============================================================
Driver Interrupt constants
 ===============================================================*/
#define TOO_MANY_MISSED_IRQ	10


/*=============================================================================
Global variables
 =============================================================================*/

/**
 * Number of devices detected during probe
 */
static uint32_t dm35424_device_count;

/**
 * Character device descriptor
 */
static struct cdev dm35424_cdev;

/**
 * Character device major number; dynamically assigned
 */
static int dm35424_major;

/**
 * DM35424 device descriptors
 */

static struct dm35424_device_descriptor *dm35424_devices;

static struct class *dev_class = NULL;

static struct device *dev = NULL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
static struct pci_dev *pdev = NULL;
#endif

/**
 * Table of devices supported by the driver.  This array is used by
 * dm35424_probe_devices() to walk the entire PCI device list looking for DM35424
 * devices.  The table is terminated by a "NULL" entry.
 *
 * The individual structures in this array are set up using ANSI C standard
 * format initialization (which is the preferred method in 2.6 kernels) instead
 * of tagged initialization (which is the preferred method in 2.4 kernels).
 */

static const struct pci_device_id dm35424_pci_device_table[] = {
	{
	 .vendor = DM35424_PCI_VENDOR_ID,
	 .device = DM35424_PCI_DEVICE_ID,
	 .subvendor = PCI_ANY_ID,
	 .subdevice = PCI_ANY_ID,
	 .class = 0,
	 .class_mask = 0,
	 .driver_data = 0},
	{
	 .vendor = 0,
	 .device = 0,
	 .subvendor = 0,
	 .subdevice = 0,
	 .class = 0,
	 .class_mask = 0,
	 .driver_data = 0}
};

MODULE_DEVICE_TABLE(pci, dm35424_pci_device_table);


/*=============================================================================
Driver functions
 =============================================================================*/

/******************************************************************************
Register the character device
 ******************************************************************************/
static int dm35424_register_char_device(int *major)
{
	dev_t device, devno;
	int status;
	char dev_file_name[30];
	unsigned int minor = 0;

	status = alloc_chrdev_region(&device,
					0,
					dm35424_device_count,
					DRIVER_NAME);
	if (status < 0) {
		return status;
	}

	cdev_init(&dm35424_cdev, &dm35424_file_ops);
	dm35424_cdev.owner = THIS_MODULE;

	status = cdev_add(&dm35424_cdev, device, dm35424_device_count);
	if (status < 0) {
		unregister_chrdev_region(device, dm35424_device_count);
		return status;
	}

	*major = MAJOR(device);

	dev_class = class_create(THIS_MODULE, DRIVER_NAME);

	if (dev_class == NULL) {
		unregister_chrdev_region(device, dm35424_device_count);
		return -ENODEV;
	}



	for (minor = 0; minor < dm35424_device_count; minor ++) {
		sprintf(dev_file_name, "%s-%u", DRIVER_NAME, minor);
		devno = MKDEV(*major, minor);
		dev = device_create(dev_class,
					NULL,
					devno,
					NULL,
					dev_file_name,
					0);

		if (dev == NULL) {
			return -ENODEV;
		}
	}

	return 0;
}


/******************************************************************************
*  DM35424 device descriptor initialization
*  Perform any required initialization of data structures in the device
*  descriptor.
 ******************************************************************************/
static void
dm35424_init_device_desc(struct dm35424_device_descriptor *dm35424_device)
{

	dm35424_device->remove_isr_flag = 0x00;
	init_waitqueue_head(&(dm35424_device->int_wait_queue));

	dm35424_device->int_queue_missed = 0;
	dm35424_device->int_queue_count = 0;
	dm35424_device->int_queue_in_marker = 0;
	dm35424_device->int_queue_out_marker = 0;


    	INIT_LIST_HEAD(&(dm35424_device->dma_descr_list));

}


/******************************************************************************
Write to a standard PCI region
 ******************************************************************************/
static void
dm35424_region_write(const struct dm35424_device_descriptor *dm35424_device,
			struct dm35424_pci_access_request *pci_request,
			unsigned long address)
{

	/*###########################################################
	   Determine whether the region is memory or I/O mapped
	   ####################################################### */

	if (dm35424_device->pci[pci_request->region].virt_addr != NULL) {

		/*
		 * Region is memory mapped
		 */

		/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		   Determine how many bits are to be accessed
		   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

		switch (pci_request->size) {
		case DM35424_PCI_REGION_ACCESS_8:
			IO_MEMORY_WRITE8(pci_request->data.data8,
					 (unsigned long *)address);
#ifdef DM35424_DEBUG
			printk(KERN_DEBUG "%s Write to address 0x%lx (0x%x) << 0x%x",
				dm35424_device->name,
				address,
				pci_request->offset,
				pci_request->data.data8);
#endif
			break;

		case DM35424_PCI_REGION_ACCESS_16:
			IO_MEMORY_WRITE16(pci_request->data.data16,
					  (unsigned long *)address);
#ifdef DM35424_DEBUG
			printk(KERN_DEBUG "%s Write to address 0x%lx (0x%x) << 0x%x",
				dm35424_device->name,
				address,
				pci_request->offset,
				pci_request->data.data16);
#endif
			break;

		case DM35424_PCI_REGION_ACCESS_32:
			IO_MEMORY_WRITE32(pci_request->data.data32,
					  (unsigned long *)address);
#ifdef DM35424_DEBUG
			printk(KERN_DEBUG "%s Write to address 0x%lx (0x%x) << 0x%x",
				dm35424_device->name,
				address,
				pci_request->offset,
				pci_request->data.data32);
#endif
			break;
		default:
			printk(KERN_ERR "Could not determine write access size (%d)",
				pci_request->size);
			break;
		}
	} else {

		/*
		 * Region is I/O mapped
		 */

		/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		   Determine how many bits are to be accessed
		   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

		switch (pci_request->size) {
		case DM35424_PCI_REGION_ACCESS_8:
			outb(pci_request->data.data8, address);
			break;

		case DM35424_PCI_REGION_ACCESS_16:
			outw(pci_request->data.data16, address);
			break;

		case DM35424_PCI_REGION_ACCESS_32:
			outl(pci_request->data.data32, address);
			break;
		default:
			printk(KERN_ERR "Could not determine write access size (%d)",
				pci_request->size);
			break;
		}
	}
}


/******************************************************************************
Read from a standard PCI region
 ******************************************************************************/
static void
dm35424_region_read(const struct dm35424_device_descriptor *dm35424_device,
			struct dm35424_pci_access_request *pci_request,
			unsigned long address)
{
	/*############################################################
	   Determine whether the region is memory or I/O mapped
	######################################################### */

	if (dm35424_device->pci[pci_request->region].virt_addr != NULL) {

		/*
		 * Region is memory mapped
		 */
		/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		   Determine how many bits are to be accessed
		   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

		switch (pci_request->size) {
		case DM35424_PCI_REGION_ACCESS_8:
			pci_request->data.data8 =
				IO_MEMORY_READ8((unsigned long *)address);

#ifdef DM35424_DEBUG
			printk(KERN_DEBUG "%s Read from address 0x%lx (0x%x) >> 0x%x",
				dm35424_device->name,
				address,
				pci_request->offset,
				pci_request->data.data8);
#endif
			break;

		case DM35424_PCI_REGION_ACCESS_16:
			pci_request->data.data16 =
				IO_MEMORY_READ16((unsigned long *)address);
#ifdef DM35424_DEBUG
			printk(KERN_DEBUG "%s Read from address 0x%lx (0x%x) >> 0x%x",
				dm35424_device->name,
				address,
				pci_request->offset,
				pci_request->data.data16);
#endif

			break;

		case DM35424_PCI_REGION_ACCESS_32:
			pci_request->data.data32 =
				IO_MEMORY_READ32((unsigned long *)address);
#ifdef DM35424_DEBUG
			printk(KERN_DEBUG "%s Read from address 0x%lx (0x%x) >> 0x%x",
				dm35424_device->name,
				address,
				pci_request->offset,
				pci_request->data.data32);
#endif

			break;
		default:
			printk(KERN_ERR "Could not determine read access size (%d)",
				pci_request->size);
			break;
		}
	} else {

		/*
		 * Region is I/O mapped
		 */

		/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		   Determine how many bits are to be accessed
		   @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

		switch (pci_request->size) {
		case DM35424_PCI_REGION_ACCESS_8:
			pci_request->data.data8 =
				(uint8_t) inb(address);
			break;

		case DM35424_PCI_REGION_ACCESS_16:
			pci_request->data.data16 =
				(uint16_t) inw(address);
			break;

		case DM35424_PCI_REGION_ACCESS_32:
			pci_request->data.data32 =
				(uint32_t) inl(address);
			break;
		default:
			printk(KERN_ERR "Could not determine read access size (%d)",
				pci_request->size);
			break;
		}
	}
}


/******************************************************************************
Access a standard PCI region
 ******************************************************************************/
static void
dm35424_access_pci_region(const struct dm35424_device_descriptor *dm35424_device,
				struct dm35424_pci_access_request *pci_request,
				enum dm35424_pci_region_access_dir direction)
{
	unsigned long address;

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Compute the address to be accessed
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	address = pci_request->offset;

	if (dm35424_device->pci[pci_request->region].virt_addr != NULL) {
		address += (unsigned long)
			dm35424_device->pci[pci_request->region].virt_addr;
	} else {
		address += dm35424_device->pci[pci_request->region].io_addr;
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Determine whether access is a read or write
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (direction == DM35424_PCI_REGION_ACCESS_READ) {

		/*
		 * Region is to be read
		 */
		dm35424_region_read(dm35424_device,
					pci_request,
					address);


	} else {

		/*
		 * Region is to be written
		 */
		dm35424_region_write(dm35424_device,
					pci_request,
					address);

	}
}



/******************************************************************************
Validate an DM35424 device descriptor
 ******************************************************************************/
static int
dm35424_validate_device(const struct dm35424_device_descriptor *dm35424_device)
{
	uint32_t minor_number;

	for (minor_number = 0; minor_number < dm35424_device_count;
		 minor_number++) {
		if (dm35424_device == &((dm35424_devices)[minor_number])) {
			return 0;
		}
	}

	printk(KERN_ERR "%s: Could not validate device descriptor.",
	   	dm35424_device->name);

	return -EBADFD;
}



/******************************************************************************
Validate user-space PCI region access
 ******************************************************************************/
static int
dm35424_validate_pci_access(const struct dm35424_device_descriptor * dm35424_device,
				const struct dm35424_pci_access_request *pci_request)
{
	uint16_t align_mask;
	uint8_t access_bytes;

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Validate the data size
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/*
	 * Verify the data size in bits.  Set the number of bytes being accessed;
	 * this is used to determine whether or not the region offset actually lies
	 * within the region.  Set the offset alignment bit mask; this is used to
	 * determine whether the region offset is suitably aligned for the access.
	 */

	switch (pci_request->size) {
	case DM35424_PCI_REGION_ACCESS_8:
		access_bytes = 1;
		align_mask = 0x0;
		break;

	case DM35424_PCI_REGION_ACCESS_16:
		access_bytes = 2;
		align_mask = 0x1;
		break;

	case DM35424_PCI_REGION_ACCESS_32:
		access_bytes = 4;
		align_mask = 0x3;
		break;

	default:
		printk(KERN_ERR "%s: Attempting to access memory with size %d.",
			dm35424_device->name, pci_request->size);
		return -EMSGSIZE;
		break;
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Validate the PCI region
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	switch (pci_request->region) {
	case DM35424_PCI_REGION_GBC:
	case DM35424_PCI_REGION_FB:
		break;

	default:
		return -EINVAL;
		break;
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Validate the PCI region offset
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/*
	 * All bytes being accessed must lie entirely within the PCI region
	 */

	if (pci_request->offset >
		  (dm35424_device->pci[pci_request->region].length - access_bytes)) {
		printk(KERN_ERR "%s Request for PCI access outside of allowed range: "
				"Region: %d  Length: 0x%lx  Offset: 0x%x  Access bytes: %d.",
				dm35424_device->name, pci_request->region,
	      			dm35424_device->pci[pci_request->region].length,
				pci_request->offset,
				access_bytes);
		return -ERANGE;
	}

	/*
	 * Offset where access will occur must be suitably aligned for the size of
	 * access
	 */

	if (pci_request->offset & align_mask) {
		return -EOPNOTSUPP;
	}

	return 0;
}



/******************************************************************************
Validate DMA function
 ******************************************************************************/
static int
dm35424_validate_dma(const struct dm35424_device_descriptor *dm35424_device,
			const struct dm35424_ioctl_dma *dma_function)
{

	int result = 0;

	/**
	  * Validate DMA channel being request
	  */
	if (dma_function->channel < 0 ||
		dma_function->buffer < 0) {
		return -EINVAL;
	}

	switch (dma_function->function) {
	case DM35424_DMA_INITIALIZE:
		if ((dma_function->buffer_size <= 0) ||
		    (dma_function->buffer_size & 0x03) ||
		    (dma_function->buffer_size > DM35424_DMA_MAX_BUFFER_SIZE)) {
			    printk(KERN_ERR "%s: Invalid buffer size value (%d)",
			    	dm35424_device->name,
			    	dma_function->buffer_size);
			    return -EINVAL;
		}

		switch (dma_function->pci.region) {
		case DM35424_PCI_REGION_FB:
			break;

		default:
			printk(KERN_ERR "%s: Invalid PCI region (%d)",
				dm35424_device->name,
				dma_function->pci.region);
			return -EINVAL;
			break;
		}

		result = dm35424_validate_pci_access(dm35424_device,
							&(dma_function->pci));
		if (result != 0) {
			return result;
		}

		break;
	case DM35424_DMA_READ:
		/* break omitted */
	case DM35424_DMA_WRITE:
		if ((dma_function->buffer_size <= 0) ||
		    (dma_function->buffer_size & 0x03) ||
		    (dma_function->buffer_size > DM35424_DMA_MAX_BUFFER_SIZE)) {
			    printk(KERN_ERR "%s: Invalid buffer size value (%d)",
			    	dm35424_device->name,
			    	dma_function->buffer_size);
			    return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}


	return 0;
}




/******************************************************************************
Read from a PCI region
 ******************************************************************************/
static int
dm35424_pci_region_read(struct dm35424_device_descriptor * dm35424_device,
			   unsigned long ioctl_param)
{
	union dm35424_ioctl_argument ioctl_argument;
	int status;
	unsigned long irq_flags;

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Copy arguments in from user space and validate them
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (copy_from_user(&ioctl_argument,
			   (union dm35424_ioctl_argument *) ioctl_param,
			   sizeof(union dm35424_ioctl_argument))) {
		return -EFAULT;
	}


	status = dm35424_validate_pci_access(dm35424_device,
					   &(ioctl_argument.readwrite.access));
	if (status != 0) {

		return status;
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Do the actual read
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	spin_lock_irqsave(&(dm35424_device->device_lock), irq_flags);
	dm35424_access_pci_region(dm35424_device,
				 &(ioctl_argument.readwrite.access),
				 DM35424_PCI_REGION_ACCESS_READ);

	spin_unlock_irqrestore(&(dm35424_device->device_lock), irq_flags);

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Copy results back to user space
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (copy_to_user((union dm35424_ioctl_argument *) ioctl_param,
			 &ioctl_argument, sizeof(union dm35424_ioctl_argument))) {
		return -EFAULT;
	}

	return 0;
}


/******************************************************************************
Write to a PCI region
 ******************************************************************************/
static int
dm35424_pci_region_write(struct dm35424_device_descriptor * dm35424_device,
			unsigned long ioctl_param)
{
	union dm35424_ioctl_argument ioctl_argument;
	int status;
	unsigned long irq_flags;

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Copy arguments in from user space and validate them
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (copy_from_user(&ioctl_argument,
			   (union dm35424_ioctl_argument *) ioctl_param,
			   sizeof(union dm35424_ioctl_argument))) {
		return -EFAULT;
	}

	status = dm35424_validate_pci_access(dm35424_device,
					   &(ioctl_argument.readwrite.access));
	if (status != 0) {
		return status;
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Do the actual write
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	spin_lock_irqsave(&(dm35424_device->device_lock), irq_flags);

	dm35424_access_pci_region(dm35424_device,
				 &(ioctl_argument.readwrite.access),
				 DM35424_PCI_REGION_ACCESS_WRITE);

	spin_unlock_irqrestore(&(dm35424_device->device_lock), irq_flags);

	return 0;
}




/******************************************************************************
Read from a PCI region, modify bits in the value, and write the new value back
to the region
 ******************************************************************************/
static int
dm35424_pci_region_modify(struct dm35424_device_descriptor *dm35424_device,
			 unsigned long ioctl_param)
{
	union dm35424_ioctl_argument ioctl_argument;
	struct dm35424_pci_access_request pci_request;
	int status;
	unsigned long irq_flags;

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Copy arguments in from user space and validate them
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (copy_from_user(&ioctl_argument,
			   (union dm35424_ioctl_argument *) ioctl_param,
			   sizeof(union dm35424_ioctl_argument))) {
		return -EFAULT;
	}

	status = dm35424_validate_pci_access(dm35424_device,
					   &(ioctl_argument.modify.access));
	if (status != 0) {

		return status;
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Do the actual read/modify/write
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/*
	 * Make a copy of user arguments to keep from overwriting them
	 */

	pci_request = ioctl_argument.modify.access;

	/*
	 * Read current value
	 */
	spin_lock_irqsave(&(dm35424_device->device_lock), irq_flags);
	dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_READ);

	/*
	 * Modify the value based upon mask
	 */

	switch (ioctl_argument.modify.access.size) {
	case DM35424_PCI_REGION_ACCESS_8:

		/*
		 * Preserve bits which are not to be changed and clear bits which
		 * will be changed
		 */

		pci_request.data.data8 &= ~ioctl_argument.modify.mask.mask8;

		/*
		 * Fold in the new value but don't allow bits to be set which are
		 * are not modifiable according to the mask
		 */

		pci_request.data.data8 |=
			(ioctl_argument.modify.access.data.data8 & ioctl_argument.
			 modify.mask.mask8);

		break;

	case DM35424_PCI_REGION_ACCESS_16:

		/*
		 * Preserve bits which are not to be changed and clear bits which
		 * will be changed
		 */

		pci_request.data.data16 &= ~ioctl_argument.modify.mask.mask16;

		/*
		 * Fold in the new value but don't allow bits to be set which are
		 * are not modifiable according to the mask
		 */

		pci_request.data.data16 |=
			(ioctl_argument.modify.access.data.data16 & ioctl_argument.
			 modify.mask.mask16);

		break;

	case DM35424_PCI_REGION_ACCESS_32:

		/*
		 * Preserve bits which are not to be changed and clear bits which
		 * will be changed
		 */

		pci_request.data.data32 &= ~ioctl_argument.modify.mask.mask32;

		/*
		 * Fold in the new value but don't allow bits to be set which are
		 * are not modifiable according to the mask
		 */

		pci_request.data.data32 |=
			(ioctl_argument.modify.access.data.data32 & ioctl_argument.
			 modify.mask.mask32);

		break;
	default:
		printk(KERN_ERR "Could not determine modify access size (%d)",
			ioctl_argument.modify.access.size);
		break;
	}

	/*
	 * Write new value
	 */

	dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_WRITE);

	spin_unlock_irqrestore(&(dm35424_device->device_lock), irq_flags);

	return 0;
}




/******************************************************************************
Pull the next interrupt off the queue (if there is one)
This function assumes the caller has a spinlock
*******************************************************************************/
static void
dm35424_dequeue_interrupt(struct dm35424_device_descriptor *dm35424_device,
			  int *int_fb,
			  int *int_available)
{

	*int_available = 0;

	/*
	 * If there is an interrupt in the queue then retrieve the data.
	 */
	if (dm35424_device->int_queue_count > 0) {

		/*
		 * Cache local copies of the interrupt status
		 */

		*int_fb = dm35424_device->interrupt_fb[dm35424_device->int_queue_out_marker];
		*int_available = 1;

		/*
		 * Make copy of the calculated number of interrupts in the queue and
		 * return this value -1 to signify how many more interrupts the
		 * reading device needs to receive
		 */

		dm35424_device->int_queue_count--;


		dm35424_device->int_queue_out_marker++;

		if (dm35424_device->int_queue_out_marker == DM35424_INT_QUEUE_SIZE) {

			/*
			 * wrap around if we have to
			 */

			dm35424_device->int_queue_out_marker = 0;

		}
#if defined(DM35424_DEBUG_INTERRUPTS)

		if (*int_fb < 0) {
			printk(KERN_DEBUG
				   "%s: Removing DMA interrupt: FB%d (Remaining: %d)\n",
				   dm35424_device->name,
				   ((*int_fb) & 0x7FFFFFFF),
				   dm35424_device->int_queue_count);
		} else {
			printk(KERN_DEBUG
				   "%s: Removing interrupt: FB%d (Remaining: %d)\n",
				   dm35424_device->name,
				   *int_fb,
				   dm35424_device->int_queue_count);
		}
#endif

	}

}



/******************************************************************************
Send interrupt status information back to user
 ******************************************************************************/
static int
dm35424_get_interrupt_info(struct dm35424_device_descriptor *dm35424_device,
			  unsigned long ioctl_param)
{
	int int_fb;
	unsigned long irq_flags;
	union dm35424_ioctl_argument ioctl_arg;
	int remaining = 0;
	int interrupt_available = 0;

	spin_lock_irqsave(&(dm35424_device->device_lock), irq_flags);

	dm35424_dequeue_interrupt(dm35424_device,
				  &int_fb,
				  &interrupt_available);

	remaining = dm35424_device->int_queue_count;

	spin_unlock_irqrestore(&(dm35424_device->device_lock), irq_flags);

	if (interrupt_available) {
		ioctl_arg.interrupt.valid_interrupt = 1;
		ioctl_arg.interrupt.interrupt_fb = int_fb;
		ioctl_arg.interrupt.error_occurred = 0;
		ioctl_arg.interrupt.interrupts_remaining = remaining;

	} else {
		printk(KERN_WARNING "%s: Attempted to get interrupt function "
		       "block, but none were in the queue.",
		       dm35424_device->name);

		ioctl_arg.interrupt.valid_interrupt = 0;
		ioctl_arg.interrupt.error_occurred = 1;

	}


	/*
	 * Copy the interrupt status back to user space
	 */
	if (copy_to_user((union dm35424_ioctl_argument *) ioctl_param,
			 &ioctl_arg,
			 sizeof(union dm35424_ioctl_argument))) {
		return -EFAULT;
	}

	return 0;
}



/******************************************************************************
Read from DMA (Copy DMA buffer to user space)
 ******************************************************************************/
static int
dm35424_dma_read(struct dm35424_device_descriptor *dm35424_device,
				struct dm35424_ioctl_dma *dma)
{


	struct list_head *cursor;
	int status = -ENXIO;

    	list_for_each(cursor, &(dm35424_device->dma_descr_list)) {
        	struct dm35424_dma_descriptor *list_item;

        	list_item = list_entry(cursor,
        				struct dm35424_dma_descriptor,
        				list);

        	if (list_item->fb_num == dma->fb_num &&
        		list_item->channel == dma->channel &&
        		list_item->buffer == dma->buffer) {

			status = copy_to_user(dma->buffer_ptr,
				list_item->virt_addr,
				dma->buffer_size);

			if (status != 0) {

			    printk(KERN_ERR "ERROR: DMA Read failed when copying to user space.");
			    return -EFAULT;
			}
#ifdef DM35424_DEBUG_DMA
			printk(KERN_DEBUG "%s: Reading DMA buffer for FB 0x%x, Channel %d, Buffer %d\n",
				dm35424_device->name,
				list_item->fb_num,
				list_item->channel,
				list_item->buffer);
#endif
		}

	}

	return status;

}


/******************************************************************************
Write to DMA (Copy DMA buffer from user space)
 ******************************************************************************/
static int
dm35424_dma_write(struct dm35424_device_descriptor *dm35424_device,
				struct dm35424_ioctl_dma *dma)
{

	struct list_head *cursor;
	int status = -ENXIO;


    	list_for_each(cursor, &(dm35424_device->dma_descr_list)) {
        	struct dm35424_dma_descriptor *list_item;

        	list_item = list_entry(cursor,
        				struct dm35424_dma_descriptor,
        				list);

        	if (list_item->fb_num == dma->fb_num &&
        		list_item->channel == dma->channel &&
        		list_item->buffer == dma->buffer) {

			status = copy_from_user(list_item->virt_addr,
						dma->buffer_ptr,
						dma->buffer_size);

			if (status != 0) {

			    printk(KERN_ERR "ERROR: DMA Write failed copying "
			    			"data from user space\n");
			    return -EFAULT;
			}
#ifdef DM35424_DEBUG_DMA
			printk(KERN_DEBUG "%s: Writing to DMA buffer for FB 0x%x, Channel %d, Buffer %d\n",
				dm35424_device->name,
				list_item->fb_num,
				list_item->channel,
				list_item->buffer);
#endif

		}

	}

	return status;

}


/******************************************************************************
Initialize DMA buffer areas
 ******************************************************************************/
static int
dm35424_dma_initialize(struct dm35424_device_descriptor *dm35424_device,
				struct dm35424_ioctl_dma *dma)
{

	dma_addr_t bus_address;
	void *virtual_address;
	struct list_head *cursor;
	struct dm35424_dma_descriptor *dma_descriptor;

	int already_allocated = 0;

	/**
	 * Check that we haven't already allocated a buffer for
	 * this function block and channel.
	 */
    	list_for_each(cursor, &(dm35424_device->dma_descr_list)) {
        	struct dm35424_dma_descriptor *list_item;

        	list_item = list_entry(cursor,
        				struct dm35424_dma_descriptor,
        				list);

        	if (list_item->fb_num == dma->fb_num &&
        		list_item->channel == dma->channel &&
        		list_item->buffer == dma->buffer) {
				already_allocated = 1;
		}

	}

	if (already_allocated) {
		printk(KERN_WARNING "%s: Tried to initialize an already allocated "
			"DMA buffer.  Func block: %u, Channel: %d.\n",
			dm35424_device->name,
			dma->fb_num,
			dma->channel);
		return -EBUSY;
	}
	
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,0,0)
	dma_set_coherent_mask(dev,DMA_BIT_MASK(32)); 
	virtual_address = dma_alloc_coherent(&(pdev->dev), dma->buffer_size,
					&bus_address,

					GFP_KERNEL);
#else
virtual_address = dma_alloc_coherent(NULL, dma->buffer_size,
					&bus_address,
					GFP_KERNEL);
#endif
	if (virtual_address == NULL) {
		return -ENOMEM;
	}

	/* Write DMA bus address to the 64-bit register on the board
	*/
	dma->pci.data.data32 = (bus_address & 0xFFFFFFFF);

	dm35424_access_pci_region(dm35424_device,
					&(dma->pci),
					DM35424_PCI_REGION_ACCESS_WRITE);

	if (sizeof(bus_address) > 4) {
		dma->pci.data.data32 = bus_address >> 32;
	}
	else {
		dma->pci.data.data32 = 0;
	}
	dma->pci.offset += 4;

	dm35424_access_pci_region(dm35424_device,
					&(dma->pci),
					DM35424_PCI_REGION_ACCESS_WRITE);

	dma_descriptor = kmalloc(sizeof(struct dm35424_dma_descriptor), GFP_KERNEL);

	if (dma_descriptor == NULL) {
		printk(KERN_WARNING "%s: Could not allocate memory for DMA descriptor\n",
			dm35424_device->name);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,0,0)
		dma_free_coherent(&(pdev->dev), dma->buffer_size, virtual_address, bus_address);
#else
		dma_free_coherent(NULL, dma->buffer_size, virtual_address, bus_address);
#endif
		return -ENOMEM;

	}

	dma_descriptor->fb_num = dma->fb_num;
	dma_descriptor->channel = dma->channel;
	dma_descriptor->virt_addr = virtual_address;
	dma_descriptor->bus_addr = bus_address;
	dma_descriptor->buffer_size = dma->buffer_size;
	dma_descriptor->buffer = dma->buffer;

	list_add_tail(&(dma_descriptor->list), &(dm35424_device->dma_descr_list));

#ifdef DM35424_DEBUG_DMA
	printk(KERN_DEBUG "%s: Allocated DMA buffer for FB 0x%x, Channel %d, Buffer %d, Bus Addr %llx\n",
		dm35424_device->name,
		dma->fb_num,
		dma->channel,
		dma->buffer,
		bus_address);
#endif

	return 0;
}


/******************************************************************************
 * Release DMA buffers
 ******************************************************************************/
static void
dm35424_dma_release(struct dm35424_device_descriptor *dm35424_device)
{
	unsigned long irq_flags;
	struct list_head *cursor;
	struct list_head *next;

	list_for_each_safe(cursor, next, &(dm35424_device->dma_descr_list)) {
		struct dm35424_dma_descriptor *dma_descr;

		/*
		 * Get address of containing structure of this list_head; the structure
		 * is a DMA buffer list item
		 */
		spin_lock_irqsave(&(dm35424_device->device_lock), irq_flags);
		dma_descr = list_entry(cursor, struct dm35424_dma_descriptor, list);

		/*
		 * Delete element from list to prevent duplicate memory frees
		 */
		list_del(cursor);
		spin_unlock_irqrestore(&(dm35424_device->device_lock), irq_flags);

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,0,0)
		dma_free_coherent(&(pdev->dev), dma_descr->buffer_size,
                		                dma_descr->virt_addr,
                               			dma_descr->bus_addr);
#else
		dma_free_coherent(NULL, dma_descr->buffer_size,
                                		dma_descr->virt_addr,
                                		dma_descr->bus_addr);
#endif
	

#ifdef DM35424_DEBUG_DMA

		printk(KERN_DEBUG "%s: Releasing DMA resources for FB 0x%x, Channel %d, Buffer %d\n",
			dm35424_device->name,
			dma_descr->fb_num,
			dma_descr->channel,
			dma_descr->buffer);

#endif
		kfree(dma_descr);

	}

}


/******************************************************************************
Process the DMA-related function
 ******************************************************************************/
static int
dm35424_dma_function(struct dm35424_device_descriptor *dm35424_device,
			  unsigned long ioctl_param)
{

	int status = 0;
	union dm35424_ioctl_argument ioctl_argument;


	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Copy arguments in from user space and validate them
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	if (copy_from_user(&ioctl_argument,
			   (union dm35424_ioctl_argument *) ioctl_param,
			   sizeof(union dm35424_ioctl_argument))) {
		return -EFAULT;
	}

	status = dm35424_validate_dma(dm35424_device,
					   &(ioctl_argument.dma));
	if (status != 0) {

		return status;
	}

	switch(ioctl_argument.dma.function) {
	case DM35424_DMA_INITIALIZE:
		status = dm35424_dma_initialize(dm35424_device,
						&(ioctl_argument.dma));
		break;
	case DM35424_DMA_READ:
		status = dm35424_dma_read(dm35424_device,
						&(ioctl_argument.dma));
		break;
	case DM35424_DMA_WRITE:
		status = dm35424_dma_write(dm35424_device,
						&(ioctl_argument.dma));
		break;
	default:
		break;

	}

	return status;

}


/******************************************************************************
Handle ioctl(2) system calls
 ******************************************************************************/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
static int dm35424_ioctl(struct inode *inode,
			  struct file *file,
			  unsigned int request_code,
			  unsigned long ioctl_param)
#else
static long dm35424_ioctl(struct file *file,
			  unsigned int request_code,
			  unsigned long ioctl_param)
#endif
{


	int result = 0;
	struct dm35424_device_descriptor *dm35424_device;


	result = dm35424_validate_device(file->private_data);

	if (result != 0) {
		return result;
	}


	dm35424_device = (struct dm35424_device_descriptor *) file->private_data;


	switch (request_code) {

	case DM35424_IOCTL_REGION_READ:
		result = dm35424_pci_region_read(dm35424_device, ioctl_param);
		break;

	case DM35424_IOCTL_REGION_WRITE:
		result = dm35424_pci_region_write(dm35424_device, ioctl_param);
		break;

	case DM35424_IOCTL_REGION_MODIFY:
		result = dm35424_pci_region_modify(dm35424_device, ioctl_param);
		break;

	case DM35424_IOCTL_INTERRUPT_GET:
		result = dm35424_get_interrupt_info(dm35424_device, ioctl_param);
		break;

	case DM35424_IOCTL_DMA_FUNCTION:
		result = dm35424_dma_function(dm35424_device, ioctl_param);
		break;

	case DM35424_IOCTL_WAKEUP:
	{
		unsigned long irq_flags;
		spin_lock_irqsave(&(dm35424_device->device_lock),
		      irq_flags);
		dm35424_device->remove_isr_flag = 0xFF;
		spin_unlock_irqrestore(&(dm35424_device->device_lock),
			   irq_flags);
		wake_up_interruptible(&(dm35424_device->int_wait_queue));
		result = 0;
		break;
	}
	default:

		result = -ENOTTY;
		break;


	}


	return result;

}




/******************************************************************************
 Add an interrupt to the interrupt queue
 This function assumes the caller has a spinlock
 ******************************************************************************/
static void
dm35424_int_queue_add(struct dm35424_device_descriptor *dm35424_device,
			 int func_block_num)
{

	/*
	 * This is where the information is added to the queue if there is room
	 * otherwise we indicate queue overflow and log a missed interrupt
	 */

	if (dm35424_device->int_queue_count < DM35424_INT_QUEUE_SIZE) {
		/*
		 * Collect interrupt data and store in the device structure
		 */
		dm35424_device->interrupt_fb[dm35424_device->int_queue_in_marker] =
			func_block_num;

		dm35424_device->int_queue_in_marker++;

		if (dm35424_device->int_queue_in_marker == (DM35424_INT_QUEUE_SIZE)) {
			/*
			 * Wrap around to the front of the queue
			 */
			dm35424_device->int_queue_in_marker = 0;

		}

		dm35424_device->int_queue_count++;
#if defined(DM35424_DEBUG_INTERRUPTS)

		if (func_block_num < 0) {

			printk(KERN_DEBUG "%s: Adding DMA interrupt: FB%d (Count now: %d)\n",
				   dm35424_device->name, (func_block_num & 0x7FFFFFFF),
				   dm35424_device->int_queue_count);
		} else {
			   printk(KERN_DEBUG "%s: Adding interrupt: FB%d (Count now: %d)\n",
			   dm35424_device->name, func_block_num,
			   dm35424_device->int_queue_count);
		}
#endif

	} else {
		/*
		 * Indicate interrupt status queue overflow
		 */
		printk(KERN_WARNING
			   "%s: WARNING: Missed interrupt info because queue is full\n",
			   dm35424_device->name);

		dm35424_device->int_queue_missed++;

	}
}



/******************************************************************************
Put an interrupt in the queue for every bit in the interrupt status
This function assumes the caller has a spinlock
*******************************************************************************/
static int
dm35424_process_interrupt_status(struct dm35424_device_descriptor *dm35424_device)
{
	struct dm35424_pci_access_request pci_request;
	int fb_num;
	uint32_t fb_mask = 1;
	uint32_t irq_status_register = 0;
	uint32_t dma_irq_status_register = 0;
	uint32_t fb_clear_mask0 = 0;
	uint32_t fb_clear_mask1 = 0;
	int num_ints_processed = 0;

	/**
	 * Read the lower 32 bits of the IRQ and DMA IRQ registers
	 */
	pci_request.region = DM35424_PCI_REGION_GBC;
	pci_request.offset = DM35424_OFFSET_GBC_IRQ_STATUS;
	pci_request.size = DM35424_PCI_REGION_ACCESS_32;
	dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_READ);
	irq_status_register = pci_request.data.data32;

	pci_request.offset = DM35424_OFFSET_GBC_DMA_IRQ_STATUS;
	pci_request.size = DM35424_PCI_REGION_ACCESS_32;
	dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_READ);
	dma_irq_status_register = pci_request.data.data32;

#if defined(DM35424_DEBUG_INTERRUPTS)

		printk(KERN_DEBUG "%s: IRQ Status (lower 32): 0x%x  "
				  "DMA_IRQ Status (lower 32): 0x%x\n",
			   dm35424_device->name, irq_status_register,
			   dma_irq_status_register);
#endif

	if (irq_status_register || dma_irq_status_register) {

		for (fb_num = 0; fb_num < 32; fb_num ++) {

			if ((irq_status_register & fb_mask) ||
				(dma_irq_status_register & fb_mask)) {

				if (dma_irq_status_register & fb_mask) {

					dm35424_int_queue_add(dm35424_device,
								0x80000000 | fb_num);
				}
				if (irq_status_register & fb_mask) {

					dm35424_int_queue_add(dm35424_device,
								fb_num);
				}

				num_ints_processed++;
				/* Create our mask to clear the */
				/* status register when done */
				fb_clear_mask0 |= fb_mask;
			}

			fb_mask *= 2;

		}

	}


	/**
	 * Read the upper 32 bits of the IRQ and DMA IRQ registers
	 */
	pci_request.offset = DM35424_OFFSET_GBC_IRQ_STATUS + 4;
	dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_READ);
	irq_status_register = pci_request.data.data32;

	pci_request.offset = DM35424_OFFSET_GBC_DMA_IRQ_STATUS + 4;
	dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_READ);
	dma_irq_status_register = pci_request.data.data32;


#if defined(DM35424_DEBUG_INTERRUPTS)

		printk(KERN_DEBUG "%s: IRQ Status (upper 32): 0x%x  DMA_IRQ Status (upper 32): 0x%x\n",
			   dm35424_device->name, irq_status_register, dma_irq_status_register);
#endif

	/*
	 * Mask off the reserved bits before any processing
	 */
	irq_status_register &= 0x0FFFFFFF;
	dma_irq_status_register &= 0x0FFFFFFF;

	fb_mask = 1;
	if (irq_status_register || dma_irq_status_register) {

		for (fb_num = 0; fb_num < 28; fb_num ++) {

			if ((irq_status_register & fb_mask) ||
				(dma_irq_status_register & fb_mask)) {
				if (dma_irq_status_register & fb_mask) {
					dm35424_int_queue_add(dm35424_device,
								0x80000000 | (fb_num + 32));

				}

				if (irq_status_register & fb_mask) {

					dm35424_int_queue_add(dm35424_device,
								(fb_num + 32));
				}

				num_ints_processed++;

				/* Create our mask to clear the */
				/* status register when done */
				fb_clear_mask1 |= fb_mask;
			}

			fb_mask *= 2;

		}

	}

	/*
	 * Clear the status registers, if required
	 */

	if (fb_clear_mask0 != 0) {
		pci_request.offset = DM35424_OFFSET_GBC_IRQ_STATUS;
		pci_request.data.data32 = fb_clear_mask0;
		dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_WRITE);

		pci_request.offset = DM35424_OFFSET_GBC_DMA_IRQ_STATUS;
		dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_WRITE);

	}

	if (fb_clear_mask1 != 0) {
		pci_request.data.data32 = fb_clear_mask1;
		pci_request.offset = DM35424_OFFSET_GBC_IRQ_STATUS + 4;
		dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_WRITE);
		pci_request.offset = DM35424_OFFSET_GBC_DMA_IRQ_STATUS + 4;
		dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_WRITE);

	}

	return num_ints_processed;
}


/******************************************************************************
DM35424 device hardware initialization
 ******************************************************************************/
static void
dm35424_initialize_hardware(const struct dm35424_device_descriptor *dm35424_device)
{

	struct dm35424_pci_access_request pci_request;

	/*
	 * Reset the board
	 */
	pci_request.region = DM35424_PCI_REGION_GBC;
	pci_request.offset = DM35424_OFFSET_GBC_BOARD_RESET;
	pci_request.size = DM35424_PCI_REGION_ACCESS_8;
	pci_request.data.data8 = DM35424_BOARD_RESET_VALUE;

	dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_WRITE);

}



/******************************************************************************
DM35424 interrupt handler
 ******************************************************************************/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)
static irqreturn_t dm35424_interrupt_handler(int irq_number, void *device_id,
						struct pt_regs *registers)

#else
static irqreturn_t dm35424_interrupt_handler(int irq_number, void *device_id)
#endif
{
	struct dm35424_device_descriptor *dm35424_device;

	int result = 0;
	int interrupts_processed = 0;

	dm35424_device = (struct dm35424_device_descriptor *) device_id;

	/**
	 * Verify that the device ID passed is one of ours
	 */
	result = dm35424_validate_device(dm35424_device);

	if (result != 0) {

		printk(KERN_ERR
			"%s: ERROR: Invalid device descriptor in interrupt\n",
			dm35424_device->name);
		return IRQ_NONE;
	}


	spin_lock(&(dm35424_device->device_lock));

	/*
	 * Verify this IRQ number is ours
	 */
	if (irq_number != dm35424_device->irq_number) {


		printk(KERN_ERR
			"%s: ERROR: IRQ passed (%d) to handler "
			"was not device IRQ (%d)\n",
			dm35424_device->name,
			irq_number,
			dm35424_device->irq_number);
		spin_unlock(&(dm35424_device->device_lock));
		return IRQ_NONE;
	}

	if (dm35424_device->int_queue_missed > TOO_MANY_MISSED_IRQ) {
		printk(KERN_EMERG "%s: Missed %d interrupts due to full queue.  Resetting board.",
		 	dm35424_device->name, TOO_MANY_MISSED_IRQ);
		 dm35424_initialize_hardware(dm35424_device);

		 spin_unlock(&(dm35424_device->device_lock));
		 return IRQ_HANDLED;
	 }

	interrupts_processed = dm35424_process_interrupt_status(dm35424_device);

	/* No interrupts found?  Must be someone else's IRQ */
	if (interrupts_processed == 0) {
		spin_unlock(&(dm35424_device->device_lock));

		return IRQ_NONE;
	}

	spin_unlock(&(dm35424_device->device_lock));

	wake_up_interruptible(&(dm35424_device->int_wait_queue));
#if (defined(DM35424_DEBUG) || defined(DM35424_DEBUG_INTERRUPTS))
	printk(KERN_INFO "%s Interrupt Handled\n", dm35424_device->name);
#endif
	return IRQ_HANDLED;

}


/******************************************************************************
Release region resources
 ******************************************************************************/
static void
dm35424_release_region_resources(struct dm35424_device_descriptor *dm35424_device)
{
	int region;

	for (region = 0; region < DM35424_PCI_NUM_REGIONS; region++) {

		/*
		 * Determine how region is mapped
		 */

		if (dm35424_device->pci[region].virt_addr != NULL) {

			/*
			 * Region is memory-mapped
			 */

			/*
			 * If memory range allocation succeeded, free the range
			 */

			if (dm35424_device->pci[region].allocated != 0x00) {
				release_mem_region(dm35424_device->pci[region].phys_addr,
						   dm35424_device->pci[region].length);

				printk(KERN_INFO
					   "%s: Released I/O memory range %#lx-%#lx\n",
					   &((dm35424_device->name)[0]), (unsigned long)
					   dm35424_device->pci[region].phys_addr,
					   ((unsigned long)
					dm35424_device->pci[region].phys_addr +
					dm35424_device->pci[region].length - 1));
			}

			/*
			 * Unmap region from kernel's address space
			 */

			iounmap(dm35424_device->pci[region].virt_addr);

			printk(KERN_INFO
				   "%s: Unmapped kernel mapping at %#lx\n",
				   &((dm35424_device->name)[0]),
				   (unsigned long)
				   dm35424_device->pci[region].virt_addr);
		} else if (dm35424_device->pci[region].io_addr != 0) {

			/*
			 * Region is I/O-mapped
			 */

			/*
			 * Free I/O port range
			 */

			release_region(dm35424_device->pci[region].
					   phys_addr,
					   dm35424_device->pci[region].
					   length);

			printk(KERN_INFO
				   "%s: Released I/O port range %#lx-%#lx\n",
				   &((dm35424_device->name)[0]),
				   (unsigned long)
				   dm35424_device->pci[region].phys_addr,
				   ((unsigned long)
				dm35424_device->pci[region].phys_addr +
				dm35424_device->pci[region].length - 1));
		}
	}


}


/******************************************************************************
Release resources allocated by driver
 ******************************************************************************/

static void dm35424_release_resources(void)
{
	uint32_t minor_number;

	if (dm35424_devices != NULL) {
		/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		   Release device-level resources
		   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

		for (minor_number = 0; minor_number < dm35424_device_count; minor_number++) {

			struct dm35424_device_descriptor *dm35424_device;

			dm35424_device = &((dm35424_devices)[minor_number]);

			/*
			 * Free any allocated IRQ
			 */

			if (dm35424_device->irq_number != 0) {
				free_irq(dm35424_device->irq_number, dm35424_device);
				printk(KERN_INFO "%s: Freed IRQ %u\n",
					   &((dm35424_device->name)[0]),
					   dm35424_device->irq_number);
			}

			/*
			 * Free any resources allocated for the PCI regions
			 */

			dm35424_release_region_resources(dm35424_device);

		}

		/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		   Release driver-level resources
		   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

		/*
		 * Free device descriptor memory after all references to it are finished
		 */

		kfree(dm35424_devices);
		dm35424_devices = NULL;
	}
}


/******************************************************************************
Set up standard PCI regions
 ******************************************************************************/
static int
dm35424_process_pci_regions(struct dm35424_device_descriptor * dm35424_device,
			   const struct pci_dev *pci_device)
{
	uint8_t region;

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Process each standard PCI region
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	for (region = 0; region < DM35424_PCI_NUM_REGIONS; region++) {
		unsigned long address = 0;
		unsigned long length;
		unsigned long flags;

		/*#####################################################################
		   Get region's physical address and length in bytes.  If either is zero,
		   the region is unused and should be ignored.
		   ################################################################## */

		address = pci_resource_start(pci_device, region);
		if (address == 0) {
			continue;
		}

		length = pci_resource_len(pci_device, region);
		if (length == 0) {
			continue;
		}

		/*#####################################################################
		   Save information in PCI region descriptor
		   ################################################################## */

		dm35424_device->pci[region].phys_addr = address;
		dm35424_device->pci[region].length = length;

		/*#####################################################################
		   Determine how the region is mapped
		   ################################################################## */

		flags = pci_resource_flags(pci_device, region);

		if (flags & IORESOURCE_IO) {

			/*
			 * The region is I/O mapped
			 */

			/*
			 * Allocate the I/O port range
			 */

			if (request_region
				(address, length, &((dm35424_device->name)[0])) == NULL) {
				printk(KERN_ERR
					   "%s: ERROR: I/O port range %#lx-%#lx allocation FAILED\n",
					   &((dm35424_device->name)[0]),
					   address, (address + length - 1)
					);
				dm35424_release_resources();
				return -EBUSY;
			}

			dm35424_device->pci[region].io_addr = address;

			printk(KERN_INFO
				   "%s: Allocated I/O port range %#lx-%#lx\n",
				   &((dm35424_device->name)[0]), address,
				   (address + length - 1)
				);
		} else if (flags & IORESOURCE_MEM) {

			/*
			 * The region is memory mapped
			 */

			/*
			 * Remap the region's physical address into the kernel's virtual
			 * address space and allocate the memory range
			 */

			dm35424_device->pci[region].virt_addr =
				ioremap(address, length);
			if (dm35424_device->pci[region].virt_addr == NULL) {
				printk(KERN_ERR
					   "%s: ERROR: BAR%u remapping FAILED\n",
					   &((dm35424_device->name)[0]),
					   region);
				dm35424_release_resources();
				return -ENOMEM;
			}

			if (request_mem_region
				(address, length, &((dm35424_device->name)[0])
				)
				== NULL) {
				printk(KERN_ERR
					   "%s: ERROR: I/O memory range %#lx-%#lx allocation FAILED\n",
					   &((dm35424_device->name)[0]),
					   address, (address + length - 1)
					);
				dm35424_release_resources();
				return -EBUSY;
			}

			dm35424_device->pci[region].allocated = 0xFF;

			printk(KERN_INFO
				   "%s: Allocated I/O memory range %#lx-%#lx\n",
				   &((dm35424_device->name)[0]), address,
				   (address + length - 1)
				);
		} else {

			/*
			 * The region has invalid resource flags
			 */

			printk(KERN_ERR "%s: ERROR: Invalid PCI region flags\n",
				   &((dm35424_device->name)[0])
				);
			dm35424_release_resources();
			return -EIO;
		}

		/*#####################################################################
		   Print information about the region
		   ################################################################## */

		printk(KERN_INFO "%s: BAR%u Region:\n",
			   &((dm35424_device->name)[0]), region);

		if (dm35424_device->pci[region].io_addr != 0) {
			printk(KERN_INFO "	Address: %#lx (I/O mapped)\n",
				   dm35424_device->pci[region].io_addr);
		} else {
			printk(KERN_INFO "	Address: %#lx (memory mapped)\n",
				   (unsigned long)dm35424_device->pci[region].
				   virt_addr);
			printk(KERN_INFO "	Address: %#lx (physical)\n",
				   dm35424_device->pci[region].phys_addr);
		}

		printk(KERN_INFO "	Length:  %#lx\n",
			   dm35424_device->pci[region].length);
	}

	return 0;
}


/******************************************************************************
 IRQ line allocation
 ******************************************************************************/
static int
dm35424_allocate_irq(struct dm35424_device_descriptor *dm35424_device,
			const struct pci_dev *pci_device)
{
	int status;

	/*
	 * The fourth request_irq() argument MUST refer to memory which will remain
	 * valid until the driver is unloaded.  request_irq() simply stores this
	 * address in a structure rather than making a copy of the string it points
	 * to.
	 */
	dm35424_device->irq_number = pci_device->irq;
	status = request_irq(pci_device->irq,
				(irq_handler_t) dm35424_interrupt_handler,
				 IRQF_SHARED,
				 dm35424_device->name,
				 (void *) dm35424_device);
	if (status != 0) {
		printk(KERN_ERR
			   "%s: ERROR: Unable to allocate IRQ %u (error = %u)\n",
			   &((dm35424_device->name)[0]), pci_device->irq,
			   -status);
		dm35424_device->irq_number = 0;
		dm35424_release_resources();
		return status;
	}

	printk(KERN_INFO "%s: Allocated IRQ %u\n",
		   &((dm35424_device->name)[0]), pci_device->irq);

	return 0;
}


/******************************************************************************
Determine whether or not a device is readable
 ******************************************************************************/
static unsigned int
dm35424_poll(struct file *file, struct poll_table_struct *poll_table)
{
	struct dm35424_device_descriptor *dm35424_device;
	unsigned int interrupts_in_queue;
	unsigned int status_mask = 0;
	unsigned long irq_flags;

	/*
	 * If we don't have a valid DM35424 device descriptor, no status is available
	 */

	if (dm35424_validate_device(file->private_data) != 0) {

		/*
		 * This value causes select(2) to indicate that a file descriptor is
		 * present in its file descriptor sets but it will be in the exception
		 * set rather than in the input set.
		 */

		return POLLPRI;
	}

	dm35424_device = (struct dm35424_device_descriptor *) file->private_data;

	/*
	 * Register with the file system layer so that it can wait on and check for
	 * DM35424 events
	 */

	poll_wait(file, &(dm35424_device->int_wait_queue), poll_table);

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Waiting is done interruptibly, which means that a signal could have been
	   delivered.  Thus we might have been woken up by a signal before an
	   interrupt occurred.  Therefore, the process needs to examine the device's
	   interrupt flag.
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/*
	 * Prevent a race condition with the interrupt handler and make a local copy
	 * of the interrupt count
	 */

	spin_lock_irqsave(&(dm35424_device->device_lock), irq_flags);

	interrupts_in_queue = dm35424_device->int_queue_count;

	if (dm35424_device->remove_isr_flag) {
		status_mask = (POLLIN | POLLRDNORM);
	}

	spin_unlock_irqrestore(&(dm35424_device->device_lock), irq_flags);

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Interpret interrupt flag
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/*
	 * The flag is cleared after reading.  See if it is clear or not.
	 */

	if (interrupts_in_queue > 0) {

		status_mask |= (POLLIN | POLLRDNORM);

	}

	return status_mask;
}


/******************************************************************************
Probe and configure all DM35424 devices
 ******************************************************************************/
static int
dm35424_probe_devices(uint32_t *device_count,
			 struct dm35424_device_descriptor **device_descriptors)
{
	struct pci_dev *pci_device = NULL;
	uint32_t minor_number;
	struct dm35424_device_descriptor *dm35424_device;
	struct dm35424_pci_access_request pci_request;
	int err, name_len;

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Count the number of supported devices in the kernel's PCI device list
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	*device_count = 0;

	while ((pci_device = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, pci_device))
		   != NULL) {

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2, 6, 12 ))
		if (pci_match_device(dm35424_pci_device_table, pci_device) == NULL) {
#else
		if (pci_match_id(dm35424_pci_device_table, pci_device) == NULL) {
#endif
			continue;
		}

		(*device_count)++;
	}

	if (*device_count == 0) {
		printk(KERN_ERR "%s: ERROR: No devices found\n", DRIVER_NAME);
		return -ENODEV;
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Allocate memory for the device descriptors
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	*device_descriptors = kmalloc((*device_count *
				sizeof(struct dm35424_device_descriptor)),
				GFP_KERNEL);
	if (*device_descriptors == NULL) {
		printk(KERN_ERR
			   "%s: ERROR: Device descriptor memory allocation FAILED\n",
			   DRIVER_NAME);
		return -ENOMEM;
	}

	// Clear memory allocated.
	memset(*device_descriptors,
		   0, (*device_count * sizeof(struct dm35424_device_descriptor)));

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Set up all DM35424 devices
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	pci_device = NULL;
	minor_number = 0;

	while ((pci_device = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, pci_device))
			!= NULL) {

		/*
		 * See if the current PCI device is in the table of devices supported
		 * by the driver.  If not, just ignore it and go to next PCI device.
		 */

#if (LINUX_VERSION_CODE < KERNEL_VERSION( 2, 6, 12 ))
		if (pci_match_device(dm35424_pci_device_table, pci_device) == NULL) {
#else
		if (pci_match_id(dm35424_pci_device_table, pci_device) == NULL) {
#endif
			continue;
		}

		printk(KERN_INFO
			   "%s: Minor %u: DM35424 found at bus %u, slot %02X, function %02X\n",
			   DRIVER_NAME,
			   minor_number,
			   pci_device->bus->number,
			   PCI_SLOT(pci_device->devfn),
			   PCI_FUNC(pci_device->devfn)
			);

		dm35424_device = &((*device_descriptors)[minor_number]);

		spin_lock_init(&(dm35424_device->device_lock));

		dm35424_init_device_desc(dm35424_device);

		/*
		 * Create the full device name
		 */

		name_len = snprintf(&((dm35424_device->name)[0]),
				  DM35424_NAME_LENGTH,
				  "%s-%u", DRIVER_NAME, minor_number);
		if (name_len >= DM35424_NAME_LENGTH) {
			printk(KERN_ERR
				   "%s-%u> ERROR: Device name creation FAILED\n",
				   DRIVER_NAME, minor_number);
			dm35424_release_resources();
			pci_dev_put(pci_device);
			return -ENAMETOOLONG;
		}

		err = pci_enable_device(pci_device);

		if (err) {
			printk(KERN_ERR "%s: Error attempting to enable PCI device.\n", dm35424_device->name);
			dm35424_release_resources();
			pci_dev_put(pci_device);
			return err;

		}

		/*
		 * Determine 1) how many standard PCI regions are present, 2) how the
		 * regions are mapped, and 3) how many bytes are in each region.  Also,
		 * remap any memory-mapped region into the kernel's address space.
		 */
		err = dm35424_process_pci_regions(dm35424_device, pci_device);
		if (err) {
			printk(KERN_ERR "%s: Error processing PCI regions.\n", dm35424_device->name);
			dm35424_release_resources();
			pci_dev_put(pci_device);
			return err;
		}

		/*
		 * Associate device IRQ line with device in kernel
		 */
		err = dm35424_allocate_irq(dm35424_device, pci_device);
		if (err) {
			printk(KERN_ERR "%s: Error allocating IRQ.\n", dm35424_device->name);
			dm35424_release_resources();
			pci_dev_put(pci_device);
			return err;
		}

		/*
		 * Read and print FPGA version information
		 */

		pci_request.region = DM35424_PCI_REGION_GBC;
		pci_request.offset = DM35424_OFFSET_GBC_FPGA_BUILD;
		pci_request.size = DM35424_PCI_REGION_ACCESS_32;

		dm35424_access_pci_region(dm35424_device, &pci_request,
					 DM35424_PCI_REGION_ACCESS_READ);

		printk(KERN_INFO "%s: FPGA version: %u\n",
			   dm35424_device->name,
			   (uint32_t) pci_request.data.data32);

		dm35424_initialize_hardware(dm35424_device);

		pci_set_master(pci_device);
		
		#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
		pdev = pci_device;
		#endif

		minor_number++;
	}

	printk(KERN_INFO "%s: Found %u DM35424 device(s)\n", DRIVER_NAME,
		   *device_count);

	return 0;
}



/******************************************************************************
Unregister DM35424 character devices
 ******************************************************************************/
static int dm35424_unregister_char_device(void)
{
	unsigned int minor = 0;

	cdev_del(&dm35424_cdev);

	for (minor = 0; minor < dm35424_device_count; minor ++) {
		device_destroy(dev_class, MKDEV(dm35424_major, minor));

	}

	class_unregister(dev_class);

	class_destroy(dev_class);

	unregister_chrdev_region(MKDEV(dm35424_major, 0), dm35424_device_count);
	return 0;
}


/******************************************************************************
Initialization of board upon driver module load
 ******************************************************************************/
int dm35424_init(void)
{
	int status;

	printk(KERN_INFO "%s: Initializing module (version %s).\n",
		   DRIVER_NAME, DRIVER_VERSION);

	printk(KERN_INFO "%s: %s\n", DRIVER_NAME, DRIVER_DESCRIPTION);
	printk(KERN_INFO "%s: %s\n", DRIVER_NAME, DRIVER_COPYRIGHT);

	dm35424_devices = NULL;
	dm35424_major = 0;

	status = dm35424_probe_devices(&dm35424_device_count, &dm35424_devices);
	if (status != 0) {
		return status;
	}

	status = dm35424_register_char_device(&dm35424_major);
	if (status != 0) {
		printk(KERN_ERR
			   "%s: ERROR: Dynamic character device registration FAILED (errno "
			   "= %d)\n", DRIVER_NAME, -status);
		dm35424_release_resources();
		return status;
	}

	printk(KERN_INFO
		   "%s: Driver registered using character major number %d\n",
		   DRIVER_NAME, dm35424_major);

	return 0;
}


/******************************************************************************
Perform a board reset
 ******************************************************************************/
static void dm35424_board_reset(struct dm35424_device_descriptor *dm35424_device) {

	struct dm35424_pci_access_request pci_request;
	/*
	 * Reset the board
	 */
	pci_request.region = DM35424_PCI_REGION_GBC;
	pci_request.offset = DM35424_OFFSET_GBC_BOARD_RESET;
	pci_request.size = DM35424_PCI_REGION_ACCESS_8;
	pci_request.data.data8 = DM35424_BOARD_RESET_VALUE;

	dm35424_access_pci_region(dm35424_device, &pci_request,
				 DM35424_PCI_REGION_ACCESS_WRITE);

}


/******************************************************************************
Open a DM35424 device
 ******************************************************************************/
static int dm35424_open(struct inode *inode, struct file *file)
{


	struct dm35424_device_descriptor *dm35424_device;
	unsigned int minor_number;
	unsigned long irq_flags;
	
	minor_number = iminor(inode);

	dm35424_device = &(dm35424_devices[minor_number]);
	
	spin_lock_irqsave(&(dm35424_device->device_lock), irq_flags);
	
	if (dm35424_device->reference_count) {
		spin_unlock_irqrestore(&(dm35424_device->device_lock), irq_flags);
		return -EBUSY;
	}
	dm35424_device->reference_count++;
	file->private_data = dm35424_device;
	spin_unlock_irqrestore(&(dm35424_device->device_lock), irq_flags);
	
	dm35424_init_device_desc(dm35424_device);

	return 0;
}


/******************************************************************************
Close a DM35424 device file
 ******************************************************************************/

static int dm35424_release(struct inode *inode, struct file *file)
{

	struct dm35424_device_descriptor *dm35424_device;
	unsigned long irq_flags;
	
	/*
	 * If we don't have a valid DM35424 device descriptor, no status is available
	 */
	if (dm35424_validate_device(file->private_data) != 0) {

		return -EBADF;
	}

	dm35424_device = (struct dm35424_device_descriptor *) file->private_data;
	dm35424_board_reset(dm35424_device);

	dm35424_dma_release(dm35424_device);
	
	spin_lock_irqsave(&(dm35424_device->device_lock), irq_flags);
	dm35424_device->reference_count--;
	file->private_data = NULL;
	spin_unlock_irqrestore(&(dm35424_device->device_lock), irq_flags);

	return 0;

}


/******************************************************************************
Deinitialize DM35424 driver and devices
 ******************************************************************************/
void dm35424_unload(void)
{
	int status;


	dm35424_release_resources();

	status = dm35424_unregister_char_device();
	if (status != 0) {
		printk(KERN_ERR
			   "%s: ERROR: Character device unregistration FAILED (errno "
			   " = %d)!\n", DRIVER_NAME, -status);
		printk(KERN_ERR
			   "%s: ERROR: A system reboot should be performed\n",
			   DRIVER_NAME);
	}

	printk(KERN_INFO "%s: Character device %d unregistered\n",
		   DRIVER_NAME, dm35424_major);



	printk(KERN_INFO "%s: Module unloaded.\n", DRIVER_NAME);
}


/******************************************************************************
Define the functions to execute when the driver module is loaded and unloaded
 ******************************************************************************/
module_init(dm35424_init);
module_exit(dm35424_unload);


/******************************************************************************
Set module properties
 ******************************************************************************/
MODULE_AUTHOR(DRIVER_COPYRIGHT);
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_LICENSE("GPL");


/******************************************************************************
Set file operation functions
 ******************************************************************************/
static struct file_operations dm35424_file_ops = {
	.owner = THIS_MODULE,
	.poll = dm35424_poll,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
	.ioctl = dm35424_ioctl,
#else
	.unlocked_ioctl = dm35424_ioctl,
#endif
	.open = dm35424_open,
	.release = dm35424_release,
};


