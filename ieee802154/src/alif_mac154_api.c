/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include <zephyr/kernel.h>

#include "alif_mac154_api.h"

#include "alif_mac154_shared.h"

#include "ahi_msg_lib.h"
#include "alif_ahi.h"
#include "es0_power_manager.h"
#include "alif_mac154_key_storage.h"
#include "alif_mac154_parser.h"
#include "alif_mac154_ccm_encode.h"

#define LOG_MODULE_NAME alif_154_api

#if defined(CONFIG_IEEE802154_DRIVER_LOG_LEVEL)
#define LOG_LEVEL CONFIG_IEEE802154_DRIVER_LOG_LEVEL
#else
#define LOG_LEVEL LOG_LEVEL_DBG
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

/*
 * Initial Version number definition supported by default
 */

#define VERS_INIT_MAJOR 1
#define VERS_INIT_MINOR 0
#define VERS_INIT_PATCH 0

#define MODULE_VERSION_INITIAL 0x10006
/*
 * Latest supported Link layer version
 */
#define VERS_LATEST_MAJOR      1
#define VERS_LATEST_MINOR      1
#define VERS_LATEST_PATCH      0

#define VERSION(major, minor, patch) ((major << 16) | (minor << 8) | (patch))

#define MODULE_VERSION_1_1_0 0x10100

#define HAL_MSG_TIMEOUT_MS 200

/*
 * Static variables
 */
K_MUTEX_DEFINE(api_mutex);

/* Wait for specific message from AHI */
static K_SEM_DEFINE(ahi_receive_sem, 0, 1);

static struct msg_buf *resp_msg_ptr;
struct msg_buf ahi_msg;

/* API Callback functions */
struct alif_mac154_api_cb api_cb;

/*Hardware capabilities*/
static uint32_t ll_hw_version;
static uint32_t ll_sw_version;
static uint32_t hw_capabilities;

void ahi_msg_received_callback(struct msg_buf *p_msg)
{
	struct alif_rx_frame_received frame;

	if (alif_ahi_msg_resp_event_recv(resp_msg_ptr, p_msg)) {
		resp_msg_ptr = NULL;
		k_sem_give(&ahi_receive_sem);
		LOG_DBG("Excpected msg received");
	} else if (api_cb.rx_frame_recv_cb && (ll_sw_version < VERSION(1, 1, 0)) &&
		   alif_ahi_msg_recv_ind_recv(p_msg, &frame.ctx, &frame.rssi, &frame.frame_pending,
					      &frame.timestamp, &frame.len, &frame.p_data)) {
		/* Set default values for Old ROM */
		frame.ack_frame_cnt = 0xDEADC0DE;
		frame.ack_key_idx = 0xff;
		frame.ack_sec = false;
		api_cb.rx_frame_recv_cb(&frame);
		LOG_DBG("frame received");
	} else if (api_cb.rx_frame_recv_cb && (ll_sw_version >= VERSION(1, 1, 0)) &&
		   alif_ahi_msg_recv_ind_recv_1_1_0(p_msg, &frame.ctx, &frame.rssi,
						    &frame.frame_pending, &frame.timestamp,
						    &frame.len, &frame.p_data, &frame.ack_sec,
						    &frame.ack_frame_cnt, &frame.ack_key_idx)) {
		api_cb.rx_frame_recv_cb(&frame);
		LOG_DBG("frame received");
	} else if (api_cb.rx_status_cb && alif_ahi_msg_error_recv(p_msg, NULL, NULL)) {
		api_cb.rx_status_cb(ALIF_MAC154_STATUS_OUT_OF_SYNC);
		LOG_DBG("Error received");
	} else if (api_cb.rx_status_cb && alif_ahi_msg_reset_recv(p_msg, NULL, NULL)) {
		api_cb.rx_status_cb(ALIF_MAC154_STATUS_RESET);
		LOG_DBG("Reset received");
	} else if (api_cb.rx_status_cb && (ll_sw_version >= VERSION(1, 1, 0)) &&
		   alif_ahi_msg_rx_start_end_recv_1_1_0(p_msg, NULL, NULL)) {
		api_cb.rx_status_cb(ALIF_MAC154_STATUS_RX_STOPPED);
		LOG_DBG("RX start received");
	} else if (api_cb.rx_status_cb && alif_ahi_msg_rx_start_end_recv(p_msg, NULL, NULL)) {
		api_cb.rx_status_cb(ALIF_MAC154_STATUS_RX_STOPPED);
		LOG_DBG("RX start received");
	} else if (api_cb.rx_status_cb && alif_ahi_msg_rx_stop_end_recv(p_msg, NULL, NULL, NULL)) {
		api_cb.rx_status_cb(ALIF_MAC154_STATUS_RX_STOPPED);
		LOG_DBG("RX stop received");
	} else {
		LOG_WRN("un-handled message received len:%d", p_msg->msg_len);
	}
}

