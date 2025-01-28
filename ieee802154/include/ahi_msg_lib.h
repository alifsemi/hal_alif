/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _802154_ROM_INCLUDE_AHI_MSG_LIB_H_
#define _802154_ROM_INCLUDE_AHI_MSG_LIB_H_

#include <stdbool.h>
#include "alif_mac154_def.h"

#define MAX_MSG_LEN 300

struct __aligned(4) msg_buf {
	uint16_t msg_len;
	uint16_t rsp_msg;
	uint16_t rsp_event;
	uint8_t dummy;
	uint8_t msg[MAX_MSG_LEN];
};

#define VENDOR_OUI_LENGTH 3

struct mac_ahi_header_ie_vendor_specific {
	uint8_t vendor_oui[VENDOR_OUI_LENGTH];
	uint8_t *vendor_specific_info;
};

struct mac_ahi_header_ie_rendezvous_time {
	uint16_t rendezvous_time;
	uint16_t wakeup_interval;
	bool full_info;
};

struct mac_ahi_header_ie_csl {
	uint16_t csl_phase;
	uint16_t csl_period;
	uint16_t csl_rendezvous_time;
	bool full_info;
};

struct mac_ahi_header_ie {
	uint16_t length;
	uint16_t element_id_low;
	uint16_t element_id_high;
	uint16_t type;
	uint8_t content_type;
	union {
		struct mac_ahi_header_ie_vendor_specific vendor_specific;
		struct mac_ahi_header_ie_csl csl;
		struct mac_ahi_header_ie_rendezvous_time rendezvous_time;
	} content;
};

/*Receiving handling*/
/*
 * return -1 message is corrupted
 *         0 more data needed
 *         1 complete message received
 */
int alif_ahi_msg_valid_message(struct msg_buf *p_msg);

bool alif_ahi_msg_resp_event_recv(struct msg_buf *p_dest_msg, struct msg_buf *p_src_msg);

bool alif_ahi_msg_recv_ind_recv(struct msg_buf *p_msg, uint16_t *p_ctx, int8_t *p_rssi,
				bool *p_frame_pending, uint64_t *p_timestamp, uint8_t *p_len,
				uint8_t **p_data);
bool alif_ahi_msg_recv_ind_recv_1_1_0(struct msg_buf *p_msg, uint16_t *p_ctx, int8_t *p_rssi,
				      bool *p_frame_pending, uint64_t *p_timestamp, uint8_t *p_len,
				      uint8_t **p_data, bool *ack_sec, uint32_t *ack_fc,
				      uint8_t *ack_key_idx);

bool alif_ahi_msg_rx_start_end_recv(struct msg_buf *p_msg, uint16_t *p_dummy,
				    enum alif_mac154_status_code *p_status);
bool alif_ahi_msg_rx_start_end_recv_1_1_0(struct msg_buf *p_msg, uint16_t *p_dummy,
				    enum alif_mac154_status_code *p_status);

bool alif_ahi_msg_rx_stop_end_recv(struct msg_buf *p_msg, uint16_t *p_dummy, uint16_t *p_nreceived,
				   enum alif_mac154_status_code *p_status);

bool alif_ahi_msg_reset_recv(struct msg_buf *p_msg, uint16_t *p_dummy, uint8_t *p_activity);

bool alif_ahi_msg_error_recv(struct msg_buf *p_msg, uint16_t *p_dummy,
			     enum alif_mac154_status_code *p_status);

/*message create*/
void alif_ahi_msg_pan_id_set(struct msg_buf *p_msg, uint16_t ctx, uint16_t pan_id);
void alif_ahi_msg_pan_id_set1_1_0(struct msg_buf *p_msg, uint16_t ctx, uint16_t pan_id);
void alif_ahi_msg_cca_mode_set(struct msg_buf *p_msg, uint16_t ctx, enum alif_mac154_cca_mode mode);
void alif_ahi_msg_ed_threshold_set(struct msg_buf *p_msg, uint16_t ctx, int8_t threshold);
void alif_ahi_msg_pan_id_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_short_id_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_long_id_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_cca_mode_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_ed_threshold_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_short_id_set(struct msg_buf *p_msg, uint16_t ctx, uint16_t short_id);
void alif_ahi_msg_short_id_set_1_1_0(struct msg_buf *p_msg, uint16_t ctx, uint16_t short_id);
/* Not ported since v1.1.0 */
void alif_ahi_msg_pending_short_id_find(struct msg_buf *p_msg, uint16_t ctx, uint16_t short_id);
void alif_ahi_msg_pending_short_id_insert(struct msg_buf *p_msg, uint16_t ctx, uint16_t short_id);
void alif_ahi_msg_pending_short_id_remove(struct msg_buf *p_msg, uint16_t ctx, uint16_t short_id);
/* Implement insert and remove since v1.1.0 */
void alif_ahi_msg_pending_short_id_configure_1_1_0(struct msg_buf *p_msg, uint16_t ctx,
						   uint16_t short_id, bool enabled);
