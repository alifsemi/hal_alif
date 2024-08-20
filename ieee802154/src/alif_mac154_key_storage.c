/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "alif_mac154_key_storage.h"

static struct alif_mac154_key_storage mac_sec_key_storage[MAC_KEY_STORAGE_SIZE];
static int mac_sec_key_storage_size;
static uint32_t mac_sec_frame_counter;

void alif_mac154_sec_frame_counter_set(uint32_t frame_counter)
{
	mac_sec_frame_counter = frame_counter;
}

uint32_t alif_mac154_sec_frame_counter_get(void)
{
	return mac_sec_frame_counter++;
}

int alif_mac154_key_storage_key_description_set(struct alif_mac154_key_description *key_desc_list,
						int list_size)
{
	if (list_size > MAC_KEY_STORAGE_SIZE) {
		return -1;
	}

	mac_sec_key_storage_size = list_size;

	for (int i = 0; i < list_size; i++) {

		memcpy(mac_sec_key_storage[i].key_value, key_desc_list[i].key_value,
		       MAC_SEC_KEY_SIZE);
		memcpy(mac_sec_key_storage[i].key_id, key_desc_list[i].key_id,
		       IEEE_MAC_KEY_SOURCE_MAX_SIZE);
		mac_sec_key_storage[i].frame_counter = key_desc_list[i].frame_counter;
		mac_sec_key_storage[i].frame_counter_per_key =
			key_desc_list[i].frame_counter_per_key;
		mac_sec_key_storage[i].key_id_mode = key_desc_list[i].key_id_mode;
	}

	return 0;
}

struct alif_mac154_key_storage *
alif_mac154_key_storage_key_description_get(enum mac154_sec_keyid_mode key_id_mode, uint8_t *key_id)
{
	int length = 0;

	switch (key_id_mode) {
	case MAC154_KEY_IDENTIFIER_MODE_1:
		length = 1;
		break;
	case MAC154_KEY_IDENTIFIER_MODE_2:
		length = 5;
		break;
	case MAC154_KEY_IDENTIFIER_MODE_3:
		length = 9;
		break;
	default:
		return NULL;
	}

	for (int i = 0; i < mac_sec_key_storage_size; i++) {
		if (mac_sec_key_storage[i].key_id_mode != key_id_mode) {
			continue;
		}
		if (memcmp(mac_sec_key_storage[i].key_id, key_id, length)) {
			continue;
		}
		return &mac_sec_key_storage[i];
	}

	return NULL;
}
