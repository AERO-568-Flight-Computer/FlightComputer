/**
    @file

    @brief
        Definitions for the DM35424 Reference Adjustment Library.

    $Id: dm35424_ref_adjust_library.h 60276 2012-06-05 16:04:15Z rgroner $
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


#ifndef _DM35424_REF_ADJUST_LIBRARY__H_
#define _DM35424_REF_ADJUST_LIBRARY__H_


#include "dm35424_gbc_library.h"

#ifdef __cplusplus
extern "C" {
#endif


 /**
  * @defgroup DM35424_Ref_Adjust_Library_Constants DM35424 Reference Adjustment Library Constants
  * @{
 */

/*=============================================================================
Constants
 =============================================================================*/
/**
 * @brief
 *      Register value for SPI Interface is busy.
 */
#define DM35424_REF_ADJUST_SPI_BUSY		0x00

/**
 * @brief
 * 	Register value for SPI Interface is ready.
 */
#define DM35424_REF_ADJUST_SPI_READY	0x01

/**
 * @brief
 * 	Register value for starting the SPI transaction.
 */
#define DM35424_REF_ADJUST_START_TRANS	0x01

/**
 * @brief
 * 	Register value for writing to the ADC Volatile memory.
 */
#define DM35424_REF_ADJUST_WRITE_ADC_VOLATILE	0x0100

/**
 * @brief
 * 	Register value for writing to the DAC Volatile memory.
 */
#define DM35424_REF_ADJUST_WRITE_DAC_VOLATILE	0x0200

/**
 * @brief
 * 	Register value for writing to the ADC Non-Volatile memory.
 */
#define DM35424_REF_ADJUST_WRITE_ADC_NON_VOLATILE	0x1100

/**
 * @brief
 * 	Register value for writing to the DAC Non-Volatile memory.
 */
#define DM35424_REF_ADJUST_WRITE_DAC_NON_VOLATILE	0x1200

/**
 * @brief
 * 	Register value for copying ADC data from Volatile to Non-Volatile
 */
#define DM35424_REF_ADJUST_COPY_ADC_VOL_TO_NON	0x2100

/**
 * @brief
 * 	Register value for copying DAC data from Volatile to Non-Volatile
 */
#define DM35424_REF_ADJUST_COPY_DAC_VOL_TO_NON	0x2200

/**
 * @brief
 * 	Register value for copying ADC and DAC data from Volatile
 *      to Non-Volatile
 */
#define DM35424_REF_ADJUST_COPY_BOTH_VOL_TO_NON	0x2300

/**
 * @brief
 * 	Register value for copying ADC data from Non-Volatile to Volatile
 */
#define DM35424_REF_ADJUST_COPY_ADC_NON_TO_VOL	0x3100

/**
 * @brief
 * 	Register value for copying DAC data from Non-Volatile to Volatile
 */
#define DM35424_REF_ADJUST_COPY_DAC_NON_TO_VOL	0x3200

/**
 * @brief
 * 	Register value for copying ADC and DAC data from Non-Volatile to Volatile
 */
#define DM35424_REF_ADJUST_COPY_BOTH_NON_TO_VOL	0x3300



/**
 * @brief
 *      Direction of Reference Adjustment data copy action
 */
enum DM35424_Copy_Directions {

	/**
	 * ADC Volatile to Non-Volatile
	 */
	DM35424_ADC_VOL_TO_NON_VOL,

	/**
	 * DAC Volatile to Non-Volatile
	 */
	DM35424_DAC_VOL_TO_NON_VOL,

	/**
	 * ADC and DAC Volatile to Non-Volatile
	 */
	DM35424_BOTH_VOL_TO_NON_VOL,

	/**
	 * ADC Non-Volatile to Volatile
	 */
	DM35424_ADC_NON_VOL_TO_VOL,

	/**
	 * DAC Non-Volatile to Volatile
	 */
	DM35424_DAC_NON_VOL_TO_VOL,

