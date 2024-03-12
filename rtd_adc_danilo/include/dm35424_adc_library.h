/**
    @file

    @brief
        Definitions for the DM35424 ADC Library

    $Id: dm35424_adc_library.h 106898 2017-03-08 13:44:23Z rgroner $
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

#ifndef _DM35424_ADC_LIBRARY__H_
#define _DM35424_ADC_LIBRARY__H_

#include "dm35424_gbc_library.h"

#ifdef __cplusplus
extern "C" {
#endif


 /**
  * @defgroup DM35424_Adc_Library_Constants DM35424 ADC Library Constants
  * @{
 */

/*=============================================================================
Constants
 =============================================================================*/

/**
 * @brief
 *      Register value for ADC Mode Reset
 */
#define DM35424_ADC_MODE_RESET			0x00
/**
 * @brief
 *      Register value for ADC Mode Pause
 */
#define DM35424_ADC_MODE_PAUSE			0x01

/**
 * @brief
 *      Register value for ADC Mode Go (Single Shot)
 */
#define DM35424_ADC_MODE_GO_SINGLE_SHOT		0x02

/**
 * @brief
 *      Register value for ADC Mode Go (Rearm after Stop)
 */
#define DM35424_ADC_MODE_GO_REARM		0x03
/**
 * @brief
 *      Register value for ADC Mode Uninitialized
 */
#define DM35424_ADC_MODE_UNINITIALIZED		0x04

/**
 * @brief
 *      Register value for ADC Status - Stopped
 */
#define DM35424_ADC_STAT_STOPPED		0x00

/**
 * @brief
 *      Register value for ADC Status - Filling Pre-Start Buffer
 */
#define DM35424_ADC_STAT_FILLING_PRE_TRIG_BUFF	0x01

/**
 * @brief
 *      Register value for ADC Status - Waiting for Start Trigger
 */
#define DM35424_ADC_STAT_WAITING_START_TRIG	0x02

/**
 * @brief
 *      Register value for ADC Status - Sampling Data
 */
#define DM35424_ADC_STAT_SAMPLING		0x03

/**
 * @brief
 *      Register value for ADC Status - Filling Post-Stop Buffer
 */
#define DM35424_ADC_STAT_FILLING_POST_TRIG_BUFF	0x04

/**
 * @brief
 *      Register value for ADC Status - Wait for Rearm
 */
#define DM35424_ADC_STAT_WAIT_REARM		0x05

/**
 * @brief
 *      Register value for ADC Status - Done
 */
#define DM35424_ADC_STAT_DONE			0x07

/**
 * @brief
 *      Register value for ADC Status - Uninitialized
 */
#define DM35424_ADC_STAT_UNINITIALIZED		0x08

/**
 * @brief
 *      Register value for ADC Status - Initializing
 */
#define DM35424_ADC_STAT_INITIALIZING		0x09

/**
 * @brief
 *      Register value for Interrupt Mask - Sample Taken
 */
#define DM35424_ADC_INT_SAMPLE_TAKEN_MASK		0x01

/**
 * @brief
 *      Register value for Interrupt Mask - Channel Threshold Exceeded
 */
#define DM35424_ADC_INT_CHAN_THRESHOLD_MASK		0x02

/**
 * @brief
 *      Register value for Interrupt Mask - Pre-Start Buffer Filled
 */
#define DM35424_ADC_INT_PRE_BUFF_FULL_MASK		0x04

/**
 * @brief
 *      Register value for Interrupt Mask - Start Trigger Occurred
 */
#define DM35424_ADC_INT_START_TRIG_MASK		0x08

/**
 * @brief
 *      Register value for Interrupt Mask - Stop Trigger Occurred
 */
#define DM35424_ADC_INT_STOP_TRIG_MASK		0x10

/**
 * @brief
 *      Register value for Interrupt Mask - Post-Stop Buffer Filled
 */
#define DM35424_ADC_INT_POST_BUFF_FULL_MASK		0x20

/**
 * @brief
 *      Register value for Interrupt Mask - Sampling Complete
 */
