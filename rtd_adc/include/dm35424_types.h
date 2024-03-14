/**
    @file

    @brief
        Defines for the DM35424.  Values for the general board, not specific
        to a particular function block.

    $Id: dm35424_types.h 127189 2020-09-16 13:22:33Z lfrankenfield $
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

#ifndef __DM35424_TYPES_H__
#define __DM35424_TYPES_H__

/**
 * @defgroup DM35424_General_Definitions DM35424 Board Types
 * @{
 */

/**
 * @brief
 *     Constant for FB subtype 0
 */
#define DM35424_SUBTYPE_00	0

/**
 * @brief
 *     Constant for FB subtype 1
 */
#define DM35424_SUBTYPE_01	1

/**
 * @brief
 *     Constant for FB subtype 2
 */
#define DM35424_SUBTYPE_02	2

/**
 * @brief
 *     Constant for FB subtype 3
 */
#define DM35424_SUBTYPE_03	3


/**
* @brief
*       Constant value indicating an invalid subtype
*/
#define DM35424_SUBTYPE_INVALID 0xFF

/**
 * @brief
 *     Constant value indicating an invalid function block
 */
#define DM35424_FUNC_BLOCK_INVALID	0x0000

/**
 * @brief
 *     Constant value indicating an invalid function block
 */
#define DM35424_FUNC_BLOCK_INVALID2	0xFFFF

/**
 * @brief
 *     Function Block Constant for SyncBus
 */
#define DM35424_FUNC_BLOCK_SYNCBUS			0x0001

/**
 * @brief
 *     Function Block Constant for Global Clocking
 */
#define DM35424_FUNC_BLOCK_EXT_CLOCKING		0x0002

/**
 * @brief
 *     Function Block Constant for External Clocking (0003)
 */
#define DM35424_FUNC_BLOCK_CLK0003			0x0003

/**
 * @brief
 * 	Function Block Constant for Capture Window
 */
#define DM35424_FUNC_BLOCK_CAPTWIN		0x0005

/**
 * @brief
 *     Function Block Constant for ADC
 */
#define DM35424_FUNC_BLOCK_ADC			0x1000

/**
 * @brief
 *     Function Block Constant for 10 MHz ADC (1001)
 */
#define DM35424_FUNC_BLOCK_ADC1001			0x1001

/**
 * @brief
 *     Function Block Constant for DAC
 */
#define DM35424_FUNC_BLOCK_DAC			0x2000

/**
 * @brief
 *     Function Block Constant for High Speed DAC (2001)
 */
#define DM35424_FUNC_BLOCK_DAC2001                      0x2001

/**
 * @brief
 *     Function Block Constant for DIO
 */
#define DM35424_FUNC_BLOCK_DIO			0x3000

/**
 * @brief
 *     Function Block Constant for ADIO
 */
#define DM35424_FUNC_BLOCK_ADIO			0x3001

/**
 * @brief
 * 	Function Block Constant for ADIO3010
 */
#define DM35424_FUNC_BLOCK_ADIO3010			0x3010

/**
 * @brief
 *     Function Block Constant for Synchronous/Asynchronous Serial Port
 */
#define DM35424_FUNC_BLOCK_USART			0x4000

/**
 * @brief
 *     Function Block Constant for Reference Adjustment
 */
#define DM35424_FUNC_BLOCK_REF_ADJUST		0xF000

/**
 * @brief
 *     Function Block Constant for Temperature Sensor
 */
#define DM35424_FUNC_BLOCK_TEMPERATURE_SENSOR	0xF001

/**
 * @brief
 *     Function Block Constant for Flash Programmer
 */
#define DM35424_FUNC_BLOCK_FLASH_PROGRAMMER		0xF002

/**
 * @brief
 * 	Function Block Constant for Clock Generator
 */
#define DM35424_FUNC_BLOCK_CLK_GEN			0xF003

/**
 * @brief
 *     Function Block Constant for Digital Input (3011)
 */
#define DM35424_FUNC_BLOCK_DIN3011       	0x3011

/**
 * @brief
 *     Function Block Constant for Digital Output (3012)
 */
#define DM35424_FUNC_BLOCK_DOT3012       	0x3012

/**
 * @brief
 *     Function Block Constant for Incremental Encoder (3200)
 */
#define DM35424_FUNC_BLOCK_INC3200       	0x3200

/**
 * @brief
 *     Function Block Constant for PWM (3100)
 */
#define DM35424_FUNC_BLOCK_PWM3100             0x3100

