/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef SOC_MEMORY_MAP_H
#define SOC_MEMORY_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/devicetree.h>

#define ITCM_BASE (DT_REG_ADDR(DT_NODELABEL(itcm)))
#define ITCM_SIZE (DT_REG_SIZE(DT_NODELABEL(itcm)))
#define DTCM_BASE (DT_REG_ADDR(DT_NODELABEL(dtcm)))
#define DTCM_SIZE (DT_REG_SIZE(DT_NODELABEL(dtcm)))
#define ITCM_GLOBAL_BASE DT_PROP(DT_NODELABEL(itcm), global_base)
#define DTCM_GLOBAL_BASE DT_PROP(DT_NODELABEL(dtcm), global_base)

static inline uint32_t local_to_global(const volatile void *local_addr)
{
	uint32_t addr = (uint32_t)local_addr;

	if ((addr >= DTCM_BASE) && (addr < (DTCM_BASE + DTCM_SIZE))) {
		return (addr - DTCM_BASE + DTCM_GLOBAL_BASE);
	} else if ((addr < (ITCM_BASE + ITCM_SIZE))) {
		return (addr - ITCM_BASE + ITCM_GLOBAL_BASE);
	} else {
		return (addr);
	}
}

#ifdef __cplusplus
}
#endif
#endif /* SOC_MEMORY_MAP_H */
