/*
 * Copyright (C) 2026 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DRIVERS_ISP_ISP_WRAPPER_INC_SENSOR_ATTRIBUTES_H
#define DRIVERS_ISP_ISP_WRAPPER_INC_SENSOR_ATTRIBUTES_H

#include "vsi_comm_sns.h"

#ifdef __cplusplus
extern "C"{
#endif

static AE_SNS_DEFAULT_S sensor_attributes = {
	.fullLinesMax = 0xFFFF,
	.fullLinesStd = 0xA47B,
	.fullLines = 0xA47B,
	.fps = 5 * ISP_SNS_FPS_ACCU,
	.maxIntLine = 0xA478,
	.minIntLine = 0x3,
	.intLineStep = 1,
	.maxAgain = 80 * 1024,
	.minAgain = 10 * 1024,
	.againStep = 1,
	.maxDgain = 8 * 1024,
	.minDgain = 1 * 1024,
	.dgainStep = 1,
	.aeRunInterval = 1,
	.aeTarget = 100,

	.dampOver = 0x40,
	.dampUnder = 0x40,
	.tolerance = 10,

	.initExposure = 100000,
	.aeMode = AE_MODE_FIX_FRAME_RATE,
	.gainThreshold = 10 * 1024,
};

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_ISP_ISP_WRAPPER_INC_SENSOR_ATTRIBUTES_H */
