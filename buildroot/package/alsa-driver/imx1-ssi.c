/*
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*!
 * @file ssi.c
 * @brief This file contains the implementation of the SSI driver main services
 *
 *
 * @ingroup SSI
 */
#include <linux/config.h>

#ifndef CONFIG_MACH_APF9328
# error "ERROR"
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
/*#include <asm/arch/clock.h>*/

/*#include "registers.h"*/
#include "imx-ssi_registers.h"
#include <asm/arch/hardware.h>
#include "imx1-ssi.h"

static spinlock_t ssi_lock;

EXPORT_SYMBOL(ssi_clock_off);
EXPORT_SYMBOL(ssi_enable);
EXPORT_SYMBOL(ssi_get_data);
EXPORT_SYMBOL(ssi_get_status);
EXPORT_SYMBOL(ssi_i2s_mode);
EXPORT_SYMBOL(ssi_interrupt_disable);
EXPORT_SYMBOL(ssi_interrupt_enable);
EXPORT_SYMBOL(ssi_network_mode);
EXPORT_SYMBOL(ssi_receive_enable);
EXPORT_SYMBOL(ssi_rx_bit0);
EXPORT_SYMBOL(ssi_rx_clock_direction);
/*EXPORT_SYMBOL(ssi_rx_clock_divide_by_two);*/
EXPORT_SYMBOL(ssi_rx_clock_polarity);
EXPORT_SYMBOL(ssi_rx_clock_prescaler);
EXPORT_SYMBOL(ssi_rx_early_frame_sync);
EXPORT_SYMBOL(ssi_rx_fifo_counter);
EXPORT_SYMBOL(ssi_rx_fifo_enable);
EXPORT_SYMBOL(ssi_rx_fifo_full_watermark);
EXPORT_SYMBOL(ssi_rx_flush_fifo);
EXPORT_SYMBOL(ssi_rx_frame_direction);
EXPORT_SYMBOL(ssi_rx_frame_rate);
EXPORT_SYMBOL(ssi_rx_frame_sync_active);
EXPORT_SYMBOL(ssi_rx_frame_sync_length);
/*EXPORT_SYMBOL(ssi_rx_mask_time_slot);*/
EXPORT_SYMBOL(ssi_rx_prescaler_modulus);
EXPORT_SYMBOL(ssi_rx_shift_direction);
EXPORT_SYMBOL(ssi_rx_word_length);
EXPORT_SYMBOL(ssi_set_data);
/*EXPORT_SYMBOL(ssi_set_wait_states);*/
EXPORT_SYMBOL(ssi_synchronous_mode);
EXPORT_SYMBOL(ssi_system_clock);
EXPORT_SYMBOL(ssi_transmit_enable);
/*EXPORT_SYMBOL(ssi_two_channel_mode);*/
EXPORT_SYMBOL(ssi_tx_bit0);
EXPORT_SYMBOL(ssi_tx_clock_direction);
/*EXPORT_SYMBOL(ssi_tx_clock_divide_by_two);*/
EXPORT_SYMBOL(ssi_tx_clock_polarity);
EXPORT_SYMBOL(ssi_tx_clock_prescaler);
EXPORT_SYMBOL(ssi_tx_early_frame_sync);
EXPORT_SYMBOL(ssi_tx_fifo_counter);
EXPORT_SYMBOL(ssi_tx_fifo_empty_watermark);
EXPORT_SYMBOL(ssi_tx_fifo_enable);
EXPORT_SYMBOL(ssi_tx_flush_fifo);
EXPORT_SYMBOL(ssi_tx_frame_direction);
EXPORT_SYMBOL(ssi_tx_frame_rate);
EXPORT_SYMBOL(ssi_tx_frame_sync_active);
EXPORT_SYMBOL(ssi_tx_frame_sync_length);
/*EXPORT_SYMBOL(ssi_tx_mask_time_slot);*/
EXPORT_SYMBOL(ssi_tx_prescaler_modulus);
EXPORT_SYMBOL(ssi_tx_shift_direction);
EXPORT_SYMBOL(ssi_tx_word_length);

