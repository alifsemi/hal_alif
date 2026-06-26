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
	.fullLinesStd = 2000,
	.fullLines = 2000,
	.fps = 30 * ISP_SNS_FPS_ACCU,
	.maxIntLine = 1996,
	.minIntLine = 2,
	.intLineStep = 1,
	.maxAgain = 15360,
	.minAgain = 1024,
	.againStep = 1,
	.maxDgain = 1024,
	.minDgain = 1024,
	.dgainStep = 1,
	.aeRunInterval = 1,
	.aeTarget = 100,

	.dampOver = 0x10,
	.dampUnder = 0x10,
	.tolerance = 30,

	.initExposure = 4079616,
	.aeMode = AE_MODE_FIX_FRAME_RATE,
	.gainThreshold = 15360,
};

#ifdef __cplusplus
}
#endif
