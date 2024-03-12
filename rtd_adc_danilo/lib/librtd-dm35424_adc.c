/**
	@file

	@brief
		DM35424 ADC library source code

	$Id: librtd-dm35424_adc.c 103507 2016-10-12 20:58:56Z rgroner $
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


#include "dm35424_registers.h"
#include "dm35424_gbc_library.h"
#include "dm35424.h"
#include "dm35424_util_library.h"
#include "dm35424_adc_library.h"
#include "dm35424_types.h"
#include "dm35424_board_access_structs.h"
#include "dm35424_board_access.h"


/**
 * @defgroup DM35424_ADC_Library_Constants DM35424 ADC library constants
 * @{
 */

/******************************************************************************
 * ADC Constants
 *****************************************************************************/
#define DM35424_INIT_WAIT_TIME_USEC		10000000
#define DM35424_INIT_SLEEP_USEC		10000
#define DM35424_FECONFIG_SLEEP_USEC		1000

/**
 * @} DM35424_ADC_Library_Constants
 */


/******************************************************************************
 * ADC Library Functions
 *****************************************************************************/
/**
 * @defgroup DM35424_ADC_Library_Source DM35424 ADC library source code
 * @{
 */

/*=============================================================================
Private functions
 =============================================================================*/

/**
 * @defgroup DM35424_ADC_Library_Private_Functions DM35424 ADC library
 *		private functions source code
 * @{
 */

 /**
  * @internal
  */

/**
*******************************************************************************
@brief
    Validate a clock source.

@param
    clk_src

    The clock source to validate.

@retval
    0

    Success.

@retval
    -1

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      clock source is not valid.
 *******************************************************************************
 */

static int
DM35424_Adc_Validate_Clock(uint8_t clk_src)
{

	switch (clk_src) {
		/* breaks intentionally omitted */
	case DM35424_CLK_SRC_IMMEDIATE:
	case DM35424_CLK_SRC_NEVER:
	case DM35424_CLK_SRC_BUS2:
	case DM35424_CLK_SRC_BUS3:
	case DM35424_CLK_SRC_BUS4:
	case DM35424_CLK_SRC_BUS5:
	case DM35424_CLK_SRC_BUS6:
	case DM35424_CLK_SRC_BUS7:
	case DM35424_CLK_SRC_CHAN_THRESH:
	case DM35424_CLK_SRC_CHAN_THRESH_INV:
	case DM35424_CLK_SRC_BUS2_INV:
	case DM35424_CLK_SRC_BUS3_INV:
	case DM35424_CLK_SRC_BUS4_INV:
	case DM35424_CLK_SRC_BUS5_INV:
	case DM35424_CLK_SRC_BUS6_INV:
	case DM35424_CLK_SRC_BUS7_INV:
		return 0;
	default:
		errno = EINVAL;
		return -1;
	}

}


/**
*******************************************************************************
@brief
    Validate a clock global select and source.

@param
    select

    The clock global select to validate.

@param
    source

    The clock global source to validate.

@retval
    0

    Success.

@retval
    -1

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      input is not valid.
 *******************************************************************************
 */

static int
DM35424_Validate_Clock_Global_Source(enum DM35424_Clock_Sources select,
					enum DM35424_Adc_Clock_Events driver)
{


