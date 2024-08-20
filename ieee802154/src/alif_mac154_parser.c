/*
 * Copyright (c) 2024 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <zephyr/sys/byteorder.h>
#include "alif_mac154_parser.h"

#define LOG_MODULE_NAME alif_mac154_parser
#define LOG_LEVEL       LOG_LEVEL_DBG

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

static bool source_panid_is_present(struct mac154_fcf *fcf)
{

	switch (fcf->frame_version) {
	case MAC154_FRAME_VERSION_2003:
	case MAC154_FRAME_VERSION_2006:
		if (fcf->sam == MAC154_ADDRESSING_MODE_NOT_PRESENT) {
			return false;
		}
		return !fcf->panid_compression;
	default:
	}
	/*
	 * Table 7-2—PAN ID Compression field value for frame version 0b10
	 * col: 2,4,6,8,12,13,14
	 */
	if (fcf->panid_compression) {
		return false;
	}
	/* col: 5 */
	if (fcf->dam == MAC154_ADDRESSING_MODE_NOT_PRESENT &&
	    fcf->sam != MAC154_ADDRESSING_MODE_NOT_PRESENT) {
		return true;
	}
	/* col: 9,10,11 */
	if (fcf->dam == MAC154_ADDRESSING_MODE_SHORT || fcf->sam == MAC154_ADDRESSING_MODE_SHORT) {
		return true;
	}
	/* col: 1, 3, 7 */
	return false;
}

static bool destination_panid_is_present(struct mac154_fcf *fcf)
{

	switch (fcf->frame_version) {
	case MAC154_FRAME_VERSION_2003:
	case MAC154_FRAME_VERSION_2006:
		if (fcf->dam != MAC154_ADDRESSING_MODE_NOT_PRESENT) {
			return true;
		}
		return false;
	default:
	}

	if (fcf->dam == MAC154_ADDRESSING_MODE_EXTENDED &&
	    fcf->sam == MAC154_ADDRESSING_MODE_EXTENDED) {
		return !fcf->panid_compression;
	}

	if (fcf->dam != MAC154_ADDRESSING_MODE_NOT_PRESENT &&
	    fcf->sam != MAC154_ADDRESSING_MODE_NOT_PRESENT) {
		return true;
	}

	if (fcf->sam != MAC154_ADDRESSING_MODE_NOT_PRESENT) {
		return false;
	}

	if (fcf->dam == MAC154_ADDRESSING_MODE_NOT_PRESENT) {
		return fcf->panid_compression;
	}

	return !fcf->panid_compression;
}

static bool mac_parse_header_ie(struct mac_header_IE_s *header_element, uint8_t *ptr)
{
	uint16_t ie_dummy = sys_get_le16(ptr);

	if (ie_dummy & 0x8000) {
		return false;
	}
	header_element->length = (ie_dummy & 0x007f);
	header_element->id = ((ie_dummy & 0x7f80) >> 7);
	header_element->content_ptr = ptr + 2;
	return true;
}

static bool mac_parse_payload_ie(struct mac_payload_IE_s *payload_element, uint8_t *ptr)
{
	uint16_t ie_dummy = sys_get_le16(ptr);

	if (!(ie_dummy & 0x8000)) {
		return false;
	}
	payload_element->length = (ie_dummy & 0x07ff);
	payload_element->id = ((ie_dummy & 0x7800) >> 11);
	payload_element->content_ptr = ptr + 2;
	return true;
}

static bool alif_mac154_header_ie_parse(struct alif_802154_frame_parser *mac_frame)
{

	struct mac_header_IE_s header_ie;
	uint8_t *ptr = mac_frame->mac_payload;
	struct alif_802154_ie_params *ie_params = &mac_frame->ie_info;

	if (mac_frame->mac_payload_length < 2) {
		return false;
	}

	ie_params->ie_header_ptr = ptr;

	while (mac_frame->mac_payload_length >= 2) {
		if (!mac_parse_header_ie(&header_ie, ptr)) {
			return false;
		}

		mac_frame->mac_payload_length -= 2;
		if (header_ie.length > mac_frame->mac_payload_length) {
			return false;
		}

		mac_frame->mac_payload_length -= header_ie.length;
		ie_params->ie_header_len += header_ie.length + 2;
		ptr += (2 + header_ie.length);

		if (header_ie.id == MAC_HEADER_TERMINATION1_IE_ID) {
			ie_params->payload_ie_presents = true;
			break;
		} else if (header_ie.id == MAC_HEADER_TERMINATION2_IE_ID) {
			/* Update Payload ptr */
			break;
		}
	}
	/* Update Payload pointer start */
	mac_frame->mac_payload = ptr;
	/* Update Header leghth */
	mac_frame->mac_header_length += ie_params->ie_header_len;
	return true;
}

