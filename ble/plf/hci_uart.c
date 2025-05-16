/*
 * Copyright (c) 2023 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hci_uart.h"

#include "ble_api.h"
#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/__assert.h>

#include "es0_power_manager.h"

/* change this to any other UART peripheral if desired */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_hci_uart)
static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/*
 * STRUCT DEFINITIONS
 *****************************************************************************************
 */
/* TX and RX channel class holding data used for asynchronous read and write
 * data transactions
 */
/* UART TX RX Channel */
struct uart_txrxchannel {
	/* call back function pointer */
	void (*callback)(void *a, uint8_t b);
	/* Dummy data pointer returned to callback when operation is over. */
	void *dummy;
};

/* UART environment structure */
struct uart_env_tag {
	/* tx channel */
	struct uart_txrxchannel tx;
	/* rx channel */
	struct uart_txrxchannel rx;
	/* error detect */
	uint8_t errordetect;
	/* external wakeup */
	bool ext_wakeup;
};

/* receive buffer used in UART ISR callback */
static uint8_t *rx_buf_ptr __noinit;
static uint32_t rx_buf_size __noinit;
static uint32_t rx_buf_len __noinit;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/* uart environment structure */
static struct uart_env_tag uart_env __noinit;

void hci_uart_callback(const struct device *dev, void *user_data)
{
	if (!uart_irq_update(uart_dev)) {
		return;
	}

	void (*callback)(void *, uint8_t) = NULL;
	void *data = NULL;

	while (uart_irq_rx_ready(uart_dev) && rx_buf_len < rx_buf_size) {
		int read_bytes = uart_fifo_read(uart_dev, rx_buf_ptr + rx_buf_len, 1);

		rx_buf_len += read_bytes;
	}

	if (rx_buf_len == rx_buf_size) {
		uart_irq_rx_disable(uart_dev);
		/* Retrieve callback pointer */
		callback = uart_env.rx.callback;
		data = uart_env.rx.dummy;

		if (callback != NULL) {
			/* Clear callback pointer */
			uart_env.rx.callback = NULL;
			uart_env.rx.dummy = NULL;

			/* Call handler */
			callback(data, ITF_STATUS_OK);
		}
	}

	/* TODO error/overflow handling */
}

int32_t hci_uart_init(void)
{
	if (!device_is_ready(uart_dev)) {
		printk("UART device not found!");
		return -1;
	}

	uart_irq_rx_enable(uart_dev);
	uart_irq_callback_user_data_set(uart_dev, hci_uart_callback, NULL);

	/* We cannot initialize RX transfer callback here
	 * as that might be kept in retention and also when
	 * read operation is started a (new) callback is always set.
	 */
	uart_env.tx.callback = NULL;
	return 0;
}

void hci_uart_read(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
	__ASSERT(bufptr != NULL, "Invalid buffer pointer");
	__ASSERT(size != 0, "Invalid size");
	__ASSERT(callback != NULL, "Invalid callback");

	uart_env.rx.callback = callback;
	uart_env.rx.dummy = dummy;

	rx_buf_ptr = bufptr;
	rx_buf_size = size;
	rx_buf_len = 0;

	/* Deassert&assert rts_n, falling edge triggers wake up the RF core */
	wake_es0(uart_dev);

	uart_irq_rx_enable(uart_dev);
}

void hci_uart_write(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
	__ASSERT(bufptr != NULL, "Invalid buffer pointer");
	__ASSERT(size != 0, "Invalid size");
	__ASSERT(callback != NULL, "Invalid callback");

	/* Deassert&assert rts_n, falling edge triggers wake up the RF core */
	wake_es0(uart_dev);

	uart_env.tx.callback = callback;
	uart_env.tx.dummy = dummy;

	for (int i = 0; i < size; i++) {
		uart_poll_out(uart_dev, bufptr[i]);
	}

	if (callback != NULL) {
		/* Clear callback pointer */
		uart_env.tx.callback = NULL;
		uart_env.tx.dummy = NULL;
		/* Call handler */
		callback(dummy, ITF_STATUS_OK);
	}
}

void hci_uart_flow_on(void)
{
	/* Not supported */
	/* TODO */
}

bool hci_uart_flow_off(void)
{
	/* Not supported */
	/* TODO */
	return true;
}

