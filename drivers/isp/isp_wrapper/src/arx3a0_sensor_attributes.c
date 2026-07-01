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
	.fullLinesStd = 0x04A6,
	.fullLines = 0x04A6,
	.fps = 5 * ISP_SNS_FPS_ACCU,
	.maxIntLine = 0x0441,
	.minIntLine = 0x1,
	.intLineStep = 1,
	.maxAgain = 10 * 1024,
	.minAgain = 1 * 1024,
	.againStep = 1,
	.maxDgain = 8 * 1024,
	.minDgain = 1 * 1024,
	.dgainStep = 1,
	.aeRunInterval = 1,
	.aeTarget = 100,

	.dampOver = 0x40,
	.dampUnder = 0x40,
	.tolerance = 10,

	.initExposure = 11709888,
	.aeMode = AE_MODE_FIX_FRAME_RATE,
	.gainThreshold = 12000,
};

#ifdef __cplusplus
}
#endif
