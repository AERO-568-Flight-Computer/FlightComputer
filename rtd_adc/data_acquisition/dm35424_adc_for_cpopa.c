/* 
Danilo Carrasco

This program is modified from RTD's example program dm35424_adc_continuous_dma.c. 
This is an attempt to reconfigure the example for the purpose of the flight computer.

Planned Functionality

- The program will output the data to a file in binary format to a python server.
- The program will allow the user to configure the adc
- The program will allow disabling the DAC functionality by the user
- Make it so this code can compile outside the driver folder (possibly difficult)

New Functionality


*/

/**
    @file

    @brief
        Example program which demonstrates the use of the ADC and
        DMA.

    @verbatim

        This example program will collect data from the ADC(s)
        specified by the user, at the rate specified by the user, and will
        write the data to a file.  It will do this continuously until the
        user hits CTRL-C (or the filesystem becomes full).

	You can put any signal you want on the ADC input pins.
	However, for convenience, this example sets up the DACs to provide
	a signal for the ADC to measure.  In order for that to work, you
	must loopback the DAC outputs to the ADC differential inputs.  Connect
	DAC0 Channel 0 to ADC_0 Channel 0+ and ADC_0 Channel 1-, and connect
	DAC0 Channel 1 to ADC_0 Channel 0- and ADC_0 Channel 1+, etc.

	Maximum sustainable throughput is HIGHLY system dependent. Higher
	sample rates might be achievable through better buffer size
	selection or use of an operating system with realtime features.


    @endverbatim

    @verbatim
	--------------------------------------------------------------------------
	This file and its contents are copyright (C) RTD Embedded Technologies,
	Inc.  All Rights Reserved.

	This software is licensed as described in the RTD End-User Software License
	Agreement.  For a copy of this agreement, refer to the file LICENSE.TXT
	(which should be included with this software) or contact RTD Embedded
	Technologies, Inc.
	--------------------------------------------------------------------------
    @endverbatim

    $Id: dm35424_adc_continuous_dma.c 108389 2017-04-27 13:27:05Z rgroner $
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <getopt.h>

#include "dm35424_gbc_library.h"
#include "dm35424_dac_library.h"
#include "dm35424_adc_library.h"
#include "dm35424_ioctl.h"
#include "dm35424_examples.h"
#include "dm35424_dma_library.h"
#include "dm35424.h"
#include "dm35424_util_library.h"
#include "udp_send_data.h"

/**
 * Default rate to use, if user does not enter one. (Hz)
 */
#define DEFAULT_RATE		10000

/**
 * Number of samples in the DAC buffer (to form the wave pattern)
 */
#define BUFFER_SIZE_SAMPLES	1000

/**
 * Size of DAC DMA buffer, in bytes
 */
#define BUFFER_SIZE_BYTES	(BUFFER_SIZE_SAMPLES * sizeof(int))

/**
 * Name of file when saving as ASCII
 */
#define ASCII_FILE_NAME "./adc_dma.txt"

/**
 * Name of file when saving as binary
 */
#define BIN_FILE_NAME "./adc_dma.bin"

/**
 * Name of the program as invoked on the command line
 */

static char *program_name;

/**
 * Boolean flag indicating if there was a DMA error.
 */
static int dma_has_error = 0;

/**
 * Pointer to board descriptor
 */
static struct DM35424_Board_Descriptor *board;

/**
 * Pointer to array of function blocks that will hold the ADC descriptors
 */
static struct DM35424_Function_Block my_adc;

/**
 * Array of buffer counts, used to track progress of each ADC
 * as data is copied.
 */
static unsigned long buffer_count[DM35424_NUM_ADC_DMA_CHANNELS];

/**
 * Pointer to local memory buffer where data is copied from the kernel buffers
 * when a DMA buffer becomes full.
 */
static int **local_buffer[DM35424_NUM_ADC_DMA_CHANNELS];

/**
 * Boolean indicating the program should exit.
 */
static volatile int exit_program = 0;

/**
 * Size of the buffer allocated, in bytes.
 */
static unsigned long buffer_size_bytes = 0;

/**
 * Which buffer is next to be copied from DMA
 */
static unsigned int next_buffer[DM35424_NUM_ADC_DMA_CHANNELS];

/**
*******************************************************************************
@brief
    Print information on stderr about how the program is to be used.  After
    doing so, the program is exited.
 *******************************************************************************
*/

