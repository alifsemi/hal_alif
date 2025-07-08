/*
 * Copyright (C) 2024 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "ospi_hal.h"

#define HAL_OSPI_MAX_INST                   2
#define HAL_OSPI_INVALID_INST               -1
#define HAL_OSPI_AES_RX_DS_DELAY_REG_OFFSET 0x20

/** OSPI_Instance */
struct hal_ospi_inst {
	int8_t   is_avail;
	int8_t   cs_pin;
	uint32_t *regs;
	uint32_t  bus_speed;
	uint32_t  tx_fifo_threshold;
	uint32_t  rx_fifo_threshold;
	uint32_t  rx_sample_delay;
	uint32_t  ddr_drive_edge;
	uint32_t  core_clk;
	uint32_t  *aes_regs;

	/* Data Transfer */
	struct ospi_transfer transfer;

	/* XiP Config*/
	struct ospi_xip_config   xip_config;

	/* Event Notifier */
	hal_event_notify_cb *event_cb;
	void  *user_data;
};

/* Fixed Instances */
struct hal_ospi_inst
	g_ospi_instance[HAL_OSPI_MAX_INST] = {
		{.is_avail = 1},
		{.is_avail = 1}
	};

/* Helper : To fetch Instnace from Handle. */
static struct hal_ospi_inst *get_inst_by_handle(HAL_OSPI_Handle_T handle)
{
	if (handle >= HAL_OSPI_MAX_INST)
		return NULL;

	return &(g_ospi_instance[handle]);
}

/**
 * \fn          alif_hal_ospi_initialize
 * \brief       Get Instance and Initialized with given parameter
 * \param[out]  handle  Instance handler
 * \param[in]   init_d  Instance's initial values
 * \return      0 on Success, else error code
 */
int32_t alif_hal_ospi_initialize(HAL_OSPI_Handle_T *handle,
				struct ospi_init *init_d)
{
	struct ospi_regs       *ospi_regs;
	struct ospi_aes_regs   *aes_regs;
	int32_t         i  = 0;

	struct hal_ospi_inst *ospi_inst;

	if (handle  == NULL || init_d == NULL)
		return OSPI_ERR_INVALID_PARAM;

	if (init_d->tx_fifo_threshold > OSPI_TX_FIFO_DEPTH)
		return OSPI_ERR_INVALID_PARAM;

	if (init_d->rx_fifo_threshold > OSPI_RX_FIFO_DEPTH)
		return OSPI_ERR_INVALID_PARAM;

	/*Initialize the Handle*/
	*handle = HAL_OSPI_INVALID_INST;

	/* Get the free instance */
	for (i = 0; i < HAL_OSPI_MAX_INST ; i++) {
		if (g_ospi_instance[i].is_avail) {
			*handle = i;
			break;
		}
	}

	if (i == HAL_OSPI_MAX_INST)
		return OSPI_ERR_INVALID_HANDLE;

	/* Re-initialize */
	ospi_inst = &g_ospi_instance[i];

	ospi_inst->is_avail = 0;
	ospi_inst->cs_pin = init_d->cs_pin;
	ospi_inst->regs = init_d->base_regs;
	ospi_inst->bus_speed = init_d->bus_speed;
	ospi_inst->core_clk  = init_d->core_clk;
	ospi_inst->ddr_drive_edge = init_d->ddr_drive_edge;
	ospi_inst->rx_fifo_threshold = init_d->rx_fifo_threshold;
	ospi_inst->tx_fifo_threshold = init_d->tx_fifo_threshold;
	ospi_inst->event_cb = init_d->event_cb;
	ospi_inst->user_data = init_d->user_data;
	ospi_inst->aes_regs = init_d->aes_regs;

	/* Clear Transfer Object */
	memset(&ospi_inst->transfer, 0, sizeof(struct ospi_transfer));

	/* Clear XiP Configuration Object */
	memset(&ospi_inst->xip_config, 0, sizeof(struct ospi_xip_config));

