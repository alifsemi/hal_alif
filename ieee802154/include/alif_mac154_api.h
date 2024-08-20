/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ALIF_MAC154_API_H_
#define ALIF_MAC154_API_H_

#include "alif_mac154_def.h"
#include <zephyr/net/ieee802154_ie.h>

/**
 * Maximum ACK Frame size
 */
#define ACK_MAX_FRAME_LEN 127

/**
 * @brief Supported HW features
 *
 * ALIF_IEEE802154_MAC_TXTIME
 *	Delayed transmission is supported. Timestamps on received frames has
 *	uint64_t timestamps and transmission can be controlled using fractional part
 *
 * ALIF_IEEE802154_MAC_TX_SEC
 *	Transmission security supported
 *
 * ALIF_IEEE802154_MAC_RX_OPT
 *	RX can be kept on during transmissions and does not need to be
 *	stopped and started
 *
 * ALIF_IEEE802154_MAC_RXTIME
 *	Support CSL Slot configurations and timed RX operations.
 */
#define ALIF_IEEE802154_MAC_TXTIME BIT(1)
#define ALIF_IEEE802154_MAC_TX_SEC BIT(2)
#define ALIF_IEEE802154_MAC_RX_OPT BIT(3)
#define ALIF_IEEE802154_MAC_RXTIME BIT(4)

/**
 * @brief Transmission request parameters
 *
 */
struct alif_tx_req {
	uint32_t timestamp;
	const uint8_t *p_payload;
	uint8_t msg_id;
	uint8_t length;
	uint8_t channel;
	bool acknowledgment_asked;
	bool cca_requested;
	bool csma_requested;
};

/**
 * @brief Acknowledge parameters
 *
 */
struct alif_tx_ack_resp {
	uint64_t ack_timestamp;
	uint8_t ack_msg[ACK_MAX_FRAME_LEN];
	uint8_t ack_msg_len;
	int8_t ack_rssi;
};

/**
 * @brief Reception start parameters
 *
 */
struct alif_rx_enable {
	uint32_t timestamp;
	uint8_t frames;
	uint8_t channel;
};

/**
 * @brief Reception start parameters
 *
 */
struct alif_rx_frame_received {
	uint64_t timestamp;
	uint8_t *p_data;
	uint32_t ack_frame_cnt;
	uint16_t ctx;
	uint8_t len;
	uint8_t ack_key_idx;
	int8_t rssi;
	bool frame_pending;
	bool ack_sec;
};

/**
 * @brief Energy detection measurement parameters
 *
 */
struct alif_energy_detect {
	uint32_t timestamp;
	uint8_t channel;
	uint8_t nb_tics;
	int8_t threshold;
};

/**
 * @brief Energy measurement result
 *
 */
struct alif_energy_detect_response {
	uint8_t nb_measure;
	uint8_t average;
	int8_t max;
};

/**
 * @brief CSL configuration parameters
 *
 */
struct alif_mac154_csl_config {
	uint16_t csl_period;
};

/**
 * @brief Configure RX SLOT
 *
 */
struct alif_mac154_rx_slot {
	uint32_t expected_rx_time;
	uint32_t start;
	uint16_t duration;
	uint8_t channel;
};

/**
 * @brief current CSL phase parameters
 *
 */
struct alif_mac154_csl_phase {
	uint64_t timestamp;
	uint16_t csl_phase;
};

/**
 * @brief Security Key description
 *
 */
struct alif_mac154_key_description {
	uint8_t *key_value;
	uint8_t *key_id;
	uint32_t frame_counter;
	uint8_t key_id_mode;
	bool frame_counter_per_key;
};

/**
 * @brief Function prototype for RX Frame reception callback
 *
 */
typedef void (*rx_frame_received_callback)(struct alif_rx_frame_received *p_frame);

/**
 * @brief Function prototype for RX operation status callback
 *
 */
typedef void (*rx_status_callback)(enum alif_mac154_status_code status);

/**
 * @brief API callback functions.
 *
 */
struct alif_mac154_api_cb {
	rx_frame_received_callback rx_frame_recv_cb;
	rx_status_callback rx_status_cb;
};

/**
 * @brief Initialize 802.15.4 HAL
 *
 */
