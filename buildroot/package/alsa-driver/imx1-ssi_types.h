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
  * @file ssi_types.h
  * @brief This header file contains SSI types.
  *
  * @ingroup SSI
  */

#ifndef __IMX1_SSI_TYPES_H__
#define __IMX1_SSI_TYPES_H__

/*!
 * This enumeration describes the FIFO number.
 */
typedef enum {
	/*!
	 * FIFO 0
	 */
	ssi_fifo_0 = 0,
	/*!
	 * FIFO 1
	 */
	ssi_fifo_1 = 1
} fifo_nb;

/*!
 * This enumeration describes the type of transfer (Transmit/Receive).
 */
typedef enum {
	/*!
	 * Receive
	 */
	ssi_Rx = 0,
	/*!
	 * Transmit
	 */
	ssi_Tx = 1
} transfer_type;

/*!
 * This enumeration describes the clock idle state.
 */
typedef enum {
	/*!
	 * Clock idle state is 1
	 */
	clock_idle_state_1 = 0,
	/*!
	 * Clock idle state is 0
	 */
	clock_idle_state_0 = 1
} idle_state;

/*!
 * This enumeration describes I2S mode.
 */
typedef enum {
	/*!
	 * Normal mode
	 */
	i2s_normal = 0,
	/*!
	 * Master mode
	 */
	i2s_master = 1,
	/*!
	 * Slave mode
	 */
	i2s_slave = 2
} mode_i2s;

/*!
 * This enumeration describes index for both SSI1 and SSI2 modules.
 */
typedef enum {
	/*!
	 * SSI1 index
	 */
	SSI1 = 0,
	/*!
	 * SSI2 index
	 */
	SSI2 = 1
} ssi_mod;

/*!
 * This enumeration describes the status/enable bits for interrupt source of the SSI module.
 */
typedef enum {
	/*!
	 * SSI Receive FIFO full bit
	 */
	ssi_rx_fifo_full = 0x00000001,
	/*!
	 * SSI Receive Frame Sync bit
	 */
	ssi_rfs = 0x00000002,
	/*!
	 * SSI Transmit Frame Sync bit
	 */
	ssi_tfs = 0x00000004,
	/*!
	 * SSI Transmitter underrun bit
	 */
	ssi_transmitter_underrun = 0x00000008,
	/*!
	 * SSI Receiver overrun bit
	 */
	ssi_receiver_overrun = 0x00000010,
	/*!
	 * SSI Transmit Data register empty bit
	 */
	ssi_tx_data_reg_empty = 0x00000020,
	/*!
	 * SSI Receive Data Ready bit
	 */
	ssi_rx_data_ready = 0x00000040
} ssi_status_enable_mask;

/*!
 * This enumeration describes the clock edge to clock in or clock out data.
 */
typedef enum {
	/*!
	 * Clock on rising edge
	 */
	ssi_clock_on_rising_edge = 0,
	/*!
	 * Clock on falling edge
	 */
	ssi_clock_on_falling_edge = 1
} ssi_tx_rx_clock_polarity;

/*!
 * This enumeration describes the clock direction.
 */
typedef enum {
	/*!
	 * Clock is external
	 */
	ssi_tx_rx_externally = 0,
	/*!
	 * Clock is generated internally
	 */
	ssi_tx_rx_internally = 1
} ssi_tx_rx_direction;

/*!
 * This enumeration describes the early frame sync behavior.
 */
typedef enum {
	/*!
	 * Frame Sync starts on the first data bit
	 */
	ssi_frame_sync_first_bit = 0,
	/*!
	 * Frame Sync starts one bit before the first data bit
	 */
	ssi_frame_sync_one_bit_before = 1
} ssi_tx_rx_early_frame_sync;

/*!
 * This enumeration describes the Frame Sync active value.
 */
typedef enum {
	/*!
	 * Frame Sync is active when high
	 */
	ssi_frame_sync_active_high = 0,
	/*!
	 * Frame Sync is active when low
	 */
	ssi_frame_sync_active_low = 1
} ssi_tx_rx_frame_sync_active;

/*!
 * This enumeration describes the Frame Sync active length.
 */
typedef enum {
	/*!
	 * Frame Sync is active when high
	 */
	ssi_frame_sync_one_word = 0,
	/*!
	 * Frame Sync is active when low
	 */
	ssi_frame_sync_one_bit = 1
} ssi_tx_rx_frame_sync_length;

/*!
 * This enumeration describes the Tx/Rx frame shift direction.
 */
typedef enum {
	/*!
	 * MSB first
	 */
	ssi_msb_first = 0,
	/*!
	 * LSB first
	 */
	ssi_lsb_first = 1
} ssi_tx_rx_shift_direction;

/*!
 * This enumeration describes the wait state number.
 */
typedef enum {
	/*!
	 * 0 wait state
	 */
	ssi_waitstates0 = 0x0,
	/*!
	 * 1 wait state
	 */
	ssi_waitstates1 = 0x1,
	/*!
	 * 2 wait states
	 */
	ssi_waitstates2 = 0x2,
	/*!
	 * 3 wait states
	 */
	ssi_waitstates3 = 0x3
} ssi_wait_states;

/*!
 * This enumeration describes the word length.
 */
typedef enum {
	/*!
	 * 2 bits long
	 */
	ssi_2_bits = 0x0,
	/*!
	 * 4 bits long
	 */
	ssi_4_bits = 0x1,
	/*!
	 * 6 bits long
	 */
	ssi_6_bits = 0x2,
	/*!
	 * 8 bits long
	 */
	ssi_8_bits = 0x3,
	/*!
	 * 10 bits long
	 */
	ssi_10_bits = 0x4,
	/*!
	 * 12 bits long
	 */
	ssi_12_bits = 0x5,
	/*!
	 * 14 bits long
	 */
	ssi_14_bits = 0x6,
	/*!
	 * 16 bits long
	 */
	ssi_16_bits = 0x7,
	/*!
	 * 18 bits long
	 */
	ssi_18_bits = 0x8,
	/*!
	 * 20 bits long
	 */
	ssi_20_bits = 0x9,
	/*!
	 * 22 bits long
	 */
	ssi_22_bits = 0xA,
	/*!
	 * 24 bits long
	 */
	ssi_24_bits = 0xB,
	/*!
	 * 26 bits long
	 */
	ssi_26_bits = 0xC,
	/*!
	 * 28 bits long
	 */
	ssi_28_bits = 0xD,
	/*!
	 * 30 bits long
	 */
	ssi_30_bits = 0xE,
	/*!
	 * 32 bits long
	 */
	ssi_32_bits = 0xF
} ssi_word_length;

typedef enum { false, true } bool;


#endif				/* __IMX1_SSI_TYPES_H__ */