	/* XiP Configs */
	ospi_inst->xip_config.wrap_cmd = init_d->xip_wrap_cmd;
	ospi_inst->xip_config.incr_cmd = init_d->xip_incr_cmd;
	ospi_inst->xip_config.xip_cs_pin = init_d->cs_pin;
	ospi_inst->xip_config.xip_cnt_time_out = init_d->xip_cnt_time_out;
	ospi_inst->xip_config.aes_rx_ds_dlay = init_d->rx_ds_delay;
	ospi_inst->xip_config.xip_rxds_vl_en = init_d->xip_rxds_vl_en;
	ospi_inst->xip_config.xip_wait_cycles = init_d->xip_wait_cycles;

	ospi_regs = (struct ospi_regs *) init_d->base_regs;

	ospi_set_tx_threshold(ospi_regs, init_d->tx_fifo_threshold);

	ospi_set_rx_threshold(ospi_regs, init_d->rx_fifo_threshold);

	ospi_set_rx_sample_delay(ospi_regs, init_d->rx_sample_delay);

	ospi_set_ddr_drive_edge(ospi_regs, init_d->ddr_drive_edge);

	ospi_mask_interrupts(ospi_regs);

	ospi_mode_master(ospi_regs);

	ospi_set_bus_speed(ospi_regs, init_d->bus_speed, init_d->core_clk);

	aes_regs = (struct ospi_aes_regs *) init_d->aes_regs;

	aes_regs->AES_RXDS_DLY = init_d->rx_ds_delay;

	switch (init_d->baud2_delay) {
	case OSPI_BAUD2_DELAY_DISABLE:
		ospi_aes_set_baud2_delay(aes_regs, false);
		break;
	case OSPI_BAUD2_DELAY_ENABLE:
		ospi_aes_set_baud2_delay(aes_regs, true);
		break;

	case OSPI_BAUD2_DELAY_AUTO:
	    /* 'automatic' setting based on divisor */
		ospi_aes_set_baud2_delay(aes_regs, ospi_regs->OSPI_BAUDR == 2);
		break;
	default:
		return OSPI_ERR_INVALID_PARAM;
	}

	return OSPI_ERR_NONE;
}

/**
 * \fn          alif_hal_ospi_deInit
 * \brief       Release the initialized the instance.
 * \param[in]   handle  Instance handler
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_deinit(HAL_OSPI_Handle_T handle)
{
	struct hal_ospi_inst *ospi_inst;

	ospi_inst = get_inst_by_handle(handle);

	if (ospi_inst == NULL)
		return OSPI_ERR_INVALID_HANDLE;

	ospi_inst->is_avail = 1;

	/* Clear rest. */
	ospi_inst->cs_pin = -1;
	ospi_inst->regs = 0;
	ospi_inst->bus_speed = 0;
	ospi_inst->core_clk  = 0;
	ospi_inst->ddr_drive_edge = 0;
	ospi_inst->rx_fifo_threshold = 0;
	ospi_inst->tx_fifo_threshold = 0;

	/* Clear Transfer Object */
	memset(&ospi_inst->transfer, 0, sizeof(struct ospi_transfer));

	/* Clear XiP Configuration Object */
	memset(&ospi_inst->xip_config, 0, sizeof(struct ospi_xip_config));

	return  OSPI_ERR_NONE;
}

/**
 * \fn          alif_hal_ospi_prepare_transfer
 * \brief       Configure the OSPI instance before any transreceive
 * \param[in]   handle  Instance handler
 * \param[in]   trans_conf  Configuration values
 * \return      0 on Success, else error code
 */
int32_t alif_hal_ospi_prepare_transfer(HAL_OSPI_Handle_T handle,
				struct ospi_trans_config *trans_conf)
{
	struct hal_ospi_inst *ospi_inst;
	struct ospi_regs *ospi_regs;

	ospi_inst = get_inst_by_handle(handle);

	if (trans_conf == NULL)
		return OSPI_ERR_INVALID_PARAM;

	if (ospi_inst == NULL)
		return OSPI_ERR_INVALID_HANDLE;

	ospi_regs = (struct ospi_regs *) ospi_inst->regs;

	ospi_set_dfs(ospi_regs, trans_conf->frame_size);

