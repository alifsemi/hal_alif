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
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>
#include "dma_event_router.h"

/* Register HCI UART log module with standard UART log level */
LOG_MODULE_REGISTER(hci_uart, CONFIG_UART_LOG_LEVEL);

/* Define appropriate timeouts */
#define TX_TIMEOUT_US 400 /* 400us */
#define RX_TIMEOUT_US 10 /* 10us */

/* UART DMA request numbers from board-specific overlay */
#define DMA_UART_TX_GROUP   0  /* DMA group for UART TX */
#define DMA_UART_RX_GROUP   0  /* DMA group for UART RX */

#include "es0_power_manager.h"

/* change this to any other UART peripheral if desired */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_hci_uart)
static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* Wait for specific message from HCI */
static K_SEM_DEFINE(hci_tx_sem, 0, 1);

/* RX DMA is not optimal so it is disabled by default */
#define HCI_RX_DMA_ENABLED 0

#if HCI_RX_DMA_ENABLED
#define BUFF_SIZE 32
#define MY_RING_BUF_BYTES 256
RING_BUF_DECLARE(hci_ring_buf, MY_RING_BUF_BYTES);
static K_SEM_DEFINE(hci_rx_sem, 0, 1);
static bool pingpong;
static uint8_t rx_buffer[2][BUFF_SIZE];

struct hci_rx_job {
	struct k_work work;
	uint8_t *buf;
	uint32_t size;
	void (*callback)(void *, uint8_t);
	void *metadata;
};

K_KERNEL_STACK_DEFINE(hci_worker_stack, 2048);
static struct k_work_q hci_worker_queue;


static void hci_rx_handle_func(struct k_work *work)
{
	struct hci_rx_job *job = CONTAINER_OF(work, struct hci_rx_job, work);

	int remaining = job->size;

	while (0 < remaining) {
		/* Read Pending data */
		if (ring_buf_is_empty(&hci_ring_buf)) {
			k_sem_take(&hci_rx_sem, K_FOREVER);
		}

		int read_bytes = ring_buf_get(&hci_ring_buf, job->buf, remaining);
		if (read_bytes < 0) {
			continue;
		}
		remaining -= read_bytes;
		job->buf += read_bytes;
	}

	if (job->callback) {
		job->callback(job->metadata, ITF_STATUS_OK);
	}
}

static struct hci_rx_job hci_rx_job = {
	.work = Z_WORK_INITIALIZER(hci_rx_handle_func),
};
#endif


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
	bool dma_enabled;
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
};

/* receive buffer used in UART callback */
static uint8_t *rx_buf_ptr __noinit;
static uint32_t rx_buf_size __noinit;
static uint32_t rx_buf_len __noinit;

/* uart environment structure */
static struct uart_env_tag uart_env __noinit;

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

	switch (evt->type) {
	case UART_TX_DONE:
		/* TX completed successfully */
		LOG_DBG("UART TX completed successfully");
		k_sem_give(&hci_tx_sem);
		break;

	case UART_TX_ABORTED:
		/* TX was aborted */
		LOG_ERR("UART TX was aborted, sent %d bytes", evt->data.tx.len);
		callback = uart_env.tx.callback;
		data = uart_env.tx.dummy;
		k_sem_give(&hci_tx_sem);
		break;
#if HCI_RX_DMA_ENABLED
	case UART_RX_RDY:
		/* Data received and ready for processing */
		char *p_start = evt->data.rx.buf + evt->data.rx.offset;
		/* Push To ring buffer */
		ring_buf_put(&hci_ring_buf, p_start, evt->data.rx.len);
		LOG_DBG("Rxd %d , offset %d", evt->data.rx.len, evt->data.rx.offset);
		k_sem_give(&hci_rx_sem);
		break;

	case UART_RX_BUF_REQUEST:
		/* UART driver is requesting a new buffer for continuous reception */
		pingpong ^= true;
		uart_rx_buf_rsp(dev, rx_buffer[pingpong], BUFF_SIZE);
		break;

	case UART_RX_BUF_RELEASED:
		/* Buffer has been released */
		break;

	case UART_RX_DISABLED:
		/* RX has been disabled */
		uart_env.rx_enabled = false;
		break;

	case UART_RX_STOPPED:
		/* RX has been stopped due to error */
		LOG_ERR("UART RX stopped due to error: %d", evt->data.rx_stop.reason);
		uart_env.rx_enabled = false;
		break;
#endif
	default:
		LOG_ERR("Unknown UART event: %d", evt->type);
		break;
	}
}

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

static bool hci_uart_rx_dma_driver_check(void)
{
/* RX DMA is disabled until we find a proper solution */
#if CONFIG_UART_ASYNC_API && HCI_RX_DMA_ENABLED

	/* RX DMA is disabled because it not give great value yet */
#if DT_NODE_HAS_STATUS(DT_DMAS_CTLR_BY_NAME(DT_NODELABEL(uart_hci), rx), okay)
	const struct device *rxdma =
		DEVICE_DT_GET_OR_NULL(DT_DMAS_CTLR_BY_NAME(DT_NODELABEL(uart_hci), rx));

	if (rxdma) {

		if (device_is_ready(rxdma)) {
			int rx_config = DT_DMAS_CELL_BY_NAME(DT_NODELABEL(uart_hci), rx, periph);

			LOG_DBG("DMA RX event enable %d", rx_config);
			dma_event_router_configure(DMA_UART_RX_GROUP, rx_config, false);
			uart_callback_set(uart_dev, hci_uart_async_callback, NULL);

			return true;
		}
	}
#else
	LOG_ERR("NO RX DMA");
#endif
#endif
	uart_irq_rx_enable(uart_dev);
	uart_irq_callback_user_data_set(uart_dev, hci_uart_callback, NULL);
	return false;
}

