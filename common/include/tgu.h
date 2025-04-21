/*
 * Copyright (c) 2025 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TGU_H
#define TGU_H
#include <stdint.h>
#include <limits.h>

#define ITGU_BASE                 (0xE001E500UL)
#define ITGU_CTRL                 (ITGU_BASE + 0x0)
#define ITGU_CFG                  (ITGU_BASE + 0x4)
#define ITGU_LUT(n)               (ITGU_BASE + 0x10 + 4 * n)

#define DTGU_BASE                 (0xE001E600UL)
#define DTGU_CTRL                 (DTGU_BASE + 0x0)
#define DTGU_CFG                  (DTGU_BASE + 0x4)
#define DTGU_LUT(n)               (DTGU_BASE + 0x10 + 4 * n)
#define TGU_LUT(base, n)          ((base) + 0x10 + 4 * (n))

#define DTGU_CFG_BLKSZ            (0xFU << 0)
#define ITGU_CFG_BLKSZ            (0xFU << 0)

#define SET_BIT_RANGE(from, to) (~0U >> ((CHAR_BIT * sizeof(unsigned int)) - (to) - 1)) \
								& (~0U << (from))

enum tcm_type {ITCM, DTCM};

struct mem_region {
	uint32_t start;
	uint32_t end;
	enum tcm_type type;
};

/*
 * TGU_Setup()
 * Set up the TGU look up tables as per the information provided by
 * the platform in ns_regions array.
 */
void TGU_Setup(void);
#endif /* TGU_H */