	ospi_inst->transfer.addr_len = trans_conf->addr_len;
	ospi_inst->transfer.inst_len = trans_conf->inst_len;
	ospi_inst->transfer.dummy_cycle = trans_conf->wait_cycles;
	ospi_inst->transfer.spi_frf = trans_conf->frame_format;
	ospi_inst->transfer.ddr = trans_conf->ddr_enable;

	return  OSPI_ERR_NONE;
}

/**
 * \fn          alif_hal_ospi_cs_enable
 * \brief       Activate or De-active connected Slave device.
 * \param[in]   handle  Instance handler
 * \param[in]   enable  0 / 1
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_cs_enable(HAL_OSPI_Handle_T handle, int enable)
{
	struct hal_ospi_inst *ospi_inst;

	ospi_inst = get_inst_by_handle(handle);
	if (ospi_inst == NULL)
		return OSPI_ERR_INVALID_HANDLE;

	if (ospi_busy((struct ospi_regs *) ospi_inst->regs))
		return OSPI_ERR_CTRL_BUSY;

	ospi_control_ss((struct ospi_regs *) ospi_inst->regs, ospi_inst->cs_pin,
		(enable == 1 ? SPI_SS_STATE_ENABLE : SPI_SS_STATE_DISABLE));

	return OSPI_ERR_NONE;
}

/**
 * \fn          alif_hal_ospi_send
 * \brief       Transfer the data
 * \param[in]   handle  Instance handler
 * \param[in]   data_out  Transmit data buffer
 * \param[in]   num  length of the buffer
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_send(HAL_OSPI_Handle_T handle, void *data, int num)
{
	struct hal_ospi_inst *ospi_inst;

	ospi_inst = get_inst_by_handle(handle);
	if (ospi_inst == NULL)
		return OSPI_ERR_INVALID_HANDLE;

	if (num <= 0)
		return OSPI_ERR_INVALID_PARAM;

	if (ospi_busy((struct ospi_regs *) ospi_inst->regs))
		return OSPI_ERR_CTRL_BUSY;

	/* Update Transfer Settings */
	ospi_inst->transfer.tx_total_cnt = num;
	ospi_inst->transfer.mode = SPI_TMOD_TX;
	ospi_inst->transfer.tx_buff = data;
	ospi_inst->transfer.tx_current_cnt = 0;
	ospi_inst->transfer.status = SPI_TRANSFER_STATUS_NONE;

	/* Send */
	ospi_send((struct ospi_regs *)ospi_inst->regs, &(ospi_inst->transfer));

	return OSPI_ERR_NONE;
}

/**
 * \fn          alif_hal_ospi_transfer
 * \brief       Send and Receive data
 * \param[in]   handle  Instance handler
 * \param[in]   data_out  Transmit data buffer
 * \param[in]   data_in  Receive data buffer
 * \param[in]   num  length of the buffer
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_transfer(HAL_OSPI_Handle_T handle,
			void *data_out, void *data_in, int num)
{
	struct hal_ospi_inst *ospi_inst;

	ospi_inst = get_inst_by_handle(handle);
	if (ospi_inst == NULL)
		return OSPI_ERR_INVALID_HANDLE;

	if (ospi_busy((struct ospi_regs *) ospi_inst->regs))
		return OSPI_ERR_CTRL_BUSY;

	ospi_inst->transfer.rx_total_cnt   = num;
	ospi_inst->transfer.mode           = SPI_TMOD_TX_AND_RX;

	/* Tx total count based on address length */
	if (ospi_inst->transfer.addr_len == OSPI_ADDR_LENGTH_0_BITS)
		ospi_inst->transfer.tx_total_cnt = 1;
	else if (ospi_inst->transfer.addr_len == OSPI_ADDR_LENGTH_24_BITS)
		ospi_inst->transfer.tx_total_cnt = 4;
	else if (ospi_inst->transfer.addr_len == OSPI_ADDR_LENGTH_32_BITS)
		ospi_inst->transfer.tx_total_cnt = 2;

	ospi_inst->transfer.tx_buff        = data_out;
	ospi_inst->transfer.rx_buff        = data_in;
	ospi_inst->transfer.tx_current_cnt = 0;
	ospi_inst->transfer.rx_current_cnt = 0;
	ospi_inst->transfer.status         = SPI_TRANSFER_STATUS_NONE;

	ospi_transfer((struct ospi_regs *)ospi_inst->regs,
			&(ospi_inst->transfer));

	return OSPI_ERR_NONE;
}

