/*
 * Copyright (c) 2026 Alif Semiconductor
 * SPDX-License-Identifier: Apache-2.0
 *
 * OS agnostic VeriSilicon Hantro VC9000E JPEG Hardware Encoder Driver
 */

#ifndef JPEG_HANTRO_VC9000E_SW_H_
#define JPEG_HANTRO_VC9000E_SW_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Library version */
#define JPEG_SW_LIB_VERSION_MAJOR    1
#define JPEG_SW_LIB_VERSION_MINOR    3
#define JPEG_SW_LIB_VERSION_PATCH    0

#define JPEG_SW_STRINGIFY(x) #x
#define JPEG_SW_TOSTRING(x)  JPEG_SW_STRINGIFY(x)

#define JPEG_SW_LIB_VERSION_STRING		\
	JPEG_SW_TOSTRING(JPEG_SW_LIB_VERSION_MAJOR) "." \
	JPEG_SW_TOSTRING(JPEG_SW_LIB_VERSION_MINOR) "." \
	JPEG_SW_TOSTRING(JPEG_SW_LIB_VERSION_PATCH)

/* Packed version for compile-time comparison:
 *   #if JPEG_SW_LIB_VERSION_CODE >= JPEG_SW_LIB_MAKE_VERSION(1, 3, 0)
 */
#define JPEG_SW_LIB_MAKE_VERSION(major, minor, patch) \
	(((major) << 16) | ((minor) << 8) | (patch))
#define JPEG_SW_LIB_VERSION_CODE \
	JPEG_SW_LIB_MAKE_VERSION(JPEG_SW_LIB_VERSION_MAJOR, \
				    JPEG_SW_LIB_VERSION_MINOR, \
				    JPEG_SW_LIB_VERSION_PATCH)

const char *jpeg_sw_lib_get_version(void);

int jpeg_qf_scaling(int quality);
void jpeg_calc_q_table(int scale_factor);
void jpeg_set_q_table(uint32_t base);

/** Specifies the encoding format of thumbnail. */
typedef enum {
	/** Thumbnail encoded using JPEG format (JFXX extension). */
	JPEGENC_THUMB_JPEG = 0x10,
	/** Thumbnail stored using one byte per pixel (JFXX extension). */
	JPEGENC_THUMB_PALETTE_RGB8 = 0x11,
	/** Thumbnail stored using three bytes per pixel (JFIF APP0). */
	JPEGENC_THUMB_RGB24 = 0x13
} JpegEncThumbFormat;

/** Thumbnail information — caller prepares the data, library embeds it. */
struct jpeg_thumbnail_info {
	JpegEncThumbFormat format; /**< Thumbnail format. */
	uint8_t  width;            /**< Thumbnail width in pixels (1–255). */
	uint8_t  height;           /**< Thumbnail height in pixels (1–255). */
	const void *data;          /**< Pre-prepared thumbnail data (caller owns). */
	uint32_t data_length;      /**< Size of data in bytes. */
};

/**
 * @brief Slice-mode information.
 *
 * Caller fills this struct when the encoder is run in slice mode and
 * passes a pointer to it via @ref jpeg_header_info.slice. When provided
 * with a non-zero @ref restart_interval, @ref jpeg_header_generation
 * emits an FFDD DRI segment in the JPEG header so that decoders can
 * correctly interpret the FFD0..FFD7 restart markers that the hardware
 * emits at every slice boundary.
 *
 * For frame-mode (single-shot) encoding, leave
 * @ref jpeg_header_info.slice = NULL; no DRI segment is emitted.
 */
struct jpeg_slice_info {
	/** Restart interval, in MCUs (NOT MCU rows).
	 *  Typical value: slice_height_rows * (encoded_width / 16).
	 */
	uint16_t restart_interval;
};

struct jpeg_header_info {
	uint8_t *buffer;
	uint32_t width;
	uint32_t height;
	uint8_t num_components;
	const struct jpeg_thumbnail_info *thumbnail; /**< NULL = no thumbnail */
	const struct jpeg_slice_info     *slice;     /**< NULL = no DRI / frame mode */
};

int jpeg_header_generation(struct jpeg_header_info header);
int jpeg_header_get_size(const struct jpeg_thumbnail_info *thumbnail,
			 const struct jpeg_slice_info     *slice);

/**
 * @brief Inputs for NV12 crop offset calculation.
 */
struct jpeg_crop_input {
	uint32_t input_addr;   /**< Base address of the NV12 frame. */
	uint32_t crop_x;       /**< Horizontal crop origin (must be even). */
	uint32_t crop_y;       /**< Vertical crop origin (must be even). */
	uint32_t stride;       /**< Luma stride in bytes per row. */
	uint32_t frame_height; /**< Full luma plane height in pixels. */
};

/**
 * @brief Computed crop offsets for NV12 input.
 *
 * Both addresses are 16-byte aligned; sub-16-byte remainders go in the
 * separate offset fields, which the driver writes to chroma/luma base
 * offset registers.
 */
struct jpeg_crop_offsets {
	uint32_t luma_addr;          /**< 16-byte aligned luma base address. */
	uint32_t luma_base_offset;   /**< Sub-16-byte luma remainder (0..15). */
	uint32_t chroma_addr;        /**< 16-byte aligned chroma base address. */
	uint32_t chroma_base_offset; /**< Sub-16-byte chroma remainder (0..15). */
};

/**
 * @brief Compute NV12 luma/chroma crop offsets (vendor algorithm).
 *
 * @param in  Input parameters (caller-filled).
 * @param out Output struct populated with addresses and offsets.
 */
void jpeg_calc_crop_offsets(const struct jpeg_crop_input *in,
			    struct jpeg_crop_offsets *out);

#ifdef __cplusplus
}
#endif

#endif /* JPEG_HANTRO_VC9000E_SW_H_ */