#define DM35424_ADC_INT_SAMP_COMPL_MASK		0x40

/**
 * @brief
 *      Register value for Interrupt Mask - Pacer Clock Tick Occurred
 */
#define DM35424_ADC_INT_PACER_TICK_MASK		0x80

/**
 * @brief
 *      Register value for Interrupt Mask - All Bits
 */
#define DM35424_ADC_INT_ALL_MASK			0xFF

/**
 * @brief
 *      Register value for Channel Low Threshold Interrupt
 */
#define DM35424_ADC_CHAN_INTR_LOW_THRESHOLD_MASK	0x01

/**
 * @brief
 *      Register value for Channel High Threshold Interrupt
 */
#define DM35424_ADC_CHAN_INTR_HIGH_THRESHOLD_MASK	0x02

/**
 * @brief
 *      Register value for Channel Filter Order 0
 */
#define DM35424_ADC_CHAN_FILTER_ORDER0		0x0

/**
 * @brief
 *      Register value for Channel Filter Order 1
 */
#define DM35424_ADC_CHAN_FILTER_ORDER1		0x1

/**
 * @brief
 *      Register value for Channel Filter Order 2
 */
#define DM35424_ADC_CHAN_FILTER_ORDER2		0x2

/**
 * @brief
 *      Register value for Channel Filter Order 3
 */
#define DM35424_ADC_CHAN_FILTER_ORDER3		0x3

/**
 * @brief
 *      Register value for Channel Filter Order 4
 */
#define DM35424_ADC_CHAN_FILTER_ORDER4		0x4

/**
 * @brief
 *      Register value for Channel Filter Order 5
 */
#define DM35424_ADC_CHAN_FILTER_ORDER5		0x5

/**
 * @brief
 *      Register value for Channel Filter Order 6
 */
#define DM35424_ADC_CHAN_FILTER_ORDER6		0x6

/**
 * @brief
 *      Register value for Channel Filter Order 7
 */
#define DM35424_ADC_CHAN_FILTER_ORDER7		0x7

/**
 * @brief
 *    Register value for setting channel power to active
 */
#define DM35424_ADC_FE_CONFIG_POWER_ACTIVE			0x80

/**
 * @brief
 *    Register value for setting channel PGA to active
 */
#define DM35424_ADC_FE_CONFIG_PGA_ACTIVE			0x40

/**
 * @brief
 *    Register value for setting channel input switch to enabled.
 */
#define DM35424_ADC_FE_CONFIG_IN_SWITCH_ENABLED		0x20

/**
 * @brief
 *    Register value for measuring between DACx Chy and VREF (2.5V)
 */
#define DM35424_ADC_FE_CONFIG_DAC_LOOPBACK			0x00

/**
 * @brief
 *    Register value for measuring InP - VREF (singled ended)
 */
#define DM35424_ADC_FE_CONFIG_SNGL_END_POS			0x08

/**
 * @brief
 *    Register value for measuring VREF - InN (singled ended)
 */
#define DM35424_ADC_FE_CONFIG_SNGL_END_NEG			0x10

/**
 * @brief
 *    Register value for setting channel to In(Positive) - In(Negative)
 *    connection.  This is the most common.
 */
#define DM35424_ADC_FE_CONFIG_DIFFERENTIAL			0x18

/**
 * @brief
 *    Register value for setting a Gain of 1
 */
#define DM35424_ADC_FE_CONFIG_GAIN_1			0x00

/**
 * @brief
 *    Register value for setting a Gain of 2
 */
#define DM35424_ADC_FE_CONFIG_GAIN_2			0x04

/**
 * @brief
 *    Register value for setting a Gain of 4
 */
#define DM35424_ADC_FE_CONFIG_GAIN_4			0x02

/**
 * @brief
 *    Register value for setting a Gain of 8
 */
#define DM35424_ADC_FE_CONFIG_GAIN_8			0x06

/**
 * @brief
 *    Register value for setting a Gain of 16
 */
#define DM35424_ADC_FE_CONFIG_GAIN_16			0x01

/**
 * @brief
 *    Register value for setting a Gain of 32
 */
