/**
    @file

    @brief
        Defines for the DM35424 Example programs.  Commonly used constants
        for the example programs included with the software package.


    $Id: dm35424_examples.h 114740 2018-07-12 14:41:17Z prucz $
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


#ifndef __DM35424_EXAMPLES_H__
#define __DM35424_EXAMPLES_H__

 /**
  * @defgroup DM35424_Example_Constants DM35424 Example Programs Constants
  * @{
  */

/**
  @brief
  	Constants used for parsing command line parameters of example programs.

  @note
 *	This value won't be seen by the user (except in code), so the value
 *	can be used for any desired option.
 */
enum Help_Options {
	/**
	 * @brief
	 *   Command line parameter --help
	 */
	HELP_OPTION = 1,

	/**
	 * @brief
	 *   Command line parameter --minor
	 */
	MINOR_OPTION,

	/**
	 * @brief
	 *   Command line parameter --rate
	 */
	RATE_OPTION,

	/**
	 * @brief
	 *   Command line parameter --chan
	 */
	CHANNELS_OPTION,

	/**
	 * @brief
	 *   Command line parameter for including a file
	 */
	FILE_OPTION,

	/**
	 * @brief
	 *   Command line parameter --start
	 */
	START_OPTION,

	/**
	 * @brief
	 *   Command line parameter --wave
	 */
	WAVE_OPTION,

	/**
	 * @brief
	 *   Command line parameter --test
	 */
	TEST_OPTION,

	/**
	 * @brief
	 *   Command line parameter --nostop
	 */
	NOSTOP_OPTION,

	/**
	 * @brief
	 *   Command line parameter --syncbus
	 */
	SYNCBUS_OPTION,

	/**
	 * @brief
	 *   Command line parameter --dump
	 */
	DUMP_OPTION,

	/**
	 * @brief
	 *   Command line parameter --hours
	 */
	HOURS_OPTION,

	/**
	 * @brief
	 *   Command line parameter --output_rms
	 */
	OUTPUT_RMS_OPTION,

	/**
	 * @brief
	 *   Command line parameter --output_adc
	 */
	OUTPUT_ADC_OPTION,

	/**
	 * @brief
	 *   Command line parameter --num_adc
	 */
	ADC_NUM_OPTION,

	/**
	 * @brief
	 *   Command line parameter --num_dac
	 */
	DAC_NUM_OPTION,

	/**
	 * @brief
	 *   Command line parameter --adc
	 */
	ADC_OPTION,

	/**
	 * @brief
	 *   Command line parameter --dac
	 */
	DAC_OPTION,

	/**
	 * @brief
	 *   Command line parameter --pattern
	 */
	PATTERN_OPTION,

	/**
	 * @brief
	 *   Command line parameter --samples
	 */
	SAMPLES_OPTION,

	/**
	 * @brief
	 *   Command line parameter --mode
	 */
	MODE_OPTION,

	/**
	 * @brief
	 *   Command line parameter --ad_mode
	 */
	AD_MODE_OPTION,

	/**
	 * @brief
	 *   Command line parameter --ref
	 */
	REF_NUM_OPTION,

	/**
	 * @brief
	 *   Command line parameter --binary
	 */
	BINARY_OPTION,

	/**
	 * @brief
	 *   Command line parameter --sender
	 */
	SENDER_OPTION,

	/**
	 * @brief
	 *   Command line parameter --receiver
	 */
	RECEIVER_OPTION,

	/**
	 * @brief
	 *   Command line parameter --range
	 */
	RANGE_OPTION,

	/**
	 * @brief
	 *   Command line parameter --refill
	 */
	REFILL_FIFO_OPTION,

	/**
	 * @brief
	 *   Command line parameter --low
	 */
	LOW_THRESHOLD_OPTION,

	/**
	 * @brief
	 *   Command line parameter --port
	 */
	 PORT_OPTION,

	/**
	 * @brief
	 *   Command line parameter --baud
	 */
	 BAUD_OPTION,

	/**
	 * @brief
	 *   Command line parameter --external
	 */
	 EXTERNAL_OPTION,

	/**
	 * @brief
	 *   Command line parameter --size
	 */
	 SIZE_OPTION,

	/**
	 * @brief
	 *   Command line parameter --verbose
	 */
	 VERBOSE_OPTION,

	 /**
	  * @brief
	  *  Command line parameter --userid
	  */
	 USER_ID_OPTION,

	 /**
	  * @brief
	  *  Command line parameter --count
	  */
	 COUNT_OPTION,

	 /**
	  * @brief
	  *  Command line parameter --num
	  */
	 NUM_OPTION,

	 /**
	  * @brief
	  *  Command line parameter --syncterm
	  */
	 SYNC_TERM_OPTION,

	 /**
	  * @brief
	  * Command line parameter --bin2txt
	  */
	 BIN2TXT_OPTION,

	 /**
	  * @brief
	  *   Command line parameter --store
	  */
	 STORE_OPTION,
	 
	 /**
	  * @brief
	  *   Command line parameter --term
	  *   (Termination)
	  */
	 TERM_OPTION,
	
