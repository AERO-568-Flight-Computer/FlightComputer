/**
	@file

	@brief
		DM35424 Util library source code

	$Id: librtd-dm35424_util.c 107475 2017-03-28 15:17:54Z rgroner $
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

#include <sys/time.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <error.h>

#include "dm35424_util_library.h"

#define DM35424_ONE_SEC_IN_MICRO	1000000
#define DM35424_MICRO_TO_NANO(x)	((x) * 1000)
#define DM35424_SEC_TO_MICRO(x)	((x) * 1000000)


long DM35424_Get_Time_Diff(struct timeval last, struct timeval first) {
	long firstTime;
	long lastTime;

	lastTime = DM35424_SEC_TO_MICRO(last.tv_sec) + last.tv_usec;
	firstTime = DM35424_SEC_TO_MICRO(first.tv_sec) + first.tv_usec;

	return (lastTime - firstTime);
}


void DM35424_Micro_Sleep(unsigned long microsecs) {

	struct timespec sleep_time, temp_time;
	long seconds = microsecs / DM35424_ONE_SEC_IN_MICRO;

	microsecs = microsecs % DM35424_ONE_SEC_IN_MICRO;

	sleep_time.tv_sec = seconds;
	sleep_time.tv_nsec = DM35424_MICRO_TO_NANO(microsecs);

	nanosleep(&sleep_time, &temp_time);

}


uint32_t DM35424_Get_Maskable(uint16_t data, uint16_t mask)
{

	return (data << 16) | mask;
}


int DM35424_Generate_Signal_Data(enum DM35424_Waveforms waveform,
					int32_t *data,
					uint32_t data_count,
					int32_t max,
					int32_t minimum,
					int32_t offset,
					uint32_t mask)
{
	double value = 0.0;
	double increment = 0.0;
	unsigned int index = 0;
	int32_t center;

	if (data == NULL ||
		(data_count < 1)) {
		errno = EINVAL;
		return -1;
	}

	if (mask == 0) {
		mask = 0xFFFFFFFF;
	}


	switch (waveform) {
	case DM35424_SINE_WAVE:

		center = minimum + ((max - minimum) / 2);

		increment = (2.0 * M_PI) / ((double) data_count);

		for (index = 0; index < data_count; index ++) {

			data[index] = (int32_t) (sin(value) * ((double) (max - center))) + center;
			if (data[index] < minimum) {
				data[index] = minimum;
			}
			if (data[index] > max) {
				data[index] = max;
			}
			data[index] += offset;
			data[index] = data[index] & mask;
			value += increment;
		}

		break;
	case DM35424_SQUARE_WAVE:

		for (index = 0; index < (data_count / 2); index ++) {
			data[index] = max + offset;
			data[index] = data[index] & mask;

		}

		for (index = (data_count / 2); index < data_count; index ++) {
			data[index] = minimum + offset;
			data[index] = data[index] & mask;
		}
		break;

	case DM35424_SAWTOOTH_WAVE:
		increment = ((float) (max - minimum)) / (float) data_count;
		value = (float) minimum;

		for (index = 0; index < data_count; index ++) {
			data[index] = ((int32_t) value) + offset;
			data[index] = data[index] & mask;
			value += increment;
		}
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
    Check the result of an operation, usually a library call.  If the result is
    non-zero, then it is an error and output the passed message.

@param
    return_val

    Value to be evaluated.  Non-zero values will be considered an error.

@param
    message

    Pointer to string that will be output if an error condition exists.

@retval
    None

 *******************************************************************************
*/
void check_result(int return_val, char *message) {

	if (return_val != 0) {

        	error(EXIT_FAILURE, errno, "\n\nERROR(%d): %s", return_val, message);


    	}
}
