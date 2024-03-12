/**
    @file

    @brief
        Defines for the DM35424 Registers (Offsets)

    $Id: dm35424_registers.h 124951 2020-03-05 16:34:24Z lfrankenfield $
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

#ifndef __DM35424_REGISTERS_H__
#define __DM35424_REGISTERS_H__


 /**
  * @defgroup DM35424_Register_Offsets DM35424 Register Offsets
  * @{
  */

/******************************************************************
 *  General Board Control (BAR0)
 ******************************************************************/
/**
 * @brief
 *     Offset to General Board Control (BAR0) Format ID register
 */
#define DM35424_OFFSET_GBC_FORMAT			0x00

/**
 * @brief
 *     Offset to General Board Control (BAR0) Format ID register
 */
#define DM35424_OFFSET_GBC_REV			0x01

/**
 * @brief
 *     Offset to General Board Control (BAR0) EOI (End of Interrupt) register
 */
#define DM35424_OFFSET_GBC_END_INTERRUPT		0x02

/**
 * @brief
 *     Offset to General Board Control (BAR0) Board Reset register
 */
#define DM35424_OFFSET_GBC_BOARD_RESET		0x03

/**
 * @brief
 *     Offset to General Board Control (BAR0) PDP Number register
 */
#define DM35424_OFFSET_GBC_PDP_NUMBER		0x04

/**
 * @brief
 *     Offset to General Board Control (BAR0) FPGA Build register
 */
#define DM35424_OFFSET_GBC_FPGA_BUILD		0x08

/**
 * @brief
 *     Offset to General Board Control (BAR0) System Clock register
 */
#define DM35424_OFFSET_GBC_SYS_CLK_FREQ		0x0c


/**
 * @brief
 *     Offset to General Board Control (BAR0) IRQ Status register.  Each bit
 *     corresponds to a function block.
 */
#define DM35424_OFFSET_GBC_IRQ_STATUS		0x10

/**
 * @brief
 *     Offset to General Board Control (BAR0) DMA IRQ Status register.  Each bit
 *     corresponds to a function block.
 */
#define DM35424_OFFSET_GBC_DMA_IRQ_STATUS		0x18

/**
 * @brief
 *     Offset to the beginning of the Function Blocks section of the GBC.
 */
#define DM35424_OFFSET_GBC_FB_START			0x20

/**
 * @brief
 *     Size of the function block entries in the GBC
 */
#define DM35424_GBC_FB_BLK_SIZE			0x10

/**
 * @brief
 *     Offset to Function Block ID, from the start of the function block
 *     section.
 */
#define DM35424_OFFSET_GBC_FB_ID			0x00

/**
 * @brief
 *     Bit mask for TYPE portion of FB ID
 */
#define DM35424_FB_ID_TYPE_MASK		0x0000FFFF

/**
 * @brief
 *     Bit mask for SUBTYPE portion of FB ID
 */
#define DM35424_FB_ID_SUBTYPE_MASK		0x00FF0000

/**
 * @brief
 *     Bit mask for TYPE REV portion of FB ID
 */
#define DM35424_FB_ID_TYPE_REV_MASK		0xFF000000

/**
 * @brief
 *     Offset to the FB Offset in the GBC, from the start of the
 *     FB data block.
 */
#define DM35424_OFFSET_GBC_FB_OFFSET		0x04

/**
 * @brief
 *     Offset to the FB DMA Offset in the GBC, from the start of the
 *     FB data block.
 */
#define DM35424_OFFSET_GBC_FB_DMA_OFFSET		0x08


/******************************************************************
 *  DMA Control (BAR2)
 ******************************************************************/
/**
 * @brief
 *     Offset to the DMA Action Register (BAR2)
 */
#define DM35424_OFFSET_DMA_ACTION		0x00

/**
 * @brief
 *     Offset to the DMA Setup Register (BAR2)
 */
#define DM35424_OFFSET_DMA_SETUP		0x01

/**
 * @brief
 *     Offset to the DMA Status (Overflow) Register (BAR2)
 */
#define DM35424_OFFSET_DMA_STAT_OVERFLOW	0x02

/**
 * @brief
 *     Offset to the DMA Status (Underflow) Register (BAR2)
 */
#define DM35424_OFFSET_DMA_STAT_UNDERFLOW	0x03

