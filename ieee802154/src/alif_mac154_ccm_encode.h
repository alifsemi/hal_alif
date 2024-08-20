/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "alif_mac154_def.h"

/**
 * @brief Do CCM encode for give mac frame.
 *
 * @param[in]	mac_frame Pointer to mac frame structure
 * @param[in]   mac64 Device 64-bit mac address
 *
 * @return 	0 Encode ok
 * @return      <0 Encode fail
 *
 */
int alif_mac154_ccm_encode_packet(struct alif_802154_frame_parser *mac_frame, uint8_t *mac64);
