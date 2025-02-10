/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2024 Alif Semiconductor.
 */

#include <stdint.h>
#include <soc_memory_map.h>
#include <cmsis_core.h>

uint64_t ethosu_address_remap(uint64_t address, int index)
{
	(void)index;

	/* Double cast to avoid build warning about pointer/integer size mismatch */
	return local_to_global((void *) (uint32_t) address);
}

void ethosu_flush_dcache(uint32_t *p, size_t bytes)
{
       if (p) {
               SCB_CleanDCache_by_Addr(p, bytes);
       } else {
               SCB_CleanDCache();
       }
}

void ethosu_invalidate_dcache(uint32_t *p, size_t bytes)
{
       if (p) {
               SCB_InvalidateDCache_by_Addr(p, bytes);
       } else {
               SCB_InvalidateDCache();
       }
}

