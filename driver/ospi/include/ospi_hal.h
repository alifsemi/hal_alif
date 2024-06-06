/*
 * Copyright (C) 2024 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OSPI_ALIF_HAL_H__
#define __OSPI_ALIF_HAL_H__

#include "ospi.h"


/*----- OSPI Custom Control codes -----*/

#define OSPI_ADDR_LENGTH_0_BITS            0x0     /* Address Length  0 bits */
#define OSPI_ADDR_LENGTH_8_BITS            0x2     /* Address Length  8 bits */
#define OSPI_ADDR_LENGTH_24_BITS           0x6     /* Address Length 24 bits */
#define OSPI_ADDR_LENGTH_32_BITS           0x8     /* Address Length 32 bits */

#define OSPI_DDR_DISABLE                   0x0     /* Disable DDR Mode */
#define OSPI_DDR_ENABLE                    0x1     /* Enable DDR Mode */

#define OSPI_FRF_STANDRAD                  0x0      /* 0x0 Standard OSPI Format */
#define OSPI_FRF_DUAL                      0x1      /* 0x1 Dual OSPI Format */
#define OSPI_FRF_QUAD                      0x2      /* 0X2 Quad OSPI Format */
#define OSPI_FRF_OCTAL                     0x3      /* 0X2 Octal OSPI Format */

/*---- OSPI Slave Select Signal definitions ----*/
#define OSPI_SS_INACTIVE                   0UL      /* OSPI Slave Select Signal Inactive */
#define OSPI_SS_ACTIVE                     1UL      /* OSPI Slave Select Signal Active */

/*---- OSPI Event ---------------------*/
#define OSPI_EVENT_TRANSFER_COMPLETE (1UL << 0)     /* Data Transfer completed */
#define OSPI_EVENT_DATA_LOST         (1UL << 1)     /* Data lost: Rx overflow / Tx underflow */
#define OSPI_EVENT_MODE_FAULT        (1UL << 2)     /* Master Mode Fault */

/*---- OSPI Event ---------------------*/
#define OSPI_ERR_NONE               0               /* Success */
#define OSPI_ERR_INVALID_PARAM      -100            /* Parameter Invalid */
#define OSPI_ERR_INVALID_STATE      -101            /* State Invalid */
#define OSPI_ERR_CTRL_BUSY          -102            /* Controller Busy */
#define OSPI_ERR_INVALID_HANDLE     -103            /* Handler not valid */

/*---- OSPI Status ---------------------*/
struct ospi_status {
	uint32_t busy       : 1;              /* Transmitter/Receiver busy flag */
	uint32_t data_lost  : 1;              /* Data lost: Rx overflow / Tx underflow */
	uint32_t mode_fault : 1;              /* Mode fault detected; optional */
	uint32_t reserved   : 29;
};

/*---- OSPI State ---------------------*/
struct ospi_driver_state {
	uint32_t initialized: 1;                /* SPI driver initialized status */
	uint32_t powered    : 1;                /* SPI driver powered status */
	uint32_t reserved   : 30;               /* Reserved */
};

/*---- HAL_OSPI Instance Handler ---------------*/
typedef int8_t HAL_OSPI_Handle_T;

typedef void hal_event_notify_cb(uint32_t event, void *u_data);

/*---- OSPI Event ---------------------*/
struct ospi_init {
	uint32_t  bus_speed;                    /* Contrllor speed. */
	uint32_t  tx_fifo_threshold;            /* Tx Fifo Threshold */
	uint32_t  rx_fifo_threshold;            /* Rx Fifo Threshold */
	uint32_t  rx_sample_delay;              /* Rx Sample Delay */
	uint32_t  ddr_drive_edge;               /* DDR Drive Edge */
	uint32_t  core_clk;                     /* System Clock */
	uint32_t  cs_pin;                       /* Slave Pin */
	uint32_t  rx_ds_delay;                  /* Rx-DS Delay */

	uint32_t  *base_regs;                   /* OSPI REG */
	uint32_t  *aes_regs;                    /* AES REG */

	void      *user_data;                   /* User data*/
	hal_event_notify_cb *event_cb;          /* Event Callback*/
};

/*---- OSPI Event ---------------------*/
struct ospi_trans_config {
	uint8_t  frame_size;            /* Data Frame Size [8, 16, 32] */
	uint8_t  frame_format;          /* STD, Dual, Quad and Octal */
	uint8_t  trans_type;            /* Extended Transer Frame Type */
	uint8_t  addr_len;              /* Address Length */
	uint8_t  inst_len;              /* Instruction Length */
	uint8_t  wait_cycles;           /* Number of Wait Cycles */
	uint8_t  ddr_enable;            /* Enable DDR Mode */
	uint8_t  rx_ds_enable;          /* Read Data Strobe Enable */
};

/**
 * \fn          alif_hal_ospi_initialize
 * \brief       Get Instance and Initialized with given parameter
 * \param[out]  handle  Instance handler
 * \param[in]   init_d  Instance's initial values
 * \return      0 on Success, else error code
 */
int32_t alif_hal_ospi_initialize(HAL_OSPI_Handle_T *handle,
				struct ospi_init *init_d);

/**
 * \fn          alif_hal_ospi_prepare_transfer
 * \brief       Configure the OSPI instance before any transreceive
 * \param[in]   handle  Instance handler
 * \param[in]   trans_conf  Configuration values
 * \return      0 on Success, else error code
 */
int32_t alif_hal_ospi_prepare_transfer(HAL_OSPI_Handle_T handle,
			struct ospi_trans_config *trans_conf);

/**
 * \fn          alif_hal_ospi_cs_enable
 * \brief       Activate or De-active connected Slave device.
 * \param[in]   handle  Instance handler
 * \param[in]   enable  0 / 1
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_cs_enable(HAL_OSPI_Handle_T handle, int enable);

/**
 * \fn          alif_hal_ospi_send
 * \brief       Transfer the data
 * \param[in]   handle  Instance handler
 * \param[in]   data_out  Transmit data buffer
 * \param[in]   num  length of the buffer
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_send(HAL_OSPI_Handle_T handle,
			void *data_out, int num);

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
			void *data_out, void *data_in, int num);

/**
 * \fn          alif_hal_ospi_receive
 * \brief       Send and Receive data
 * \param[in]   handle  Instance handler
 * \param[in]   data_out  Receive data buffer
 * \param[in]   num  length of the buffer
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_receive(HAL_OSPI_Handle_T handle,
				void *data_out, int num);

/**
 * \fn          alif_hal_ospi_irq_handler
 * \brief       Interrupt Handler for OSPI interface.
 * \param[in]   handle  Instance handlerd
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_irq_handler(HAL_OSPI_Handle_T handle);

/**
 * \fn          alif_hal_ospi_deinit
 * \brief       Release the initialized instance.
 * \param[in]   handle  Instance handler
 * \return      0 on Success, else error code.
 */
int32_t alif_hal_ospi_deinit(HAL_OSPI_Handle_T handle);

#endif /*__OSPI_ALIF_HAL_H__ */