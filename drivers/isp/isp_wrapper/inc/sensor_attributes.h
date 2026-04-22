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
	.minIntLine = 0x1,
	.intLineStep = 1,
	.maxAgain = 0x80000,
	.minAgain = 0x10000,
	.againStep = 1,
	.maxDgain = 0x1FF,
	.minDgain = 64,
	.dgainStep = 1,
	.aeRunInterval = 1,
	.aeTarget = 50,

	.dampOver = 0x40,
	.dampUnder = 0x40,
	.tolerance = 1,

	.initExposure = 80,
	.aeMode = AE_MODE_FIX_FRAME_RATE,
	.gainThreshold = 1024,
};

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_ISP_ISP_WRAPPER_INC_SENSOR_ATTRIBUTES_H */