void alif_mac154_init(struct alif_mac154_api_cb *p_api);

/**
 * @brief Reset 802.15.4 radio.
 *
 * @return     ALIF_MAC154_STATUS_OK           Operation OK
 *             ALIF_MAC154_STATUS_FAILED       Operation failed
 *             ALIF_MAC154_STATUS_COMM_FAILURE Module not connected
 */
enum alif_mac154_status_code alif_mac154_reset(void);

/**
 * @brief Get Alif 15.4 HAL version.
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_version_get(uint8_t *p_major, uint8_t *p_minor,
						     uint8_t *p_patch);

/**
 * @brief Get Alif 15.4 HW capabilities.
 *
 * @return	Mac Hardware supported features
 */
uint32_t alif_mac154_capabilities_get(void);
/**
 * @brief Get current timestamp
 *
 * @param	p_timestamp		timestamp in us
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_timestamp_get(uint64_t *p_timestamp);

/**
 * @brief Transmission of frame
 *
 * @param[in]	p_tx		Pointer to transmision parameters
 * @param[out]	p_tx_ack	Pointer received Ack parameters
 *
 * @return	ALIF_MAC154_STATUS_OK				When transmission Done
 *		ALIF_MAC154_STATUS_NO_ACK			ACK required, but not received
 *		ALIF_MAC154_STATUS_CHANNEL_ACCESS_FAILURE	Channel busy
 *		ALIF_MAC154_STATUS_FAILED			Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE			Module not connected
 */
enum alif_mac154_status_code alif_mac154_transmit(struct alif_tx_req *p_tx,
						  struct alif_tx_ack_resp *p_tx_ack);

/**
 * @brief Start receiver.
 *
 * @param[in]	p_rx		Pointer to receiver parameters
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_receive_start(struct alif_rx_enable *p_rx);

/**
 * @brief Stop receiver
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_receive_stop(void);

/**
 * @brief Energy detection measure
 *
 * @param[in]	p_energy_measure	Parameters for ED measurement
 * @param[out]	p_energy_measure_result	ED result
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code
alif_mac154_energy_detection(struct alif_energy_detect *p_energy_detect,
			     struct alif_energy_detect_response *p_energy_detect_result);

/**
 * @brief Set device short address
 *
 * @param[in]	short_address		Short address of the device
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_short_address_set(uint16_t short_address);

/**
 * @brief Set device extended address
 *
 * @param[in]	extended_address	8 bytes long extended address.
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_extended_address_set(uint8_t *p_extended_address);

/**
 * @brief Set device PAN ID.
 *
 * @param[in]	pan_id			the PAN id of the device
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_pan_id_set(uint16_t pan_id);

/**
 * @brief Insert an short address to the pending list
 *
 * @param[in]	short_address	Short address added to pendings list
 *				0xFFFF : Clear the list of short address
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_pendings_short_address_insert(uint16_t short_address);

/**
 * @brief remove short address from the pending list
 *
 * @param[in]	short_address	short address removed from the list
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_pendings_short_address_remove(uint16_t short_address);

/**
 * @brief Insert an extended address in the pending list
 *
 * @param[in]	p_extended_address	extended address added to list
 *				NULL : Clear the list of long address
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_pendings_long_address_insert(uint8_t *p_extended_address);

/**
 * @brief Remove an extended address from the pending list
 *
 * @param[in]	p_extended_address	extended address removed from list
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_pendings_long_address_remove(uint8_t *p_extended_address);

/**
 * @brief Set the promiscuous mode
 *
 * @param[in]	dbm			tx Power in dBm.
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_promiscious_mode_set(bool promiscuous_mode);

/**
 * @brief Set the TX power of transmissions
 *
 * @param[in]	dbm			tx Power in dBm.
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_tx_power_set(int16_t dbm);

/**
 * @brief Set the CCA detection mode
 *
 * @param[in]	mode			CCA Mode.
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_cca_mode_set(enum alif_mac154_cca_mode mode);

/**
 * @brief Set energy detection level for CCA
 *
 * @param[in]	input			input Signal level to detect
 *					channel as busy
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_ed_threshold_set(int8_t input);

/**
 * @brief RF debug command
 *
 * @param[in]	write			1 if value is written
 * @param[in]	key			parameter key identifier
 * @param[in]	value			Value to write
 * @param[in]	p_read			Value read from driver
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_dbg_rf(uint8_t write, uint32_t key, uint32_t value,
						uint32_t *p_read);

/**
 * @brief Set the csl period and enable the CSL receiver mode
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code
alif_mac154_csl_config_set(struct alif_mac154_csl_config *p_csl_config);

/**
 * @brief Set RX slot for receiver
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_rx_slot_set(struct alif_mac154_rx_slot *p_rx_slot_config);

/**
 * @brief Set expected RX time
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_expected_rx_time_set(uint32_t expected_rx_time);

/**
 * @brief Set Security frame counter
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code alif_mac154_security_frame_counter_set(uint32_t frame_counter);

/**
 * @brief Update Security frame counter if new value is larger
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code
alif_mac154_security_frame_counter_set_if_larger(uint32_t frame_counter);

/**
 * @brief Config Security key description list
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code
alif_mac154_key_value_description_set(struct alif_mac154_key_description *key_desc_list,
				       int list_size);

/**
 * @brief Parse mac header information from given packet
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 */
enum alif_mac154_status_code
alif_mac154_tx_packet_parse(struct alif_802154_frame_parser *mac_frame);