	switch (driver) {
		/* breaks intentionally omitted */
	case DM35424_ADC_CLK_BUS_SRC_DISABLE:
	case DM35424_ADC_CLK_BUS_SRC_SAMPLE_TAKEN:
	case DM35424_ADC_CLK_BUS_SRC_CHAN_THRESH:
	case DM35424_ADC_CLK_BUS_SRC_PRE_START_BUFF_FULL:
	case DM35424_ADC_CLK_BUS_SRC_START_TRIG:
	case DM35424_ADC_CLK_BUS_SRC_STOP_TRIG:
	case DM35424_ADC_CLK_BUS_SRC_POST_STOP_BUFF_FULL:
	case DM35424_ADC_CLK_BUS_SRC_SAMPLING_COMPLETE:
	case DM35424_ADC_CLK_BUS_SRC_PACER_TICK:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	switch (select) {
		/* breaks intentionally omitted */
	case DM35424_CLK_SRC_BUS2:
	case DM35424_CLK_SRC_BUS3:
	case DM35424_CLK_SRC_BUS4:
	case DM35424_CLK_SRC_BUS5:
	case DM35424_CLK_SRC_BUS6:
	case DM35424_CLK_SRC_BUS7:
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return 0;

}



/**
*******************************************************************************
@brief
    Validate an input range.

@param
    rng

    The input range to validate.

@retval
    0

    Success.

@retval
    -1

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      range is not valid.
 *******************************************************************************
 */
static int
DM35424_Adc_Validate_Input_Range(enum DM35424_Input_Ranges rng)
{

	switch (rng) {
	/* Breaks intentionally omitted */
	case DM35424_ADC_RNG_UNIPOLAR_5V:
	case DM35424_ADC_RNG_BIPOLAR_2_5V:
	case DM35424_ADC_RNG_BIPOLAR_1_25V:
	case DM35424_ADC_RNG_BIPOLAR_625mV:
	case DM35424_ADC_RNG_BIPOLAR_312mV:
	case DM35424_ADC_RNG_BIPOLAR_156mV:
	case DM35424_ADC_RNG_BIPOLAR_78mV:
	case DM35424_ADC_RNG_BIPOLAR_39mV:
	case DM35424_ADC_RNG_BIPOLAR_19mV:
		break;

	default:
		errno = EINVAL;
		return -1;
		break;
	}

	return 0;
}



/**
*******************************************************************************
@brief
    Validate the input_mode requested

@param
    input_mode

    The input mode requested.  This value will be valid based on the subtype
    of the ADC.

@retval
    0

    Success.

@retval
    -1

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      input mode is not valid.
 *******************************************************************************
 */
static int
DM35424_Adc_Validate_Input_Mode(enum DM35424_Input_Mode input_mode)
{
	switch(input_mode)
	{
	case DM35424_ADC_INPUT_DIFFERENTIAL:
	case DM35424_ADC_INPUT_SINGLE_ENDED_POS:
	case DM35424_ADC_INPUT_DAC_LOOPBACK:
	case DM35424_ADC_INPUT_SINGLE_ENDED_NEG:
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	return 0;

}


/**
*******************************************************************************
@brief
    Validate the channel setup requested

@param
    func_block

    The descriptor of the function block

@param
    rng

    The input voltage range requested.  Validation depends on subtype.

@param
    mode

    The input mode (input line configuration).  Validation depends on subtype.

@retval
    0

    Success.

@retval
    -1

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      inputs are not valid.
 *******************************************************************************
 */
static int
DM35424_Adc_Validate_Channel_Setup(const struct DM35424_Function_Block *func_block,
					unsigned int channel,
					enum DM35424_Input_Ranges rng,
					enum DM35424_Input_Mode mode) {

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	if(DM35424_Adc_Validate_Input_Mode(mode))
	{
		errno = EINVAL;
		return -1;
	}

	if (DM35424_Adc_Validate_Input_Range(rng)) {
		errno = EINVAL;
		return -1;
	}


	return 0;
}


/**
*******************************************************************************
@brief
    Validate an AD Configuration Mode.

@param
    func_block

    The descriptor of the function block

@param
    mode

    The mode to validate.

@retval
    0

    Success.

@retval
    -1

    Failure.@n@n
    errno may be set as follows:
        @arg \c
            EINVAL      mode is not valid.
 *******************************************************************************
 */
static int
DM35424_Adc_Validate_Ad_Config_Mode(enum DM35424_Sampling_Mode mode)
{


	switch (mode) {
	/* break intentionally omitted */
	case DM35424_ADC_MODE_CONFIG_HIGH_SPEED:
	case DM35424_ADC_MODE_CONFIG_HIGH_RES:
	case DM35424_ADC_MODE_CONFIG_LOW_POWER:
	case DM35424_ADC_MODE_CONFIG_LOW_SPEED:
		break;

	default:
		errno = EINVAL;
		return -1;
		break;
	}


	return 0;

}


/**
 * @} DM35424_ADC_Library_Private_Functions
 */

/*=============================================================================
Public functions
 =============================================================================*/

/**
 * @defgroup DM35424_ADC_Library_Public_Functions DM35424 ADC source
 *		code for public library functions
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

    Number of ADC to open.  The first ADC on the board will be 0.

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
		struct DM35424_Function_Block *func_block)
{

	return DM35424_Function_Block_Open_Module(handle,
						DM35424_FUNC_BLOCK_ADC,
						number_of_type,
						func_block);

}



DM35424LIB_API
int DM35424_Adc_Get_Start_Trigger(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *func_block,
				uint8_t *trigger)
{

	int result = 0;
	union dm35424_ioctl_argument ioctl_request;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_START_TRIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	result = DM35424_Read(handle, &ioctl_request);

	*trigger = ioctl_request.readwrite.access.data.data8;

	return result;
}



DM35424LIB_API
int DM35424_Adc_Set_Start_Trigger(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *func_block,
				uint8_t trigger)
{


	union dm35424_ioctl_argument ioctl_request;

	if (DM35424_Adc_Validate_Clock(trigger)) {
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_START_TRIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = trigger;

	return DM35424_Write(handle, &ioctl_request);

}



DM35424LIB_API
int DM35424_Adc_Get_Stop_Trigger(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *func_block,
				uint8_t *trigger)
{

	int result = 0;
	union dm35424_ioctl_argument ioctl_request;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_STOP_TRIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	result = DM35424_Read(handle, &ioctl_request);

	*trigger = ioctl_request.readwrite.access.data.data8;

	return result;
}


DM35424LIB_API
int DM35424_Adc_Set_Stop_Trigger(struct DM35424_Board_Descriptor *handle,
				struct DM35424_Function_Block *func_block,
				uint8_t trigger)
{


	union dm35424_ioctl_argument ioctl_request;

	if (DM35424_Adc_Validate_Clock(trigger)) {
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_STOP_TRIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = trigger;

	return DM35424_Write(handle, &ioctl_request);

}



DM35424LIB_API
int DM35424_Adc_Get_Pre_Trigger_Samples(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t *pre_capture_count)
{

	union dm35424_ioctl_argument ioctl_request;
	int result = 0;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_PRE_CAPT_COUNT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	result = DM35424_Read(handle, &ioctl_request);

	*pre_capture_count = ioctl_request.readwrite.access.data.data32;

	return result;
}


DM35424LIB_API
int DM35424_Adc_Set_Pre_Trigger_Samples(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t pre_capture_count)
{

	union dm35424_ioctl_argument ioctl_request;

	if (pre_capture_count > DM35424_FIFO_SAMPLE_SIZE) {
		errno = EINVAL;
		return -1;
	}
	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_PRE_CAPT_COUNT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = pre_capture_count;

	return DM35424_Write(handle, &ioctl_request);


}


DM35424LIB_API
int DM35424_Adc_Get_Post_Stop_Samples(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t *post_capture_count)
{

	union dm35424_ioctl_argument ioctl_request;
	int result = 0;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_POST_CAPT_COUNT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	result = DM35424_Read(handle, &ioctl_request);

	*post_capture_count = ioctl_request.readwrite.access.data.data32;

	return result;
}


DM35424LIB_API
int DM35424_Adc_Set_Post_Stop_Samples(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					uint32_t post_capture_count)
{

	union dm35424_ioctl_argument ioctl_request;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_POST_CAPT_COUNT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = post_capture_count;

	return DM35424_Write(handle, &ioctl_request);
}



DM35424LIB_API
int DM35424_Adc_Get_Clock_Src(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					enum DM35424_Clock_Sources *source)
{

	union dm35424_ioctl_argument ioctl_request;
	int result = 0;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_CLK_SRC;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	result = DM35424_Read(handle, &ioctl_request);

	*source = ioctl_request.readwrite.access.data.data8;

	return result;
}



DM35424LIB_API
int DM35424_Adc_Set_Clock_Src(struct DM35424_Board_Descriptor *handle,
					const struct DM35424_Function_Block *func_block,
					enum DM35424_Clock_Sources source)
{

	union dm35424_ioctl_argument ioctl_request;

	if (DM35424_Adc_Validate_Clock(source)) {
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_CLK_SRC;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = (uint8_t) source;

	return DM35424_Write(handle, &ioctl_request);

}



/*******************************************************************************
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
				const struct DM35424_Function_Block *func_block)
{
	union dm35424_ioctl_argument ioctl_request;
	int return_code = 0;
	uint8_t mode_status;
	uint32_t sleep_counts = 0;
	uint32_t max_sleep_counts = 0;
	unsigned int channel = 0;
	uint16_t fe_config;


	/*
	 * Before initializing the ADC, at least one of the channels *MUST* be out
	 * of low power mode.  Otherwise initialization will not complete.  So,
	 * we will check all channels to make sure at least 1 is active.
	 */
	do {
		return_code = DM35424_Adc_Channel_Get_Front_End_Config(handle,
									func_block,
									channel,
									&fe_config);

		if (return_code != 0) {
			return return_code;
		}

		channel ++;

	} while (channel < func_block->num_dma_channels &&
		(~fe_config & DM35424_ADC_FE_CONFIG_POWER_ACTIVE));

	if (~fe_config & DM35424_ADC_FE_CONFIG_POWER_ACTIVE) {
		errno = ENODEV;
		return -1;
	}

	/*
	 * Check the current AD Status, because if it is Uninitialized, then we
	 * need to set it to Reset to make it ready for use.
	 */
	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_MODE_STATUS;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_code = DM35424_Read(handle, &ioctl_request);

	mode_status = ioctl_request.readwrite.access.data.data8;

	if (return_code != 0) {
		return return_code;
	}

	/*
	 * Get just the status
	 */
	mode_status >>= 4;

	if (mode_status != DM35424_ADC_STAT_UNINITIALIZED &&
	    mode_status != DM35424_ADC_STAT_INITIALIZING) {
		return return_code;
	}

	if (mode_status != DM35424_ADC_STAT_INITIALIZING) {
		/*
		 * It is Uninitialized, so tell it to reset
		 */
		ioctl_request.readwrite.access.data.data8 = DM35424_ADC_MODE_RESET;
		return_code = DM35424_Write(handle, &ioctl_request);

		if (return_code != 0) {
			return return_code;
		}
	}

	/*
	 * The status is not reset, so set it to that status and then
	 * sleep until it resets or we run out of time.  For some boards,
	 * the time required to initialize is dependent on the mode and
	 * sampling rate.  So we'll wait for a maximum of 10 seconds,
	 * which should cover most configurations.
	 */

	max_sleep_counts = DM35424_INIT_WAIT_TIME_USEC / DM35424_INIT_SLEEP_USEC;

	while (mode_status != DM35424_ADC_STAT_STOPPED &&
		(sleep_counts < max_sleep_counts)) {

		// Sleep for 10 msec
		DM35424_Micro_Sleep(DM35424_INIT_SLEEP_USEC);

		return_code = DM35424_Read(handle, &ioctl_request);

		mode_status = ioctl_request.readwrite.access.data.data8;

		if (return_code != 0) {
			return return_code;
		}

		/*
		 * Get just the status
		 */
		mode_status >>= 4;

		sleep_counts++;
	}

	if (mode_status != DM35424_ADC_STAT_STOPPED) {
		errno = EBUSY;
		return -1;
	}
	else {

		return return_code;
	}

}



DM35424LIB_API
int DM35424_Adc_Set_Clk_Divider(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t divider)
{
	union dm35424_ioctl_argument ioctl_request;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_CLK_DIV;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = divider;

	return DM35424_Write(handle, &ioctl_request);
}



/*******************************************************************************
@brief
    Set the sampling rate for the ADC.

 */
DM35424LIB_API
int DM35424_Adc_Set_Sample_Rate(struct DM35424_Board_Descriptor *handle,
				const struct DM35424_Function_Block *func_block,
				uint32_t rate,
				uint32_t *actual_rate)
{

	int return_code = 0;

	uint32_t system_clock_rate;
	int is_std_clk = 0;
	uint32_t pacer_sample_rate = 1;
	uint32_t divider;
	unsigned int min_divider = 1;
	uint16_t mode;
	union dm35424_ioctl_argument ioctl_request;

	ioctl_request.readwrite.access.offset = func_block->control_offset +
						DM35424_OFFSET_ADC_AD_CONFIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	return_code = DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}
	mode = (ioctl_request.readwrite.access.data.data32 >> 16);

	switch (mode) {
	case DM35424_ADC_MODE_CONFIG_HIGH_SPEED:
		pacer_sample_rate = 256;
		min_divider = DM35424_ADC_HIGH_SPD_MIN_DIV;
		break;
	case DM35424_ADC_MODE_CONFIG_LOW_POWER:
		min_divider = DM35424_ADC_LOW_POW_MIN_DIV;
		pacer_sample_rate = 256;
		break;
	case DM35424_ADC_MODE_CONFIG_HIGH_RES:
		min_divider = DM35424_ADC_HIGH_RES_MIN_DIV;
		pacer_sample_rate = 512;
		break;
	case DM35424_ADC_MODE_CONFIG_LOW_SPEED:
		min_divider = DM35424_ADC_LOW_SPD_MIN_DIV;
		pacer_sample_rate = 512;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	if (rate < 1 || rate > DM35424_ADC_MAX_RATE) {
		errno = EINVAL;
		return -1;
	}

	return_code = DM35424_Gbc_Get_Sys_Clock_Freq(handle,
							&system_clock_rate,
							&is_std_clk);

	if (return_code != 0) {
		return return_code;
	}

	if (!is_std_clk) {
		errno = ENODEV;
		return -1;
	}

	divider = system_clock_rate / (rate * pacer_sample_rate);

	if (divider < min_divider) {
		divider = min_divider;
	}

	*actual_rate = system_clock_rate / (divider * pacer_sample_rate);

	if (*actual_rate == 0) {
		errno = ERANGE;
		return -1;
	}

	/* The actual value written to the register is minus one.  The previous checks
	 * against a min divider prevents this value from going negative.
	 */
	return DM35424_Adc_Set_Clk_Divider(handle,
									func_block,
									divider - 1);

}


/*******************************************************************************
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
					uint16_t *fe_config)
{

	union dm35424_ioctl_argument ioctl_request;
	int status = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_FRONT_END_CONFIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	status = DM35424_Read(handle, &ioctl_request);

	*fe_config = ioctl_request.readwrite.access.data.data32 >> 16;

	return status;

}


/*******************************************************************************
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
				enum DM35424_Sampling_Mode mode)
{
	union dm35424_ioctl_argument ioctl_request;

	if (DM35424_Adc_Validate_Ad_Config_Mode(mode)) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_AD_CONFIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = DM35424_Get_Maskable(mode, 0xFFFF);

	return DM35424_Write(handle, &ioctl_request);

}

/*******************************************************************************
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

    Returned mode value from the register

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
				uint16_t *mode)
{

	union dm35424_ioctl_argument ioctl_request;
	int status = 0;

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_AD_CONFIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	status = DM35424_Read(handle, &ioctl_request);

	*mode = ioctl_request.readwrite.access.data.data32 >> 16;

	return status;

}

/*******************************************************************************
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
					uint16_t interrupt_src,
					int enable)
{
	union dm35424_ioctl_argument ioctl_request;
    	int return_code;
	uint32_t value = 0;

	if (enable) {
		value = 0xFFFF0000;

		/*
		 * Clear the interrupt status before enabling so we don't
		 * get an interrupt from previous data
		 */
		return_code = DM35424_Adc_Interrupt_Clear_Status(handle, func_block,
		    						interrupt_src);
		if (return_code != 0) {
			return return_code;
		}
	}