	 /**
	  * @brief
	  *   Command line parameter --refclk 
	  *   (Reference clock selection)
	  */
	 REFCLK_OPTION,
    
	 /**
	  * @brief
	  *   Command line parameter --ofile 
	  *   (Output file selection)
	  */
	 OFILE_OPTION,
	 
	 /**
	 * @brief
	 *   Command line parameter --packed
	 *   (Packed, 16-bit samples selection)
	 */
	PACKED_OPTION,
	
	/**
	 * @brief
	 * 	Master minor option for synchronization.
	 */
	MASTER_OPTION,
	
	/**
	 * @brief
	 * 	Slave minor option for synchronization.
	 */
	SLAVE_OPTION,
	
	/**
	 * @brief
	 * 	Syncbus connector option
	 */
	SYNC_CONN_OPTION,
};

/**
 * @brief
 *   Boolean indicating buffer valid
 */
#define BUFFER_VALID		1

/**
 * @brief
 *   Boolean indicating buffer not valid
 */
#define BUFFER_NO_VALID		0

/**
 * @brief
 *   Boolean indicating buffer halt set
 */
#define BUFFER_HALT		1

/**
 * @brief
 *   Boolean indicating buffer halt not set
 */
#define BUFFER_NO_HALT		0

/**
 * @brief
 *   Boolean indicating buffer loop set
 */
#define BUFFER_LOOP		1

/**
 * @brief
 *   Boolean indicating buffer loop not set
 */
#define BUFFER_NO_LOOP		0

/**
 * @brief
 *   Boolean indicating buffer interrupt
 */
#define BUFFER_INTERRUPT	1

/**
 * @brief
 *   Boolean indicating no buffer interrupt
 */
#define BUFFER_NO_INTERRUPT	0

/**
 * @brief
 *   Boolean indicating buffer should pause when filled.
 */
#define BUFFER_PAUSE        1

/**
 * @brief
 *   Boolean indicating buffer should not pause when filled.
 */
#define BUFFER_NO_PAUSE     0

/**
 * @brief
 *   Boolean indicating ignore used buffers
 */
#define IGNORE_USED		1

/**
 * @brief
 *   Boolean indicating not ignore used buffers
 */
#define NOT_IGNORE_USED		0

/**
 * @brief
 *   Boolean indicating to clear an interrupt
 */
#define CLEAR_INTERRUPT		1

/**
 * @brief
 *   Boolean indicating to not clear an interrupt
 */
#define NO_CLEAR_INTERRUPT	0

/**
 * @brief
 *   Boolean indicating interrupt enable
 */
#define INTERRUPT_ENABLE	1

/**
 * @brief
 *   Boolean indicating interrupt disable
 */
#define INTERRUPT_DISABLE	0

/**
 * @brief
 *   Boolean indicating error interrupt enable
 */
#define ERROR_INTR_ENABLE	1

/**
 * @brief
 *   Boolean indicating error interrupt disable
 */
#define ERROR_INTR_DISABLE	0

/**
 * @brief
 *   Value indicating no Syncbus option was chosen
 */
#define SYNCBUS_NONE	0

/**
 * @brief
 *   Value indicating Syncbus Master was chosen
 */
#define SYNCBUS_MASTER	1

/**
 * @brief
 *   Value indicating Syncbus Slave was chosen
 */
#define SYNCBUS_SLAVE	2

/**
 * @brief
 *   Constant for selecting Channel 0
 */
#define CHANNEL_0	0

/**
 * @brief
 *   Constant for selecting Channel 1
 */
#define CHANNEL_1	1

/**
 * @brief
 *   Constant for selecting Channel 2
 */
#define CHANNEL_2	2

/**
 * @brief
 *   Constant for selecting Channel 3
 */
#define CHANNEL_3	3

/**
 * @brief
 *   Constant for selecting Buffer 0
 */
#define BUFFER_0	0

/**
 * @brief
 *   Constant for selecting Buffer 1
 */
#define BUFFER_1	1

/**
 * @brief
 *   Constant for selecting ADC 0
 */
#define ADC_0	0

/**
 * @brief
 *   Constant for selecting ADC 1
 */
#define ADC_1	1

/**
 * @brief
 *   Constant for selecting DAC 0
 */
#define DAC_0	0

/**
 * @brief
 *   Constant for selecting DAC 1
 */
#define DAC_1	1

/**
 * @brief
 *   Constant for selecting DAC 2
 */
#define DAC_2	2

/**
 * @brief
 *   Constant for selecting DAC 3
 */
#define DAC_3	3

/**
 * @brief
 *   Constant for selecting REF 0
 */
#define REF_0	0

/**
 * @brief
 *   Constant for selecting REF 1
 */
#define REF_1	1

/**
 * @brief
 *   Constant for selecting DIO 0
 */
#define DIO_0	0

/**
 * @brief
 *   Constant for selecting ADIO 0
 */
#define ADIO_0	0

/**
 * @brief
 *   Constant to indicate an Enabled value
 */
#define ENABLED		1

/**
 * @brief
 *   Constant to indicate a Disabled value
 */
#define DISABLED	0


 /**
  * @} DM35424_Example_Constants
  */
#endif