void alif_ahi_msg_long_id_set(struct msg_buf *p_msg, uint16_t ctx, uint8_t *p_extended_address);
void alif_ahi_msg_long_id_set_1_1_0(struct msg_buf *p_msg, uint16_t ctx,
				    uint8_t *p_extended_address);
void alif_ahi_msg_pending_long_id_find(struct msg_buf *p_msg, uint16_t ctx,
				       uint8_t *p_extended_address);
void alif_ahi_msg_pending_long_id_insert(struct msg_buf *p_msg, uint16_t ctx,
					 uint8_t *p_extended_address);
void alif_ahi_msg_pending_long_id_remove(struct msg_buf *p_msg, uint16_t ctx,
					 uint8_t *p_extended_address);
void alif_ahi_msg_pending_long_id_configure_1_1_0(struct msg_buf *p_msg, uint16_t ctx,
						  uint8_t *p_extended_address, bool enabled);
void alif_ahi_msg_ie_purge_all(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_ie_header_gen(struct msg_buf *p_msg, uint16_t ctx, uint16_t short_id,
				const uint8_t *p_extended_address,
				const struct mac_ahi_header_ie *p_header_ie);

void alif_ahi_msg_csl_period_set(struct msg_buf *p_msg, uint16_t ctx, uint16_t period);
void alif_ahi_msg_csl_period_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_config_rx_slot(struct msg_buf *p_msg, uint16_t ctx, uint32_t start,
				 uint16_t duration, uint8_t channel);
void alif_ahi_msg_config_expected_rx_time(struct msg_buf *p_msg, uint16_t ctx,
					  uint32_t expected_time);
void alif_ahi_msg_config_sec_key(struct msg_buf *p_msg, uint16_t ctx, uint8_t *key_value,
				 uint8_t *key_id, uint8_t key_id_mode, uint32_t frame_counter,
				 bool frame_counter_per_key);
void alif_ahi_msg_clear_sec_keys(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_config_frame_counter(struct msg_buf *p_msg, uint16_t ctx, uint32_t frame_counter,
				       bool if_larger);
void alif_ahi_msg_timestamp_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_rx_stop(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_promiscuous_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_promiscuous_set(struct msg_buf *p_msg, uint16_t ctx, uint8_t input);
void alif_ahi_msg_promiscuous_set_1_1_0(struct msg_buf *p_msg, uint16_t ctx, uint8_t input);
void alif_ahi_msg_tx_power_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_min_tx_power_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_max_tx_power_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_last_rssi_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_max_tx_power_set(struct msg_buf *p_msg, uint16_t ctx, uint8_t dbm);
void alif_ahi_msg_ed_start(struct msg_buf *p_msg, uint16_t ctx, uint8_t channel, int8_t threshold,
			   uint8_t ticks, uint32_t timestamp);
void alif_ahi_msg_reset(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_version_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_tx_start(struct msg_buf *p_msg, uint16_t ctx, uint8_t channel,
			   uint8_t cca_requested, uint8_t acknowledgment_asked, uint32_t timestamp,
			   const uint8_t *p_data, uint8_t data_len);
void alif_ahi_msg_rx_start(struct msg_buf *p_msg, uint16_t ctx, uint8_t channel,
			   bool mute_indications, uint8_t nb_frames, uint32_t timestamp);
void alif_ahi_msg_tx_prio_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_rx_prio_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_ed_prio_get(struct msg_buf *p_msg, uint16_t ctx);
void alif_ahi_msg_tx_prio_set(struct msg_buf *p_msg, uint16_t ctx, uint8_t priority);
void alif_ahi_msg_rx_prio_set(struct msg_buf *p_msg, uint16_t ctx, uint8_t priority);
void alif_ahi_msg_ed_prio_set(struct msg_buf *p_msg, uint16_t ctx, uint8_t priority);
void alif_ahi_msg_dbg_rf(struct msg_buf *p_msg, uint16_t ctx, uint8_t write, uint32_t address,
			 uint32_t value);
void alif_ahi_msg_dbg_mem(struct msg_buf *p_msg, uint16_t ctx, uint8_t write, uint32_t address,
			  uint32_t value);
void alif_ahi_msg_dbg_reg(struct msg_buf *p_msg, uint16_t ctx, uint8_t write, uint32_t address,
			  uint32_t value);
void alif_ahi_msg_csl_phase_get(struct msg_buf *p_msg, uint16_t ctx);

enum alif_mac154_status_code alif_ahi_msg_status(struct msg_buf *p_msg, uint8_t *p_ctx);
enum alif_mac154_status_code alif_ahi_msg_dbm(struct msg_buf *p_msg, uint8_t *p_ctx, int8_t *p_dbm);
enum alif_mac154_status_code alif_ahi_msg_prio(struct msg_buf *p_msg, uint8_t *p_ctx,
					       int8_t *p_prio);
enum alif_mac154_status_code alif_ahi_msg_pan_id(struct msg_buf *p_msg, uint8_t *p_ctx,
						 uint8_t *p_pan_id);
enum alif_mac154_status_code alif_ahi_msg_short_id(struct msg_buf *p_msg, uint8_t *p_ctx,
						   uint8_t *p_short_id);
enum alif_mac154_status_code alif_ahi_msg_long_id(struct msg_buf *p_msg, uint8_t *p_ctx,
						  uint8_t *p_extended_address);
enum alif_mac154_status_code alif_ahi_msg_cca_mode(struct msg_buf *p_msg, uint8_t *p_ctx,
						   uint8_t *p_cca_mode);
enum alif_mac154_status_code alif_ahi_msg_ed_threshold(struct msg_buf *p_msg, uint8_t *p_ctx,
						       int8_t *p_ed_threshold);
enum alif_mac154_status_code alif_ahi_msg_timestamp(struct msg_buf *p_msg, uint8_t *p_ctx,
						    uint64_t *p_timestamp);
enum alif_mac154_status_code alif_ahi_msg_promiscuous_mode(struct msg_buf *p_msg, uint8_t *p_ctx,
							   uint8_t *p_answer);
enum alif_mac154_status_code alif_ahi_msg_tx_power_set_status(struct msg_buf *p_msg,
							      uint8_t *p_ctx);
enum alif_mac154_status_code alif_ahi_msg_reset_status(struct msg_buf *p_msg, uint8_t *p_ctx);
enum alif_mac154_status_code alif_ahi_msg_version(struct msg_buf *p_msg, uint8_t *p_ctx,
						  uint32_t *p_hw_ver, uint32_t *p_sw_ver);

enum alif_mac154_status_code alif_ahi_msg_energy_detect_resp(struct msg_buf *p_msg, uint8_t *p_ctx,
							     uint8_t *p_nb_measure,
							     uint8_t *p_average, uint8_t *p_max);
enum alif_mac154_status_code alif_ahi_msg_tx_start_resp(struct msg_buf *p_msg, uint8_t *p_ctx,
							int8_t *p_rssi, uint64_t *p_timestamp,
							uint8_t *p_ack, uint8_t *p_ack_len);
enum alif_mac154_status_code alif_ahi_msg_tx_start_resp_1_1_0(struct msg_buf *p_msg, uint8_t *p_ctx,
							      int8_t *p_rssi, uint64_t *p_timestamp,
							      uint8_t *p_ack, uint8_t *p_ack_len);
enum alif_mac154_status_code alif_ahi_msg_rx_start_resp(struct msg_buf *p_msg, uint8_t *p_ctx);
enum alif_mac154_status_code alif_ahi_msg_stop_rx_resp(struct msg_buf *p_msg, uint8_t *p_ctx,
						       uint16_t *p_nreceived);
enum alif_mac154_status_code alif_ahi_msg_rf_dbg_resp(struct msg_buf *p_msg, uint8_t *p_ctx,
						      uint32_t *p_value);
enum alif_mac154_status_code alif_ahi_msg_mem_dbg_resp(struct msg_buf *p_msg, uint8_t *p_ctx,
						       uint32_t *p_value);
enum alif_mac154_status_code alif_ahi_msg_mem_reg_resp(struct msg_buf *p_msg, uint8_t *p_ctx,
						       uint32_t *p_value);
enum alif_mac154_status_code alif_ahi_msg_csl_phase_get_resp(struct msg_buf *p_msg, uint8_t *p_ctx,
							 uint64_t *p_timestamp,
							 uint16_t *p_csl_phase);
enum alif_mac154_status_code alif_ahi_msg_config_rx_slot_resp(struct msg_buf *p_msg,
							      uint8_t *p_ctx);
enum alif_mac154_status_code alif_ahi_msg_set_expected_rx_time_resp(struct msg_buf *p_msg,
								    uint8_t *p_ctx);
enum alif_mac154_status_code alif_ahi_msg_set_key_desc_resp(struct msg_buf *p_msg, uint8_t *p_ctx);

enum alif_mac154_status_code alif_ahi_msg_clear_key_desc_resp(struct msg_buf *p_msg, uint8_t *p_ctx);

enum alif_mac154_status_code alif_ahi_msg_set_frame_counter_resp(struct msg_buf *p_msg,
								 uint8_t *p_ctx, bool if_larger);

#endif /* _802154_ROM_INCLUDE_AHI_MSG_LIB_H_ */