static void set_register_bits(unsigned int mask, unsigned int data,
		       volatile u32 * regPtr)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&ssi_lock, flags);
	*regPtr = ((*regPtr) & (~mask)) | data;
	spin_unlock_irqrestore(&ssi_lock, flags);
}

/*!
 * This function turns off/on the ccm_ssi_clk to reduce power consumption.
 *
 * @param        module        the module number
 * @param        state         the state for ccm_ssi_clk (true: turn off, else:turn on)
 */
void ssi_clock_off(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_CLOCK_OFF_SHIFT,
			  state << SSI_CLOCK_OFF_SHIFT, &SSI_SCSR);

}

/*!
 * This function enables/disables the SSI module.
 *
 * @param        module        the module number
 * @param        state         the state for SSI module
 */
void ssi_enable(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_ENABLE_SHIFT, state << SSI_ENABLE_SHIFT,
			  &SSI_SCSR);
}

/*!
 * This function gets the data word in the Receive FIFO of the SSI module.
 *
 * @param        module        the module number
 * @param        fifo          the Receive FIFO to read
 * @return       This function returns the read data.
 */
unsigned short int ssi_get_data(ssi_mod module)
{
	return SSI_SRX;
}

/*!
 * This function returns the status of the SSI module (SISR register) as a combination of status.
 *
 * @param        module        the module number
 * @return       This function returns the status of the SSI module
 */
ssi_status_enable_mask ssi_get_status(ssi_mod module)
{
	unsigned int result;

	result = SSI_SCSR;
	result &= ((1 << SSI_IRQ_STATUS_NUMBER) - 1);

	return (ssi_status_enable_mask) result;
}

/*!
 * This function selects the I2S mode of the SSI module.
 *
 * @param        module        the module number
 * @param        mode          the I2S mode
 */
void ssi_i2s_mode(ssi_mod module, mode_i2s mode)
{
	set_register_bits(3 << SSI_I2S_MODE_SHIFT, mode << SSI_I2S_MODE_SHIFT,
			  &SSI_SCSR);
}

/*!
 * This function disables the interrupts of the SSI module.
 *
 * @param        module        the module number
 * @param        mask          the mask of the interrupts to disable
 */
void ssi_interrupt_disable(ssi_mod module, ssi_status_enable_mask mask,
				transfer_type transfer)
{
	if (ssi_Tx == transfer) {
		set_register_bits(mask, 0, &SSI_STCR);
	} else {
		set_register_bits(mask, 0, &SSI_SRCR);
	}
}

/*!
 * This function enables the interrupts of the SSI module.
 *
 * @param        module        the module number
 * @param        mask          the mask of the interrupts to enable
 */
void ssi_interrupt_enable(ssi_mod module, ssi_status_enable_mask mask,
				transfer_type transfer)
{
	if (ssi_Tx == transfer) {
		set_register_bits(0, mask, &SSI_STCR);
	} else {
		set_register_bits(0, mask, &SSI_SRCR);
	}
}

/*!
 * This function enables/disables the network mode of the SSI module.
 *
 * @param        module        the module number
 * @param        state         the network mode state
 */
void ssi_network_mode(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_NETWORK_MODE_SHIFT,
			  state << SSI_NETWORK_MODE_SHIFT, &SSI_SCSR);
}

/*!
 * This function enables/disables the receive section of the SSI module.
 *
 * @param        module        the module number
 * @param        state         the receive section state
 */
void ssi_receive_enable(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_RECEIVE_ENABLE_SHIFT,
			  state << SSI_RECEIVE_ENABLE_SHIFT, &SSI_SCSR);
}

/*!
 * This function configures the SSI module to receive data word at bit position 0 or 23 in the Receive shift register.
 *
 * @param        module        the module number
 * @param        state         the state to receive at bit 0
 */
void ssi_rx_bit0(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_BIT_0_SHIFT, state << SSI_BIT_0_SHIFT,
			  &SSI_SRCR);
}

