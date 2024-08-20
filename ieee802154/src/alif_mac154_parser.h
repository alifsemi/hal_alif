/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "alif_mac154_def.h"

#ifndef IEEE802154_SRC_ALIF_MAC154_PARSER_H_
#define IEEE802154_SRC_ALIF_MAC154_PARSER_H_

/* Mac Security level mask
 */
#define MAC154_SECURITY_LEVEL_MASK 0x07

/* Frame control field: Frame type
 */
#define MAC154_FRAME_TYPE_OFFSET 0
#define MAC154_FRAME_TYPE_MASK   0x07

/* Frame control field: Security enabled */
#define MAC154_SECURITY_ENABLED_OFFSET 0
#define MAC154_SECURITY_ENABLED_BIT    0x08

/* Frame control field: Frame pending bit */
#define MAC154_FRAME_PENDING_OFFSET 0
#define MAC154_FRAME_PENDING_BIT    0x10

/* Frame control field: pan id compression */
#define MAC154_ACKNOWLEDGE_REQUEST_OFFSET 0
#define MAC154_ACKNOWLEDGE_REQUEST_BIT    0x20

/* Frame control field: pan id compression */
#define MAC154_PAN_ID_COMPRESSION_OFFSET 0
#define MAC154_PAN_ID_COMPRESSION_BIT    0x40

/* Frame control field: reserved */
#define MAC154_FRAME_CONTROL_FIELD_RESERVED_OFFSET 0
#define MAC154_FRAME_CONTROL_FIELD_RESERVED_BIT    0x80

/* Frame control field: Sequence number suppression */
#define MAC154_SEQUENCE_NUMBER_SUPPRESSION_OFFSET 1
#define MAC154_SEQUENCE_NUMBER_SUPPRESSION_BIT    0x01

/* Frame control field: Information elements present */
#define MAC154_INFORMATION_ELEMENTS_PRESENT_OFFSET 1
#define MAC154_INFORMATION_ELEMENTS_PRESENT_BIT    0x02

/* Frame control field: Destination addressing mode */
#define MAC154_DESTINATION_ADDRESSING_MODE_OFFSET 1
#define MAC154_DESTINATION_ADDRESSING_MODE_MASK   0x0c
#define MAC154_DESTINATION_ADDRESSING_MODE_SHIFT  2

/* Frame control field: Frame version */
#define MAC154_FRAME_VERSION_OFFSET 1
#define MAC154_FRAME_VERSION_MASK   0x30
#define MAC154_FRAME_VERSION_SHIFT  4

/* Frame control field: Source addressing mode */
#define MAC154_SOURCE_ADDRESSING_MODE_OFFSET 1
#define MAC154_SOURCE_ADDRESSING_MODE_MASK   0xc0
#define MAC154_SOURCE_ADDRESSING_MODE_SHIFT  6

/* Secure control field */
#define MAC154_SECURE_CONTROL_FIELD_SIZE                          1
#define MAC154_SECURE_FRAME_COUNTER_SIZE                          4
#define MAC154_SECURE_CONTROL_FIELD_FRAME_COUNTER_SUPPRESSION_BIT 0x20
#define MAC154_SECURE_CONTROL_FIELD_SECURITY_LEVEL_MASK           0x07
#define MAC154_SECURE_CONTROL_FIELD_KEY_IDENTIFIER_MODE_MASK      0x18
#define MAC154_SECURE_CONTROL_FIELD_KEY_IDENTIFIER_MODE_SHIFT     3

/**
 * @brief Get security enabled bit value from Frame control field.
 *
 * @param[in]	p_frame Pointer to frame start
 *
 * @return	True Frame security and authentication is ensabled.
 * @return	False Frame security and authentication is disabled.
 */
static inline bool alif_mac154_header_parser_security_enabled(const uint8_t *p_frame)
{
	return ((p_frame[MAC154_SECURITY_ENABLED_OFFSET] & MAC154_SECURITY_ENABLED_BIT) ==
		MAC154_SECURITY_ENABLED_BIT);
}

/**
 * @brief Get Frame pending bit value from Frame control field.
 *
 * @param[in]	p_frame Pointer to frame start
 *
 * @return	True Data pending is at sender.
 * @return	False No pending data.
 */