static bool hci_uart_tx_dma_driver_check(void)
{
#ifdef CONFIG_UART_ASYNC_API
#if DT_NODE_HAS_STATUS(DT_DMAS_CTLR_BY_NAME(DT_NODELABEL(uart_hci), rx), okay)
	const struct device *txdma =
		DEVICE_DT_GET_OR_NULL(DT_DMAS_CTLR_BY_NAME(DT_NODELABEL(uart_hci), tx));

	if (txdma) {

		if (device_is_ready(txdma)) {
			int tx_config = DT_DMAS_CELL_BY_NAME(DT_NODELABEL(uart_hci), tx, periph);

			LOG_DBG("DMA TX event enable %d", tx_config);
			dma_event_router_configure(DMA_UART_RX_GROUP, tx_config, false);
			uart_callback_set(uart_dev, hci_uart_async_callback, NULL);
			return true;
		}
	}
#endif
#endif
	uart_irq_callback_user_data_set(uart_dev, hci_uart_callback, NULL);
	return false;
}

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

	/* Set the UART callback for async operations */
	uart_env.rx.dma_enabled = hci_uart_rx_dma_driver_check();
	uart_env.tx.dma_enabled = hci_uart_tx_dma_driver_check();
#if HCI_RX_DMA_ENABLED
	if (uart_env.rx.dma_enabled) {

		/* Create Receiver worker */
		k_work_queue_start(&hci_worker_queue, hci_worker_stack,
				   K_KERNEL_STACK_SIZEOF(hci_worker_stack),
				   CONFIG_ALIF_BLE_HOST_THREAD_PRIORITY - 1, NULL);
		k_thread_name_set(&hci_worker_queue.thread, "hci_worker");

		uart_rx_disable(uart_dev);
		uart_irq_rx_disable(uart_dev);
		
		int ret = uart_rx_enable(uart_dev, rx_buffer[pingpong], BUFF_SIZE, RX_TIMEOUT_US);
		
		if (ret < 0) {
			LOG_ERR("Failed to enable UART: %d", ret);
			return ret;
		}
		uart_env.rx_enabled = true;
	}
#endif

	/* We cannot initialize RX transfer callback here
	 * as that might be kept in retention and also when
	 * read operation is started a (new) callback is always set.
	 */
	uart_env.tx.callback = NULL;
	uart_env.rx.callback = NULL;
	return 0;
}

void hci_uart_read(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
	LOG_DBG("HCI UART read request: %d bytes", size);

	__ASSERT(bufptr != NULL, "Invalid buffer pointer");
	__ASSERT(size != 0, "Invalid size");
	__ASSERT(callback != NULL, "Invalid callback");

	/* Store callback and user data */
	uart_env.rx.callback = callback;
	uart_env.rx.dummy = dummy;

	/* Deassert&assert rts_n, falling edge triggers wake up the RF core */
	wake_es0(uart_dev);

	if (uart_env.rx.dma_enabled) {
#if HCI_RX_DMA_ENABLED
		/* Enable RX with DMA and timeout */
		if (!uart_env.rx_enabled) {
			int ret = uart_rx_enable(uart_dev, rx_buffer[pingpong], BUFF_SIZE,
						 RX_TIMEOUT_US);

			if (ret < 0) {
				LOG_ERR("Failed to enable UART RX: %d", ret);
				/* If enabling RX fails, call the callback with error */
				if (callback) {
					callback(dummy, ITF_STATUS_ERROR);
					uart_env.rx.callback = NULL;
					uart_env.rx.dummy = NULL;
				}
				return;
			}
			uart_env.rx_enabled = true;
		}

		hci_rx_job.buf = bufptr;
		hci_rx_job.size = size;
		hci_rx_job.callback = callback;
		hci_rx_job.metadata = dummy;
		k_work_submit_to_queue(&hci_worker_queue, &hci_rx_job.work);
#endif
	} else {
		rx_buf_ptr = bufptr;
		rx_buf_size = size;
		rx_buf_len = 0;
		uart_irq_rx_enable(uart_dev);
	}
}

void hci_uart_write(uint8_t *bufptr, uint32_t size, void (*callback)(void *, uint8_t), void *dummy)
{
	LOG_DBG("HCI UART write request: %d bytes", size);

	__ASSERT(bufptr != NULL, "Invalid buffer pointer");
	__ASSERT(size != 0, "Invalid size");
	__ASSERT(callback != NULL, "Invalid callback");

	/* Deassert&assert rts_n, falling edge triggers wake up the RF core */
	wake_es0(uart_dev);

	uart_env.tx.callback = callback;
	uart_env.tx.dummy = dummy;
	if (uart_env.tx.dma_enabled) {

		/* Start TX with DMA and timeout */
		int ret = uart_tx(uart_dev, bufptr, size, TX_TIMEOUT_US);

		if (ret < 0) {
			/* If starting TX fails, call the callback with error */
			if (callback != NULL) {
				callback(dummy, ITF_STATUS_ERROR);
				uart_env.tx.callback = NULL;
				uart_env.tx.dummy = NULL;
			}
			return;
		}
		if (k_sem_take(&hci_tx_sem, K_USEC(TX_TIMEOUT_US)) != 0) {

			LOG_ERR("TX SEM TO");
			if (callback != NULL) {
				callback(dummy, ITF_STATUS_ERROR);
				uart_env.tx.callback = NULL;
				uart_env.tx.dummy = NULL;
			}
			return;
		}
	} else {

		for (int i = 0; i < size; i++) {
			uart_poll_out(uart_dev, bufptr[i]);
		}
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
