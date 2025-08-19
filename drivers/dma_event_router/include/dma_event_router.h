/*
 * Copyright (c) 2025 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ALIF_DMA_EVENT_ROUTER_H
#define ALIF_DMA_EVENT_ROUTER_H
#include <stdint.h>

/* Event router configuration for UART DMA */
#define EVTRTR2_DMA_CTRL_ENA        (1U << 4)
#define EVTRTR2_DMA_CTRL_ACK_PERIPH (0x0 << 16)
#define EVTRTR2_DMA_CTRL_ACK_ROUTER (0x1 << 16)

int dma_event_router_configure(const uint32_t dma_group, const uint32_t dma_request, bool enable_handshake);

#endif /* ALIF_DMA_EVENT_ROUTER_H */
