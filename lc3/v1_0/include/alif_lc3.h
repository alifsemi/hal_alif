/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ALIF_LC3_H
#define _ALIF_LC3_H

#include "lc3_api.h"

/**
 * @brief Initialise the Alif LC3 codec. The codec must be initialised before any other functions
 *        from lc3_api.h can be used. If a patch for the ROM image is available and enabled, it
 *        will be applied as part of this call.
 *
 * @return 0 on success, any other value indicates an error
 */
static inline int alif_lc3_init(void)
{
#ifdef CONFIG_ALIF_LC3_CODEC_PATCHING
	extern const uint32_t __lc3_patch_info_start;
	void const *patch = &__lc3_patch_info_start;
#else
	void const *patch = NULL;
#endif

	return lc3_api_rom_init(patch);
}

#endif /* _ALIF_LC3_H */