static void usage(void)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "NAME\n\n\t%s\n\n", program_name);
	fprintf(stderr, "USAGE\n\n\t%s [OPTIONS]\n\n", program_name);

	fprintf(stderr, "OPTIONS\n\n");
	fprintf(stderr, "\t--help\n");
	fprintf(stderr, "\t\tShow this help screen and exit.\n");

	fprintf(stderr, "\t--minor NUM\n");
	fprintf(stderr, "\t\tSpecify the minor number (>= 0) of the board to open.  When not specified,\n");
	fprintf(stderr, "\t\tthe device file with minor 0 is opened.\n");

	fprintf(stderr, "\t--rate RATE\n");
	fprintf(stderr, "\t\tUse the specified rate (Hz).  The default is %d.\n", DEFAULT_RATE);

	fprintf(stderr, "\t--samples NUM\n");
	fprintf(stderr, "\t\tThe number of samples to collect before stopping.  Note that the\n");
	fprintf(stderr, "\t\tactual number collected may be higher due to buffer size.\n");

	fprintf(stderr, "\t--binary\n");
	fprintf(stderr, "\t\tWrite data to file in binary format, instead of default ASCII.\n");
	fprintf(stderr, "\t\tData is stored as [ADC_0 Buff0][ADC1 Buff0]....[ADC_0 Buff1][ADC1 Buff1],etc\n");

	fprintf(stderr, "\t--bin2txt\n");
	fprintf(stderr, "\t\tThe program will convert the %s file to\n", BIN_FILE_NAME);
	fprintf(stderr, "\t\t%s and exit.\n\n", ASCII_FILE_NAME);
	fprintf(stderr, "\t\tNote: Because the rate affects the buffer size, you must\n");
	fprintf(stderr, "\t\tinclude the --rate argument as well, IF it was used to\n");
	fprintf(stderr, "\t\tcreate the binary file in the first place.\n");

	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

/**
*******************************************************************************
@brief
    Signal handler for SIGINT Control-C keyboard interrupt.

@param
    signal_number

    Signal number passed in from the kernel.

@warning
    One must be extremely careful about what functions are called from a signal
    handler.
 *******************************************************************************
*/
static void sigint_handler(int signal_number)
{
	exit_program = 0xff;
}


/**
*******************************************************************************
@brief
    Output the status of a DMA channel.  This is a helper function to determine
    the cause of an error when it occurs.

@param
    handle

    Pointer to the board handle.

@param
    func_block

    Pointer to the function block containing the DMA channel

@param
    channel

    The DMA channel we want the status of.

 @retval
    None
 *******************************************************************************
*/
void output_channel_status(struct DM35424_Board_Descriptor *handle,
			   const struct DM35424_Function_Block *func_block,
			   unsigned int channel)
{
	int result;
	unsigned int current_buffer;
	uint32_t current_count;
	int current_action;
	int status_overflow;
	int status_underflow;
	int status_used;
	int status_invalid;
	int status_complete;

	result = DM35424_Dma_Status(handle,
				    func_block,
				    channel,
				    &current_buffer,
				    &current_count,
				    &current_action,
				    &status_overflow,
				    &status_underflow,
				    &status_used,
				    &status_invalid, &status_complete);

	check_result(result, "Error getting DMA status");

	printf
	    ("FB%d Ch%d DMA Status: Current Buffer: %u  Count: %ul  Action: 0x%x  Status: "
	     "Ov: %d  Un: %d  Used: %d  Inv: %d  Comp: %d\n",
	     func_block->fb_num, channel, current_buffer, current_count,
	     current_action, status_overflow, status_underflow, status_used,
	     status_invalid, status_complete);
}


