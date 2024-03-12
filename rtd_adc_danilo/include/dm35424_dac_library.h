/**
    @file

    @brief
        Definitions for the DM35424 DAC Library.

    $Id: dm35424_dac_library.h 106898 2017-03-08 13:44:23Z rgroner $
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


#ifndef _DM35424_DAC_LIBRARY__H_
#define _DM35424_DAC_LIBRARY__H_


#ifdef __cplusplus
extern "C" {
#endif


 /**
  * @defgroup DM35424_Dac_Library_Constants DM35424 DAC Library Constants
  * Functions
  * @{
 */

/*=============================================================================
Public library constants
 =============================================================================*/

/**
 * @brief
 *      Register value for Interrupt Mask - Conversion Sent
 */
 #define DM35424_DAC_INT_CONVERSION_SENT_MASK	0x01

/**
 * @brief
 *      Register value for Interrupt Mask - Channel has enabled marker
 */
 #define DM35424_DAC_INT_CHAN_MARKER_MASK		0x02

/**
 * @brief
 *      Register value for Interrupt Mask - Start Trigger Occurred
 */
 #define DM35424_DAC_INT_START_TRIG_MASK		0x08

/**
 * @brief
 *      Register value for Interrupt Mask - Stop Trigger Occurred
 */
 #define DM35424_DAC_INT_STOP_TRIG_MASK		0x10

/**
 * @brief
 *      Register value for Interrupt Mask - Post-Stop Conversions Completed
 */
 #define DM35424_DAC_INT_POST_STOP_DONE_MASK	0x20

/**
 * @brief
 *      Register value for Interrupt Mask - Pacer Clock Tick
 */
 #define DM35424_DAC_INT_PACER_TICK_MASK		0x80

/**
 * @brief
 *      Register value for Interrupt Mask - All Bits
 */
 #define DM35424_DAC_INT_ALL_MASK			0xBB

/**
 * @brief
 *      Register value for Mode - Reset
 */
 #define DM35424_DAC_MODE_RESET			0x00

/**
 * @brief
 *      Register value for Mode - Pause
 */
 #define DM35424_DAC_MODE_PAUSE			0x01

/**
 * @brief
 *      Register value for Mode - Go (Single Shot)
 */
 #define DM35424_DAC_MODE_GO_SINGLE_SHOT		0x02

/**
 * @brief
 *      Register value for Mode - Go (Re-arm)
 */
 #define DM35424_DAC_MODE_GO_REARM			0x03

/**
 * @brief
 *      Register value for DAC Status - Stopped
 */
 #define DM35424_DAC_STATUS_STOPPED			0x00

/**
 * @brief
 *      Register value for DAC Status - Waiting for Start Trigger
 */
 #define DM35424_DAC_STATUS_WAITING_START_TRIG	0x02

/**
 * @brief
 *      Register value for DAC Status - Converting Data
 */
 #define DM35424_DAC_STATUS_CONVERTING		0x03

/**
 * @brief
 *      Register value for DAC Status - Outputting Post-Stop
 *      conversions.
 */
 #define DM35424_DAC_STATUS_OUTPUT_POST		0x04

/**
 * @brief
 *      Register value for DAC Status - Waiting for Re-Arm
 */
 #define DM35424_DAC_STATUS_WAITING_REARM		0x05

/**
 * @brief
 *      Register value for DAC Status - Done
 */
 #define DM35424_DAC_STATUS_DONE			0x07

/**
 * @brief
 *     Max allowable rate for the DAC (Hz)
 */
#define DM35424_DAC_MAX_RATE	106000


/**
 * @brief
 *     Max value of the DAC
 */
#define DM35424_DAC_MAX_VALUE	32767

/**
 * @brief
 *     Min value of the DAC
 */
#define DM35424_DAC_MIN_VALUE	-32768

/**
 * @brief
 *     DAC LSB (at lowest voltage range)
 */
 #define DM35424_DAC_LSB_AT_MIN_RANGE	0.000152587890625f


/*=============================================================================
Enumerations
 =============================================================================*/

/**
 * @brief
 *      Clocking events that can be used as the global clock sources
 */
 enum DM35424_Dac_Clock_Events {

	/**
	 * @brief
	 *      Register value for Clock Event - Disabled
	 */
	DM35424_DAC_CLK_BUS_SRC_DISABLE = 0x00,

	/**
	 * @brief
	 *      Register value for Clock Event - Conversion Sent
	 */
	DM35424_DAC_CLK_BUS_SRC_CONVERSION_SENT = 0x80,

	/**
	 * @brief
	 *      Register value for Clock Event - Channel has enabled marker
	 */
	DM35424_DAC_CLK_BUS_SRC_CHAN_MARKER = 0x81,

	/**
	 * @brief
	 *      Register value for Clock Event - Start Trigger Occurred
	 */
	DM35424_DAC_CLK_BUS_SRC_START_TRIG = 0x83,

	/**
	 * @brief
	 *      Register value for Clock Event - Stop Trigger Occurred
	 */
	DM35424_DAC_CLK_BUS_SRC_STOP_TRIG = 0x84,

	/**
	 * @brief
	 *      Register value for Clock Event - Conversions Complete
	 */
	DM35424_DAC_CLK_BUS_SRC_CONV_COMPL = 0x85,

};




