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
#include <zephyr/logging/log.h>

/* Register HCI UART log module with standard UART log level */
LOG_MODULE_REGISTER(hci_uart, CONFIG_UART_LOG_LEVEL);

/* Define appropriate timeouts */
#define TX_TIMEOUT_US 100000 /* 100ms */
#define RX_TIMEOUT_US 500000 /* 500ms */

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
	/* Flag to track if RX is active */
	bool rx_enabled;
	/* Flag to track if TX is active */
	bool tx_active;
};

/* receive buffer used in UART callback */
static uint8_t *rx_buf_ptr __noinit;
static uint32_t rx_buf_size __noinit;
static uint32_t rx_buf_len __noinit;

/* uart environment structure */
static struct uart_env_tag uart_env __noinit;

#if CONFIG_UART_ASYNC_API
/**
 * @brief UART async event callback
 *
 * This function handles UART async events for both TX and RX operations
 * when using DMA-based transfers with the async UART API
 *
 * @param dev UART device
 * @param evt UART event
 * @param user_data User data pointer
 */
static void hci_uart_async_callback(const struct device *dev, struct uart_event *evt,
				    void *user_data)
{
	void (*callback)(void *, uint8_t) = NULL;
	void *data = NULL;

	LOG_DBG("UART async event: %d", evt->type);

	switch (evt->type) {
	case UART_TX_DONE:
		/* TX completed successfully */
		LOG_DBG("UART TX completed successfully");
		uart_env.tx_active = false;
		callback = uart_env.tx.callback;
		data = uart_env.tx.dummy;

		if (callback != NULL) {
			/* Clear callback pointer */
			uart_env.tx.callback = NULL;
			uart_env.tx.dummy = NULL;

			/* Call handler */
			callback(data, ITF_STATUS_OK);
		}
		break;

	case UART_TX_ABORTED:
		/* TX was aborted */
		LOG_WRN("UART TX was aborted, sent %d bytes", evt->data.tx.len);
		uart_env.tx_active = false;
		callback = uart_env.tx.callback;
		data = uart_env.tx.dummy;

		if (callback != NULL) {
			/* Clear callback pointer */
			uart_env.tx.callback = NULL;
			uart_env.tx.dummy = NULL;

			/* Call handler with error status */
			callback(data, ITF_STATUS_ERROR);
		}
		break;

	case UART_RX_RDY:
		/* Data received and ready for processing */
		LOG_DBG("UART RX data ready: %d bytes", evt->data.rx.len);
		rx_buf_len = evt->data.rx.len;

		/* If we've received the expected amount of data, notify the callback */
		if (rx_buf_len == rx_buf_size) {
			LOG_DBG("UART RX complete, received %d bytes", rx_buf_len);
			uart_env.rx_enabled = false;
			uart_rx_disable(uart_dev);

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
		break;

	case UART_RX_BUF_REQUEST:
		/* UART driver is requesting a new buffer for continuous reception */
		LOG_DBG("UART RX buffer request - not providing additional buffer");
		/* We're not providing an additional buffer as we want to process the data
		 * when the current buffer is filled or when a timeout occurs.
		 * This will cause the reception to stop when the current buffer is full.
		 */
		break;

	case UART_RX_BUF_RELEASED:
		/* Buffer has been released */
		LOG_DBG("UART RX buffer released");
		break;

	case UART_RX_DISABLED:
		/* RX has been disabled */
		LOG_DBG("UART RX disabled");
		uart_env.rx_enabled = false;
		break;

	case UART_RX_STOPPED:
		/* RX has been stopped due to error */
		LOG_WRN("UART RX stopped due to error: %d", evt->data.rx_stop.reason);
		uart_env.rx_enabled = false;
		callback = uart_env.rx.callback;
		data = uart_env.rx.dummy;

		if (callback != NULL) {
			/* Clear callback pointer */
			uart_env.rx.callback = NULL;
			uart_env.rx.dummy = NULL;

			/* Call handler with error status */
			callback(data, ITF_STATUS_ERROR);
		}
		break;

	default:
		LOG_ERR("Unknown UART event: %d", evt->type);
		break;
	}
}

#else  /* No async UART or DMA */

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
#endif /* CONFIG_UART_ASYNC_API && HCI_UART_HAS_DMA */

/**
 * @brief Initialize the HCI UART interface
 *
 * @return 0 on success, negative error code otherwise
 */
int32_t hci_uart_init(void)
{
	/* Get the UART device */
	uart_dev = DEVICE_DT_GET_OR_NULL(DT_ALIAS(uart_hci));

	if (!uart_dev) {
		/* Try to get the device by nodelabel as fallback */
		uart_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(uart_hci));
	}

	if (!device_is_ready(uart_dev)) {
		LOG_ERR("UART device not found or not ready!");
		return -ENODEV;
	}

#if CONFIG_UART_ASYNC_API
	/* Set the UART callback for async operations */
	int ret = uart_callback_set(uart_dev, hci_uart_async_callback, NULL);

	if (ret < 0) {
		LOG_ERR("Failed to set UART callback: %d", ret);
		return ret;
	}
#else  /* !CONFIG_UART_ASYNC_API */
	uart_irq_rx_enable(uart_dev);
	uart_irq_callback_user_data_set(uart_dev, hci_uart_callback, NULL);
#endif /* CONFIG_UART_ASYNC_API */

	/* We cannot initialize RX transfer callback here
	 * as that might be kept in retention and also when
	 * read operation is started a (new) callback is always set.
	 */
	uart_env.tx.callback = NULL;
	return 0;
}

