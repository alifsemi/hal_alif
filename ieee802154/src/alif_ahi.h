/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ALIF_AHI_H_
#define ALIF_AHI_H_

#include "ahi_msg_lib.h"

/**
 * @brief AHI message receive callback function
 *
 * @param[in]	msg_id		ID of the message received
 * @param[in]	buffer_ptr	Buffer where the received message is
 *				allocated. Only valid during function call
 * @param[in]	buffer_len	Length of the message in buffer.
 *
 */
typedef void (*msg_received_callback)(struct msg_buf *p_msg);

/**
 * @brief AHI message send
 *
 * @param[in]	p_cmd		Command buffer to be seNt
 * @param[in]	p_data		Data appended after command structure
 * @param[in]	data_length	Length of the data
 *
 * @return	0		Status OK
 *		< 0		Operation failed
 */
int alif_ahi_msg_send(struct msg_buf *p_msg, const uint8_t *p_data, uint16_t data_length);

/**
 * @brief AHI subsystem initialize
 *
 * @param[in]	callback	Callback called when message received.
 *
 */
void alif_ahi_init(msg_received_callback callback);

/**
 * @brief Get baudrate used by AHI
 *
 * @return the baudrate in use, e.g. 1000000
 */
uint32_t alif_ahi_get_baudrate(void);

/**
 * @brief Reset the AHI subsystem
 *
 */
int alif_ahi_reset(void);
#endif /* ALIF_AHI_H_ */