/**
*******************************************************************************
@brief
    The interrupt subroutine that will execute when a DMA interrupt occurs.
    This function will read from the DMA, copying data from the kernel buffers
    to the user buffers so that we can access the data.

@param
    int_info

    A structure containing information about the interrupt.

 @retval
    None.
 *******************************************************************************
*/
void ISR(struct dm35424_ioctl_interrupt_info_request int_info)
{

	int result = 0;
	unsigned int channel = CHANNEL_0;
	int channel_complete = 0, channel_error = 0;
	uint32_t function_block;

	function_block = int_info.interrupt_fb & 0x7FFFFFFF;

	if (int_info.valid_interrupt) {

		check_result(my_adc.fb_num != function_block, "Interrupt from unexpected function block.");

		// It's a DMA interrupt
		if (int_info.interrupt_fb < 0) {

			result = DM35424_Dma_Find_Interrupt(board,
							     &my_adc,
							     &channel,
							     &channel_complete,
							     &channel_error);

			check_result(result, "Error checking for DMA error.");

			if (channel_error) {
				dma_has_error = 1;
				exit_program = 1;
				return;
			}

			for (channel = CHANNEL_0; channel < DM35424_NUM_ADC_DMA_CHANNELS; channel ++) {

					result = DM35424_Dma_Read(board,
								  &my_adc,
								  channel,
								  next_buffer[channel],
								  buffer_size_bytes,
								  local_buffer[channel]
								  [next_buffer[channel]]);

					buffer_count[channel]++;
					check_result(result,
						     "Error getting DMA buffer");

					result = DM35424_Dma_Reset_Buffer(board,
									  &my_adc,
									  channel,
									  next_buffer[channel]);

					check_result(result, "Error resetting buffer");

					next_buffer[channel] = (next_buffer[channel] + 1) % DM35424_NUM_ADC_DMA_BUFFERS;

			}

			result = DM35424_Dma_Clear_Interrupt(board,
								 &my_adc,
								 CHANNEL_0,
								 NO_CLEAR_INTERRUPT,
								 NO_CLEAR_INTERRUPT,
								 NO_CLEAR_INTERRUPT,
								 NO_CLEAR_INTERRUPT,
								 CLEAR_INTERRUPT);


		}
		else {
			exit_program = 1;
			printf("Error: Non-DMA interrupt received.");
			return;
		}

		result = DM35424_Gbc_Ack_Interrupt(board);

		check_result(result, "Error calling ACK interrupt.");

	}

}


/**
*******************************************************************************
@brief
    Setup the DACs to produce a sine wave as a signal to sample, then start them.

@param
    my_dac

    Pointer to the DAC function block structure.

 @retval
    None.
 *******************************************************************************
*/
void setup_dacs_and_start(struct DM35424_Function_Block *my_dac)
{
	unsigned int dac_num, channel = 0, index;
	int result;
	int16_t max_value, min_value, offset;
	int32_t *buffer, *offset_buffer;
	uint32_t actual_rate = 0;

	printf("Setting up DACs....\n");

	buffer = (int *)malloc(BUFFER_SIZE_BYTES);
	offset_buffer = (int *)malloc(BUFFER_SIZE_BYTES);

	check_result(buffer == NULL || offset_buffer == NULL, "Error allocating space for buffer.");

	for (dac_num = 0; dac_num < DM35424_NUM_DAC_ON_BOARD; dac_num ++) {
		result = DM35424_Dac_Open(board, dac_num, &my_dac[dac_num]);

		check_result(result, "Could not open DAC");

		printf("    Found DAC%u, with %d DMA channels (%d buffers each)\n",
		       dac_num, my_dac[dac_num].num_dma_channels, my_dac[dac_num].num_dma_buffers);

		result = DM35424_Dac_Set_Clock_Src(board,
						   &my_dac[dac_num],
						   DM35424_CLK_SRC_IMMEDIATE);

		check_result(result, "Error setting DAC clock");

		result = DM35424_Dac_Set_Conversion_Rate(board,
							 &my_dac[dac_num], BUFFER_SIZE_SAMPLES, &actual_rate);

		fprintf(stdout, "    Rate requested: %d  Actual Rate Achieved: %d\n", BUFFER_SIZE_SAMPLES,
				actual_rate);
		check_result(result, "Error setting sample rate");

		result = DM35424_Dac_Volts_To_Conv(1.25f,
						&max_value);

		check_result(result, "Error converting value to conversion counts.");

		result = DM35424_Dac_Volts_To_Conv(-1.25f,
							&min_value);

		check_result(result, "Error converting value to conversion counts.");

		result = DM35424_Dac_Volts_To_Conv(2.5f,
							&offset);

		check_result(result, "Error converting value to conversion counts.");


		result = DM35424_Generate_Signal_Data(DM35424_SINE_WAVE,
						      buffer,
						      BUFFER_SIZE_SAMPLES,
						      max_value,
						      min_value,
						      offset,
						      0x0000FFFF);

		check_result(result, "Error trying to generate data for the DAC.");

		for (index = 0; index < BUFFER_SIZE_SAMPLES; index ++) {
			offset_buffer[index] = buffer[(index + (BUFFER_SIZE_SAMPLES / 2)) % BUFFER_SIZE_SAMPLES];
		}

		for (channel = 0; channel < my_dac[dac_num].num_dma_channels; channel ++) {
			fprintf(stdout, "    Initializing and configuring DMA Channel %d....",
				channel);
			result =
			    DM35424_Dma_Initialize(board, &my_dac[dac_num], channel,
						   1, BUFFER_SIZE_BYTES);

			check_result(result, "Error initializing DMA");

			result = DM35424_Dma_Setup(board,
						   &my_dac[dac_num],
						   channel,
						   DM35424_DMA_SETUP_DIRECTION_WRITE,
						   IGNORE_USED);

			check_result(result, "Error configuring DMA");

			fprintf(stdout, "success!\n");

			result = DM35424_Dma_Buffer_Setup(board,
							  &my_dac[dac_num],
							  channel,
							  BUFFER_0,
							  DM35424_DMA_BUFFER_CTRL_VALID |
							  DM35424_DMA_BUFFER_CTRL_LOOP);

			check_result(result, "Error setting up buffer control.");

			/**
			 * We load the even channels with the wave pattern, and the odd channels with
			 * the same pattern, but offset by half its length.  Doing this gives us an
			 * opposing pattern between the even and odd channels, which helps when using
			 * DAC for ADC input.
			 */
			if (channel % 2 == 0) {
				result = DM35424_Dma_Write(board,
							   &my_dac[dac_num],
							   channel,
							   BUFFER_0, BUFFER_SIZE_BYTES, buffer);

				check_result(result, "    Writing to DMA buffer failed");
			}
			else {

				result = DM35424_Dma_Write(board,
							   &my_dac[dac_num],
							   channel,
							   BUFFER_0, BUFFER_SIZE_BYTES, offset_buffer);

				check_result(result, "    Writing to DMA buffer failed");

			}

			fprintf(stdout, "    Starting DMA Channel %d......", channel);
			result = DM35424_Dma_Start(board, &my_dac[dac_num], channel);

			check_result(result, "Error starting DMA");

			printf("success.\n");


		}

		fprintf(stdout, "Starting DAC.\n");

		result = DM35424_Dac_Set_Start_Trigger(board,
						       &my_dac[dac_num],
						       DM35424_CLK_SRC_IMMEDIATE);

		check_result(result, "Error setting start trigger for DAC.");

		result = DM35424_Dac_Set_Stop_Trigger(board,
						      &my_dac[dac_num],
						      DM35424_CLK_SRC_NEVER);

		check_result(result, "Error setting stop trigger for DAC.");


		result = DM35424_Dac_Start(board, &my_dac[dac_num]);

		check_result(result, "Error starting DAC");

	}

	free(buffer);
	free(offset_buffer);
}