/*!
 * This function controls the source of the clock signal used to clock the Receive shift register.
 *
 * @param        module        the module number
 * @param        direction     the clock signal direction
 */
void ssi_rx_clock_direction(ssi_mod module, ssi_tx_rx_direction direction)
{
	set_register_bits(1 << SSI_CLOCK_DIRECTION_SHIFT,
			  direction << SSI_CLOCK_DIRECTION_SHIFT, &SSI_SRCR);
}

/*!
 * This function controls which bit clock edge is used to clock in data.
 *
 * @param        module        the module number
 * @param        polarity      the clock polarity
 */
void ssi_rx_clock_polarity(ssi_mod module, ssi_tx_rx_clock_polarity polarity)
{
	set_register_bits(1 << SSI_CLOCK_POLARITY_SHIFT,
			  polarity << SSI_CLOCK_POLARITY_SHIFT, &SSI_SRCR);
}

/*!
 * This function configures a fixed divide-by-eight clock prescaler divider of the SSI module in series with the variable prescaler for the receive section.
 *
 * @param        module        the module number
 * @param        state         the prescaler state
 */
void ssi_rx_clock_prescaler(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_PRESCALER_RANGE_SHIFT,
			  state << SSI_PRESCALER_RANGE_SHIFT,
			  &SSI_SRCR);
}

/*!
 * This function controls the early frame sync configuration.
 *
 * @param        module        the module number
 * @param        early         the early frame sync configuration
 */
void ssi_rx_early_frame_sync(ssi_mod module, ssi_tx_rx_early_frame_sync early)
{
	set_register_bits(1 << SSI_EARLY_FRAME_SYNC_SHIFT,
			  early << SSI_EARLY_FRAME_SYNC_SHIFT,
			  &SSI_SRCR);
}

/*!
 * This function gets the number of data words in the Receive FIFO.
 *
 * @param        module        the module number
 * @param        fifo          the fifo
 * @return       This function returns the number of words in the Rx FIFO.
 */
unsigned char ssi_rx_fifo_counter(ssi_mod module)
{
	return ((SSI_SFCSR & (0xF << SSI_RX_FIFO_COUNT_SHIFT))
			>> SSI_RX_FIFO_COUNT_SHIFT);
}

/*!
 * This function enables the Receive FIFO.
 *
 * @param        module        the module number
 * @param        fifo          the fifo to enable
 * @param        enable        the state of the fifo, enabled or disabled
 */

void ssi_rx_fifo_enable(ssi_mod module, bool enable)
{
	set_register_bits(1 << SSI_FIFO_ENABLE_SHIFT,
			  ((u32)enable) << SSI_FIFO_ENABLE_SHIFT,
			  &SSI_SRCR);
}

/*!
 * This function controls the threshold at which the RFFx flag will be set.
 *
 * @param        module        the module number
 * @param        fifo          the fifo to enable
 * @param        watermark     the watermark
 * @return       This function returns the result of the operation (0 if successful, -1 otherwise).
 */
int ssi_rx_fifo_full_watermark(ssi_mod module,
			       unsigned char watermark)
{
	int result = -1;

	if ((watermark > SSI_MIN_FIFO_WATERMARK) &&
	    (watermark <= SSI_MAX_FIFO_WATERMARK)) {
		set_register_bits(0xf << SSI_RX_FIFO_WATERMARK_SHIFT,
				  watermark <<
				  SSI_RX_FIFO_WATERMARK_SHIFT,
				  &SSI_SFCSR);

		result = 0;
	}

	return result;
}

/*!
 * This function flushes the Receive FIFOs.
 *
 * @param        module        the module number
 */
void ssi_rx_flush_fifo(ssi_mod module)
{
	set_register_bits(0, 1 << SSI_RECEIVER_CLEAR_SHIFT, &SSI_SOR);
}

/*!
 * This function controls the direction of the Frame Sync signal for the receive section.
 *
 * @param        module        the module number
 * @param        direction     the Frame Sync signal direction
 */