/**
 * @brief
 *     Offset to the DMA Current Count Register (BAR2)
 */
#define DM35424_OFFSET_DMA_CURRENT_COUNT	0x04

/**
 * @brief
 *     Offset to the DMA Current Buffer Register (BAR2)
 */
#define DM35424_OFFSET_DMA_CURRENT_BUFFER	0x07

/**
 * @brief
 *     Offset to the DMA Write FIFO Count Register (BAR2)
 */
#define DM35424_OFFSET_DMA_WR_FIFO_CNT	0x08

/**
 * @brief
 *     Offset to the DMA Read FIFO Count Register (BAR2)
 */
#define DM35424_OFFSET_DMA_RD_FIFO_CNT	0x0A

/**
 * @brief
 *     Offset to the DMA Status (Used) Register (BAR2)
 */
#define DM35424_OFFSET_DMA_STAT_USED		0x0C

/**
 * @brief
 *     Offset to the DMA Status (Invalid) Register (BAR2)
 */
#define DM35424_OFFSET_DMA_STAT_INVALID		0x0D

/**
 * @brief
 *     Offset to the DMA Status (Complete) Register (BAR2)
 */
#define DM35424_OFFSET_DMA_STAT_COMPLETE		0x0E

/**
 * @brief
 *     Offset to the DMA Last Action Register (BAR2)
 */
#define DM35424_OFFSET_DMA_LAST_ACTION		0x0F

/**
 * @brief
 *     Offset to the start of the buffer control section (BAR2)
 */
#define DM35424_OFFSET_DMA_BUFF_START		0x10

/**
 * @brief
 *     Offset to the buffer status register, from the start of the buffer
 *     control section (BAR2)
 */
#define DM35424_OFFSET_DMA_BUFFER_STAT		0x02

/**
 * @brief
 *     Offset to the buffer control register, from the start of the buffer
 *     control section (BAR2)
 */
#define DM35424_OFFSET_DMA_BUFFER_CTRL		0x03

/**
 * @brief
 *     Offset to the buffer size register, from the start of the buffer
 *     control section (BAR2)
 */
#define DM35424_OFFSET_DMA_BUFFER_SIZE		0x04

/**
 * @brief
 *     Offset to the buffer address register, from the start of the buffer
 *     control section (BAR2)
 */
#define DM35424_OFFSET_DMA_BUFFER_ADDRESS	0x08


/******************************************************************
 *  All Function Blocks (BAR2)
 ******************************************************************/
/**
 * @brief
 *     Offset to the DMA Channels count of the function block (BAR2)
 */
#define DM35424_OFFSET_FB_DMA_CHANNELS	0x06

/**
 * @brief
 *     Offset to the DMA buffers count of the function block (BAR2)
 */
#define DM35424_OFFSET_FB_DMA_BUFFERS	0x07

/**
 * @brief
 *     Offset to the beginning of the Function Block control section in BAR2.
 */
#define DM35424_OFFSET_FB_CTRL_START	0x08

/******************************************************************
 *  ADC Function Blocks (BAR2)
 ******************************************************************/
/**
 * @brief
 *     Offset to the ADC Mode-Status register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_MODE_STATUS		0x00

/**
 * @brief
 *     Offset to the ADC Clock Source register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_CLK_SRC		0x01

/**
 * @brief
 *     Offset to the ADC Start Trigger register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_START_TRIG		0x02

/**
 * @brief
 *     Offset to the ADC Stop Trigger register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_STOP_TRIG		0x03

/**
 * @brief
 *     Offset to the ADC Clock Divider register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_CLK_DIV		0x04

/**
 * @brief
 *     Offset to the ADC Clock Divider Counter register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_CLK_DIV_COUNTER	0x08

/**
 * @brief
 *     Offset to the ADC Pre-Start Capture Count register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_PRE_CAPT_COUNT	0x0c

/**
 * @brief
 *     Offset to the ADC Post-Stop Capture Count register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_POST_CAPT_COUNT	0x10

/**
 * @brief
 *     Offset to the ADC Sample Count register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_SAMPLE_COUNT		0x14

/**
 * @brief
 *     Offset to the ADC Interrupt Enable register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_INT_ENABLE		0x18

/**
 * @brief
 *     Offset to the ADC Interrupt Status register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_INT_STAT		0x1e

/**
 * @brief
 *     Offset to the ADC Clock Bus 2, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_CLK_BUS2		0x22

/**
 * @brief
 *     Offset to the ADC Clock Bus 3 register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_CLK_BUS3		0x23

/**
 * @brief
 *     Offset to the ADC Clock Bus 4 register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_CLK_BUS4		0x24

/**
 * @brief
 *     Offset to the ADC Clock Bus 5 register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_CLK_BUS5		0x25

/**
 * @brief
 *     Offset to the ADC Clock Bus 6 register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_CLK_BUS6		0x26

/**
 * @brief
 *     Offset to the ADC Clock Bus 7 register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_CLK_BUS7		0x27

/**
 * @brief
 *     Offset to the ADC AD Config register, from the start of the ADC
 *     control section.
 */