static bool alif_mac154_payload_ie_parse(struct alif_802154_frame_parser *mac_frame)
{
	struct mac_payload_IE_s payload_ie;
	uint8_t *ptr = mac_frame->mac_payload;
	struct alif_802154_ie_params *ie_params = &mac_frame->ie_info;

	if (!ie_params->payload_ie_presents) {
		return true;
	}

	if (mac_frame->mac_payload_length < 2) {
		return false;
	}

	ie_params->ie_payload_ptr = ptr;

	while (mac_frame->mac_payload_length >= 2) {
		if (!mac_parse_payload_ie(&payload_ie, ptr)) {
			return false;
		}

		mac_frame->mac_payload_length -= 2;
		if (payload_ie.length > mac_frame->mac_payload_length) {
			return false;
		}

		mac_frame->mac_payload_length -= payload_ie.length;

		if (payload_ie.id == MAC_PAYLOAD_TERMINATION_IE_GROUP_ID) {
			break;
		}
		ie_params->ie_payload_len += payload_ie.length + 2;
		mac_frame->mac_payload_length += payload_ie.length + 2;
		ptr += (2 + payload_ie.length);
	}

	mac_frame->mac_payload = ptr;
	return true;
}

static bool alif_mac_fcf_header_parse(struct alif_802154_frame_parser *mac_frame)
{
	struct alif_802154_header *frame_info = &mac_frame->gen_header;

	mac_frame->mac_header_length = 2;

	/* Parse FCF */
	frame_info->fcf.frame_type = alif_mac154_header_parser_frame_type(mac_frame->mac_packet);
	frame_info->fcf.frame_version =
		alif_mac154_header_parser_frame_version(mac_frame->mac_packet);
	frame_info->fcf.security_enabled =
		alif_mac154_header_parser_security_enabled(mac_frame->mac_packet);
	frame_info->fcf.panid_compression =
		alif_mac154_header_parser_pan_id_compression(mac_frame->mac_packet);
	frame_info->fcf.ack_requested =
		alif_mac154_header_parser_acknowledge_request(mac_frame->mac_packet);
	frame_info->fcf.sam = alif_mac154_header_parser_src_addr_mode(mac_frame->mac_packet);
	frame_info->fcf.dam = alif_mac154_header_parser_dst_addr_mode(mac_frame->mac_packet);

	if (frame_info->fcf.frame_version <= MAC154_FRAME_VERSION_2006) {
		frame_info->fcf.seq_nb_suppression = false;
		frame_info->fcf.ie_elements = false;
	} else {
		frame_info->fcf.seq_nb_suppression =
			alif_mac154_header_parser_seq_num_suppression(mac_frame->mac_packet);
		frame_info->fcf.ie_elements =
			alif_mac154_header_parser_ie_present(mac_frame->mac_packet);
	}

	frame_info->fcf.d_pan_id_present = destination_panid_is_present(&frame_info->fcf);
	frame_info->fcf.s_pan_id_present = source_panid_is_present(&frame_info->fcf);

	if (!frame_info->fcf.seq_nb_suppression) {
		/* Sequence number is present */
		mac_frame->mac_header_length += 1;
	}

	if (frame_info->fcf.d_pan_id_present) {
		/* destination PANID is present */
		frame_info->dst_pan_id_offset = mac_frame->mac_header_length;
		mac_frame->mac_header_length += 2;
	}

	frame_info->dst_addr_offset = mac_frame->mac_header_length;
	if (frame_info->fcf.dam == MAC154_ADDRESSING_MODE_SHORT) {
		/* short destination address is present */

		mac_frame->mac_header_length += 2;
	} else if (frame_info->fcf.dam == MAC154_ADDRESSING_MODE_EXTENDED) {
		/* extended destination address is present */
		mac_frame->mac_header_length += 8;
	} else {
		return false;
	}

	if (frame_info->fcf.s_pan_id_present) { /* source PANID is present */
		frame_info->src_pan_id_offset = mac_frame->mac_header_length;
		mac_frame->mac_header_length += 2;
	}

	frame_info->src_addr_offset = mac_frame->mac_header_length;
	if (frame_info->fcf.sam == MAC154_ADDRESSING_MODE_SHORT) {
		/* short destination address is present */
		mac_frame->mac_header_length += 2;
	} else if (frame_info->fcf.sam == MAC154_ADDRESSING_MODE_EXTENDED) {
		/* extended destination address is present */
		mac_frame->mac_header_length += 8;
	} else {
		return false;
	}
	return true;
}

