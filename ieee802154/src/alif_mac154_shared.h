/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ALIF_MAC154_SHARED_H_
#define ALIF_MAC154_SHARED_H_

/*Shared memory base*/
#define ALIF_MAC154_SHARED_BASE_ADDR (0x501DB000)

#define ALIF_MAC154_SHARED_PROMISCUOUS_MODE                                                        \
	(*(volatile bool *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x00))

#define ALIF_MAC154_SHARED_TX_POWER_DBM (*(volatile int8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x01))

#define ALIF_MAC154_SHARED_LAST_RSSI_DBM (*(volatile int8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x02))

#define ALIF_MAC154_SHARED_SHORT_ID (*(volatile uint16_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x04))

#define ALIF_MAC154_SHARED_LONG_ID ((volatile uint8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x06))

#define ALIF_MAC154_SHARED_PAN_ID (*(volatile uint16_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x18))

#define ALIF_MAC154_SHARED_PRIO_TX (*(volatile uint8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x1a))

#define ALIF_MAC154_SHARED_PRIO_RX (*(volatile uint8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x1b))

#define ALIF_MAC154_SHARED_PRIO_ED (*(volatile uint8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x1c))

#define ALIF_MAC154_SHARED_CCA_MODE (*(volatile uint8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x300))

#define ALIF_MAC154_SHARED_CCA_THR (*(volatile int8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x304))

#define ALIF_MAC154_SHARED_CCA_MODE_1_1                                                            \
	(*(volatile uint8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x1d))

#define ALIF_MAC154_SHARED_CCA_THR_1_1 (*(volatile int8_t *)(ALIF_MAC154_SHARED_BASE_ADDR + 0x21))

#endif /* ALIF_MAC154_SHARED_H_ */