void hci_uart_read(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
	LOG_DBG("HCI UART read request: %d bytes", size);

	__ASSERT(bufptr != NULL, "Invalid buffer pointer");
	__ASSERT(size != 0, "Invalid size");
	__ASSERT(callback != NULL, "Invalid callback");

#if CONFIG_UART_ASYNC_API
	/* If RX is already enabled, disable it first */
	if (uart_env.rx_enabled) {
		int ret = uart_rx_disable(uart_dev);

		if (ret < 0) {
			LOG_ERR("Failed to disable UART RX: %d", ret);
			/* Call callback with error status */
			callback(dummy, ITF_STATUS_ERROR);
			return;
		}
		uart_env.rx_enabled = false;
	}
#endif /* CONFIG_UART_ASYNC_API */

	/* Store callback and user data */
	uart_env.rx.callback = callback;
	uart_env.rx.dummy = dummy;

	rx_buf_ptr = bufptr;
	rx_buf_size = size;
	rx_buf_len = 0;

	/* Deassert&assert rts_n, falling edge triggers wake up the RF core */
	wake_es0(uart_dev);

#if CONFIG_UART_ASYNC_API
	/* Enable RX with DMA and timeout */
	int ret = uart_rx_enable(uart_dev, bufptr, size, RX_TIMEOUT_US);

	if (ret < 0) {
		LOG_ERR("Failed to enable UART RX: %d", ret);
		/* If enabling RX fails, call the callback with error */
		if (callback) {
			callback(dummy, ITF_STATUS_ERROR);
			uart_env.rx.callback = NULL;
			uart_env.rx.dummy = NULL;
			rx_buf_ptr = NULL;
			rx_buf_size = 0;
		}
		return;
	}

	uart_env.rx_enabled = true;
#else  /* No async UART */
	uart_irq_rx_enable(uart_dev);
#endif /* CONFIG_UART_ASYNC_API */
}

void hci_uart_write(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
	LOG_DBG("HCI UART write request: %d bytes", size);

	__ASSERT(bufptr != NULL, "Invalid buffer pointer");
	__ASSERT(size != 0, "Invalid size");
	__ASSERT(callback != NULL, "Invalid callback");

#if CONFIG_UART_ASYNC_API
	/* If TX is already active, don't start a new transfer */
	if (uart_env.tx_active) {
		/* Call the callback with error status */
		callback(dummy, ITF_STATUS_ERROR);
		return;
	}
#endif /* CONFIG_UART_ASYNC_API */

	/* Deassert&assert rts_n, falling edge triggers wake up the RF core */
	wake_es0(uart_dev);

	uart_env.tx.callback = callback;
	uart_env.tx.dummy = dummy;


#if CONFIG_UART_ASYNC_API
	/* Start TX with DMA and timeout */
	int ret = uart_tx(uart_dev, bufptr, size, TX_TIMEOUT_US);

	if (ret < 0) {
		/* If starting TX fails, call the callback with error */
		if (callback) {
			callback(dummy, ITF_STATUS_ERROR);
			uart_env.tx.callback = NULL;
			uart_env.tx.dummy = NULL;
		}
		return;
	}

	uart_env.tx_active = true;
#else  /* No async UART */

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
#endif /* CONFIG_UART_ASYNC_API */
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