#define DM35424_ADC_FE_CONFIG_GAIN_32			0x05

/**
 * @brief
 *    Register value for setting a Gain of 64
 */
#define DM35424_ADC_FE_CONFIG_GAIN_64			0x03

/**
 * @brief
 *    Register value for setting a Gain of 128
 */
#define DM35424_ADC_FE_CONFIG_GAIN_128			0x07

/**
 * @brief
 *    Bit mask for the channel power bit of the FE Config
 */
#define DM35424_ADC_FE_CONFIG_POWER_MASK		0x80

/**
 * @brief
 *    Bit mask for the channel PGA bit of the FE Config
 */
#define DM35424_ADC_FE_CONFIG_PGA_MASK			0x40

/**
 * @brief
 *    Bit mask for the channel input switch bit of the FE Config
 */
#define DM35424_ADC_FE_CONFIG_INPUT_SW_ENABLE_MASK	0x20

/**
 * @brief
 *    Bit mask for the channel input line bits of the FE Config
 */
#define DM35424_ADC_FE_CONFIG_INPUT_LINE_MASK		0x18

/**
 * @brief
 *    Bit mask for the channel gain bits of the FE Config
 */
#define DM35424_ADC_FE_CONFIG_GAIN_MASK		0x07

/**
 * @brief
 *    Maximum allowable value to write to the threshold register
 */
#define DM35424_ADC_THRESHOLD_MAX	   8388607L

/**
 * @brief
 *    Minimum allowable value to write to the threshold register
 */
#define DM35424_ADC_THRESHOLD_MIN        -8388608L

/**
 * @brief
 *    Minimum divider allowed for HIGH SPEED mode
 */
#define DM35424_ADC_HIGH_SPD_MIN_DIV		2

/**
 * @brief
 *    Minimum divider allowed for HIGH RES mode
 */
#define DM35424_ADC_HIGH_RES_MIN_DIV		2

/**
 * @brief
 *    Minimum divider allowed for LOW POWER mode
 */
#define DM35424_ADC_LOW_POW_MIN_DIV		4

/**
 * @brief
 *    Minimum divider allowed for LOW SPEED mode
 */
#define DM35424_ADC_LOW_SPD_MIN_DIV		10

/**
 * @brief
 *     Max rate of the ADC (Hz)
 */
#define DM35424_ADC_MAX_RATE	106000

/**
 * @brief
 *    Max possible value for ADC
 */
#define DM35424_ADC_MAX_VALUE		8388607

/**
 * @brief
 *    Min possible value for ADC
 */
#define DM35424_ADC_MIN_VALUE		-8388608

/*=============================================================================
Enumerations
 =============================================================================*/


/**
  @brief
      Clock events for the global source clocks.

 */
enum DM35424_Adc_Clock_Events {


	/**
	 * @brief
	 *      Register value for Clock Event - Disabled
	 */
	DM35424_ADC_CLK_BUS_SRC_DISABLE = 0x00,

	/**
	 * @brief
	 *      Register value for Clock Event - Sample Taken
	 */
	DM35424_ADC_CLK_BUS_SRC_SAMPLE_TAKEN = 0x80,

	/**
	 * @brief
	 *      Register value for Clock Event - Channel Threshold Exceeded
	 */
	DM35424_ADC_CLK_BUS_SRC_CHAN_THRESH = 0x81,

	/**
	 * @brief
	 *      Register value for Clock Event - Pre-Start Buffer Full
	 */
	DM35424_ADC_CLK_BUS_SRC_PRE_START_BUFF_FULL = 0x82,

	/**
	 * @brief
	 *      Register value for Clock Event - Start Trigger Occurred
	 */
	DM35424_ADC_CLK_BUS_SRC_START_TRIG = 0x83,

	/**
	 * @brief
	 *      Register value for Clock Event - Stop Trigger Occurred
	 */
	DM35424_ADC_CLK_BUS_SRC_STOP_TRIG = 0x84,

	/**
	 * @brief
	 *      Register value for Clock Event - Post-Stop Buffer Full
	 */
	DM35424_ADC_CLK_BUS_SRC_POST_STOP_BUFF_FULL = 0x85,

