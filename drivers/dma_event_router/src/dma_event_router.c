/*
 * Copyright (c) 2025 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/types.h>
#include "dma_event_router.h"
#include "soc.h"

int dma_event_router_configure(const uint32_t dma_group, const uint32_t dma_request, bool enable_handshake)
{
	uint32_t regdata;

	if (dma_group > 3) {
		return -EINVAL;
	}

	if (dma_request > 31) {
		return -EINVAL;
	}

	/* Enable event router channel */
	regdata = EVTRTR2_DMA_CTRL_ENA | EVTRTR2_DMA_CTRL_ACK_PERIPH | dma_group;
	sys_write32(regdata, EVTRTRLOCAL_DMA_CTRL0 + (dma_request * 0x4));

	/* DMA Handshake disable */
	regdata = sys_read32(EVTRTRLOCAL_DMA_ACK_TYPE0 + (dma_group * 0x4));
	if (enable_handshake) {
		regdata |= (0x1 << dma_request);
	} else {
		regdata &= ~(0x1 << dma_request);
	}
	regdata &= ~(0x1 << dma_request);
	sys_write32(regdata, EVTRTRLOCAL_DMA_ACK_TYPE0 + (dma_group * 0x4));

	return 0;
}