static inline bool alif_mac154_header_parser_frame_pending(const uint8_t *p_frame)
{
	return ((p_frame[MAC154_FRAME_PENDING_OFFSET] & MAC154_FRAME_PENDING_BIT) ==
		MAC154_FRAME_PENDING_BIT);
}

/**
 * @brief Get acknowledge request bit value from Frame control field.
 *
 * @param[in]	p_frame Pointer to frame start
 *
 * @return	True Ack is requested.
 * @return	False Ack is not requested.
 */
static inline bool alif_mac154_header_parser_acknowledge_request(const uint8_t *p_frame)
{
	return ((p_frame[MAC154_ACKNOWLEDGE_REQUEST_OFFSET] & MAC154_ACKNOWLEDGE_REQUEST_BIT) ==
		MAC154_ACKNOWLEDGE_REQUEST_BIT);
}

/**
 * @brief Get PANID compression bit value from Frame control field.
 *
 * @param[in]	p_frame Pointer to frame start
 *
 * @return	True PANID compression enabled at frame.
 * @return	False PANID compression disabled at frame.
 */
static inline bool alif_mac154_header_parser_pan_id_compression(const uint8_t *p_frame)
{
	return ((p_frame[MAC154_PAN_ID_COMPRESSION_OFFSET] & MAC154_PAN_ID_COMPRESSION_BIT) ==
		MAC154_PAN_ID_COMPRESSION_BIT);
}


/**
 * @brief Get Sequence number supression bit value from Frame control field.
 *
 * @param[in]	p_frame Pointer to frame start
 *
 * @return	True Sequence number suppressed
 * @return	False Sequence number is at frame
 */
static inline bool alif_mac154_header_parser_seq_num_suppression(const uint8_t *p_frame)
{
	return ((p_frame[MAC154_SEQUENCE_NUMBER_SUPPRESSION_OFFSET] &
		 MAC154_SEQUENCE_NUMBER_SUPPRESSION_BIT) == MAC154_SEQUENCE_NUMBER_SUPPRESSION_BIT);
}


/**
 * @brief Get information elements present bit value from Frame control field
 *
 * @param[in]	p_frame Pointer to frame start
 *
 * @return	True Informatio elemenent are included at frame
 * @return	False no Information elements not included at frame
 */
static inline bool alif_mac154_header_parser_ie_present(const uint8_t *p_frame)
{
	return ((p_frame[MAC154_INFORMATION_ELEMENTS_PRESENT_OFFSET] &
		 MAC154_INFORMATION_ELEMENTS_PRESENT_BIT) ==
		MAC154_INFORMATION_ELEMENTS_PRESENT_BIT);
}


/**
 * @brief Get Frame type from Frame control field
 *
 * @param[in]	p_frame Pointer to frame start
 *
 * @return	Parsed 15.4 frame tye
 */
static inline uint8_t alif_mac154_header_parser_frame_type(const uint8_t *p_frame)
{
	return p_frame[MAC154_FRAME_TYPE_OFFSET] & MAC154_FRAME_TYPE_MASK;
}


/**
 * @brief Get Destination addressing mode from Frame control field
 *
 * @param[in]	p_frame	Pointer to frame start
 *
 * @return	Parsed 15.4 Destiantion address type
 */
static inline uint8_t alif_mac154_header_parser_dst_addr_mode(const uint8_t *p_frame)
{
	return ((p_frame[MAC154_DESTINATION_ADDRESSING_MODE_OFFSET] &
		 MAC154_DESTINATION_ADDRESSING_MODE_MASK) >>
		MAC154_DESTINATION_ADDRESSING_MODE_SHIFT);
}

/**
 * @brief Get Frame version from Frame control field
 *
 * @param[in]	p_frame	Pointer to frame start
 *
 * @return	Parsed 15.4 Trame version type
 */
static inline uint8_t alif_mac154_header_parser_frame_version(const uint8_t *p_frame)
{
	return (p_frame[MAC154_FRAME_VERSION_OFFSET] & MAC154_FRAME_VERSION_MASK) >>
	       MAC154_FRAME_VERSION_SHIFT;
}