	/**
	 * @brief
	 *      Register value for Clock Event - Sampling Complete
	 */
	DM35424_ADC_CLK_BUS_SRC_SAMPLING_COMPLETE = 0x86,

	/**
	 * @brief
	 *      Register value for Clock Event - Pacer Tick Occurred
	 */
	DM35424_ADC_CLK_BUS_SRC_PACER_TICK = 0x87

};

/**
  @brief
      Input range of the ADC input pin.  This combines polarity and gain into a
      single enumeration, and is the preferred way of setting polarity and gain.

  @note
      Not all values in this enumeration may apply to your board,as
      this is a shared library.  Please consult the board manual
      for legal values.
*/
enum DM35424_Input_Ranges {

	/**
	 * Bipolar Mode, -2.5 to 2.5 V
	 */
	DM35424_ADC_RNG_BIPOLAR_2_5V,

	/**
	 * Bipolar Mode, -1.25 to 1.25 V
	 */
	DM35424_ADC_RNG_BIPOLAR_1_25V,

	/**
	 * Bipolar Mode, -625 mV to 625 mV
	 */
	DM35424_ADC_RNG_BIPOLAR_625mV,

	/**
	 * Bipolar Mode, -312.5 mV to 312.5 mV
	 */
	DM35424_ADC_RNG_BIPOLAR_312mV,

	/**
	 * Bipolar Mode, -156.25 mV to 156.25 mV
	 */
	DM35424_ADC_RNG_BIPOLAR_156mV,

	/**
	 * Bipolar Mode, -78.125 mV to 78.125 mV
	 */
	DM35424_ADC_RNG_BIPOLAR_78mV,

	/**
	 * Bipolar Mode, -39.0626 mV to 39.0626 mV
	 */
	DM35424_ADC_RNG_BIPOLAR_39mV,

	/**
	 * Bipolar Mode, -19.53125 mV to 19.53125 mV
	 */
	DM35424_ADC_RNG_BIPOLAR_19mV,

	/**
	 * Unipolar Mode, 0 to 5 V
	 */
	DM35424_ADC_RNG_UNIPOLAR_5V,

};


/**
  @brief
      Input mode of the ADC pin.

  @note
      Not all values in this enumeration may apply to your board,as
      this is a shared library.  Please consult the board manual
      for valid values.
*/
enum DM35424_Input_Mode {
	/**
	 * Differential Operation
	 */
	DM35424_ADC_INPUT_DIFFERENTIAL,

	/**
	 * Single-Ended operation, input connected to
	 * positive ADC input.
	 */
	DM35424_ADC_INPUT_SINGLE_ENDED_POS,

	/**
	 * Single-Ended operation, input connected to
	 * negative ADC input.
	 */
	 DM35424_ADC_INPUT_SINGLE_ENDED_NEG,

	/**
	 * DAC Output internally looped-back to ADC
	 * input.  See hardware manual for more info.
	 */
	 DM35424_ADC_INPUT_DAC_LOOPBACK,

};


/**
  @brief
      Input gain to apply to the incoming signal.  Note that the preferred method
      of setting the gain is through the input range enumeration.

  @note
      Not all values in this enumeration may apply to your board,as
      this is a shared library.  Please consult the board manual
      for legal values.

*/
enum DM35424_Gains {

	/**
	 * Input Half-Gain
	 */
	DM35424_ADC_GAIN_05,

	/**
	 * Input Gain of 1
	 */
	DM35424_ADC_GAIN_1,

	/**
	 * Input Gain of 2
	 */
	DM35424_ADC_GAIN_2,

	/**
	 * Input Gain of 4
	 */
	DM35424_ADC_GAIN_4,

	/**
	 * Input Gain of 8
	 */
	DM35424_ADC_GAIN_8,

	/**
	 * Input Gain of 16
	 */
	DM35424_ADC_GAIN_16,

	/**
	 * Input Gain of 32
	 */
	DM35424_ADC_GAIN_32,

