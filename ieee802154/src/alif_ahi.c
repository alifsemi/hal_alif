/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#include "alif_ahi.h"
#include "es0_power_manager.h"
#include "dma_event_router.h"

#define LOG_MODULE_NAME alif_ahi

#if defined(CONFIG_IEEE802154_DRIVER_LOG_LEVEL)
#define LOG_LEVEL CONFIG_IEEE802154_DRIVER_LOG_LEVEL
#else
#define LOG_LEVEL LOG_LEVEL_NONE
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

/* change this to any other UART peripheral if desired */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_ahi_uart)
static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

struct msg_buf rx_msg;

static msg_received_callback receive_cb;
static bool ahi_tx_dma_enabled;
static K_SEM_DEFINE(ahi_tx_sem, 0, 1);
K_MUTEX_DEFINE(receive_mutex);

/*AHI Protocol defines*/
#define AHI_KE_MSG_TYPE 0x10

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
static void ahi_uart_async_callback(const struct device *dev, struct uart_event *evt,
				    void *user_data)
{

	switch (evt->type) {
	case UART_TX_DONE:
		/* TX completed successfully */
		LOG_ERR("UART TX completed successfully");
		k_sem_give(&ahi_tx_sem);
		break;

	case UART_TX_ABORTED:
		/* TX was aborted */
		LOG_ERR("UART TX was aborted, sent %d bytes", evt->data.tx.len);
		k_sem_give(&ahi_tx_sem);
		break;

	case UART_RX_RDY:
		break;

	case UART_RX_BUF_REQUEST:
		break;

	case UART_RX_BUF_RELEASED:
		break;

	case UART_RX_DISABLED:

		break;

	case UART_RX_STOPPED:
		break;

	default:
		LOG_ERR("Unknown UART event: %d", evt->type);
		break;
	}
}

void ahi_uart_callback(const struct device *dev, void *user_data)
{
	int read_bytes = 1;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	if (uart_irq_tx_ready(uart_dev)) {
		return;
	}
	if (!uart_irq_rx_ready(uart_dev)) {
		return;
	}
	while (read_bytes > 0 && rx_msg.msg_len < MAX_MSG_LEN) {
		read_bytes = uart_fifo_read(uart_dev, rx_msg.msg + rx_msg.msg_len, 1);
		if (read_bytes < 0) {
			LOG_ERR("read failed");
			break;
		}
		rx_msg.msg_len += read_bytes;

		int status = alif_ahi_msg_valid_message(&rx_msg);

		if (status == 1) {
			if (receive_cb) {
				receive_cb(&rx_msg);
			}
			rx_msg.msg_len = 0;
		} else if (status < 0) {
			LOG_ERR("message corrupt %d", status);
			/*Clean the buffer until it becomes empty or valid*/
			while (rx_msg.msg_len && status < 0) {
				memmove(rx_msg.msg, rx_msg.msg + 1, rx_msg.msg_len - 1);
				rx_msg.msg_len--;
				status = alif_ahi_msg_valid_message(&rx_msg);
			}
			read_bytes = 1;
		}
	}
}

static int alif_send_with_dma(const uint8_t data_ptr, uint16_t length)
{
#ifdef CONFIG_UART_ASYNC_API
	/* Start TX with DMA and timeout */
	LOG_ERR("Send %u", length);
	int ret = uart_tx(uart_dev, data_ptr, length, 200);

	if (ret < 0) {
		/* If starting TX fails, call the callback with error */
		LOG_ERR("TX send fail");
		return ret;
	}
	if (k_sem_take(&ahi_tx_sem, K_USEC(200)) != 0) {

		LOG_ERR("TX SEM TO");
		return -ETIME;
	}
	return 0;
#else
	return -EIO;
#endif
}

int alif_ahi_msg_send(struct msg_buf *p_msg, const uint8_t *p_data, uint16_t data_length)
{
	if (p_msg == NULL) {
		return -1;
	}

	/* Deassert&assert rts_n, falling edge triggers wake up the RF core */
	wake_es0(uart_dev);

	if (ahi_tx_dma_enabled) {
		int ret = alif_send_with_dma(p_msg->msg, p_msg->msg_len);

		if (ret) {
			return ret;
		}
		if (p_data && data_length) {
			ret = alif_send_with_dma(p_msg->msg, p_msg->msg_len);
			if (ret) {
				return ret;
			}
		}
	} else {
		for (int i = 0; i < p_msg->msg_len; i++) {
			uart_poll_out(uart_dev, p_msg->msg[i]);
		}
		if (p_data && data_length) {
			for (int i = 0; i < data_length; i++) {
				uart_poll_out(uart_dev, p_data[i]);
			}
		}
	}

	return 0;
}

static bool ahi_uart_tx_dma_driver_check(void)
{
#ifdef CONFIG_UART_ASYNC_API
#if DT_NODE_HAS_STATUS(DT_DMAS_CTLR_BY_NAME(DT_NODELABEL(uart_ahi), tx), okay)
	const struct device *txdma =
		DEVICE_DT_GET_OR_NULL(DT_DMAS_CTLR_BY_NAME(DT_NODELABEL(uart_ahi), tx));

	if (txdma && device_is_ready(txdma)) {
		/* Register DMA callback */
		int tx_config = DT_DMAS_CELL_BY_NAME(DT_NODELABEL(uart_ahi), tx, periph);

		uart_callback_set(uart_dev, ahi_uart_async_callback, NULL);

		LOG_DBG("DMA tX event enable %d", tx_config);

		dma_event_router_configure(0, tx_config, false);

		return true;
	}
#endif
	uart_irq_tx_enable(uart_dev);
	return false;
}

int alif_ahi_reset(void)
{
	if (!device_is_ready(uart_dev)) {
		LOG_INF("UART device not found!");
		return -1;
	}

	uart_rx_disable(uart_dev);
	uart_irq_rx_disable(uart_dev);
	uart_irq_tx_disable(uart_dev);

	ahi_tx_dma_enabled = ahi_uart_tx_dma_driver_check();
	/* Register UART imterrupt handler */
	uart_irq_callback_user_data_set(uart_dev, ahi_uart_callback, NULL);

	uart_irq_rx_enable(uart_dev);
	/* Clear receive buffers */
	rx_msg.msg_len = 0;
	return 0;
}

void alif_ahi_init(msg_received_callback callback)
{
	receive_cb = callback;
}

static int ahi_uart_initialize(void)
{
	LOG_INF("ahi uart initialized");
	alif_ahi_reset();
	return 0;
}

SYS_INIT(ahi_uart_initialize, POST_KERNEL, 80);