/**
 * \fn          alif_hal_ospi_irq_handler
 * \brief       Interrupt Handler for OSPI interface.
 * \param[in]   handle  Instance handler
 * \param[out]  event_status  Status of transreceive status
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_irq_handler(HAL_OSPI_Handle_T handle)
{
	struct hal_ospi_inst *ospi_inst;

	ospi_inst = get_inst_by_handle(handle);

	struct ospi_regs *ospi_reg = (struct ospi_regs *) ospi_inst->regs;

	ospi_inst->transfer.status = SPI_TRANSFER_STATUS_NONE;

	ospi_irq_handler(ospi_reg, &ospi_inst->transfer);

	if (ospi_inst->transfer.status == SPI_TRANSFER_STATUS_COMPLETE) {

		ospi_inst->transfer.status = SPI_TRANSFER_STATUS_NONE;

		/* update event Status */
		ospi_inst->event_cb(OSPI_EVENT_TRANSFER_COMPLETE,
						ospi_inst->user_data);
	}

	if (ospi_inst->transfer.status == SPI_TRANSFER_STATUS_OVERFLOW ||
		ospi_inst->transfer.status == SPI_TRANSFER_STATUS_RX_UNDERFLOW) {

		ospi_inst->transfer.status = SPI_TRANSFER_STATUS_NONE;

		/* update event Status */
		ospi_inst->event_cb(OSPI_EVENT_DATA_LOST, ospi_inst->user_data);
	}

	return OSPI_ERR_NONE;
}

/**
 * \fn          alif_hal_ospi_receive
 * \brief       Send and Receive data
 * \param[in]   handle  Instance handler
 * \param[in]   data_out  Receive data buffer
 * \param[in]   num  length of the buffer to receive
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_receive(HAL_OSPI_Handle_T handle,
				void *data_out, int num)
{
	/*TODO*/
	return OSPI_ERR_NONE;
}


/**
 * \fn          alif_hal_ospi_xip_enable
 * \brief       Enable XiP.
 * \param[in]   handle  Instance handlerd
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_xip_enable(HAL_OSPI_Handle_T handle)
{
	struct hal_ospi_inst *ospi_inst;

	ospi_inst = get_inst_by_handle(handle);
	if (ospi_inst == NULL)
		return OSPI_ERR_INVALID_HANDLE;

	ospi_control_ss((struct ospi_regs *) ospi_inst->regs,
			ospi_inst->cs_pin, SPI_SS_STATE_DISABLE);

	ospi_xip_enable((struct ospi_regs *) ospi_inst->regs,
			(struct ospi_aes_regs *) ospi_inst->aes_regs,
			&ospi_inst->xip_config);

	ospi_control_ss((struct ospi_regs *) ospi_inst->regs,
			ospi_inst->cs_pin, SPI_SS_STATE_ENABLE);

	return OSPI_ERR_NONE;
}

/**
 * \fn          alif_hal_ospi_xip_disable
 * \brief       Disable XiP.
 * \param[in]   handle  Instance handlerd
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_xip_disable(HAL_OSPI_Handle_T handle)
{
	struct hal_ospi_inst *ospi_inst;

	ospi_inst = get_inst_by_handle(handle);
	if (ospi_inst == NULL)
		return OSPI_ERR_INVALID_HANDLE;

	ospi_control_ss((struct ospi_regs *) ospi_inst->regs,
			ospi_inst->cs_pin, SPI_SS_STATE_DISABLE);


	ospi_xip_disable((struct ospi_regs *) ospi_inst->regs,
			(struct ospi_aes_regs *) ospi_inst->aes_regs,
			&ospi_inst->transfer, &ospi_inst->xip_config);

	ospi_control_ss((struct ospi_regs *) ospi_inst->regs,
			ospi_inst->cs_pin, SPI_SS_STATE_ENABLE);

	return OSPI_ERR_NONE;
}