	/**
	 * Input Gain of 64
	 */
	DM35424_ADC_GAIN_64,

	/**
	 * Input Gain of 128
	 */
	DM35424_ADC_GAIN_128
};



/**
    @brief
        Sampling mode for the AD Config Register
*/
enum DM35424_Sampling_Mode {

    /**
     * @brief
     *      Register value for ADC AD Config - High Speed
     */
    DM35424_ADC_MODE_CONFIG_HIGH_SPEED          =0x01,

    /**
     * @brief
     *      Register value for ADC AD Config - High Resolution
     */
    DM35424_ADC_MODE_CONFIG_HIGH_RES	        =0x03,

    /**
     * @brief
     *      Register value for ADC AD Config - Low Power
     */
    DM35424_ADC_MODE_CONFIG_LOW_POWER	        =0x04,

    /**
     * @brief
     *      Register value for ADC AD Config - Low Speed
     */
    DM35424_ADC_MODE_CONFIG_LOW_SPEED           =0x06
};
/**
 * @} DM35424_Adc_Library_Constants
 */



/*=============================================================================
Public library functions
 =============================================================================*/


 /**
  * @defgroup DM35424_Adc_Library_Functions DM35424 ADC Public Library Functions
  * @{
 */


/**
*******************************************************************************
@brief
    Open the ADC indicated, and determine register locations of control
    blocks needed to control it.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    number_of_type

    Which ADC to open.  The first ADC on the board will be 0.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@retval
    0

    Success.

@retval
    -1

    Failure.
 */
DM35424LIB_API
int DM35424_Adc_Open(struct DM35424_Board_Descriptor *handle,
			unsigned int number_of_type,
			struct DM35424_Function_Block *func_block);



/**
*******************************************************************************
@brief
    Get the start trigger for data collection.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    start_trigger

    Pointer to the returned trigger value.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Get_Start_Trigger(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *func_block,
				uint8_t *start_trigger);


/**
*******************************************************************************
@brief
    Set the start trigger for data collection.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    start_trigger

    Trigger to start capturing values.  See the hardware manual for valid
    trigger values.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      An invalid value was passed for a start trigger
 */
DM35424LIB_API
int DM35424_Adc_Set_Start_Trigger(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *func_block,
				uint8_t start_trigger);


/**
*******************************************************************************
@brief
    Get the stop trigger for data collection.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    stop_trigger

    Pointer to the returned trigger value

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Get_Stop_Trigger(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *func_block,
				uint8_t *stop_trigger);


/**
*******************************************************************************
@brief
    Set the stop trigger for data collection.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    stop_trigger

    Trigger to stop capturing values.  See the hardware manual for valid
    trigger values.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      An invalid value was passed for a stop trigger
 */
DM35424LIB_API
int DM35424_Adc_Set_Stop_Trigger(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *func_block,
				uint8_t stop_trigger);


/**
*******************************************************************************
@brief
    Get the amount of data to capture prior to start trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    count

    Pointer to the returned capture count

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Get_Pre_Trigger_Samples(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t *count);


/**
*******************************************************************************
@brief
    Set the amount of data to capture prior to start trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    count

    Number of samples to capture prior to the start trigger.

@note
    The amount of data that can be captured prior to the start trigger is
    limited by the size of the FIFO.  Consult the user's manual for this
    information.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      The size is not within the valid value range.
 */
DM35424LIB_API
int DM35424_Adc_Set_Pre_Trigger_Samples(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t count);


/**
*******************************************************************************
@brief
    Get the amount of data to capture after stop trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    count

    Pointer to the returned count.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Get_Post_Stop_Samples(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t *count);


/**
*******************************************************************************
@brief
    Set the amount of data to capture after stop trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    count

    Number of samples to capture after the stop trigger.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Set_Post_Stop_Samples(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t count);


/**
*******************************************************************************
@brief
    Get the clock source for the ADC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    source

    Pointer to returned clock source.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Get_Clock_Src(struct DM35424_Board_Descriptor *handle,
                           const struct DM35424_Function_Block *func_block,
                            enum DM35424_Clock_Sources *source);


/**
*******************************************************************************
@brief
    Set the clock source for the ADC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    source

    Clock source to use for the ADC.  Consult the user's manual for the list
    of available sources.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      The clock source selected is not valid.
 */