/**
*******************************************************************************
@brief
    Setup the ADCs to sample.

@param
	rate

	The sampling rate to set the ADC to.

 @retval
    None.
 *******************************************************************************
*/
void setup_adc(uint32_t rate)
{
	unsigned int channel = 0, buff;
	uint8_t buff_status, buff_control;
	uint32_t buff_size, actual_rate;
	int result;

	result = DM35424_Adc_Open(board, ADC_0, &my_adc);

	check_result(result, "Could not open ADC");

	printf("Found ADC_0, with %d DMA channels (%d buffers each)\n",
	       my_adc.num_dma_channels,
	       my_adc.num_dma_buffers);

	result = DM35424_Adc_Set_Clock_Src(board,
					   &my_adc,
					   DM35424_CLK_SRC_IMMEDIATE);

	check_result(result, "Error setting ADC clock");


	for (channel = 0; channel < my_adc.num_dma_channels; channel ++) {

		fprintf(stdout, "Initializing DMA Channel 0....");
		result = DM35424_Dma_Initialize(board,
						&my_adc,
						channel,
						my_adc.num_dma_buffers,
						buffer_size_bytes);

		check_result(result, "Error initializing DMA");

		result = DM35424_Dma_Setup(board,
					   &my_adc,
					   channel,
					   DM35424_DMA_SETUP_DIRECTION_READ,
					   NOT_IGNORE_USED);

		check_result(result, "Error configuring DMA");

		fprintf(stdout, "Setting DMA Interrupts......");
		result = DM35424_Dma_Configure_Interrupts(board,
							  &my_adc,
							  channel,
							  INTERRUPT_ENABLE,
							  ERROR_INTR_ENABLE);

		check_result(result, "Error setting DMA Interrupts");
		fprintf(stdout, "success!\n");

		for (buff = 0; buff < my_adc.num_dma_buffers; buff++) {

			buff_control = DM35424_DMA_BUFFER_CTRL_VALID;

			if (buff == (DM35424_NUM_ADC_DMA_BUFFERS - 1)) {
				buff_control |= DM35424_DMA_BUFFER_CTRL_LOOP;

			}

			// All channels will complete at the same time, so we only
			// need 1 interrupt to tell us.
			if (channel == CHANNEL_0) {
				buff_control |= DM35424_DMA_BUFFER_CTRL_INTR;

			}

			result = DM35424_Dma_Buffer_Setup(board,
							  &my_adc,
							  channel,
							  buff,
							  buff_control);

			check_result(result, "Error setting buffer control.");

			result = DM35424_Dma_Buffer_Status(board,
							   &my_adc,
							   channel,
							   buff,
							   &buff_status,
							   &buff_control,
							   &buff_size);

			check_result(result, "Error getting buffer status.");

			fprintf(stdout,
				"    Buffer %d: Stat: 0x%x  Ctrl: 0x%x  Size: %d\n",
				buff, buff_status, buff_control, buff_size);
		}

		result = DM35424_Adc_Channel_Setup(board,
							&my_adc,
							channel,
							DM35424_ADC_RNG_BIPOLAR_2_5V,
							DM35424_ADC_INPUT_DIFFERENTIAL);

		check_result(result, "Error setting up channel.");

		next_buffer[channel] = 0;

	}

	result = DM35424_Adc_Ad_Config_Set_Mode(board,
						&my_adc,
						DM35424_ADC_MODE_CONFIG_HIGH_SPEED);

	check_result(result, "Error setting AD config.");

	printf("success.\nInitializing ADC......");
	result = DM35424_Adc_Set_Start_Trigger(board,
					       &my_adc,
					       DM35424_CLK_SRC_IMMEDIATE);
	check_result(result, "Error setting start trigger.");

	result = DM35424_Adc_Set_Stop_Trigger(board,
					      &my_adc,
					      DM35424_CLK_SRC_NEVER);
	check_result(result, "Error setting stop trigger.");


	result = DM35424_Adc_Set_Sample_Rate(board,
					     &my_adc,
					     rate, &actual_rate);

	check_result(result, "Failed to set sample rate for ADC.");
	fprintf(stdout,
		"success.\nADC:0 Rate requested: %d  Actual Rate Achieved: %d\n",
		rate, actual_rate);

	result = DM35424_Adc_Initialize(board, &my_adc);

	check_result(result, "Failed or timed out initializing ADC.");

}