/**
 * @brief CCM endode and authenticate formac packet
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code
alif_mac154_mac_data_encode_and_authenticate(struct alif_802154_frame_parser *mac_frame,

/**
 * @brief Discover selected Information header data
 *
 * @return	True		IE element header is parsed and available
 *		False	IE element not exist
 */					     uint8_t *mac64);
bool alif_mac154_ie_header_element_get(uint8_t *header_ptr, uint16_t length,
				       struct mac_header_IE_s *header_ie);

/**
 * @brief get current CSL phase and timestamp when that was calculated.
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code
alif_mac154_csl_phase_get(struct alif_mac154_csl_phase *p_csl_phase_resp);

/**
 * @brief Set enhanced ACK IE elements.
 * supports:IEEE802154_HEADER_IE_ELEMENT_ID_CSL_IE
 *
 * @return	ALIF_MAC154_STATUS_OK		Operation OK
 *		ALIF_MAC154_STATUS_FAILED	Operation failed
 *		ALIF_MAC154_STATUS_COMM_FAILURE	Module not connected
 */
enum alif_mac154_status_code
alif_mac154_ack_header_ie_set(uint16_t short_address, const uint8_t *p_extended_address,
			      bool delete_ie, const struct ieee802154_header_ie *p_header_ie);

/**
 * @brief Get promiscuous mode configuration
 *
 * @return	true/false
 */
bool alif_mac154_get_promiscuous_mode(void);

/**
 * @brief Get Configured transmission power
 *
 * @return	value in dbm
 */
int8_t alif_mac154_get_tx_power(void);

/**
 * @brief Get last RSSI received
 *
 * @return	value in dbm
 */
int8_t alif_mac154_get_last_rssi(void);

/**
 * @brief Get Short address of the device
 *
 * @return	short address
 */
uint16_t alif_mac154_get_short_address(void);

/**
 * @brief Get extended address of the device
 *
 * @return	pointer to 8 byte array
 */
void alif_mac154_get_extended_address(uint8_t *external_address);

/**
 * @brief Get PANID of the device
 *
 * @return	pan id
 */
uint16_t alif_mac154_get_pan_id(void);

/**
 * @brief Get Transmission operation Priority
 *
 * @return	priority
 */
uint8_t alif_mac154_get_priority_tx(void);

/**
 * @brief Get receive operation Priority
 *
 * @return	priority
 */
uint8_t alif_mac154_get_priority_rx(void);

/**
 * @brief Get Energy detection operation Priority
 *
 * @return	priority
 */
uint8_t alif_mac154_get_priority_ed(void);

/**
 * @brief Get Configured CCA mode
 *
 * @return	cca mode
 */
enum alif_mac154_cca_mode alif_mac154_get_cca_mode(void);

/**
 * @brief Get Configured CCA threshold
 *
 * @return	cca threshold in dbm
 */
int8_t alif_mac154_get_cca_threshold(void);

#endif /* ALIF_MAC154_API_H_ */