void alif_hal_msg_wait(struct msg_buf *p_msg_ptr)
{
	resp_msg_ptr = p_msg_ptr;
	p_msg_ptr->msg_len = 0;

	if (k_sem_take(&ahi_receive_sem, K_MSEC(HAL_MSG_TIMEOUT_MS)) != 0) {
		LOG_ERR("uart read timeout!, %u", p_msg_ptr->rsp_msg);
	}
}

/*
 * MAC interface functions
 */
void alif_mac154_init(struct alif_mac154_api_cb *p_api)
{
	LOG_INF("mac154_init()");

	hw_capabilities = 0;
	ll_hw_version = 0;
	ll_sw_version = 0;
	api_cb.rx_frame_recv_cb = p_api->rx_frame_recv_cb;
	api_cb.rx_status_cb = p_api->rx_status_cb;
	alif_ahi_init(ahi_msg_received_callback);
	int ret = take_es0_into_use(); /* temporary direct stuff */

	LOG_INF("boot SE0 ret:%d", ret);
}

enum alif_mac154_status_code alif_mac154_reset(void)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("");

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_reset(&ahi_msg, 0);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("reset failed %x", ret);
	}

	return ret;
}

static enum alif_mac154_status_code alif_mac154_ll_version_get(uint32_t *p_hw_version,
							       uint32_t *p_sw_version)
{
	enum alif_mac154_status_code ret;

	alif_ahi_msg_version_get(&ahi_msg, 0);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_version(&ahi_msg, NULL, p_hw_version, p_sw_version);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("version get failed %x", ret);
	}
	return ret;
}

enum alif_mac154_status_code alif_mac154_version_get(uint8_t *p_major, uint8_t *p_minor,
						     uint8_t *p_patch)
{
	enum alif_mac154_status_code ret;
	int n = 0;

	k_mutex_lock(&api_mutex, K_FOREVER);

	do {
		ret = alif_mac154_ll_version_get(&ll_hw_version, &ll_sw_version);
		n++;
	} while (ret != ALIF_MAC154_STATUS_OK && n < 100);
	/* Check version compatibility
	 */
	LOG_INF("hw:%x, sw:%x", ll_hw_version, ll_sw_version);

	if (ret != ALIF_MAC154_STATUS_OK) {
		return ALIF_MAC154_STATUS_COMM_FAILURE;
	}
	if (ll_sw_version > MODULE_VERSION_INITIAL) {
		*p_major = VERS_LATEST_MAJOR;
		*p_minor = VERS_LATEST_MINOR;
		*p_patch = VERS_LATEST_PATCH;
		hw_capabilities |= ALIF_IEEE802154_MAC_RX_OPT;
		hw_capabilities |= ALIF_IEEE802154_MAC_TXTIME;
		hw_capabilities |= ALIF_IEEE802154_MAC_RXTIME;
		if (IS_ENABLED(CONFIG_IEEE802154_ALIF_TX_ENCRYPT)) {
			hw_capabilities |= ALIF_IEEE802154_MAC_TX_SEC;
		}
	} else {
		/*Backward compatibility to 1.0.2 */
		*p_major = VERS_INIT_MAJOR;
		*p_minor = VERS_INIT_MINOR;
		*p_patch = VERS_INIT_PATCH;
	}

	k_mutex_unlock(&api_mutex);
	return ALIF_MAC154_STATUS_OK;
}

uint32_t alif_mac154_capabilities_get(void)
{
	return hw_capabilities;
}

enum alif_mac154_status_code alif_mac154_timestamp_get(uint64_t *p_timestamp)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("");

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_timestamp_get(&ahi_msg, 0);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_timestamp(&ahi_msg, NULL, p_timestamp);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("timestamp get failed %x", ret);
	}
	return ret;
}

