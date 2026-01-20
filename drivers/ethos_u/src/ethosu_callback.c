/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2024 Alif Semiconductor.
 */

#include <stdint.h>
#include <soc_memory_map.h>
#include <cmsis_core.h>

#define GLOBAL_CLEAN_INVALIDATE_THRESHOLD_SIZE (128 * 1024)

typedef struct address_range {
	uintptr_t base;
	uintptr_t limit;
} address_range_t;

static const address_range_t no_need_to_invalidate_or_flush_areas[] = {
	/* TCM is never cached */
	{
		.base = DT_REG_ADDR(DT_NODELABEL(itcm)),
		.limit = DT_REG_ADDR(DT_NODELABEL(itcm)) + DT_REG_SIZE(DT_NODELABEL(itcm)) - 1,
	},
	{
		.base = DT_REG_ADDR(DT_NODELABEL(dtcm)),
		.limit = DT_REG_ADDR(DT_NODELABEL(dtcm)) + DT_REG_SIZE(DT_NODELABEL(dtcm)) - 1,
	},
	/* MRAM should never change while running */
	{
		.base = CONFIG_FLASH_BASE_ADDRESS,
		.limit = CONFIG_FLASH_BASE_ADDRESS + (CONFIG_FLASH_SIZE * 1024) - 1,
	}};


static bool area_needs_invalidate_or_flush(const void *p, size_t bytes)
{
	if (bytes == 0) {
		return false;
	}

	uintptr_t base = (uintptr_t)p;
	uintptr_t limit = base + bytes - 1;
	size_t nn_flush_size = ARRAY_SIZE(no_need_to_invalidate_or_flush_areas);

	for (size_t i = 0; i < nn_flush_size; i++) {
		if (base >= no_need_to_invalidate_or_flush_areas[i].base &&
		    limit <= no_need_to_invalidate_or_flush_areas[i].limit) {
			return false;
		}
	}

	return true;
}

bool __attribute__((weak)) ethosu_area_needs_invalidate_dcache(const void *p, size_t bytes)
{
	/* Passing NULL as p argument expects the whole cache to be flushed. */
	if (!p) {
		return true;
	}

	return area_needs_invalidate_or_flush(p, bytes);
}

bool __attribute__((weak)) ethosu_area_needs_flush_dcache(const void *p, size_t bytes)
{
	/* Passing NULL as p argument expects the whole cache to be flushed. */
	if (!p) {
		return true;
	}

	return area_needs_invalidate_or_flush(p, bytes);
}

uint64_t ethosu_address_remap(uint64_t address, int index)
{
	(void)index;

	/* Double cast to avoid build warning about pointer/integer size mismatch */
	return local_to_global((void *) (uint32_t) address);
}

void ethosu_flush_dcache(uint32_t *p, size_t bytes)
{
	if (p && ethosu_area_needs_flush_dcache(p, bytes)) {
		if (bytes <= GLOBAL_CLEAN_INVALIDATE_THRESHOLD_SIZE) {
			/*
			 * For small ranges, do what we were told, but if excessively big,
			 * faster to do a full cache operation
			 */
			SCB_CleanDCache_by_Addr(p, bytes);
		} else {
			SCB_CleanDCache();
		}
	} else {
		__DSB();
	}
}

void ethosu_invalidate_dcache(uint32_t *p, size_t bytes)
{
	if (p && ethosu_area_needs_invalidate_dcache(p, bytes)) {
		if (bytes <= GLOBAL_CLEAN_INVALIDATE_THRESHOLD_SIZE) {
			/*
			 * For small ranges, do what we were told, but if excessively big,
			 * faster to do a full cache operation
			 */
			SCB_InvalidateDCache_by_Addr(p, bytes);
		} else {
			/*
			 * Not safe to globally invalidate without cleaning
			 * unless we know there are no write-back areas in the system
			 */
			SCB_CleanInvalidateDCache();
		}
	} else {
		__DSB();
	}
}