/**
 * @brief
 *     Function Block Constant for Programmable Clock (0004)
 */
#define DM35424_FUNC_BLOCK_CLK0004       	0x0004

/**
 * @brief
 *     Maximum possible number of function blocks on a board.
 */
#define DM35424_MAX_FB		62

/**
 * @brief
 *    Maximum possible number of DMA buffers for any function block.
 */
#define MAX_DMA_BUFFERS		16

/**
 * @brief
 *    Maximum possible number of DMA channels for any function block.
 */
#define MAX_DMA_CHANNELS	32


/**
 * @brief
 *     Maximum possible DMA buffer size
 */
#define DM35424_DMA_MAX_BUFFER_SIZE			0xFFFFFC


/**
 * @brief
 *      Value to write to the EOI register to acknowledge interrupts.
 */
#define DM35424_BOARD_ACK_INTERRUPT	0x1

/**
 * @brief
 *      Value to write to the Reset register in order to reset the board.
 */
#define DM35424_BOARD_RESET_VALUE	0xAA

/**
 * @brief
 *      Minimum function block revision that supports direct FIFO
 *      read/write access.
 */
#define DM35424_FIFO_ACCESS_FB_REVISION 0x01

/**
 * @} DM35424_General_Definitions
 */


/**
 * @brief
 * Possible clock sources used by function blocks.  Note that some clock sources
 * may not be available on your particular board.  Check the hardware manual
 * to verify which clock sources can be used.
 */
enum DM35424_Clock_Sources {

	/**
	 *      Clock Source - Immediate (0x00)
	 */
	DM35424_CLK_SRC_IMMEDIATE,

	/**
	 *      Clock Source - Never (0x01)
	 */
	DM35424_CLK_SRC_NEVER,

	/**
	 *      Clock Source - Bus 2 (0x02)
	 */
	DM35424_CLK_SRC_BUS2,

	/**
	 *      Clock Source - Bus 3 (0x03)
	 */
	DM35424_CLK_SRC_BUS3,

	/**
	 *      Clock Source - Bus 4 (0x04)
	 */
	DM35424_CLK_SRC_BUS4,

	/**
	 *      Clock Source - Bus 5 (0x05)
	 */
	DM35424_CLK_SRC_BUS5,

	/**
	 *      Clock Source - Bus 6 (0x06)
	 */
	DM35424_CLK_SRC_BUS6,

	/**
	 *      Clock Source - Bus 7 (0x07)
	 */
	DM35424_CLK_SRC_BUS7,

	/**
	 *      Clock Source - Threshold Exceeded (0x08)
	 */
	DM35424_CLK_SRC_CHAN_THRESH = 0x08,

	/**
	 *      Clock Source - Threshold Inverse (None Exceeded) (0x09)
	 */
	DM35424_CLK_SRC_CHAN_THRESH_INV = 0x09,

	/**
	 *      Clock Source - Bus 2 Inverse (0x0A)
	 */
	DM35424_CLK_SRC_BUS2_INV = 0x0A,

	/**
	 *      Clock Source - Bus 3 Inverse (0x0B)
	 */
	DM35424_CLK_SRC_BUS3_INV,

	/**
	 *      Clock Source - Bus 4 Inverse (0x0C)
	 */
	DM35424_CLK_SRC_BUS4_INV,

	/**
	 *      Clock Source - Bus 5 Inverse (0x0D)
	 */
	DM35424_CLK_SRC_BUS5_INV,

	/**
	 *      Clock Source - Bus 6 Inverse (0x0E)
	 */
	DM35424_CLK_SRC_BUS6_INV,

	/**
	 *      Clock Source - Bus 7 Inverse (0x0F)
	 */
	DM35424_CLK_SRC_BUS7_INV

};

/**
 * @brief
 * Clock buses available to the function block.
 */
enum DM35424_Clock_Buses {

	/**
	 *  @brief
	 *  Clock Bus 2
	 */
	DM35424_CLK_BUS2 = 2,

	/**
	 *  @brief
	 *  Clock Bus 3
	 */
	DM35424_CLK_BUS3,

	/**
	 *  @brief
	 *  Clock Bus 4
	 */
	DM35424_CLK_BUS4,

	/**
	 *  @brief
	 *  Clock Bus 5
	 */
	DM35424_CLK_BUS5,

	/**
	 *  @brief
	 *  Clock Bus 6
	 */
	DM35424_CLK_BUS6,

	/**
	 *  @brief
	 *  Clock Bus 7
	 */
	DM35424_CLK_BUS7

};

/**
 */

#endif