/**
*******************************************************************************
@brief
    Handler to detect when user hits Ctrl-C

 @retval
    None.
 *******************************************************************************
*/
void setup_ctrlc_handler()
{
	struct sigaction signal_action;

	signal_action.sa_handler = sigint_handler;
	sigfillset(&(signal_action.sa_mask));
	signal_action.sa_flags = 0;

	if (sigaction(SIGINT, &signal_action, NULL) < 0) {
		error(EXIT_FAILURE, errno, "ERROR: sigaction() FAILED");
	}

}


/**
*******************************************************************************
@brief
    Convert a binary data file to ASCII values.  The format will be the same
    as the data file produced without the --binary argument.  The example
    program will exit after finishing.


 @retval
    None.
 *******************************************************************************
*/
void convert_bin_to_txt(unsigned int samples_in_buff)
{

	FILE *fp_in, *fp_out;
	unsigned long sample_num, output_index = 0;
	int num_read = 0;
	unsigned int channel;

	int **buff;

	buff = (int **) malloc(DM35424_NUM_ADC_DMA_CHANNELS * sizeof(int *));
	if (buff == NULL) {
		error(EXIT_FAILURE, errno,
		"Error allocating memory for all channels.\n");
	}

	for (channel = 0; channel < DM35424_NUM_ADC_DMA_CHANNELS; channel ++) {

		buff[channel] = (int *) malloc(samples_in_buff * sizeof(int));

		if (buff[channel] == NULL) {
			error(EXIT_FAILURE, errno,
			"Error allocating memory per channel.\n");

		}
	}

	fp_in = fopen(BIN_FILE_NAME, "rb");

	if (fp_in == NULL) {
		error(EXIT_FAILURE, errno,
		"open() FAILED to open binary input file %s.\n", BIN_FILE_NAME);
	}

	fp_out = fopen(ASCII_FILE_NAME, "w");

	if (fp_out == NULL) {
		error(EXIT_FAILURE, errno,
		"open() FAILED to open ASCII output file %s.\n", ASCII_FILE_NAME);
	}

	for (channel = 0; channel < DM35424_NUM_ADC_DMA_CHANNELS; channel ++) {

		num_read = fread(buff[channel], sizeof(int), samples_in_buff, fp_in);
	}


	while (num_read > 0) {

		for (sample_num = 0; sample_num < samples_in_buff; sample_num ++) {
			fprintf(fp_out, "%lu", output_index);
			for (channel = 0; channel < DM35424_NUM_ADC_DMA_CHANNELS; channel ++) {
				fprintf(fp_out, "\t%d", buff[channel][sample_num]);
			}
			fprintf(fp_out, "\n");

			output_index ++;
		}
		for (channel = 0; channel < DM35424_NUM_ADC_DMA_CHANNELS; channel ++) {

			num_read = fread(buff[channel], sizeof(int), samples_in_buff, fp_in);
		}
	}

	for (channel = 0; channel < DM35424_NUM_ADC_DMA_CHANNELS; channel ++) {
		free(buff[channel]);
	}

	free(buff);
	fclose(fp_in);
	fclose(fp_out);

}


