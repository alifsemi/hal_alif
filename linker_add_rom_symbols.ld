/*
 * SPDX-License-Identifier: Apache-2.0
 */


#include <zephyr/linker/sections.h>
#include <zephyr/devicetree.h>

#include <zephyr/linker/linker-defs.h>
#include <zephyr/linker/linker-tool.h>

#include <zephyr/arch/arm/cortex_m/scripts/linker.ld>

/* Symbols in ROM */

#if defined(CONFIG_ALIF_BLE_ROM_IMAGE_V1_2) && CONFIG_ALIF_BLE_ROM_IMAGE_V1_2
#include "lc3/v1_2/rom_symbols_lc3.lds"
#include "ble/v1_2/rom_symbols_ble.lds"
#else
#error "wrong ROM version"
#endif