DM35424LIB_API
int DM35424_Adc_Set_Clock_Src(struct DM35424_Board_Descriptor *handle,
                           const struct DM35424_Function_Block *func_block,
                            enum DM35424_Clock_Sources source);



/**
*******************************************************************************
@brief
    Prepare the ADC for actual data collection.  Moves the ADC from uninitialized
    to stopped.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@note
    In many cases, several other steps have to occur before initialization is
    attempted, or the device will not initialize correctly or at all.  Please
    review the user's manual for the correct steps to take.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EPERM      Attempted to initialize an ADC with no active channels.
            EBUSY	Device did not complete initialization in the time
            		expected (timeout).
 */
DM35424LIB_API
int DM35424_Adc_Initialize(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block);



/**
*****************************************************************************
@brief
    Set the Clock Divider for the ADC function block.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    divider

    The requested clock divider.

@retval
    0

    Success.

@retval
    -1

    Failure.
 */
DM35424LIB_API
int DM35424_Adc_Set_Clk_Divider(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t divider);


/**
*******************************************************************************
@brief
    Set the sampling rate for the ADC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    rate

    The requested sampling rate for the ADC (Hz).

@param
    actual_rate

    Pointer to the actual rate achieved by the ADC (Hz).  Due to divider and clock
    values, the actual rate will rarely ever be the exact same as the
    requested rate.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Asked for an invalid sampling rate (negative or 0)
            ERANGE	Requested sampling rate is outside of the possible
            		range for this ADC.
 */
DM35424LIB_API
int DM35424_Adc_Set_Sample_Rate(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t rate,
				uint32_t *actual_rate);



/**
*******************************************************************************
@brief
    Get the front-end config register contents

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel the FE Config is requested for.

@param
    fe_config

    Pointer to the returned FE Config register value

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Adc_Channel_Get_Front_End_Config(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					uint16_t *fe_config);

/**
*******************************************************************************
@brief
    Set the Configuration of the AD Mode

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    mode

    Mode to set for the AD Config

@note
    This library function will only apply to some ADC subtypes, and may not be
    applicable to the DM35424.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid mode requested, or does not apply to this ADC.
            ENODEV	Attempted to set for an invalid ADC
 */
DM35424LIB_API
int DM35424_Adc_Ad_Config_Set_Mode(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				enum DM35424_Sampling_Mode mode);

/**
*****************************************************************************
@brief
    Get AD Config register

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    mode

    Returned AD_Config register value

@note
    This library function will only apply to some ADC subtypes, and may not be
    applicable to the DM35424.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid mode requested, or does not apply to this ADC.
            ENODEV	Attempted to set for an invalid ADC
 */
DM35424LIB_API
int DM35424_Adc_Ad_Config_Get_Mode(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint16_t *mode);