/**
 * @} DM35424_Dac_Library_Constants
 */


 /**
  * @defgroup DM35424_Dac_Library_Functions DM35424 DAC Library Public Functions
  * @{
 */


/*=============================================================================
Public library functions
 =============================================================================*/

/**
*******************************************************************************
@brief
    Open the DAC indicated, and determine register locations of control
    blocks needed to control it.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    number_of_type

    Which DAC to open.  The first DAC on the board will be 0.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Open(struct DM35424_Board_Descriptor *handle,
		unsigned int number_of_type,
		struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Set the clock source of the DAC

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    source

    The clock source that we want to set for this DAC.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Set_Clock_Src(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		enum DM35424_Clock_Sources source);


/**
*******************************************************************************
@brief
    Get the clock source of the DAC

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    source

    Pointer to the returned clock set for this DAC.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Get_Clock_Src(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		enum DM35424_Clock_Sources *source);


/**
*******************************************************************************
@brief
    Get the clock divider value.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    divider

    Pointer to the clock divider returned.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Get_Clock_Div(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t *divider);


/**
*******************************************************************************
@brief
    Set the clock divider value.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    divider

    Divider value to set this DAC clock to.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Set_Clock_Div(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t divider);


/**
*******************************************************************************
@brief
    Set the conversion rate of this DAC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    requested_rate

    Requested rate of conversion for the DAC (Hz).

@param
    actual_rate

    Pointer to the returned value of the actual rate achieved (Hz).

@note
    The actual obtainable rate depends on many board-specific values and clocks,
    and so the returned rate will rarely be the exact same as the requested rate.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.

    errno may be set as follows:
        @arg \c
            EINVAL	Invalid rate requested.
 */
DM35424LIB_API
int DM35424_Dac_Set_Conversion_Rate(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t requested_rate,
					uint32_t *actual_rate);


/**
*******************************************************************************
@brief
    Set the interrupt configuration for this DAC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    interrupt_src

    Bitmask indicating which interrupts to set.

@param
    enable

    A boolean value indicating whether selected interrupts are to be enabled
    or disabled.


@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Interrupt_Set_Config(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		uint16_t interrupt_src,
                            		int enable);


/**
*******************************************************************************
@brief
    Get the interrupt configuration for this DAC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    interrupt_ena

    Pointer to the returned bitmask indicating which interrupts are set.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Interrupt_Get_Config(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		uint16_t *interrupt_ena);


/**
*******************************************************************************
@brief
    Set the start trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    trigger_value

    Trigger value (event) that will initiate conversions on this DAC.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Set_Start_Trigger(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint8_t trigger_value);


/**
*******************************************************************************
@brief
    Set the stop trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    trigger_value

    Trigger value (event) that will halt conversions on this DAC.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Set_Stop_Trigger(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint8_t trigger_value);


/**
*******************************************************************************
@brief
    Get the start trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    trigger_value

    Pointer to the returned rigger value (event) that will initiate
    conversions on this DAC.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Get_Start_Trigger(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint8_t *trigger_value);


/**
*******************************************************************************
@brief
    Get the stop trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    trigger_value

    Pointer to the returned trigger value (event) that will halt
    conversions on this DAC.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Get_Stop_Trigger(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint8_t *trigger_value);


/**
*******************************************************************************
@brief
    Set the DAC Mode to Start.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Start(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Set the DAC Mode to Reset.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Reset(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Set the DAC Mode to Pause.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Pause(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block);


/**
*******************************************************************************
@brief
    Get the Mode and Status of the DAC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    mode_status

    Pointer to the value of the returned mode_status register.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Get_Mode_Status(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint8_t *mode_status);


/**
*******************************************************************************
@brief
    Get the value of the last conversion of the DAC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DAC channel that we want the last conversion value from.

@param
    marker

    Pointer to the returned value for the marker byte.

@param
    value

    Pointer to the returned signed value of the last conversion.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Get_Last_Conversion(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		unsigned int channel,
                            		uint8_t *marker,
                            		int16_t *value);


/**
*******************************************************************************
@brief
    Set a value to be converted by the DAC immediately.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    channel

    DAC channel that we want the last conversion set to.

@param
    marker

    Value of the marker bits (top 8 bits)

@param
    value

    Value to be converted by DAC and set on its output pin.

@note
    The DAC will set its output value to the last conversion register
    value only if the DAC is in Reset mode.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Set_Last_Conversion(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					uint8_t marker,
					int16_t value);


/**
*******************************************************************************
@brief
    Get a count of the number of conversions that DAC has executed.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    value

    Pointer to the returned count of conversions executed.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Get_Conversion_Count(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		uint32_t *value);


/**
*******************************************************************************
@brief
    Get a interrupt status register of the DAC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    value

    Pointer to the returned interrupt status.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Interrupt_Get_Status(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						uint16_t *value);


/**
*******************************************************************************
@brief
    Clear the interrupt status register of the DAC.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    value

    Bitmask indicating which interrupts to clear.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Interrupt_Clear_Status(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						uint16_t value);


/**
*******************************************************************************
@brief
    Set the number of conversions the DAC will make after a stop trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    value

    Number of conversions.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Set_Post_Stop_Conversion_Count(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						uint32_t value);


/**
*******************************************************************************
@brief
    Get the number of conversions the DAC will make after a stop trigger.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    value

    Pointer to the returned number of conversions.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
 */