void ssi_rx_frame_direction(ssi_mod module, ssi_tx_rx_direction direction)
{
	set_register_bits(1 << SSI_FRAME_DIRECTION_SHIFT,
			  direction << SSI_FRAME_DIRECTION_SHIFT,
			  &SSI_SRCR);
}

/*!
 * This function configures the Receive frame rate divider for the receive section.
 *
 * @param        module        the module number
 * @param        ratio         the divide ratio from 1 to 32
 * @return       This function returns the result of the operation (0 if successful, -1 otherwise).
 */
int ssi_rx_frame_rate(ssi_mod module, unsigned char ratio)
{
	int result = -1;

	if ((ratio >= SSI_MIN_FRAME_DIVIDER_RATIO) &&
	    (ratio <= SSI_MAX_FRAME_DIVIDER_RATIO)) {
		set_register_bits(SSI_FRAME_DIVIDER_MASK <<
				  SSI_FRAME_RATE_DIVIDER_SHIFT,
				  (ratio - 1) << SSI_FRAME_RATE_DIVIDER_SHIFT,
				  &SSI_SRCCR);
		result = 0;
	}

	return result;
}

/*!
 * This function controls the Frame Sync active polarity for the receive section.
 *
 * @param        module        the module number
 * @param        active        the Frame Sync active polarity
 */
void ssi_rx_frame_sync_active(ssi_mod module,
			      ssi_tx_rx_frame_sync_active active)
{
	set_register_bits(1 << SSI_FRAME_SYNC_INVERT_SHIFT,
			  active << SSI_FRAME_SYNC_INVERT_SHIFT,
			  &SSI_SRCR);
}

/*!
 * This function controls the Frame Sync length (one word or one bit long) for the receive section.
 *
 * @param        module        the module number
 * @param        length        the Frame Sync length
 */
void ssi_rx_frame_sync_length(ssi_mod module,
			      ssi_tx_rx_frame_sync_length length)
{
	set_register_bits(1 << SSI_FRAME_SYNC_LENGTH_SHIFT,
			  length << SSI_FRAME_SYNC_LENGTH_SHIFT,
			  &SSI_SRCR);
}

/*!
 * This function configures the time slot(s) to mask for the receive section.
 *
 * @param        module        the module number
 * @param        mask          the mask to indicate the time slot(s) masked
 */
/*void ssi_rx_mask_time_slot(ssi_mod module, unsigned int mask)
{
	set_register_bits(0xFFFFFFFF, mask, MXC_SSISRMSK);
}*/

/*!
 * This function configures the Prescale divider for the receive section.
 *
 * @param        module        the module number
 * @param        divider       the divide ratio from 1 to  256
 * @return       This function returns the result of the operation (0 if successful, -1 otherwise).
 */
int ssi_rx_prescaler_modulus(ssi_mod module, unsigned int divider)
{
	int result = -1;

	if ((divider >= SSI_MIN_PRESCALER_MODULUS_RATIO) &&
	    (divider <= SSI_MAX_PRESCALER_MODULUS_RATIO)) {

		set_register_bits(SSI_PRESCALER_MODULUS_MASK <<
				  SSI_PRESCALER_MODULUS_SHIFT,
				  (divider - 1) << SSI_PRESCALER_MODULUS_SHIFT,
				  &SSI_SRCCR);
		result = 0;
	}

	return result;
}

/*!
 * This function controls whether the MSB or LSB will be received first in a sample.
 *
 * @param        module        the module number
 * @param        direction     the shift direction
 */
void ssi_rx_shift_direction(ssi_mod module, ssi_tx_rx_shift_direction direction)
{
	set_register_bits(1 << SSI_SHIFT_DIRECTION_SHIFT,
			  direction << SSI_SHIFT_DIRECTION_SHIFT,
			  &SSI_SRCR);
}

/*!
 * This function configures the Receive word length.
 *
 * @param        module        the module number
 * @param        length        the word length
 */