	/**
	 * ADC and DAC Non-Volatile to Volatile
	 */
	DM35424_BOTH_NON_VOL_TO_VOL

};

/**
 * @} DM35424_Ref_Adjust_Library_Constants
 */


/**
 * @defgroup DM35424_Ref_Adjust_Library_Functions DM35424 Reference Adjustment Public Library Functions
 * @{
*/


/**
*******************************************************************************
@brief
   Open the reference adjustment function block, getting address values that
   will be used later by other library functions.

@param
   handle

   Address of the handle pointer, which will contain the device
   descriptor.

@param
   ordinal_to_open

   Which function block on the board to open (0th, 1st, 2nd, etc)

@param
   fb_temp

   Pointer to function block structure that will hold register offset values.

@retval
   0

   Success.

@retval
   Non-Zero

   Failure.
*/
DM35424LIB_API
int DM35424_Ref_Adjust_Open(struct DM35424_Board_Descriptor *handle,
				unsigned int ordinal_to_open,
				struct DM35424_Function_Block *fb_temp);



/**
*******************************************************************************
@brief
   Write the ADC Reference Adjustment value to volatile memory.

@param
   handle

   Address of the handle pointer, which will contain the device
   descriptor.

@param
   fb

   Address of the function block that contains the reference adjustment we're
   using.

@param
   adjustment

   Reference adjustment value.

@retval
   0

   Success.

@retval
   Non-Zero

   Failure.
*/
DM35424LIB_API
int DM35424_Ref_Adjust_Write_Adc_To_Volatile(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					uint8_t adjustment);


/**
*******************************************************************************
@brief
   Write the ADC Reference Adjustment value to non-volatile memory.

@param
   handle

   Address of the handle pointer, which will contain the device
   descriptor.

@param
   fb

   Address of the function block that contains the reference adjustment we're
   using.

@param
   adjustment

   Reference adjustment value.

@retval
   0

   Success.

@retval
   Non-Zero

   Failure.
*/
DM35424LIB_API
int DM35424_Ref_Adjust_Write_Adc_To_NonVolatile(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					uint8_t adjustment);


/**
*******************************************************************************
@brief
   Write the DAC Reference Adjustment value to volatile memory.

@param
   handle

   Address of the handle pointer, which will contain the device
   descriptor.

@param
   fb

   Address of the function block that contains the reference adjustment we're
   using.

@param
   adjustment

   Reference adjustment value.

@retval
   0

   Success.

@retval
   Non-Zero

   Failure.
*/
DM35424LIB_API
int DM35424_Ref_Adjust_Write_Dac_To_Volatile(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					uint8_t adjustment);


/**
*******************************************************************************
@brief
   Write the DAC Reference Adjustment value to non-volatile memory.

@param
   handle

   Address of the handle pointer, which will contain the device
   descriptor.

@param
   fb

   Address of the function block that contains the reference adjustment we're
   using.

@param
   adjustment

   Reference adjustment value.

@retval
   0

   Success.

@retval
   Non-Zero

   Failure.
*/
DM35424LIB_API
int DM35424_Ref_Adjust_Write_Dac_To_NonVolatile(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					uint8_t adjustment);


/**
*******************************************************************************
@brief
   Copy the reference adjustment data from volatile to non-volatile, or vice
   versa.

@param
   handle

   Address of the handle pointer, which will contain the device
   descriptor.

@param
   fb

   Address of the function block that contains the reference adjustment we're
   using.

@param
   direction

   Direction of the copy, including whether it is ADC, DAC or both.

@retval
   0

   Success.

@retval
   Non-Zero

   Failure.
*/
DM35424LIB_API
int DM35424_Ref_Adjust_Copy_Data(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *fb,
					enum DM35424_Copy_Directions direction);



/**
 * @} DM35424_Ref_Adjust_Library_Functions
 */

#ifdef __cplusplus
}
#endif

#endif