#define DM35424_OFFSET_ADC_AD_CONFIG		0x28

/**
 * @brief
 *     Offset to the start of the Channel Control Section, from
 *     the start of the ADC control section.
 */
#define DM35424_OFFSET_ADC_CHAN_CTRL_BLK_START		0x2c

/**
 * @brief
 *      Constant size of ADC channel section in function block.
 */
#define DM35424_ADC_CHAN_CTRL_BLK_SIZE	0x18

/**
 * @brief
 *     Offset to the Channel Front End Config register, from the
 *     start of the ADC channel control section.
 */
#define DM35424_OFFSET_ADC_CHAN_FRONT_END_CONFIG	0x00

/**
 * @brief
 *     Offset to the Channel FIFO Data count register, from the
 *     start of the ADC channel control section.
 */
#define DM35424_OFFSET_ADC_CHAN_DATA_COUNT		0x04

/**
 * @brief
 *     Offset to the Channel Filter register, from the
 *     start of the ADC channel control section.
 */
#define DM35424_OFFSET_ADC_CHAN_FILTER		0x09

/**
 * @brief
 *     Offset to the Channel Interrupt Status register, from the
 *     start of the ADC channel control section.
 */
#define DM35424_OFFSET_ADC_CHAN_INTR_STAT		0x0a

/**
 * @brief
 *     Offset to the Channel Interrupt Enable register, from the
 *     start of the ADC channel control section.
 */
#define DM35424_OFFSET_ADC_CHAN_INTR_ENABLE		0x0b

/**
 * @brief
 *     Offset to the Channel Low Threshold register, from the
 *     start of the ADC channel control section.
 */
#define DM35424_OFFSET_ADC_CHAN_LOW_THRESHOLD	0x0c

/**
 * @brief
 *     Offset to the Channel High Threshold register, from the
 *     start of the ADC channel control section.
 */
#define DM35424_OFFSET_ADC_CHAN_HIGH_THRESHOLD	0x10

/**
 * @brief
 *     Offset to the Channel Last Sample register, from the
 *     start of the ADC channel control section.
 */
#define DM35424_OFFSET_ADC_CHAN_LAST_SAMPLE		0x14

/**
 * @brief
 *     Offset to the start of the FIFO Control Section, from
 *     the start of the ADC control section.
 */
#define DM35424_OFFSET_ADC_FIFO_CTRL_BLK_START	0x334

/**
 * @brief
 *      Constant size of ADC FIFO section in function block.
 */
#define DM35424_ADC_FIFO_CTRL_BLK_SIZE	0x4

/**
 * @brief
 *      Offset to the FIFO for non-DMA read and write operations.
 *
 * @note
 *      This value should be used directly. It is used in conjunction
 *      with a channel number.
 */
#define DM35424_OFFSET_FB_ADC_FIFO              0x0334




/******************************************************************
 *  DAC Function Blocks (BAR2)
 ******************************************************************/

/**
 * @brief
 *     Offset to the Mode/Status register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_MODE_STATUS	0x00

/**
 * @brief
 *     Offset to the Clock Source register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CLK_SRC		0x01

/**
 * @brief
 *     Offset to the Start Trigger register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_START_TRIG	0x02

/**
 * @brief
 *     Offset to the Stop Trigger register, from the
 *     start of the DAC control section.
		 */
#define DM35424_OFFSET_DAC_STOP_TRIG	0x03