/**
*******************************************************************************
@brief
    The main program.

@param
    argument_count

    Number of args passed on the command line, including the executable name

@param
    arguments

    Pointer to array of character strings, which are the args themselves.

@retval
    0

    Success

@retval
    Non-Zero

    Failure.

 *******************************************************************************
*/
int main(int argument_count, char **arguments)
{
	unsigned long int minor = 0;
	int result;
	int index;
	FILE *fp;

	unsigned int buffer_to_get;
	unsigned int buffers_copied;
	unsigned long local_buffer_count[DM35424_NUM_ADC_DMA_CHANNELS];
	unsigned long output_index = 0;
	unsigned long samples_to_collect = -1;
	unsigned long bytes_written = 0;

	unsigned int channel = 0, buff;

	uint32_t rate = DEFAULT_RATE;
	struct DM35424_Function_Block my_dac[DM35424_NUM_DAC_ON_BOARD];

	int store_in_binary = 0, samples_in_buffer;
	int help_option_given = 0, convert_bin_file = 0;
	int status;
	char *invalid_char_p;
	struct option options[] = {
		{"help", 0, 0, HELP_OPTION},
		{"minor", 1, 0, MINOR_OPTION},
		{"rate", 1, 0, RATE_OPTION},
		{"samples", 1, 0, SAMPLES_OPTION},
		{"binary", 0, 0, BINARY_OPTION},
		{"bin2txt", 0, 0, BIN2TXT_OPTION},
		{0, 0, 0, 0}
	};

	int all_channels_copied = 0;

	program_name = arguments[0];

	// Show usage, parse arguments
	while (1) {
		/*
		 * Parse the next command line option and any arguments it may require
		 */
		status = getopt_long(argument_count,
				     arguments, "", options, NULL);

		/*
		 * If getopt_long() returned -1, then all options have been processed
		 */
		if (status == -1) {
			break;
		}

		/*
		 * Figure out what getopt_long() found
		 */
		switch (status) {

		/*#################################################################
			  User entered '--help'
		################################################################# */
		case HELP_OPTION:
			help_option_given = 0xFF;
			break;

		/*#################################################################
			User entered '--minor'
		################################################################# */
		case MINOR_OPTION:
			/*
			 * Convert option argument string to unsigned long integer
			 */
			errno = 0;
			minor = strtoul(optarg, &invalid_char_p, 10);

			/*
			 * Catch unsigned long int overflow
			 */
			if ((minor == ULONG_MAX)
			    && (errno == ERANGE)) {
				error(0, 0,
				      "ERROR: Device minor number caused numeric overflow");
				usage();
			}

			/*
			 * Catch argument strings with valid decimal prefixes, for
			 * example "1q", and argument strings which cannot be converted,
			 * for example "abc1"
			 */
			if ((*invalid_char_p != '\0')
			    || (invalid_char_p == optarg)) {
				error(0, 0,
				      "ERROR: Non-decimal device minor number");
				usage();
			}

			break;

		/*#################################################################
			  User entered rate
		################################################################# */
		case RATE_OPTION:
			/*
			 * Convert option argument string to unsigned long integer
			 */
			errno = 0;
			rate = strtoul(optarg, &invalid_char_p, 10);

			/*
			 * Catch unsigned long int overflow
			 */
			if ((rate == ULONG_MAX)
			    && (errno == ERANGE)) {
				error(0, 0,
				      "ERROR: Rate number caused numeric overflow");
				usage();
			}

			/*
			 * Catch argument strings with valid decimal prefixes, for
			 * example "1q", and argument strings which cannot be converted,
			 * for example "abc1"
			 */
			if ((*invalid_char_p != '\0')
			    || (invalid_char_p == optarg)) {
				error(0, 0,
				      "ERROR: Non-decimal rate value entered");
				usage();
			}
			break;


		/*#################################################################
		   User entered number of samples
		 ################################################################# */
		case SAMPLES_OPTION:
			/*
			 * Convert option argument string to unsigned long integer
			 */
			errno = 0;
			samples_to_collect =
			    strtoul(optarg, &invalid_char_p, 10);

			/*
			 * Catch unsigned long int overflow
			 */
			if ((samples_to_collect == ULONG_MAX)
			    && (errno == ERANGE)) {
				error(0, 0,
				      "ERROR: Samples number caused numeric overflow");
				usage();
			}

			/*
			 * Catch argument strings with valid decimal prefixes, for
			 * example "1q", and argument strings which cannot be converted,
			 * for example "abc1"
			 */
			if ((*invalid_char_p != '\0')
			    || (invalid_char_p == optarg)) {
				error(0, 0,
				      "ERROR: Non-decimal samples value entered");
				usage();
			}

			break;

		/*#################################################################
		   User entered '--binary'
		 ################################################################# */
		case BINARY_OPTION:
			/*
			 * '--binary' option has been seen
			 */
			store_in_binary = 0xFF;
			break;

		/*#################################################################
		   User entered '--bin2txt'
		 ################################################################# */
		case BIN2TXT_OPTION:
			convert_bin_file = 1;
			break;

		/*#################################################################
		   User entered unsupported option
		   ################################################################# */
		case '?':
			usage();
			break;

		/*#################################################################
		   getopt_long() returned unexpected value
		   ################################################################# */
		default:
			error(EXIT_FAILURE,
			      0,
			      "ERROR: getopt_long() returned unexpected value %#x",
			      status);
			break;
		}
	}

	/*
	 * Recognize '--help' option before any others
	 */

	if (help_option_given) {
		usage();
	}

	if (rate < 1 || rate > 105468) {
		error(0, 0, "Error: Rate given not within range of board.");
		usage();
	}

	/*
	 * Trying to come up with a reasonable size for buffers that doesn't
	 * take forever to fill up with slower rates, but also makes it possible
	 * to run at higher rates.
	 */
	buffer_size_bytes = rate;
	if (buffer_size_bytes < (20 * sizeof(int))) {
		buffer_size_bytes = 20 * sizeof(int);
	}

	buffer_size_bytes &= ~0x3;
	samples_in_buffer = buffer_size_bytes / sizeof(int);

	if (convert_bin_file) {
		convert_bin_to_txt(samples_in_buffer);
		return 0;
	}

	setup_ctrlc_handler();

	if (store_in_binary) {
		fp = fopen(BIN_FILE_NAME, "wb");
	} else {
		fp = fopen(ASCII_FILE_NAME, "w");
	}

	if (fp == NULL) {
		error(EXIT_FAILURE, errno,
		      "open() FAILED on output data file.\n");
	}

	printf("Opening board.....");
	result = DM35424_Board_Open(minor, &board);

	check_result(result, "Could not open board");
	printf("success.\nResetting board.....");
	result = DM35424_Gbc_Board_Reset(board);

	check_result(result, "Could not reset board");
	printf("success.\n");

	/**
	 * First, setup the DACS.  They will produce a sine wave that needs to
	 * be looped back to the ADC inputs.  This will cause the ADC to see
	 * what looks like a max-value sine wave.
	 */
	printf("success.\n");

	setup_dacs_and_start(my_dac);

	setup_adc(rate);


	// Allocate local memory for data.
	for (channel = 0; channel < my_adc.num_dma_channels; channel++) {
		buffer_count[channel] = 0;
		local_buffer_count[channel] = 0;

		local_buffer[channel] =
		    malloc(sizeof(int *) * my_adc.num_dma_buffers);
		check_result(local_buffer[channel] == NULL,
			     "Could not allocate for local buffer");

		for (buff = 0; buff < my_adc.num_dma_buffers; buff++) {

			local_buffer[channel][buff] = malloc(buffer_size_bytes);

			check_result(local_buffer[channel][buff] == NULL,
				     "Could not allocate for local buffer");

		}

	}

	fprintf(stdout, "Installing user ISR .....");
	result = DM35424_General_InstallISR(board, ISR);
	check_result(result, "DM35424_General_InstallISR()");


	for (channel = 0; channel < my_adc.num_dma_channels; channel++) {

		fprintf(stdout, "Starting ADC_0 DMA %d ......", channel);
		result = DM35424_Dma_Start(board, &my_adc, channel);

		check_result(result, "Error starting DMA");

		printf("success.\n");

	}

	printf("Starting ADC 0\n");

	result = DM35424_Adc_Start(board, &my_adc);

	check_result(result, "Error starting ADC");

	buffers_copied = 0;

	/*
	 * Loop here until an error occurs, or the user hits CTRL-C.  Loop through the ADCs
	 * and see if a buffer has been copied from kernel space.  If so, then write it out
	 * to disk.
	 */
	while (!exit_program && output_index < samples_to_collect) {

		all_channels_copied = 1;
		for (channel = 0; channel < my_adc.num_dma_channels; channel++) {
			if ((buffer_count[channel] -
			     local_buffer_count[channel]) >
			    my_adc.num_dma_buffers) {
				fprintf(stdout,
					"Local buffer for ADC Chan %d was overrun.\n",
					channel);
				exit_program = 1;
			}

			/** Check to see if any channel has not yet been copied
			    from DMA. */
			if (local_buffer_count[channel] == buffer_count[channel]) {
				all_channels_copied = 0;
			}
		}

		if (exit_program) {
			// Exit out of the while loop
			break;
		}

		if (all_channels_copied) {

			buffer_to_get = local_buffer_count[0] % my_adc.num_dma_buffers;

			if (store_in_binary) {

				for (channel = 0; channel < my_adc.num_dma_channels; channel ++) {
					fwrite(local_buffer[channel][buffer_to_get],
							sizeof(int),
							(buffer_size_bytes / sizeof(int)),
							fp);

					// Send the data over UDP
					udp_send_data(local_buffer[channel][buffer_to_get], (buffer_size_bytes / sizeof(int)) * sizeof(int), 12345);
					
					bytes_written += buffer_size_bytes;
				}
				output_index += samples_in_buffer;
			}

			else {
				for (index = 0;
				     index < (buffer_size_bytes / sizeof(int));
				     index++) {
					fprintf(fp, "%lu", output_index);
					for (channel = 0; channel < my_adc.num_dma_channels; channel ++) {

						fprintf(fp, "\t%d",
							local_buffer[channel]
							[buffer_to_get][index]);

					}
					fprintf(fp, "\n");
					output_index++;

				}
			}
			buffers_copied += my_adc.num_dma_channels;

			if (buffers_copied % (10 * my_adc.num_dma_channels) == 0) {
				fprintf(stdout, "Copied %d buffers.\n",
					buffers_copied);
			}

			for (channel = 0; channel < my_adc.num_dma_channels; channel ++) {

				local_buffer_count[channel]++;
			}


		}

		DM35424_Micro_Sleep(100);


	}


	if (dma_has_error) {
		printf("\n** DMA Error **\n");
	}

	for (channel = 0; channel < my_adc.num_dma_channels; channel++) {

		if (dma_has_error) {
			output_channel_status(board,
					      &my_adc, channel);
		}

		result = DM35424_Dma_Configure_Interrupts(board,
							  &my_adc,
							  channel,
							  INTERRUPT_DISABLE,
							  ERROR_INTR_DISABLE);

		check_result(result, "Error setting DMA Interrupts");

		for (buff = 0; buff < my_adc.num_dma_buffers; buff++) {

			free(local_buffer[channel][buff]);

		}

	}
	fprintf(stdout, "\n\n");

	fprintf(stdout, "Copied %u buffers.\n", buffers_copied);

	if (output_index >= samples_to_collect) {
		fprintf(stdout, "Reached number of samples (%lu)\n", samples_to_collect);
	}

	if (store_in_binary) {
		fprintf(stdout, "Wrote %lu bytes to file.\n", bytes_written);
	}

	fclose(fp);

	printf("Removing ISR\n");
	result = DM35424_General_RemoveISR(board);

	check_result(result, "Error removing ISR.");

	printf("Closing Board\n");
	result = DM35424_Board_Close(board);

	check_result(result, "Error closing board.");

	printf("Example program successfully completed.\n");
	return 0;

}