/**
*******************************************************************************
@brief
    Configure the interrupts for the ADC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    int_source

    The interrupts to configure.  The bits indicate specific interrupts.  Consult
    the user's manual for a description.

@param
    enable

    Boolean indicating to enable or disable the selected interrupts.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Interrupt_Set_Config(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		uint16_t int_source,
                            		int enable);


/**
*******************************************************************************
@brief
    Get the interrupt configuration for the ADC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    interrupt_ena

    Pointer to the interrupt configuration register.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Interrupt_Get_Config(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		uint16_t *interrupt_ena);


/**
*******************************************************************************
@brief
    Set the ADC mode to Start

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Start(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Set the ADC mode to Start-Rearm

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Start_Rearm(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Set the ADC mode to Reset

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Reset(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Set the ADC mode to Pause

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Pause(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Set the ADC mode to Uninitialized

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Uninitialize(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Get the ADC mode-status value.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    mode_status

    Pointer to the mode_status value to return.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Get_Mode_Status(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint8_t *mode_status);


/**
*******************************************************************************
@brief
    Get the last sample taken from the ADC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to get sample from.

@param
    value

    Pointer to returned sample value.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Channel_Get_Last_Sample(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		unsigned int channel,
                            		int32_t *value);


/**
*******************************************************************************
@brief
    Get the count of number of samples taken.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    value

    Pointer to returned sample count.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Get_Sample_Count(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		uint32_t *value);


/**
*******************************************************************************
@brief
    Get the interrupt status register

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    value

    Pointer to returned interrupt status.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Interrupt_Get_Status(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						uint16_t *value);


/**
*******************************************************************************
@brief
    Clear the interrupt status register

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    value

    Bit mask of which interrupts to clear.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Interrupt_Clear_Status(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						uint16_t value);


/**
*******************************************************************************
@brief
    Setup the channel input for the ADC

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to configure.

@param
    input_range

    An enumerated value representing the input voltage range of the input.

@param
    input_mode

    An enumerated value representing the mode to set the input line to.

@note
    The input line mode and input voltage ranges available for the board is
    dependent on the ADC subtype on the board.  Review the user's guide to
    see what values the ADC can be set to.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested, or requested mode/range is
                      	not possible on this ADC subtype.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Setup(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *func_block,
					unsigned int channel,
					enum DM35424_Input_Ranges input_range,
					enum DM35424_Input_Mode input_mode);

/**
*****************************************************************************
@brief
    Reset the channel front-end config

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to reset.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Reset(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						unsigned int channel);

/**
*******************************************************************************
@brief
    Setup the channel interrupts

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to configure.

@param
    interrupts_to_set

    A bit mask indicating which interrupts to set

@param
    enable

    A boolean value indicating if selected interrupts should be enabled
    or disabled.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested, or requested input mode is
                      	not possible on this ADC subtype.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Interrupt_Set_Config(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						unsigned int channel,
						uint8_t interrupts_to_set,
						int enable);


/**
*******************************************************************************
@brief
    Get the channel interrupt configuration

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to get configururation.

@param
    chan_intr_enable

    Pointer to interrupt configuration being returned.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.
 */
DM35424LIB_API
int DM35424_Adc_Channel_Interrupt_Get_Config(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						unsigned int channel,
						uint8_t *chan_intr_enable);