DM35424LIB_API
int DM35424_Dac_Get_Post_Stop_Conversion_Count(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						uint32_t *value);


/**
*******************************************************************************
@brief
    Set the source that will drive the global clock.

@param
    handle

    Address of the handle pointer, which will contain the device
    descriptor.

@param
    func_block

    Pointer to the function block descriptor.  The descriptor holds the
    information about the function block, including offsets.

@param
    clock

    Which global clock to set.

@param
    clock_driver

    Source to drive global clock.

@retval
    0

    Success.

@retval
    Non-Zero

    Failure.
    errno may be set as follows:
        @arg \c
            EINVAL	Invalid clock or source requested.

 */
DM35424LIB_API
int DM35424_Dac_Set_Clock_Source_Global(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					enum DM35424_Clock_Sources clock,
					enum DM35424_Dac_Clock_Events clock_driver);




/**
*******************************************************************************
@brief
    Set the configuration of the marker interrupts for this channel

@param
    handle

    Pointer to the board handle

@param
    func_block

    Pointer to the function block.

@param
    channel

    The channel to change.

@param
    marker_enable

    Bit values indicating whether to enable marker interrupts (1) or disable (0).

@retval
    0

    Success.

@retval
    Non-zero

 */
DM35424LIB_API
int DM35424_Dac_Channel_Set_Marker_Config(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *func_block,
					unsigned int channel,
					uint8_t marker_enable);


/**
*******************************************************************************
@brief
    Get the configuration of the marker interrupts for this channel

@param
    handle

    Pointer to the board handle

@param
    func_block

    Pointer to the function block.

@param
    channel

    The channel to change.

@param
    marker_enable

    Pointer to returned marker interrupt config.

@retval
    0

    Success.

@retval
    Non-zero

 */
DM35424LIB_API
int DM35424_Dac_Channel_Get_Marker_Config(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *func_block,
					unsigned int channel,
					uint8_t *marker_enable);



/**
*******************************************************************************
@brief
    Get the status of the marker interrupts for this channel

@param
    handle

    Pointer to the board handle

@param
    func_block

    Pointer to the function block.

@param
    channel

    The channel to change.

@param
    marker_status

    Pointer to returned marker status.

@retval
    0

    Success.

@retval
    Non-zero

 */
DM35424LIB_API
int DM35424_Dac_Channel_Get_Marker_Status(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *func_block,
					unsigned int channel,
					uint8_t *marker_status);


/**
*******************************************************************************
@brief
    Clear the marker interrupts for this channel

@param
    handle

    Pointer to the board handle

@param
    func_block

    Pointer to the function block.

@param
    channel

    The channel to change.

@param
    marker_to_clear

    Bit values indicating which bits in register to clear.

@retval
    0

    Success.

@retval
    Non-zero

 */
DM35424LIB_API
int DM35424_Dac_Channel_Clear_Marker_Status(struct DM35424_Board_Descriptor *handle,
					struct DM35424_Function_Block *func_block,
					unsigned int channel,
					uint8_t marker_to_clear);


/**
*******************************************************************************
@brief
    Write a value to the onboard FIFO.

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

    Channel to write the data to.

@param
    value

    value to write.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n

 */
DM35424LIB_API
int DM35424_Dac_Fifo_Channel_Write(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		unsigned int channel,
                            		int32_t value);

/**
*******************************************************************************
@brief
    Convert a value in volts to a DAC equivalent signed value.


@param
    volts

    The volts value we want the DAC to output.

@param
    dac_conversion

    Pointer to signed value representing the equivalent of the volts.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Function called by an unsupported function block.

 */
DM35424LIB_API
int DM35424_Dac_Volts_To_Conv(
					float volts,
					int16_t *dac_conversion);


/**
*******************************************************************************
@brief
    Convert a DAC conversion value to volts.

@param
    conversion

    DAC converter signed value.

@param
    volts

    The volts equivalent of the converter value.

@retval
    0

    Success.

@retval
    Non-zero

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL	Function called by an unsupported function block.

 */
DM35424LIB_API
int DM35424_Dac_Conv_To_Volts(
					int16_t conversion,
					float *volts);



/**
 * @} DM35424_Dac_Library_Functions
 */

#ifdef __cplusplus
}
#endif

#endif
