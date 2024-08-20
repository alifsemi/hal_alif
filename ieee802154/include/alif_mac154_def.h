/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ALIF_MAC154_DEF_H_
#define ALIF_MAC154_DEF_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Status codes
 */
enum alif_mac154_status_code {
	ALIF_MAC154_STATUS_OK = 0,
	ALIF_MAC154_STATUS_NO_ACK,
	ALIF_MAC154_STATUS_CHANNEL_ACCESS_FAILURE,
	ALIF_MAC154_STATUS_INVALID_STATE,
	ALIF_MAC154_STATUS_INVALID_MESSAGE,
	ALIF_MAC154_STATUS_HW_FAILED,
	ALIF_MAC154_STATUS_SW_FAILED,
	ALIF_MAC154_STATUS_FAILED,
	ALIF_MAC154_STATUS_COMM_FAILURE,
	ALIF_MAC154_STATUS_OUT_OF_SYNC,
	ALIF_MAC154_STATUS_ABORT,
	ALIF_MAC154_STATUS_RESET,
	ALIF_MAC154_STATUS_RX_STOPPED,
	ALIF_MAC154_STATUS_TIMER_SYNCH,
};

/**
 * CCA mode
 */
enum alif_mac154_cca_mode {
	ALIF_MAC154_CCA_MODE_ED = 0,
	ALIF_MAC154_CCA_MODE_CS,
	ALIF_MAC154_CCA_MODE_ED_OR_CS,
	ALIF_MAC154_CCA_MODE_ED_AND_CS,
};

/**
 * Security levels
 */
enum mac154_sec_level {
	MAC154_SECURITY_LEVEL_NONE = 0,
	MAC154_SECURITY_LEVEL_MIC_32 = 1,
	MAC154_SECURITY_LEVEL_MIC_64 = 2,
	MAC154_SECURITY_LEVEL_MIC_128 = 3,
	MAC154_SECURITY_LEVEL_ENC = 4,
	MAC154_SECURITY_LEVEL_ENC_MIC_32 = 5,
	MAC154_SECURITY_LEVEL_ENC_MIC_64 = 6,
	MAC154_SECURITY_LEVEL_ENC_MIC_128 = 7,
};

/**
 * Security Key indentier modes
 */
enum mac154_sec_keyid_mode {
	MAC154_KEY_IDENTIFIER_MODE_0 = 0,
	MAC154_KEY_IDENTIFIER_MODE_1 = 1,
	MAC154_KEY_IDENTIFIER_MODE_2 = 2,
	MAC154_KEY_IDENTIFIER_MODE_3 = 3,
};

/**
 * Mac frame types
 */
enum mac154_frame_type {
	MAC154_FRAME_TYPE_BEACON = 0,
	MAC154_FRAME_TYPE_DATA = 1,
	MAC154_FRAME_TYPE_ACK = 2,
	MAC154_FRAME_TYPE_COMMAND = 3,
	MAC154_FRAME_TYPE_RESERVED = 4,
	MAC154_FRAME_TYPE_MULTIPURPOSE = 5,
	MAC154_FRAME_TYPE_FRAGMENT = 6,
	MAC154_FRAME_TYPE_EXTENDED = 7,
};

/**
 * Mac frame versions
 */
enum mac154_frame_version {
	MAC154_FRAME_VERSION_2003 = 0,
	MAC154_FRAME_VERSION_2006 = 1,
	MAC154_FRAME_VERSION_2015 = 2,
	MAC154_FRAME_VERSION_RESERVED = 3,
};

/**
 * Address modes
 */
enum mac154_address_mode {
	MAC154_ADDRESSING_MODE_NOT_PRESENT = 0,
	MAC154_ADDRESSING_MODE_RESERVED = 1,
	MAC154_ADDRESSING_MODE_SHORT = 2,
	MAC154_ADDRESSING_MODE_EXTENDED = 3,
};

#define MAC_HEADER_VENDOR_SPESIFIC_IE_ID       0x00
#define MAC_HEADER_CSL_IE_ID                   0x1a
#define MAC_HEADER_ASSIGNED_EXTERNAL_ORG_IE_ID 0x2a
#define MAC_HEADER_TERMINATION1_IE_ID          0x7e
#define MAC_HEADER_TERMINATION2_IE_ID          0x7f

/**
 * Mac Header information element structure for parsing or write operation
 * This structure encapsulates security related variables,
 */
struct mac_header_IE_s {
	uint8_t *content_ptr; /**< Content data */
	unsigned length: 7;   /**< Element length 0- 127 */
	uint8_t id;           /**< Element ID */
};

#define MAC_PAYLOAD_IE_ESDU_GROUP_ID        0x00
#define MAC_PAYLOAD_MLME_IE_GROUP_ID        0x01
#define MAC_PAYLOAD_VENDOR_IE_GROUP_ID      0x02
#define MAC_PAYLOAD_MPX_IE_GROUP_ID         0x03
#define MAC_PAYLOAD_TERMINATION_IE_GROUP_ID 0x0f

/**
 * Mac Payload information element structure for parsing or write operation
 * This structure encapsulates security related variables,
 */
struct mac_payload_IE_s {
	uint8_t *content_ptr; /**< Content data */
	unsigned length: 11;  /**< Element length 0- 2047 */
	unsigned id: 4;       /**< Group ID */
};

/**
 * Mac parser generic frame control structure
 */
struct mac154_fcf {
	enum mac154_frame_type frame_type;
	enum mac154_frame_version frame_version;
	enum mac154_address_mode dam;
	enum mac154_address_mode sam;
	bool security_enabled;
	bool panid_compression;
	bool ack_requested;
	bool d_pan_id_present;
	bool s_pan_id_present;
	bool seq_nb_suppression;
	bool ie_elements;
};

/**
 * Mac parser Information element structure
 */
struct alif_802154_ie_params {
	uint8_t *ie_header_ptr;
	uint8_t *ie_payload_ptr;
	bool payload_ie_presents;
	uint8_t ie_payload_len;
	uint8_t ie_header_len;
};

/**
 * Mac parser Security header structure
 */
struct alif_802154_ccm_params {
	enum mac154_sec_level sec_level;
	enum mac154_sec_keyid_mode key_id_mode;
	uint8_t *sec_frame_counter;
	uint8_t *sec_key_source;
	uint8_t *mic;
	uint8_t sec_key_source_len;
	uint8_t mic_len;
};


/**
 * Mac generic parsed frame information with address
 */
struct alif_802154_header {
	struct mac154_fcf fcf;
	uint8_t dst_pan_id_offset;
	uint8_t dst_addr_offset;
	uint8_t src_pan_id_offset;
	uint8_t src_addr_offset;
};

/* Frame parser */
struct alif_802154_frame_parser {
	uint8_t *net_packet_ptr;
	uint8_t *mac_packet;
	uint8_t *mac_payload;
	size_t mac_packet_length;
	uint8_t mac_header_length;
	uint8_t mac_payload_length;
	struct alif_802154_header gen_header;
	struct alif_802154_ccm_params ccm_params;
	struct alif_802154_ie_params ie_info;
	bool encoded_packet;
	bool packet_ready;
};

#endif /* ALIF_MAC154_DEF_H_ */