enum alif_mac154_status_code alif_mac154_transmit(struct alif_tx_req *p_tx,
						  struct alif_tx_ack_resp *p_tx_ack)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("ch:%d, cca:%d, ack:%d, len:%d", p_tx->channel, p_tx->cca_requested,
		p_tx->acknowledgment_asked, p_tx->length);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_tx_start(&ahi_msg, p_tx->msg_id, p_tx->channel, p_tx->cca_requested,
			      p_tx->acknowledgment_asked, p_tx->timestamp, p_tx->p_payload,
			      p_tx->length);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);

	if (ll_sw_version >= VERSION(1, 1, 0)) {
		ret = alif_ahi_msg_tx_start_resp_1_1_0(&ahi_msg, 0, &p_tx_ack->ack_rssi,
						       &p_tx_ack->ack_timestamp, p_tx_ack->ack_msg,
						       &p_tx_ack->ack_msg_len);
	} else {
		ret = alif_ahi_msg_tx_start_resp(&ahi_msg, 0, &p_tx_ack->ack_rssi,
						 &p_tx_ack->ack_timestamp, p_tx_ack->ack_msg,
						 &p_tx_ack->ack_msg_len);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_receive_start(struct alif_rx_enable *p_rx)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("ch:%d ts:%d", p_rx->channel, p_rx->timestamp);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_rx_start(&ahi_msg, 0, p_rx->channel, false, p_rx->frames, p_rx->timestamp);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_rx_start_resp(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("rx start failed %x", ret);
	}
	return ret;
}