/**
 * @brief
 *     Offset to the Clock Divider register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CLK_DIV		0x04

/**
 * @brief
 *     Offset to the Clock Divider Counter register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CLK_DIV_COUNT	0x08

/**
 * @brief
 *     Offset to the Post-Stop Conversion Count register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_POST_STOP_CONV	0x10

/**
 * @brief
 *     Offset to the Conversion Count register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CONV_COUNT	0x14

/**
 * @brief
 *     Offset to the Interrupt Enable register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_INT_ENABLE	0x18

/**
 * @brief
 *     Offset to the Interrupt Status register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_INT_STAT		0x1e

/**
 * @brief
 *     Offset to the Clock Bus 2 register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CLK_BUS2	0x22

/**
 * @brief
 *     Offset to the Clock Bus 3 register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CLK_BUS3	0x23

/**
 * @brief
 *     Offset to the Clock Bus 4 register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CLK_BUS4	0x24

/**
 * @brief
 *     Offset to the Clock Bus 5 register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CLK_BUS5	0x25

/**
 * @brief
 *     Offset to the Clock Bus 6 register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CLK_BUS6	0x26

/**
 * @brief
 *     Offset to the Clock Bus 7 register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CLK_BUS7	0x27

/**
 * @brief
 *     Offset to the DA Config register, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_DA_CONFIG	0x28

/**
 * @brief
 *     Offset to the start of the DAC channel control section, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_CHAN_CTRL_BLK_START	0x2c

/**
 * @brief
 *      Constant size of channel control section in function block.
 */
 #define DM35424_DAC_CHAN_CTRL_BLK_SIZE		0x14

/**
 * @brief
 *     Offset to the Front-End Config register, from the
 *     start of the DAC channel control section.
 */
#define DM35424_OFFSET_DAC_CHAN_FRONT_END_CONFIG	0x00

/**
 * @brief
 *     Offset to the Channel marker Interrupt Status register, from the
 *     start of the DAC channel control section.
 */
#define DM35424_OFFSET_DAC_CHAN_MARKER_STATUS	0x0a

/**
 * @brief
 *     Offset to the Channel marker Interrupt Enable register, from the
 *     start of the DAC channel control section.
 */
#define DM35424_OFFSET_DAC_CHAN_MARKER_ENABLE	0x0b

/**
 * @brief
 *     Offset to the Channel Last Conversion register, from the
 *     start of the DAC channel control section.
 */
#define DM35424_OFFSET_DAC_CHAN_LAST_CONVERSION	0x10

/**
 * @brief
 *     Offset to the start of the DAC FIFO control section, from the
 *     start of the DAC control section.
 */
#define DM35424_OFFSET_DAC_FIFO_CTRL_BLK_START	0x84

/**
 * @brief
 *      Constant size of FIFO control section in function block.
 */
 #define DM35424_OFFSET_DAC_FIFO_CTRL_BLK_SIZE	0x4



/******************************************************************
 *  DIO Function Blocks (BAR2)
 ******************************************************************/

/**
 * @brief
 *     Offset to the Input Value register, from the start of
 *     the DIO control section.
 */
#define DM35424_OFFSET_DIO_INPUT_VAL		0x00

/**
 * @brief
 *     Offset to the Output Value register, from the start of
 *     the DIO control section.
 */
#define DM35424_OFFSET_DIO_OUTPUT_VAL		0x04

/**
 * @brief
 *     Offset to the Direction register, from the start of
 *     the DIO control section.
 */
#define DM35424_OFFSET_DIO_DIRECTION		0x08





/******************************************************************
 *  Temperature Function Blocks (BAR2)
 ******************************************************************/

/**
 * @brief
 *     Offset to the Temperature register, from the start of
 *     the Temperature control section.
 */
#define DM35424_OFFSET_TEMPERATURE			0x00




/**
 * @brief
 *     Offset to the Go/Busy register, from the start of the Reference
 *     Adjustment control section.
 */
#define DM35424_OFFSET_REF_ADJUST_GO_BUSY	0x00

/**
 * @brief
 *     Offset to the output latch register, from the start of the Reference
 *     Adjustment control section.
 */
#define DM35424_OFFSET_REF_OUTPUT_LATCH	0x04












/**
 * @} DM35424_Register_Offsets
 */

#endif
