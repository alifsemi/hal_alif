/*
 * Copyright (C) 2026 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "vsi_comm_isp.h"
#include "vsi_comm_sns.h"

#ifdef __cplusplus
extern "C"{
#endif

AE_SNS_DEFAULT_S sensor_attributes = {
	.fullLinesMax = 0xFFFF,
	.fullLinesStd = 1000,
	.fullLines = 1000,
	.fps = 30 * ISP_SNS_FPS_ACCU,
	.maxIntLine = 998,
	.minIntLine = 1,
	.intLineStep = 1,
	.maxAgain = 8192,
	.minAgain = 1024,
	.againStep = 1,
	.maxDgain = 1024,
	.minDgain = 1024,
	.dgainStep = 1,
	.aeRunInterval = 3,
	.aeTarget = 128,

	.dampOver = 0x20,
	.dampUnder = 0x20,
	.tolerance = 1,

	.initExposure = 512000,
	.aeMode = AE_MODE_FIX_FRAME_RATE,
	.gainThreshold = 1024,
};

#ifdef __cplusplus
}
#endif