enum alif_mac154_status_code alif_mac154_receive_stop(void)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("");

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_rx_stop(&ahi_msg, 0);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("rx stop failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code
alif_mac154_energy_detection(struct alif_energy_detect *p_energy_measure,
			     struct alif_energy_detect_response *p_energy_measure_result)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("ch:%d thr:%d", p_energy_measure->channel, p_energy_measure->threshold);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_ed_start(&ahi_msg, 0, p_energy_measure->channel, p_energy_measure->threshold,
			      p_energy_measure->nb_tics, p_energy_measure->timestamp);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_energy_detect_resp(&ahi_msg, NULL, &p_energy_measure_result->nb_measure,
					      &p_energy_measure_result->average,
					      &p_energy_measure_result->max);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("energy detect failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_short_address_set(uint16_t short_address)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("%x", short_address);

	k_mutex_lock(&api_mutex, K_FOREVER);

	if (ll_sw_version >= VERSION(1, 1, 0)) {
		alif_ahi_msg_short_id_set_1_1_0(&ahi_msg, 0, short_address);
	} else {
		alif_ahi_msg_short_id_set(&ahi_msg, 0, short_address);
	}

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("short address set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_extended_address_set(uint8_t *p_extended_address)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x", p_extended_address[7],
		p_extended_address[6], p_extended_address[5], p_extended_address[4],
		p_extended_address[3], p_extended_address[2], p_extended_address[1],
		p_extended_address[0]);

	k_mutex_lock(&api_mutex, K_FOREVER);
	if (ll_sw_version >= VERSION(1, 1, 0)) {
		alif_ahi_msg_long_id_set_1_1_0(&ahi_msg, 0, p_extended_address);
	} else {
		alif_ahi_msg_long_id_set(&ahi_msg, 0, p_extended_address);
	}

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("ext address set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_pan_id_set(uint16_t pan_id)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("0x%x", pan_id);

	k_mutex_lock(&api_mutex, K_FOREVER);
	if (ll_sw_version >= VERSION(1, 1, 0)) {
		alif_ahi_msg_pan_id_set1_1_0(&ahi_msg, 0, pan_id);
	} else {
		alif_ahi_msg_pan_id_set(&ahi_msg, 0, pan_id);
	}

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pan id set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_pendings_short_address_insert(uint16_t short_address)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("0x%x", short_address);

	k_mutex_lock(&api_mutex, K_FOREVER);
	if (ll_sw_version >= VERSION(1, 1, 0)) {
		alif_ahi_msg_pending_short_id_configure_1_1_0(&ahi_msg, 0, short_address, true);
	} else {
		alif_ahi_msg_pending_short_id_insert(&ahi_msg, 0, short_address);
	}

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pending short address set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_pendings_short_address_remove(uint16_t short_address)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("0x%x", short_address);

	k_mutex_lock(&api_mutex, K_FOREVER);

	if (ll_sw_version >= VERSION(1, 1, 0)) {
		alif_ahi_msg_pending_short_id_configure_1_1_0(&ahi_msg, 0, short_address, false);
	} else {
		alif_ahi_msg_pending_short_id_remove(&ahi_msg, 0, short_address);
	}

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pending short address remove failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_pendings_long_address_insert(uint8_t *p_extended_address)
{
	enum alif_mac154_status_code ret;
	uint8_t tmp[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	if (!p_extended_address) {
		p_extended_address = tmp;
	}
	LOG_DBG("0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x", p_extended_address[7],
		p_extended_address[6], p_extended_address[5], p_extended_address[4],
		p_extended_address[3], p_extended_address[2], p_extended_address[1],
		p_extended_address[0]);

	k_mutex_lock(&api_mutex, K_FOREVER);
	if (ll_sw_version >= VERSION(1, 1, 0)) {
		alif_ahi_msg_pending_long_id_configure_1_1_0(&ahi_msg, 0, p_extended_address, true);
	} else {
		alif_ahi_msg_pending_long_id_insert(&ahi_msg, 0, p_extended_address);
	}

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pending short address set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_pendings_long_address_remove(uint8_t *p_extended_address)
{
	enum alif_mac154_status_code ret;

	if (!p_extended_address) {
		return ALIF_MAC154_STATUS_FAILED;
	}

	LOG_DBG("0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x", p_extended_address[7],
		p_extended_address[6], p_extended_address[5], p_extended_address[4],
		p_extended_address[3], p_extended_address[2], p_extended_address[1],
		p_extended_address[0]);

	k_mutex_lock(&api_mutex, K_FOREVER);
	if (ll_sw_version >= VERSION(1, 1, 0)) {
		alif_ahi_msg_pending_long_id_configure_1_1_0(&ahi_msg, 0, p_extended_address,
							     false);
	} else {
		alif_ahi_msg_pending_long_id_remove(&ahi_msg, 0, p_extended_address);
	}

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pending long address remove failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_promiscious_mode_set(bool promiscuous_mode)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("mode:%d", promiscuous_mode);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_promiscuous_set(&ahi_msg, 0, promiscuous_mode);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("cca mode set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_tx_power_set(int16_t dbm)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("dbm:%d", dbm);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_max_tx_power_set(&ahi_msg, 0, dbm);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("tx power set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_cca_mode_set(enum alif_mac154_cca_mode mode)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("mode:%d", mode);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_cca_mode_set(&ahi_msg, 0, mode);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("cca mode set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_ed_threshold_set(int8_t input)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("thr:%d", input);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_ed_threshold_set(&ahi_msg, 0, input);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("ed threshold set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_dbg_rf(uint8_t write, uint32_t key, uint32_t value,
						uint32_t *p_read)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("key:%d value:%x", key, value);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_dbg_rf(&ahi_msg, 0, write, key, value);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_rf_dbg_resp(&ahi_msg, NULL, p_read);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("RF dbg set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_csl_config_set(struct alif_mac154_csl_config *p_csl_config)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("period:0x%x", p_csl_config->csl_period);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_csl_period_set(&ahi_msg, 0, p_csl_config->csl_period);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("csl period set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_rx_slot_set(struct alif_mac154_rx_slot *p_rx_slot_config)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("start: %d duration: %d channel: %d", p_rx_slot_config->start,
		p_rx_slot_config->duration, p_rx_slot_config->channel);

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_config_rx_slot(&ahi_msg, 0, p_rx_slot_config->start,
				    p_rx_slot_config->duration, p_rx_slot_config->channel);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_config_rx_slot_resp(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("rx slot set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_expected_rx_time_set(uint32_t expected_rx_time)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_config_expected_rx_time(&ahi_msg, 0, expected_rx_time);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_set_expected_rx_time_resp(&ahi_msg, NULL);

	k_mutex_unlock(&api_mutex);
	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("Expected RX time set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code
alif_mac154_key_value_description_set(struct alif_mac154_key_description *key_desc_list,
				       int list_size)
{
	enum alif_mac154_status_code ret = ALIF_MAC154_STATUS_OK;


	if (IS_ENABLED(CONFIG_IEEE802154_ALIF_TX_ENCRYPT)) {
		alif_ahi_msg_clear_sec_keys(&ahi_msg, 0);
		alif_ahi_msg_send(&ahi_msg, NULL, 0);
		alif_hal_msg_wait(&ahi_msg);
		ret = alif_ahi_msg_clear_key_desc_resp(&ahi_msg, NULL);
		if (ret != ALIF_MAC154_STATUS_OK) {
			LOG_WRN("Key descriotion clear failed %x", ret);
		}

		if (alif_mac154_key_storage_key_description_set(key_desc_list, list_size)) {
			return ALIF_MAC154_STATUS_FAILED;
		}

		k_mutex_lock(&api_mutex, K_FOREVER);
		while (list_size) {
			alif_ahi_msg_config_sec_key(
				&ahi_msg, 0, key_desc_list->key_value, key_desc_list->key_id,
				key_desc_list->key_id_mode, key_desc_list->frame_counter,
				key_desc_list->frame_counter_per_key);

			alif_ahi_msg_send(&ahi_msg, NULL, 0);
			alif_hal_msg_wait(&ahi_msg);
			ret = alif_ahi_msg_set_key_desc_resp(&ahi_msg, NULL);
			if (ret != ALIF_MAC154_STATUS_OK) {
				LOG_WRN("Key descriotion set failed %x", ret);
				break;
			}
			key_desc_list++;
			list_size--;
		}

		k_mutex_unlock(&api_mutex);
	}

	return ret;
}

enum alif_mac154_status_code alif_mac154_tx_packet_parse(struct alif_802154_frame_parser *mac_frame)
{
	enum alif_mac154_status_code ret = ALIF_MAC154_STATUS_FAILED;

	if (IS_ENABLED(CONFIG_IEEE802154_ALIF_TX_ENCRYPT)) {
		if (alif_mac154_mac_frame_parse(mac_frame)) {
			ret = ALIF_MAC154_STATUS_OK;
		}
	}

	return ret;
}

enum alif_mac154_status_code
alif_mac154_mac_data_encode_and_authenticate(struct alif_802154_frame_parser *mac_frame,
					     uint8_t *mac64)
{
	enum alif_mac154_status_code ret = ALIF_MAC154_STATUS_FAILED;

	if (IS_ENABLED(CONFIG_IEEE802154_ALIF_TX_ENCRYPT)) {
		if (alif_mac154_ccm_encode_packet(mac_frame, mac64) == 0) {
			return ALIF_MAC154_STATUS_OK;
		}
	}

	return ret;
}

bool alif_mac154_ie_header_element_get(uint8_t *header_ptr, uint16_t length,
				       struct mac_header_IE_s *header_ie)
{
	if (IS_ENABLED(CONFIG_IEEE802154_ALIF_TX_ENCRYPT)) {
		return alif_mac154_ie_header_discover(header_ptr, length, header_ie);
	}

	return false;
}

enum alif_mac154_status_code alif_mac154_security_frame_counter_set(uint32_t frame_counter)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	if (IS_ENABLED(CONFIG_IEEE802154_ALIF_TX_ENCRYPT)) {
		alif_mac154_sec_frame_counter_set(frame_counter);
	}

	alif_ahi_msg_config_frame_counter(&ahi_msg, 0, frame_counter, false);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_set_frame_counter_resp(&ahi_msg, NULL, false);

	k_mutex_unlock(&api_mutex);
	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("Key descriotion set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code
alif_mac154_security_frame_counter_set_if_larger(uint32_t frame_counter)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	if (IS_ENABLED(CONFIG_IEEE802154_ALIF_TX_ENCRYPT)) {
		alif_mac154_sec_frame_counter_set(frame_counter);
	}

	alif_ahi_msg_config_frame_counter(&ahi_msg, 0, frame_counter, true);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_set_frame_counter_resp(&ahi_msg, NULL, true);

	k_mutex_unlock(&api_mutex);
	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("Key descriotion set failed %x", ret);
	}

	return ret;
}

enum alif_mac154_status_code
alif_mac154_csl_phase_get(struct alif_mac154_csl_phase *p_csl_phase_resp)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("");

	k_mutex_lock(&api_mutex, K_FOREVER);

	alif_ahi_msg_csl_phase_get(&ahi_msg, 0);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);
	ret = alif_ahi_msg_csl_phase_get_resp(&ahi_msg, NULL, &p_csl_phase_resp->timestamp,
					      &p_csl_phase_resp->csl_phase);

	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("csl phase get failed %x", ret);
	}

	return ret;
}

static enum alif_mac154_status_code
alif_mac154_ie_long_id_insert(const uint8_t *p_extended_address,
			      const struct alif_802154_header_ie *ie_info)
{

	LOG_HEXDUMP_DBG(p_extended_address, 8, "long_id_insert addr:");

	alif_ahi_msg_ie_header_gen(&ahi_msg, 0, 0, p_extended_address, ie_info);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);

	return alif_ahi_msg_status(&ahi_msg, NULL);
}

static enum alif_mac154_status_code
alif_mac154_ie_short_id_insert(uint16_t short_address, const struct alif_802154_header_ie *ie_info)
{
	LOG_DBG("0x%x", short_address);

	alif_ahi_msg_ie_header_gen(&ahi_msg, 0, short_address, NULL, ie_info);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);

	return alif_ahi_msg_status(&ahi_msg, NULL);
}

static enum alif_mac154_status_code alif_mac154_ie_long_id_remove(const uint8_t *p_extended_address)
{
	LOG_HEXDUMP_DBG(p_extended_address, 8, "long_id_remove addr:");
	alif_ahi_msg_ie_header_gen(&ahi_msg, 0, 0, p_extended_address, NULL);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);

	return alif_ahi_msg_status(&ahi_msg, NULL);
}

static enum alif_mac154_status_code alif_mac154_ie_short_id_remove(uint16_t short_address)
{
	LOG_DBG("0x%x", short_address);

	alif_ahi_msg_ie_header_gen(&ahi_msg, 0, short_address, NULL, NULL);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);

	return alif_ahi_msg_status(&ahi_msg, NULL);
}

static enum alif_mac154_status_code alif_mac154_purge_all_ie(void)
{

	alif_ahi_msg_ie_purge_all(&ahi_msg, 0);
	alif_ahi_msg_send(&ahi_msg, NULL, 0);
	alif_hal_msg_wait(&ahi_msg);

	return alif_ahi_msg_status(&ahi_msg, NULL);
}

enum alif_mac154_status_code
alif_mac154_ack_header_ie_set(uint16_t short_address, const uint8_t *p_extended_address,
			      bool delete_all_ie, const struct alif_802154_header_ie *ie_info)
{
	enum alif_mac154_status_code ret;

	LOG_DBG("");

	k_mutex_lock(&api_mutex, K_FOREVER);

	if (delete_all_ie) {
		alif_mac154_purge_all_ie();
		ret = ALIF_MAC154_STATUS_OK;
		goto end;
	}

	if (!ie_info || ie_info->length == 0) {
		/*Delete IE headers for this device*/
		alif_mac154_ie_short_id_remove(short_address);
		alif_mac154_ie_long_id_remove(p_extended_address);
		ret = ALIF_MAC154_STATUS_OK;
		goto end;
	}

	/*Set the addresses*/
	alif_mac154_ie_short_id_insert(short_address, ie_info);
	alif_mac154_ie_long_id_insert(p_extended_address, ie_info);

end:
	k_mutex_unlock(&api_mutex);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("ACK header IE set failed %x", ret);
	}
	return ret;
}

bool alif_mac154_get_promiscuous_mode(void)
{
	return ALIF_MAC154_SHARED_PROMISCUOUS_MODE;
}

int8_t alif_mac154_get_tx_power(void)
{
	return ALIF_MAC154_SHARED_TX_POWER_DBM;
}

int8_t alif_mac154_get_last_rssi(void)
{
	return ALIF_MAC154_SHARED_LAST_RSSI_DBM;
}

uint16_t alif_mac154_get_short_address(void)
{
	return ALIF_MAC154_SHARED_SHORT_ID;
}

void alif_mac154_get_extended_address(uint8_t *external_address)
{
	for (int n = 0; n < 8; n++) {
		external_address[n] = ALIF_MAC154_SHARED_LONG_ID[n];
	}
}

uint16_t alif_mac154_get_pan_id(void)
{
	return ALIF_MAC154_SHARED_PAN_ID;
}

uint8_t alif_mac154_get_priority_tx(void)
{
	return ALIF_MAC154_SHARED_PRIO_TX;
}

uint8_t alif_mac154_get_priority_rx(void)
{
	return ALIF_MAC154_SHARED_PRIO_RX;
}

uint8_t alif_mac154_get_priority_ed(void)
{
	return ALIF_MAC154_SHARED_PRIO_ED;
}

enum alif_mac154_cca_mode alif_mac154_get_cca_mode(void)
{
	if (ll_sw_version > MODULE_VERSION_INITIAL) {
		return ALIF_MAC154_SHARED_CCA_MODE_1_1;
	}
	return ALIF_MAC154_SHARED_CCA_MODE;
}

int8_t alif_mac154_get_cca_threshold(void)
{
	if (ll_sw_version > MODULE_VERSION_INITIAL) {
		return ALIF_MAC154_SHARED_CCA_THR_1_1;
	}
	return ALIF_MAC154_SHARED_CCA_THR;
}