	value |= (uint32_t) interrupt_src;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_INT_ENABLE;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = value;

	return DM35424_Write(handle, &ioctl_request);


}


/*******************************************************************************
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
					uint16_t *interrupt_ena)
{
	union dm35424_ioctl_argument ioctl_request;
	int result = 0;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_INT_ENABLE;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	result = DM35424_Read(handle, &ioctl_request);

	*interrupt_ena = (ioctl_request.readwrite.access.data.data32 >> 16);

	return result;

}


/*******************************************************************************
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
					const struct DM35424_Function_Block *func_block)
{

	union dm35424_ioctl_argument ioctl_request;


	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_MODE_STATUS;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_ADC_MODE_GO_SINGLE_SHOT;


	return DM35424_Write(handle, &ioctl_request);

}


/*******************************************************************************
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
					const struct DM35424_Function_Block *func_block)
{

	union dm35424_ioctl_argument ioctl_request;


	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_MODE_STATUS;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_ADC_MODE_GO_REARM;


	return DM35424_Write(handle, &ioctl_request);

}


/*******************************************************************************
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
		const struct DM35424_Function_Block *func_block)
{

	union dm35424_ioctl_argument ioctl_request;


	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_MODE_STATUS;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_ADC_MODE_RESET;

	return DM35424_Write(handle, &ioctl_request);

}



/*******************************************************************************
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
		const struct DM35424_Function_Block *func_block)
{

	union dm35424_ioctl_argument ioctl_request;


	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_MODE_STATUS;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_ADC_MODE_PAUSE;

	return DM35424_Write(handle, &ioctl_request);

}


/*******************************************************************************
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
		const struct DM35424_Function_Block *func_block)
{

	union dm35424_ioctl_argument ioctl_request;


	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_MODE_STATUS;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = DM35424_ADC_MODE_UNINITIALIZED;

	return DM35424_Write(handle, &ioctl_request);

}


/*******************************************************************************
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
		uint8_t *mode_status)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_val;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_MODE_STATUS;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_val = DM35424_Read(handle, &ioctl_request);

	*mode_status = ioctl_request.readwrite.access.data.data8;

	return return_val;
}


/*******************************************************************************
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
					int32_t *value)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_LAST_SAMPLE;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;


	return_code = DM35424_Read(handle, &ioctl_request);

	*value = (int32_t) ioctl_request.readwrite.access.data.data32;

	return return_code;

}


/*******************************************************************************
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
						uint32_t *value)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_SAMPLE_COUNT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;


	return_code = DM35424_Read(handle, &ioctl_request);


	*value = ioctl_request.readwrite.access.data.data32;

	return return_code;

}


/*******************************************************************************
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
						uint16_t *value)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_INT_STAT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;


	return_code = DM35424_Read(handle, &ioctl_request);

	*value = ioctl_request.readwrite.access.data.data16;

	return return_code;

}


/*******************************************************************************
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
						uint16_t value)
{

	union dm35424_ioctl_argument ioctl_request;

	ioctl_request.readwrite.access.offset = func_block->control_offset + DM35424_OFFSET_ADC_INT_STAT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_16;
	ioctl_request.readwrite.access.data.data16 = value;


	return DM35424_Write(handle, &ioctl_request);


}



/*******************************************************************************
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
				enum DM35424_Input_Mode input_mode)
{

	union dm35424_ioctl_argument ioctl_request;
	uint32_t fe_config = 0;
	uint16_t fe_data;
    	int return_code;

	if (DM35424_Adc_Validate_Channel_Setup(func_block,
							channel,
							input_range,
							input_mode)) {
		errno = EINVAL;
		return -1;
	}


	ioctl_request.readwrite.access.offset = func_block->control_offset +
			DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
			(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
			DM35424_OFFSET_ADC_CHAN_FRONT_END_CONFIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	switch (input_mode) {
	case DM35424_ADC_INPUT_DIFFERENTIAL:
		fe_data = DM35424_ADC_FE_CONFIG_DIFFERENTIAL;
		break;
	case DM35424_ADC_INPUT_DAC_LOOPBACK:
		fe_data = DM35424_ADC_FE_CONFIG_DAC_LOOPBACK;
		break;
	case DM35424_ADC_INPUT_SINGLE_ENDED_POS:
		fe_data = DM35424_ADC_FE_CONFIG_SNGL_END_POS;
		break;
	case DM35424_ADC_INPUT_SINGLE_ENDED_NEG:
		fe_data = DM35424_ADC_FE_CONFIG_SNGL_END_NEG;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	switch (input_range) {
	/* Break intentionally omitted */
	case DM35424_ADC_RNG_BIPOLAR_2_5V:
	case DM35424_ADC_RNG_UNIPOLAR_5V:
		fe_data |= DM35424_ADC_FE_CONFIG_GAIN_1;
		break;

	case DM35424_ADC_RNG_BIPOLAR_1_25V:
		fe_data |= DM35424_ADC_FE_CONFIG_GAIN_2;
		break;

	case DM35424_ADC_RNG_BIPOLAR_625mV:
		fe_data |= DM35424_ADC_FE_CONFIG_GAIN_4;
		break;

	case DM35424_ADC_RNG_BIPOLAR_312mV:
		fe_data |= DM35424_ADC_FE_CONFIG_GAIN_8;
		break;

	case DM35424_ADC_RNG_BIPOLAR_156mV:
		fe_data |= DM35424_ADC_FE_CONFIG_GAIN_16;
		break;

	case DM35424_ADC_RNG_BIPOLAR_78mV:
		fe_data |= DM35424_ADC_FE_CONFIG_GAIN_32;
		break;

	case DM35424_ADC_RNG_BIPOLAR_39mV:
		fe_data |= DM35424_ADC_FE_CONFIG_GAIN_64;
		break;

	case DM35424_ADC_RNG_BIPOLAR_19mV:
		fe_data |= DM35424_ADC_FE_CONFIG_GAIN_128;
		break;

	default:
		errno = EINVAL;
		return -1;
	}
	fe_config = DM35424_Get_Maskable(DM35424_ADC_FE_CONFIG_POWER_ACTIVE |
						DM35424_ADC_FE_CONFIG_PGA_ACTIVE |
						DM35424_ADC_FE_CONFIG_IN_SWITCH_ENABLED |
						fe_data,
						DM35424_ADC_FE_CONFIG_POWER_MASK |
						DM35424_ADC_FE_CONFIG_PGA_MASK |
						DM35424_ADC_FE_CONFIG_INPUT_SW_ENABLE_MASK |
						DM35424_ADC_FE_CONFIG_GAIN_MASK 	|
						DM35424_ADC_FE_CONFIG_INPUT_LINE_MASK);


	ioctl_request.readwrite.access.data.data32 = fe_config;

	ioctl_request.readwrite.access.data.data32 = fe_config;
	return_code = DM35424_Write(handle, &ioctl_request);
    	if(return_code != 0) {

		return return_code;

    	} else {
		/* Per the board manual, the FE Config register may require
		 * up to 100 microseconds to update/settle, so we will
		 * sleep here to give it plenty of time
		 */
		DM35424_Micro_Sleep(DM35424_FECONFIG_SLEEP_USEC);
    	}


	return 0;
}