void ssi_rx_word_length(ssi_mod module, ssi_word_length length)
{
	set_register_bits(SSI_WORD_LENGTH_MASK << SSI_WORD_LENGTH_SHIFT,
			  length << SSI_WORD_LENGTH_SHIFT,
			  &SSI_SRCCR);
}

/*!
 * This function sets the data word in the Transmit FIFO of the SSI module.
 *
 * @param        module        the module number
 * @param        fifo          the FIFO number
 * @param        data          the data to load in the FIFO
 */

void ssi_set_data(ssi_mod module, unsigned int data)
{
	SSI_STX = data;
}

/*!
 * This function controls the number of wait states between the core and SSI.
 *
 * @param        module        the module number
 * @param        wait          the number of wait state(s)
 */
/*void ssi_set_wait_states(ssi_mod module, ssi_wait_states wait)
{
	set_register_bits(SSI_WAIT_STATE_MASK << SSI_WAIT_SHIFT,
			  wait << SSI_WAIT_SHIFT, MXC_SSISOR);
}
*/
/*!
 * This function enables/disables the synchronous mode of the SSI module.
 *
 * @param        module        the module number
 * @param        state         the synchronous mode state
 */
void ssi_synchronous_mode(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_SYNCHRONOUS_MODE_SHIFT,
			  state << SSI_SYNCHRONOUS_MODE_SHIFT,
			  &SSI_SCSR);
}

/*!
 * This function allows the SSI module to output the SYS_CLK at the SRCK port.
 *
 * @param        module        the module number
 * @param        state         the system clock state
 */
void ssi_system_clock(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_SYSTEM_CLOCK_SHIFT,
			  state << SSI_SYSTEM_CLOCK_SHIFT, &SSI_SCSR);
}

/*!
 * This function enables/disables the transmit section of the SSI module.
 *
 * @param        module        the module number
 * @param        state         the transmit section state
 */
void ssi_transmit_enable(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_TRANSMIT_ENABLE_SHIFT,
			  state << SSI_TRANSMIT_ENABLE_SHIFT,
			  &SSI_SCSR);
}

/*!
 * This function allows the SSI module to operate in the two channel mode.
 *
 * @param        module        the module number
 * @param        state         the two channel mode state
 */
/*void ssi_two_channel_mode(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_TWO_CHANNEL_SHIFT,
			  state << SSI_TWO_CHANNEL_SHIFT, MXC_SSISCR);
}
*/
/*!
 * This function configures the SSI module to transmit data word from bit position 0 or 23 in the Transmit shift register.
 *
 * @param        module        the module number
 * @param        state         the transmit from bit 0 state
 */
void ssi_tx_bit0(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_BIT_0_SHIFT,
			  state << SSI_BIT_0_SHIFT, &SSI_STCR);
}

/*!
 * This function controls the direction of the clock signal used to clock the Transmit shift register.
 *
 * @param        module        the module number
 * @param        direction     the clock signal direction
 */
void ssi_tx_clock_direction(ssi_mod module, ssi_tx_rx_direction direction)
{
	set_register_bits(1 << SSI_CLOCK_DIRECTION_SHIFT,
			  direction << SSI_CLOCK_DIRECTION_SHIFT,
			  &SSI_STCR);
}

/*!
 * This function configures the divide-by-two divider of the SSI module for the transmit section.
 *
 * @param        module        the module number
 * @param        state         the divider state
 */
/*void ssi_tx_clock_divide_by_two(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_DIVIDE_BY_TWO_SHIFT,
			  state << SSI_DIVIDE_BY_TWO_SHIFT,
			  MXC_SSISTCCR);
}
*/
/*!
 * This function controls which bit clock edge is used to clock out data.
 *
 * @param        module        the module number
 * @param        polarity      the clock polarity
 */
void ssi_tx_clock_polarity(ssi_mod module, ssi_tx_rx_clock_polarity polarity)
{
	set_register_bits(1 << SSI_CLOCK_POLARITY_SHIFT,
			  polarity << SSI_CLOCK_POLARITY_SHIFT,
			  &SSI_STCR);
}