/**
 * @brief Get Source addressing mode from Frame control field
 *
 * @param[in]	p_frame	Pointer to frame start
 *
 * @return	Parsed 15.4 Source address type
 */
static inline uint8_t alif_mac154_header_parser_src_addr_mode(const uint8_t *p_frame)
{
	return (p_frame[MAC154_SOURCE_ADDRESSING_MODE_OFFSET] &
		MAC154_SOURCE_ADDRESSING_MODE_MASK) >>
	       MAC154_SOURCE_ADDRESSING_MODE_SHIFT;
}

/**
 * @brief Get frame counter suppression field from security header.
 *
 * @param[in]	secure_control_field Security control information header
 *
 * @return	True No farme counter included
 * @return	False 32-bit frame counter is included
 */
static inline bool
alif_mac154_header_parser_scf_frame_counter_suppression(const uint8_t secure_control_field)
{
	return (secure_control_field & MAC154_SECURE_CONTROL_FIELD_FRAME_COUNTER_SUPPRESSION_BIT) ==
	       MAC154_SECURE_CONTROL_FIELD_FRAME_COUNTER_SUPPRESSION_BIT;
}

/**
 * @brief Get security level field from security header.
 *
 * @param[in]	secure_control_field Security control information header
 *
 * @return	Frame used Security level
 */
static inline uint8_t
alif_mac154_header_parser_scf_security_level(const uint8_t secure_control_field)
{
	return secure_control_field & MAC154_SECURE_CONTROL_FIELD_SECURITY_LEVEL_MASK;
}

/**
 * @brief Get Key id mode field from security header.
 *
 * @param[in]	secure_control_field Security control information header
 *
 * @return	Frame used Key Id mode
 */
static inline uint8_t
alif_mac154_header_parser_scf_key_identifier_mode(const uint8_t secure_control_field)
{
	return (secure_control_field & MAC154_SECURE_CONTROL_FIELD_KEY_IDENTIFIER_MODE_MASK) >>
	       MAC154_SECURE_CONTROL_FIELD_KEY_IDENTIFIER_MODE_SHIFT;
}

/**
 * @brief Get Key id field length.
 *
 * @param[in]	key_id_mode Key id mode
 *
 * @return	Length of Key id field in bytes
 */
static inline uint8_t
alif_mac154_header_parser_scf_key_identifier_length(enum mac154_sec_keyid_mode key_id_mode)
{
	switch (key_id_mode) {
	case MAC154_KEY_IDENTIFIER_MODE_2:
		return 4;
	case MAC154_KEY_IDENTIFIER_MODE_3:
		return 8;
	default:
	}
	return 0;
}

/**
 * @brief Get length of MIC.
 *
 * @param[in]	sec_level Security level
 *
 * @return	Length of MIC in bytes
 */
static inline uint8_t alif_mac154_header_parser_scf_mic_length(enum mac154_sec_level sec_level)
{
	switch (sec_level) {
	case MAC154_SECURITY_LEVEL_MIC_32:
	case MAC154_SECURITY_LEVEL_ENC_MIC_32:
		return 4;
	case MAC154_SECURITY_LEVEL_MIC_64:
	case MAC154_SECURITY_LEVEL_ENC_MIC_64:
		return 8;
	case MAC154_SECURITY_LEVEL_MIC_128:
	case MAC154_SECURITY_LEVEL_ENC_MIC_128:
		return 16;
	default:
	}
	return 0;
}

/**
 * @brief Parse 802.15.4 header.
 *
 * @param[in]	mac_frame pointer to Mac Frame
 *
 * @return	True Frame parsed succesfully
 * @return	False Frame parse fail
 */
bool alif_mac154_mac_frame_parse(struct alif_802154_frame_parser *mac_frame);

/**
 * @brief Discover selected Header information elements header.
 *
 * @param[in]	header_ptr pointer to Information element field
 * @param[in]	length length of Information element field
 * @param[in]	header_ie requested Information element strutrue info
 *
 * @return	True Information element discoved and header_ie pointer and length is updated
 * @return	False Discover fail
 */
bool alif_mac154_ie_header_discover(uint8_t *header_ptr, uint16_t length,
				    struct mac_header_IE_s *header_ie);

#endif /* IEEE802154_SRC_ALIF_MAC154_PARSER_H_ */