/*******************************************************************************
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
						unsigned int channel)
{

	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
			DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
			(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
			DM35424_OFFSET_ADC_CHAN_FRONT_END_CONFIG;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	ioctl_request.readwrite.access.data.data32 = DM35424_Get_Maskable(0,
									0xFFFF);
	return DM35424_Write(handle, &ioctl_request);
}



/*******************************************************************************
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
            EINVAL	Invalid channel requested.

 */
DM35424LIB_API
int DM35424_Adc_Channel_Interrupt_Set_Config(struct DM35424_Board_Descriptor *handle,
						const struct DM35424_Function_Block *func_block,
						unsigned int channel,
						uint8_t interrupt_to_set,
						int enable)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code;
	uint8_t data_to_write = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	if (enable) {
		data_to_write = 0xFF;

		/*
		 * Clear the interrupt status before enabling so we don't
		 * get an interrupt from previous data
		 */
		return_code = DM35424_Adc_Channel_Interrupt_Clear_Status(handle,
		    			func_block, channel, interrupt_to_set);
		if (return_code != 0) {
			return return_code;
		}
	}


	ioctl_request.modify.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_INTR_ENABLE;
	ioctl_request.modify.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.modify.access.size = DM35424_PCI_REGION_ACCESS_8;

	ioctl_request.modify.mask.mask8 = interrupt_to_set;
	ioctl_request.modify.access.data.data8 = data_to_write;

	return DM35424_Modify(handle, &ioctl_request);
}