/*!
 * This function configures a fixed divide-by-eight clock prescaler divider of the SSI module in series with the variable prescaler for the transmit section.
 *
 * @param        module        the module number
 * @param        state         the prescaler state
 */
void ssi_tx_clock_prescaler(ssi_mod module, bool state)
{
	set_register_bits(1 << SSI_PRESCALER_RANGE_SHIFT,
			  state << SSI_PRESCALER_RANGE_SHIFT,
			  &SSI_STCCR);
}

/*!
 * This function controls the early frame sync configuration for the transmit section.
 *
 * @param        module        the module number
 * @param        early         the early frame sync configuration
 */
void ssi_tx_early_frame_sync(ssi_mod module, ssi_tx_rx_early_frame_sync early)
{
	set_register_bits(1 << SSI_EARLY_FRAME_SYNC_SHIFT,
			  early << SSI_EARLY_FRAME_SYNC_SHIFT,
			  &SSI_STCR);
}

/*!
 * This function gets the number of data words in the Transmit FIFO.
 *
 * @param        module        the module number
 * @param        fifo          the fifo
 * @return       This function returns the number of words in the Tx FIFO.
 */
unsigned char ssi_tx_fifo_counter(ssi_mod module)
{
	return ((SSI_SFCSR & (0xF << SSI_TX_FIFO_COUNT_SHIFT))
			>> SSI_TX_FIFO_COUNT_SHIFT);
}

/*!
 * This function controls the threshold at which the TFEx flag will be set.
 *
 * @param        module        the module number
 * @param        fifo          the fifo to enable
 * @param        watermark     the watermark
 * @return       This function returns the result of the operation (0 if successful, -1 otherwise).
 */
int ssi_tx_fifo_empty_watermark(ssi_mod module, unsigned char watermark)
{
	int result = -1;

	if ((watermark > SSI_MIN_FIFO_WATERMARK) &&
	    (watermark <= SSI_MAX_FIFO_WATERMARK)) {
		set_register_bits(0xf << SSI_TX_FIFO_WATERMARK_SHIFT,
				  watermark <<
				  SSI_TX_FIFO_WATERMARK_SHIFT,
					  &SSI_SFCSR);
		result = 0;
	}

	return result;
}

/*!
 * This function enables the Transmit FIFO.
 *
 * @param        module        the module number
 * @param        fifo          the fifo to enable
 * @param        enable        the state of the fifo, enabled or disabled
 */

void ssi_tx_fifo_enable(ssi_mod module, bool enable)
{
	set_register_bits(1 << SSI_FIFO_ENABLE_SHIFT,
			  ((u32)enable) << SSI_FIFO_ENABLE_SHIFT,
			  &SSI_STCR);
}

/*!
 * This function flushes the Transmit FIFOs.
 *
 * @param        module        the module number
 */
void ssi_tx_flush_fifo(ssi_mod module)
{
	set_register_bits(0, 1 << SSI_TRANSMITTER_CLEAR_SHIFT,
			  &SSI_SOR);
}

/*!
 * This function controls the direction of the Frame Sync signal for the transmit section.
 *
 * @param        module        the module number
 * @param        direction     the Frame Sync signal direction
 */
void ssi_tx_frame_direction(ssi_mod module, ssi_tx_rx_direction direction)
{
	set_register_bits(1 << SSI_FRAME_DIRECTION_SHIFT,
			  direction << SSI_FRAME_DIRECTION_SHIFT,
			  &SSI_STCR);
}

/*!
 * This function configures the Transmit frame rate divider.
 *
 * @param        module        the module number
 * @param        ratio         the divide ratio from 1 to 32
 * @return       This function returns the result of the operation (0 if successful, -1 otherwise).
 */
int ssi_tx_frame_rate(ssi_mod module, unsigned char ratio)
{
	int result = -1;

	if ((ratio >= SSI_MIN_FRAME_DIVIDER_RATIO) &&
	    (ratio <= SSI_MAX_FRAME_DIVIDER_RATIO)) {

		set_register_bits(SSI_FRAME_DIVIDER_MASK <<
				  SSI_FRAME_RATE_DIVIDER_SHIFT,
				  (ratio - 1) << SSI_FRAME_RATE_DIVIDER_SHIFT,
				  &SSI_STCCR);
		result = 0;
	}

	return result;
}