static bool alif_mac_security_header_parse(struct alif_802154_frame_parser *mac_frame)
{
	struct alif_802154_header *frame_info = &mac_frame->gen_header;
	struct alif_802154_ccm_params *ccm_params = &mac_frame->ccm_params;

	ccm_params->mic = NULL;
	ccm_params->mic_len = 0;
	ccm_params->sec_level = MAC154_SECURITY_LEVEL_NONE;
	/* Init packet encode state */
	mac_frame->encoded_packet = false;

	if (frame_info->fcf.security_enabled) {
		bool suppress_frame_conter =
			alif_mac154_header_parser_scf_frame_counter_suppression(
				mac_frame->mac_packet[mac_frame->mac_header_length]);

		if (mac_frame->mac_packet_length <
		    mac_frame->mac_header_length + MAC154_SECURE_CONTROL_FIELD_SIZE) {
			return false;
		}

		ccm_params->sec_level = alif_mac154_header_parser_scf_security_level(
			mac_frame->mac_packet[mac_frame->mac_header_length]);
		ccm_params->key_id_mode = alif_mac154_header_parser_scf_key_identifier_mode(
			mac_frame->mac_packet[mac_frame->mac_header_length]);
		mac_frame->mac_header_length++;
		ccm_params->mic_len =
			alif_mac154_header_parser_scf_mic_length(ccm_params->sec_level);

		/* Parse sec level and key-id mode */
		if (!suppress_frame_conter) {
			if (mac_frame->mac_packet_length < mac_frame->mac_header_length + 4) {
				return false;
			}
			ccm_params->sec_frame_counter =
				mac_frame->mac_packet + mac_frame->mac_header_length;
			mac_frame->mac_header_length += 4;
		} else {
			ccm_params->sec_frame_counter = NULL;
		}

		if (ccm_params->key_id_mode != MAC154_KEY_IDENTIFIER_MODE_0) {

			ccm_params->sec_key_source =
				mac_frame->mac_packet + mac_frame->mac_header_length;
			ccm_params->sec_key_source_len = 1;
			ccm_params->sec_key_source_len +=
				alif_mac154_header_parser_scf_key_identifier_length(
					ccm_params->key_id_mode);
			mac_frame->mac_header_length += ccm_params->sec_key_source_len;
			if (mac_frame->mac_packet_length < mac_frame->mac_header_length) {
				return false;
			}
		}
		if (mac_frame->mac_packet_length <
		    mac_frame->mac_header_length + ccm_params->mic_len) {
			return false;
		}
		if (ccm_params->mic_len) {
			ccm_params->mic = mac_frame->mac_packet +
					  (mac_frame->mac_packet_length - ccm_params->mic_len);
		}
	}

	/* Set Payload pointer and length*/
	mac_frame->mac_payload = mac_frame->mac_packet + mac_frame->mac_header_length;
	mac_frame->mac_payload_length =
		mac_frame->mac_packet_length - mac_frame->mac_header_length - ccm_params->mic_len;
	return true;
}

static bool alif_mac154_ie_parse(struct alif_802154_frame_parser *mac_frame)
{
	bool open_payload = true;

	mac_frame->ie_info.ie_header_ptr = NULL;
	mac_frame->ie_info.ie_header_len = 0;
	mac_frame->ie_info.ie_payload_ptr = NULL;
	mac_frame->ie_info.ie_payload_len = 0;
	mac_frame->ie_info.payload_ie_presents = false;

	if (mac_frame->gen_header.fcf.ie_elements) {
		if (!alif_mac154_header_ie_parse(mac_frame)) {
			return false;
		}

		open_payload = mac_frame->ie_info.payload_ie_presents;

		if (!alif_mac154_payload_ie_parse(mac_frame)) {
			return false;
		}
	}

	if (mac_frame->gen_header.fcf.frame_type == MAC154_FRAME_TYPE_COMMAND && open_payload) {
		mac_frame->mac_header_length++;
		mac_frame->mac_payload++;
		mac_frame->mac_payload_length--;
	}

	return true;
}

bool alif_mac154_mac_frame_parse(struct alif_802154_frame_parser *mac_frame)
{

	/* Parse FCF */
	if (!alif_mac_fcf_header_parse(mac_frame)) {
		return false;
	}

	/*security header*/
	if (!alif_mac_security_header_parse(mac_frame)) {
		return false;
	}

	return alif_mac154_ie_parse(mac_frame);
}

bool alif_mac154_ie_header_discover(uint8_t *header_ptr, uint16_t length,
				    struct mac_header_IE_s *header_ie)
{
	struct mac_header_IE_s ie_element;

	while (length >= 2) {
		if (!mac_parse_header_ie(&ie_element, header_ptr)) {
			return false;
		}
		if (header_ie->id == ie_element.id) {
			header_ie->content_ptr = ie_element.content_ptr;
			header_ie->length = ie_element.length;
			return true;
		}

		length -= ie_element.length + 2;

		header_ptr += ie_element.length + 2;
	}
	return false;
}