/*******************************************************************************
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
						uint8_t *chan_intr_enable)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_INTR_ENABLE;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_code = DM35424_Read(handle, &ioctl_request);

	*chan_intr_enable = ioctl_request.modify.access.data.data8;

	return return_code;
}



/*******************************************************************************
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
						uint8_t *chan_intr_status)
{


	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_INTR_STAT;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_code = DM35424_Read(handle, &ioctl_request);

	*chan_intr_status = ioctl_request.readwrite.access.data.data8;

	return return_code;
}



/*******************************************************************************
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
						uint8_t chan_intr_status)
{


	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.modify.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_INTR_STAT;
	ioctl_request.modify.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.modify.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.modify.access.data.data8 = 0xFF;
	ioctl_request.modify.mask.mask8 = chan_intr_status;

	return DM35424_Modify(handle, &ioctl_request);


}


/*******************************************************************************
@brief
    Find the first channel with an interrupt

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
					uint8_t *channel_intr_enable)
{


	int return_code = 0;

	*channel_with_interrupt = 0;
	*channel_has_interrupt = 0;

	while (*channel_has_interrupt == 0 &&
		(*channel_with_interrupt < func_block->num_dma_channels)) {

		return_code = DM35424_Adc_Channel_Interrupt_Get_Status(handle,
									func_block,
									*channel_with_interrupt,
									channel_intr_status);
		if (return_code != 0) {
			return return_code;
		}


		if (*channel_intr_status != 0) {

			return_code = DM35424_Adc_Channel_Interrupt_Get_Config(handle,
										func_block,
										*channel_with_interrupt,
										channel_intr_enable);

			if (return_code != 0) {
				return return_code;
			}
			if (*channel_intr_status & *channel_intr_enable) {
				*channel_has_interrupt = 1;
			}
			else {
				(*channel_with_interrupt)++;

			}
		}
		else {
			(*channel_with_interrupt)++;
		}

	}

	return return_code;

}



/*******************************************************************************
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
				uint8_t chan_filter)
{


	union dm35424_ioctl_argument ioctl_request;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_FILTER;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = chan_filter;


	return DM35424_Write(handle, &ioctl_request);

}



/*******************************************************************************
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
				uint8_t *chan_filter)
{


	union dm35424_ioctl_argument ioctl_request;
	int return_code;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_FILTER;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_code = DM35424_Write(handle, &ioctl_request);

	*chan_filter = ioctl_request.readwrite.access.data.data8;

	return return_code;
}



/*******************************************************************************
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
    The comparison made on the board will not be against the full 32-bits of
    this value, nor necessarily against the full bit width of the ADC.
    Please consult the manual for the actual number of bits
    of this threshold that will be compared against the sampled value.

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
					int32_t threshold)
{

	union dm35424_ioctl_argument ioctl_request;
	int32_t max, min;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	// Determine min and max values for the thresholds
    max = DM35424_ADC_THRESHOLD_MAX;
	min = DM35424_ADC_THRESHOLD_MIN;


	// validate threshold value
	if( (threshold > max) || (threshold < min) )
	{
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_LOW_THRESHOLD;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = threshold;


	return DM35424_Write(handle, &ioctl_request);

}



/*******************************************************************************
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
    The comparison made on the board will not be against the full 32-bits of
    this value, nor necessarily against the full bit width of the ADC.
    Please consult the manual for the actual number of bits
    of this threshold that will be compared against the sampled value.

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
					int32_t threshold)
{

	union dm35424_ioctl_argument ioctl_request;
	int32_t max, min;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

    // Determine min and max values for the thresholds
	max = DM35424_ADC_THRESHOLD_MAX;
	min = DM35424_ADC_THRESHOLD_MIN;

	// validate threshold value
	if( (threshold > max) || (threshold < min) )
	{
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_HIGH_THRESHOLD;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;
	ioctl_request.readwrite.access.data.data32 = threshold;


	return DM35424_Write(handle, &ioctl_request);

}



/*******************************************************************************
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
					int32_t *high_threshold)
{

	union dm35424_ioctl_argument ioctl_request;
	int return_code = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_LOW_THRESHOLD;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;


	return_code =  DM35424_Read(handle, &ioctl_request);

	if (return_code != 0) {
		return return_code;
	}


        *low_threshold = (int32_t) ioctl_request.readwrite.access.data.data32;


	ioctl_request.readwrite.access.offset = func_block->control_offset +
		DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START +
		(channel * DM35424_ADC_CHAN_CTRL_BLK_SIZE) +
		DM35424_OFFSET_ADC_CHAN_HIGH_THRESHOLD;

	return_code = DM35424_Read(handle, &ioctl_request);

        *high_threshold = (int32_t) ioctl_request.readwrite.access.data.data32;

	return return_code;
}


DM35424LIB_API
int DM35424_Adc_Fifo_Channel_Read(struct DM35424_Board_Descriptor *handle,
                            		const struct DM35424_Function_Block *func_block,
                            		unsigned int channel,
                            		int32_t *value)
{
	union dm35424_ioctl_argument ioctl_request;
	int return_code = 0;

	if (channel >= func_block->num_dma_channels) {
		errno = EINVAL;
		return -1;
	}

	// Check that the function block revision ID is valid for direct
	// FIFO access.
	if(func_block->type_revision < DM35424_FIFO_ACCESS_FB_REVISION)
	{
		errno = EPERM;
		return -1;
	}

	// Set up the IOCTL struct
	ioctl_request.readwrite.access.offset = func_block->fb_offset +
				DM35424_OFFSET_ADC_FIFO_CTRL_BLK_START +
				(channel * DM35424_ADC_FIFO_CTRL_BLK_SIZE);
				ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
				ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_32;

	// Perform the read
	return_code = DM35424_Read(handle, &ioctl_request);

	*value = (int32_t)ioctl_request.readwrite.access.data.data32;

	return return_code;
}


/*******************************************************************************
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

    Source to set global clock to (i.e. what is driving it?)

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
					enum DM35424_Adc_Clock_Events clock_driver)
{

	int return_code = 0;
	union dm35424_ioctl_argument ioctl_request;
	unsigned int offset;

	return_code = DM35424_Validate_Clock_Global_Source(clock_select,
								clock_driver);

	if (return_code != 0) {
		return return_code;
	}


	switch (clock_select) {
	case DM35424_CLK_SRC_BUS2:
		offset = DM35424_OFFSET_ADC_CLK_BUS2;
		break;
	case DM35424_CLK_SRC_BUS3:
		offset = DM35424_OFFSET_ADC_CLK_BUS3;
		break;
	case DM35424_CLK_SRC_BUS4:
		offset = DM35424_OFFSET_ADC_CLK_BUS4;
		break;
	case DM35424_CLK_SRC_BUS5:
		offset = DM35424_OFFSET_ADC_CLK_BUS5;
		break;
	case DM35424_CLK_SRC_BUS6:
		offset = DM35424_OFFSET_ADC_CLK_BUS6;
		break;
	case DM35424_CLK_SRC_BUS7:
		offset = DM35424_OFFSET_ADC_CLK_BUS7;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
						offset;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;
	ioctl_request.readwrite.access.data.data8 = (uint8_t) clock_driver;

	return DM35424_Write(handle, &ioctl_request);
}


/*******************************************************************************
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
					int *clock_source)
{

	int return_code = 0;
	union dm35424_ioctl_argument ioctl_request;
	unsigned int offset;

	switch (clock_select) {
	case DM35424_CLK_SRC_BUS2:
		offset = DM35424_OFFSET_ADC_CLK_BUS2;
		break;
	case DM35424_CLK_SRC_BUS3:
		offset = DM35424_OFFSET_ADC_CLK_BUS3;
		break;
	case DM35424_CLK_SRC_BUS4:
		offset = DM35424_OFFSET_ADC_CLK_BUS4;
		break;
	case DM35424_CLK_SRC_BUS5:
		offset = DM35424_OFFSET_ADC_CLK_BUS5;
		break;
	case DM35424_CLK_SRC_BUS6:
		offset = DM35424_OFFSET_ADC_CLK_BUS6;
		break;
	case DM35424_CLK_SRC_BUS7:
		offset = DM35424_OFFSET_ADC_CLK_BUS7;
		break;
	default:
		return -1;
		break;
	}

	ioctl_request.readwrite.access.offset = func_block->control_offset +
						offset;
	ioctl_request.readwrite.access.region = DM35424_PCI_REGION_FB;
	ioctl_request.readwrite.access.size = DM35424_PCI_REGION_ACCESS_8;

	return_code = DM35424_Read(handle, &ioctl_request);

	*clock_source = (int) ioctl_request.readwrite.access.data.data8;

	return return_code;

}



/**
*******************************************************************************
@brief
    Convert an ADC sample to a volts value.

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
int DM35424_Adc_Sample_To_Volts(enum DM35424_Input_Ranges input_range,
					int32_t adc_sample,
					float *volts)
{

	float gain;
	int single_ended = 0;

	switch(input_range) {
	case DM35424_ADC_RNG_BIPOLAR_2_5V:
	    gain = 1;
	    break;
	case DM35424_ADC_RNG_UNIPOLAR_5V:
	    gain = 1;
	    single_ended = 1;
	    break;
	case DM35424_ADC_RNG_BIPOLAR_1_25V:
	    gain = 2;
	    break;
	case DM35424_ADC_RNG_BIPOLAR_625mV:
	    gain = 4;
	    break;
	case DM35424_ADC_RNG_BIPOLAR_312mV:
	    gain = 8;
	    break;
	case DM35424_ADC_RNG_BIPOLAR_156mV:
	    gain = 16;
	    break;
	case DM35424_ADC_RNG_BIPOLAR_78mV:
	    gain = 32;
	    break;
	case DM35424_ADC_RNG_BIPOLAR_39mV:
	    gain = 64;
	    break;
	case DM35424_ADC_RNG_BIPOLAR_19mV:
	    gain = 128;
	    break;
	default:
	    *volts = 0.0;
	    errno = EINVAL;
	    return -1;
	}

	// See equation in hardware manual
	if(single_ended) {
	*volts = ((2.5 / (gain * 8388607.0)) * (float)(adc_sample - 0x7FFFFF)) + 2.5;
	}
	else {
	*volts = (2.5 / (gain * 8388607.0)) * (float)adc_sample;
	}

	return 0;

}


/**
*******************************************************************************
@brief
    Convert volts to an ADC value.

@param
    func_block

    Pointer to the function block descriptor, which contains the offsets to
    command sections of the board.

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
				int32_t *adc_sample)
{

    float gain;
    int single_ended = 0;

    switch(input_range) {
        case DM35424_ADC_RNG_BIPOLAR_2_5V:
            gain = 1;
            break;
        case DM35424_ADC_RNG_UNIPOLAR_5V:
            gain = 1;
            single_ended = 1;
            break;
        case DM35424_ADC_RNG_BIPOLAR_1_25V:
            gain = 2;
            break;
        case DM35424_ADC_RNG_BIPOLAR_625mV:
            gain = 4;
            break;
        case DM35424_ADC_RNG_BIPOLAR_312mV:
            gain = 8;
            break;
        case DM35424_ADC_RNG_BIPOLAR_156mV:
            gain = 16;
            break;
        case DM35424_ADC_RNG_BIPOLAR_78mV:
            gain = 32;
            break;
        case DM35424_ADC_RNG_BIPOLAR_39mV:
            gain = 64;
            break;
        case DM35424_ADC_RNG_BIPOLAR_19mV:
            gain = 128;
            break;
        default:
            *adc_sample = 0;
            errno = EINVAL;
            return -1;
    }

    // See equation in hardware manual
    if(single_ended) {
        *adc_sample = (volts-2.5)*((gain*8388607.0)/2.5) + 0x7FFFFF;
    }
    else {
        *adc_sample = volts * ((gain * 8388607.0) / 2.5);
    }


	return 0;


}


/**
 * @} DM35424_ADC_Library_Public_Functions
 */

/**
 * @} DM35424_ADC_Library_Source
 */


