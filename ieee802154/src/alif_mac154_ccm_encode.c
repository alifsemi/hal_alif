/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <zephyr/sys/byteorder.h>
#include "mbedtls/ccm.h"
#include "alif_mac154_key_storage.h"

int alif_mac154_ccm_encode_packet(struct alif_802154_frame_parser *mac_frame, uint8_t *mac64)
{
	int ret_val;

	mbedtls_ccm_context ccm;
	uint8_t nonce[13];
	struct alif_mac154_key_storage *key_info;
	struct alif_802154_ccm_params *ccm_params = &mac_frame->ccm_params;

	uint32_t sec_frame_counter;

	if (ccm_params->sec_level == MAC154_SECURITY_LEVEL_NONE) {
		return 0;
	}

	/* Search a key material */
	key_info = alif_mac154_key_storage_key_description_get(ccm_params->key_id_mode,
							       ccm_params->sec_key_source);
	if (!key_info) {
		return 0;
	}

	if (mac_frame->packet_ready) {
		sec_frame_counter = sys_get_le32(ccm_params->sec_frame_counter);

	} else {
		if (key_info->frame_counter_per_key) {
			sec_frame_counter = key_info->frame_counter++;
		} else {
			sec_frame_counter = alif_mac154_sec_frame_counter_get();
		}

		sys_put_le32(sec_frame_counter, ccm_params->sec_frame_counter);
		/* Copy mac Header and mark */
		mac_frame->packet_ready = true;
		if (mac_frame->net_packet_ptr) {
			memcpy(mac_frame->net_packet_ptr, mac_frame->mac_packet,
			       mac_frame->mac_header_length);
		}
	}

	for (int i = 0; i < 8; i++) {
		nonce[7 - i] = mac64[i];
	}

	if (!ccm_params->sec_frame_counter) {
		memset(nonce + 8, 0, 4);
	} else {
		for (int i = 0; i < 4; i++) {
			nonce[11 - i] = ccm_params->sec_frame_counter[i];
		}
	}
	nonce[12] = ccm_params->sec_level;

	mbedtls_ccm_init(&ccm);

	ret_val = mbedtls_ccm_setkey(&ccm, MBEDTLS_CIPHER_ID_AES, key_info->key_value, 128);
	if (ret_val < 0) {
		goto cleanup;
	}

	ret_val = mbedtls_ccm_encrypt_and_tag(&ccm, mac_frame->mac_payload_length, nonce, 13,
					      mac_frame->mac_packet, mac_frame->mac_header_length,
					      mac_frame->mac_payload, mac_frame->mac_payload,
					      ccm_params->mic, ccm_params->mic_len);
	if (ret_val == 0) {
		/* Mark packet encode */
		mac_frame->encoded_packet = true;
	}

cleanup:
	mbedtls_ccm_free(&ccm);
	return ret_val;
}