/**
*******************************************************************************
@brief
    Get the channel interrupt status

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to get configururation.

@param
    chan_intr_status

    Pointer to interrupt status being returned.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Interrupt_Get_Status(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						unsigned int channel,
						uint8_t *chan_intr_status);


/**
*******************************************************************************
@brief
    Clear the interrupt status for this channel

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to clear.

@param
    chan_intr_status

    Bit mask indicating which interrupts to clear.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Interrupt_Clear_Status(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						unsigned int channel,
						uint8_t chan_intr_status);


/**
*******************************************************************************
@brief
    Find the first channel with an interrupt.  Note that this is only useful
    when looking for a threshold interrupt.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel_with_interrupt

    Pointer to the returned channel that has an interrupt (if any)

@param
    channel_has_interrupt

    Pointer to boolean indicating whether returned channel has interrupt or not.

@param
    channel_intr_status

    Pointer to the channel's interrupt status.

@param
    channel_intr_enable

    Pointer to the channel's interrupt enable register.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Channel_Find_Interrupt(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int *channel_with_interrupt,
					int *channel_has_interrupt,
					uint8_t *channel_intr_status,
					uint8_t *channel_intr_enable);



/**
*******************************************************************************
@brief
    Set the filter value for the channel

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to set filter

@param
    chan_filter

    Channel filter value.  Reference the user's manual for valid filter values.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Set_Filter(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				uint8_t chan_filter);



/**
*******************************************************************************
@brief
    Get the filter value for the channel

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to get filter of

@param
    chan_filter

    Pointer to returned channel filter value.  Reference the user's manual for
    valid filter values.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Get_Filter(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				unsigned int channel,
				uint8_t *chan_filter);


/**
*******************************************************************************
@brief
    Set the lower threshold for this channel.

@param
    handle

    Address of the handle pointer, which will contain the device descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to set the lower threshold of.

@param
    threshold

    Signed lower threshold value for this channel.

@note
    The threshold register is only 16-bits.  Thus, the threshold value really
    only represents the top 16-bits of the 24-bit ADC value.  For convenience,
    the threshold parameter is accepted as a 32-bit integer.  Before writing
    the value, it will be right-shifted 16 bits.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Set_Low_Threshold(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					int32_t threshold);


/**
*******************************************************************************
@brief
    Set the high threshold for this channel.

@param
    handle

    Address of the handle pointer, which will contain the device descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to set the high threshold of.

@param
    threshold

    Signed high threshold value for this channel.

@note
    The threshold register is only 16-bits.  Thus, the threshold value really
    only represents the top 16-bits of the 24-bit ADC value.  For convenience,
    the threshold parameter is accepted as a 32-bit integer.  Before writing
    the value, it will be right-shifted 16 bits.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Set_High_Threshold(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					int32_t threshold);


/**
*******************************************************************************
@brief
    Get both thresholds for this channel.

@param
    handle

    Address of the handle pointer, which will contain the device descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to set the high threshold of.

@param
    low_threshold

    Pointer to signed integer value of threshold.

@param
    high_threshold

    Pointer to signed integer value of threshold.

@note
    The threshold register is only 16-bits.  Thus, the threshold value really
    only represents the top 16-bits of the 24-bit ADC value.  For convenience,
    the threshold parameters are returned as 32-bit integers.  After getting
    the value from the register, it will be left-shifted 16-bits.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Get_Thresholds(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					int32_t *low_threshold,
					int32_t *high_threshold);


/**
*******************************************************************************
@brief
    Read an ADC sample stored in the onboard FIFO.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    channel

    Channel to get sample from.

@param
    value

    Pointer to returned sample value.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Adc_Fifo_Channel_Read(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		unsigned int channel,
                            		int32_t *value);

/**
*******************************************************************************
@brief
    Set the global clock source for the ADC.

@param
    handle

    Address of the handle pointer, which will contain the device descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    clock_select

    Which global clock source to set

@param
    clock_driver

    Source to set global clock to (what is driving it?)

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid clock select or source..

 */
DM35424LIB_API
int DM35424_Adc_Set_Clock_Source_Global(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					enum DM35424_Clock_Sources clock_select,
					enum DM35424_Adc_Clock_Events clock_driver);


/**
*******************************************************************************
@brief
    Get the global clock source for the selected clock

@param
    handle

    Address of the handle pointer, which will contain the device descriptor.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

@param
    clock_select

    Which global clock source to get

@param
    clock_source

    Pointer to the returned clock source for the selected global clock

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid clock select or source..

 */
DM35424LIB_API
int DM35424_Adc_Get_Clock_Source_Global(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					int clock_select,
					int *clock_source);


/**
*******************************************************************************
@brief
    Convert an ADC sample to a volts value.

@param
    input_range

    Enumerated value indicating what range the ADC channel has been set to,
    or NULL if the ADC does not have selectable input ranges.

@param
    adc_sample

    Signed value from the ADC that we want to convert to volts.

@param
    volts

    Pointer to the returned float value in volts.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            ENODEV	The function block passed in is the wrong subtype
 */
DM35424LIB_API
int DM35424_Adc_Sample_To_Volts(
				enum DM35424_Input_Ranges input_range,
				int32_t adc_sample,
				float *volts);


/**
*******************************************************************************
@brief
    Convert volts to an ADC value.

@param
    input_range

    Enumerated value indicating what range the ADC channel has been set to

@param
    volts

    Value to be converted to counts.

@param
    adc_sample

    Pointer to the returned ADC count value.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            ENODEV	The function block passed in is the wrong subtype
 */
DM35424LIB_API
int DM35424_Adc_Volts_To_Sample(
					enum DM35424_Input_Ranges input_range,
					float volts,
					int32_t *adc_sample);




/**
 * @} DM35424_Adc_Library_Functions
 */

#ifdef __cplusplus
}
#endif

#endif