/*!
 * This function controls the Frame Sync active polarity for the transmit section.
 *
 * @param        module        the module number
 * @param        active        the Frame Sync active polarity
 */
void ssi_tx_frame_sync_active(ssi_mod module,
			      ssi_tx_rx_frame_sync_active active)
{
	set_register_bits(1 << SSI_FRAME_SYNC_INVERT_SHIFT,
			  active << SSI_FRAME_SYNC_INVERT_SHIFT,
			  &SSI_STCR);
}

/*!
 * This function controls the Frame Sync length (one word or one bit long) for the transmit section.
 *
 * @param        module        the module number
 * @param        length        the Frame Sync length
 */
void ssi_tx_frame_sync_length(ssi_mod module,
			      ssi_tx_rx_frame_sync_length length)
{
	set_register_bits(1 << SSI_FRAME_SYNC_LENGTH_SHIFT,
			  length << SSI_FRAME_SYNC_LENGTH_SHIFT,
			  &SSI_STCR);
}

/*!
 * This function configures the time slot(s) to mask for the transmit section.
 *
 * @param        module        the module number
 * @param        mask          the mask to indicate the time slot(s) masked
 */
/*void ssi_tx_mask_time_slot(ssi_mod module, unsigned int mask)
{
	set_register_bits(0xFFFFFFFF, mask, MXC_SSISTMSK);
}
*/
/*!
 * This function configures the Prescale divider for the transmit section.
 *
 * @param        module        the module number
 * @param        divider       the divide ratio from 1 to  256
 * @return       This function returns the result of the operation (0 if successful, -1 otherwise).
 */
int ssi_tx_prescaler_modulus(ssi_mod module, unsigned int divider)
{
	int result = -1;

	if ((divider >= SSI_MIN_PRESCALER_MODULUS_RATIO) &&
	    (divider <= SSI_MAX_PRESCALER_MODULUS_RATIO)) {

		set_register_bits(SSI_PRESCALER_MODULUS_MASK <<
				  SSI_PRESCALER_MODULUS_SHIFT,
				  (divider - 1) << SSI_PRESCALER_MODULUS_SHIFT,
				  &SSI_STCCR);
		result = 0;
	}

	return result;
}

/*!
 * This function controls whether the MSB or LSB will be transmitted first in a sample.
 *
 * @param        module        the module number
 * @param        direction     the shift direction
 */
void ssi_tx_shift_direction(ssi_mod module, ssi_tx_rx_shift_direction direction)
{
	set_register_bits(1 << SSI_SHIFT_DIRECTION_SHIFT,
			  direction << SSI_SHIFT_DIRECTION_SHIFT,
			  &SSI_STCR);
}

/*!
 * This function configures the Transmit word length.
 *
 * @param        module        the module number
 * @param        length        the word length
 */
void ssi_tx_word_length(ssi_mod module, ssi_word_length length)
{
	set_register_bits(SSI_WORD_LENGTH_MASK << SSI_WORD_LENGTH_SHIFT,
			  length << SSI_WORD_LENGTH_SHIFT,
			  &SSI_STCCR);
}

/*!
 * This function implements the init function of the SSI device.
 * This function is called when the module is loaded.
 *
 * @return       This function returns 0.
 */
static int __init ssi_init(void)
{
	spin_lock_init(&ssi_lock);
	printk(KERN_INFO "SSI module loaded successfully\n");
	return 0;
}

/*!
 * This function implements the exit function of the SPI device.
 * This function is called when the module is unloaded.
 *
 */
static void __exit ssi_exit(void)
{
	printk(KERN_INFO "SSI module unloaded successfully\n");
}

module_init(ssi_init);
module_exit(ssi_exit);

MODULE_DESCRIPTION("SSI char device driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
