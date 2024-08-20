/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IEEE802154_ALIF_KEY_STORAGE_H_
#define IEEE802154_ALIF_KEY_STORAGE_H_

#include "alif_mac154_api.h"
#include "alif_mac154_def.h"

#define MAC_KEY_STORAGE_SIZE         3
#define IEEE_MAC_KEY_SOURCE_MAX_SIZE 9
#define MAC_SEC_KEY_SIZE             16

/* Key storage information */
struct alif_mac154_key_storage {
	uint8_t key_value[MAC_SEC_KEY_SIZE];
	uint8_t key_id[IEEE_MAC_KEY_SOURCE_MAX_SIZE];
	uint32_t frame_counter;
	enum mac154_sec_keyid_mode key_id_mode;
	bool frame_counter_per_key;
};

/**
 * @brief Get security enabled bit value from Frame control field.
 *
 * @param[in]	key_desc_list Pointer to key description list
 * @param[in]	list_size list size
 *
 * @return	0 key set OK.
 * @return	False Frame security and authentication is disabled.
 */
int alif_mac154_key_storage_key_description_set(struct alif_mac154_key_description *key_desc_list,
						int list_size);
/**
 * @brief Get Security description from from storage.
 *
 * @param[in]	key_id_mode Key id mode
 * @param[in]	key_id Key descrption key src
 *
 * @return	Pointer to requested key description
 * @return	NULL when unkonw key
 */
struct alif_mac154_key_storage *
alif_mac154_key_storage_key_description_get(enum mac154_sec_keyid_mode key_id_mode,
					    uint8_t *key_id);

/**
 * @brief Set global used security frame counter.
 *
 * @param[in]	frame_counter Frame counter
 *
 */
void alif_mac154_sec_frame_counter_set(uint32_t frame_counter);

/**
 * @brief Get and increment global security frame counter.
 *
 * @param[in]	frame_counter Frame counter
 *
 * @return 	Frame counter
 *
 */
uint32_t alif_mac154_sec_frame_counter_get(void);

#endif /* IEEE802154_ALIF_KEY_STORAGE_H_ */
