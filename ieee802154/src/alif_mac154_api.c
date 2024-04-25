/*
 * Copyright (c) 2023 Alif Semiconductor
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

#define LOG_MODULE_NAME alif_154_api

#if defined(CONFIG_IEEE802154_DRIVER_LOG_LEVEL)
#define LOG_LEVEL CONFIG_IEEE802154_DRIVER_LOG_LEVEL
#else
#define LOG_LEVEL LOG_LEVEL_DBG
#endif

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

/*
 * Version number definition
 */

#define VERS_MAJOR 1
#define VERS_MINOR 0
#define VERS_PATCH 0

#define VERSION(major, minor, patch) ((major << 16) | (minor << 8) | (patch))
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

uint32_t ll_hw_version;
uint32_t ll_sw_version;

void ahi_msg_received_callback(struct msg_buf *p_msg)
{
	struct alif_rx_frame_received frame;

	if (alif_ahi_msg_resp_event_recv(resp_msg_ptr, p_msg)) {
		resp_msg_ptr = NULL;
		k_sem_give(&ahi_receive_sem);
		LOG_DBG("Excpected msg received");
	} else if (api_cb.rx_frame_recv_cb &&
		   alif_ahi_msg_recv_ind_recv(p_msg, &frame.ctx, &frame.rssi, &frame.frame_pending,
					      &frame.timestamp, &frame.len, &frame.p_data)) {
		api_cb.rx_frame_recv_cb(&frame);
		LOG_DBG("frame received");
	} else if (api_cb.rx_status_cb && alif_ahi_msg_error_recv(p_msg, NULL, NULL)) {
		api_cb.rx_status_cb(ALIF_MAC154_STATUS_OUT_OF_SYNC);
		LOG_DBG("Error received");
	} else if (api_cb.rx_status_cb && alif_ahi_msg_reset_recv(p_msg, NULL, NULL)) {
		api_cb.rx_status_cb(ALIF_MAC154_STATUS_RESET);
		LOG_DBG("Reset received");
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

	if (k_sem_take(&ahi_receive_sem, K_MSEC(1000)) != 0) {
		LOG_ERR("uart read timeout!");
	}
}

/*
 * MAC interface functions
 */
void alif_mac154_init(struct alif_mac154_api_cb *p_api)
{
	LOG_INF("mac154_init()");

	api_cb.rx_frame_recv_cb = p_api->rx_frame_recv_cb;
	api_cb.rx_status_cb = p_api->rx_status_cb;
	alif_ahi_init(ahi_msg_received_callback);
	int ret = take_es0_into_use(); /* temporary direct stuff */

	LOG_INF("boot SE0 ret:%d", ret);
}

enum alif_mac154_status_code alif_mac154_reset(void)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("");

	alif_ahi_msg_reset(&ahi_msg, 0);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("rx stop failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
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
	*p_major = VERS_MAJOR;
	*p_minor = VERS_MINOR;
	*p_patch = VERS_PATCH;
	k_mutex_unlock(&api_mutex);
	return ALIF_MAC154_STATUS_OK;
}

enum alif_mac154_status_code alif_mac154_timestamp_get(uint64_t *p_timestamp)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("");

	alif_ahi_msg_timestamp_get(&ahi_msg, 0);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_timestamp(&ahi_msg, NULL, p_timestamp);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("timestamp get failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_transmit(struct alif_tx_req *p_tx,
						  struct alif_tx_ack_resp *p_tx_ack)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("ch:%d, cca:%d, ack:%d, len:%d", p_tx->channel, p_tx->cca_requested,
		p_tx->acknowledgment_asked, p_tx->length);

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

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("ch:%d ts:%d", p_rx->channel, p_rx->timestamp);

	alif_ahi_msg_rx_start(&ahi_msg, 0, p_rx->channel, false, p_rx->frames, p_rx->timestamp);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_rx_start_resp(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("rx start failed %x", ret);
	}
	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_receive_stop(void)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("");

	alif_ahi_msg_rx_stop(&ahi_msg, 0);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("rx stop failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code
alif_mac154_energy_detection(struct alif_energy_detect *p_energy_measure,
			     struct alif_energy_detect_response *p_energy_measure_result)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("ch:%d thr:%d", p_energy_measure->channel, p_energy_measure->threshold);

	alif_ahi_msg_ed_start(&ahi_msg, 0, p_energy_measure->channel, p_energy_measure->threshold,
			      p_energy_measure->nb_tics, p_energy_measure->timestamp);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_energy_detect_resp(&ahi_msg, NULL, &p_energy_measure_result->nb_measure,
					      &p_energy_measure_result->average,
					      &p_energy_measure_result->max);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("energy detect failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_short_address_set(uint16_t short_address)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("%x", short_address);

	alif_ahi_msg_short_id_set(&ahi_msg, 0, short_address);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("short address set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_extended_address_set(uint8_t *p_extended_address)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);

	LOG_DBG("0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x", p_extended_address[7],
		p_extended_address[6], p_extended_address[5], p_extended_address[4],
		p_extended_address[3], p_extended_address[2], p_extended_address[1],
		p_extended_address[0]);

	alif_ahi_msg_long_id_set(&ahi_msg, 0, p_extended_address);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("ext address set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_pan_id_set(uint16_t pan_id)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("0x%x", pan_id);

	alif_ahi_msg_pan_id_set(&ahi_msg, 0, pan_id);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pan id set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_pendings_short_address_insert(uint16_t short_address)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("0x%x", short_address);

	alif_ahi_msg_pending_short_id_insert(&ahi_msg, 0, short_address);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pending short address set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_pendings_short_address_remove(uint16_t short_address)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("0x%x", short_address);

	alif_ahi_msg_pending_short_id_remove(&ahi_msg, 0, short_address);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pending short address remove failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_pendings_long_address_insert(uint8_t *p_extended_address)
{
	enum alif_mac154_status_code ret;
	uint8_t tmp[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	k_mutex_lock(&api_mutex, K_FOREVER);
	if (!p_extended_address) {
		p_extended_address = tmp;
	}
	LOG_DBG("0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x", p_extended_address[7],
		p_extended_address[6], p_extended_address[5], p_extended_address[4],
		p_extended_address[3], p_extended_address[2], p_extended_address[1],
		p_extended_address[0]);

	alif_ahi_msg_pending_long_id_insert(&ahi_msg, 0, p_extended_address);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pending short address set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_pendings_long_address_remove(uint8_t *p_extended_address)
{
	enum alif_mac154_status_code ret;

	if (!p_extended_address) {
		return ALIF_MAC154_STATUS_FAILED;
	}

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x", p_extended_address[7],
		p_extended_address[6], p_extended_address[5], p_extended_address[4],
		p_extended_address[3], p_extended_address[2], p_extended_address[1],
		p_extended_address[0]);

	alif_ahi_msg_pending_long_id_remove(&ahi_msg, 0, p_extended_address);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("pending long address remove failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_promiscious_mode_set(bool promiscuous_mode)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("mode:%d", promiscuous_mode);

	alif_ahi_msg_promiscuous_set(&ahi_msg, 0, promiscuous_mode);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("cca mode set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_tx_power_set(int16_t dbm)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("dbm:%d", dbm);

	alif_ahi_msg_max_tx_power_set(&ahi_msg, 0, dbm);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("tx power set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_cca_mode_set(enum alif_mac154_cca_mode mode)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("mode:%d", mode);

	alif_ahi_msg_cca_mode_set(&ahi_msg, 0, mode);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("cca mode set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_ed_threshold_set(int8_t input)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("thr:%d", input);

	alif_ahi_msg_ed_threshold_set(&ahi_msg, 0, input);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_status(&ahi_msg, NULL);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("ed threshold set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
	return ret;
}

enum alif_mac154_status_code alif_mac154_dbg_rf(uint8_t write, uint32_t key, uint32_t value,
						uint32_t *p_read)
{
	enum alif_mac154_status_code ret;

	k_mutex_lock(&api_mutex, K_FOREVER);
	LOG_DBG("key:%d value:%x", key, value);

	alif_ahi_msg_dbg_rf(&ahi_msg, 0, write, key, value);

	alif_ahi_msg_send(&ahi_msg, NULL, 0);

	alif_hal_msg_wait(&ahi_msg);

	ret = alif_ahi_msg_rf_dbg_resp(&ahi_msg, NULL, p_read);

	if (ret != ALIF_MAC154_STATUS_OK) {
		LOG_WRN("RF dbg set failed %x", ret);
	}

	k_mutex_unlock(&api_mutex);
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
	return ALIF_MAC154_SHARED_CCA_MODE;
}

uint8_t alif_mac154_get_cca_threshold(void)
{
	return ALIF_MAC154_SHARED_CCA_THR;
}