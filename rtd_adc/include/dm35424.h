/**
    @file

    @brief
        Defines for the DM35424 (Device-specific values)

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

    $Id: dm35424.h 90112 2015-07-15 20:05:32Z rgroner $
*/

#ifndef __DM35424_H__
#define __DM35424_H__

/**
 * @brief
 * DM35424 PCI vendor ID
 */

#define DM35424_PCI_VENDOR_ID	0x1435

/**
 * @brief
 *     DM35424 PCI device ID
 */
#define DM35424_PCI_DEVICE_ID	0x5424

/**
 * @brief
 *     Number of ADC on the DM35424
 */
#define DM35424_NUM_ADC_ON_BOARD 	2

/**
 * @brief
 *     Number of DAC on the DM35424
 */
#define DM35424_NUM_DAC_ON_BOARD 	4

/**
 * @brief
 *     Number of channels per ADC
 */
#define DM35424_NUM_ADC_DMA_CHANNELS	8

/**
 * @brief
 *     Number of buffers per ADC DMA channel
 */
#define DM35424_NUM_ADC_DMA_BUFFERS		7

/**
 * @brief
 *     Number of channels per DAC
 */
#define DM35424_NUM_DAC_DMA_CHANNELS	4

/**
 * @brief
 *     Number of buffers per DAC DMA channel
 */
#define DM35424_NUM_DAC_DMA_BUFFERS		7

/**
 * @brief
 *     Bit Mask for the gain bits of the FE Config
 */
#define DM35424_DAC_FE_CONFIG_GAIN_MASK		0

/**
 * @brief
 *     Sample size of the FIFO
 */
#define DM35424_FIFO_SAMPLE_SIZE		511

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
 *     Max rate of the DAC
 */
#define DM35424_DAC_MAX_RATE	106000

#endif
