/*
 * Copyright (C) 2026 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "vsi_comm_calib.h"

#ifdef __cplusplus
extern "C"{
#endif

ISP_CALIB_DATA_S isp_calib_param = {
	.head = {
		.name = "OV5675_1296_972_LINEAR",
		.date = "2024_10_14",
		.version = "ISPNANO_V2401",
	},
	.modules = {
		.autoRoute = {
			.autoRoute = {100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200,
					102400, 204800, 409600, 819200, 1638400, 3276800},
		},

#if defined(CONFIG_ISP_LIB_BLS_MODULE)
		.bls = {
			.enable = 1,
			.opType = OP_TYPE_MANUAL,
			.manualAttr = {
				.blackLevel = {256, 256, 256, 256},
			},
			.autoAttr = {
				.blackLevel = {
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
					{256, 256, 256, 256},
				},
			},
		},
#endif /* defined(CONFIG_ISP_LIB_BLS_MODULE) */

#if defined(CONFIG_ISP_LIB_EXPOSUREM_MODULE)
		.aem = {
			.enable = 1,
			.expAltMode = 0,
			.blockWin = {
				.hOffs = 0,
				.vOffs = 0,
				.hSize = 1296,
				.vSize = 972,
			},
		},
#endif /* defined(CONFIG_ISP_LIB_EXPOSUREM_MODULE) */

#if defined(CONFIG_ISP_LIB_AE_MODULE)
		.ae = {
			.opType = OP_TYPE_AUTO,
			.manualAttr = {
				.intTime = 33000,
				.again = 8192,
				.dgain = 1024,
			},
			.autoAttr = {
				.expTimeRange = {
					.min = 100,
					.max = 16000,
				},
				.againRange = {
					.min = 1024,
					.max = 15360,
				},
				.dgainRange = {
					.min = 1024,
					.max = 1024,
				},
				.aeRunInterval = 2,
				.aeTarget = 50,
				.dampOver = 30,
				.dampUnder = 30,
				.tolerance = 12,
				.antiflicker = {
					.enable = 0,
					.flickerFreq = 100,
				},
				.aeMode = AE_MODE_FIX_FRAME_RATE,
				.gainThreshold = 15360,
				.aeRoute = {
					.totalNum = 0,
				},
				.aeDelayAttr = {
					.blackDelayFrame = 0,
					.whiteDelayFrame = 0,
				},
				.weight = {
					{
						1, 1, 1, 1, 1
					},
					{
						1, 1, 1, 1, 1
					},
					{
						1, 1, 1, 1, 1
					},
					{
						1, 1, 1, 1, 1
					},
					{
						1, 1, 1, 1, 1
					},
				},
			},
		},
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

#if defined(CONFIG_ISP_LIB_WBM_MODULE)
		.wbm = {
			.enable = 1,
			.measMode = ISP_AWB_MEAS_MODE_RGB,
			.measRect = {
				.hOffs = 0,
				.vOffs = 0,
				.hSize = 1296,
				.vSize = 972,
			},
			.wpRange = {
				.maxY = 0xEB,
				.refCr_MaxR = 0x80,
				.minY_MaxG = 0xC0,
				.refCb_MaxB = 0x80,
				.maxCSum = 0x14,
				.minC = 0x14,
			},
		},
#endif /* defined(CONFIG_ISP_LIB_WBM_MODULE) */

#if defined(CONFIG_ISP_LIB_WB_MODULE)
		.wb = {
			.enable = 1,
			.opType = OP_TYPE_AUTO,
			.manualAttr = {
				.wbGain = {0x100, 0x100, 0x100, 0x100},
			},
			.autoAttr = {
				.runInterval = 12,
				.speed = 64,
				.tolerance = 1,
				.initColorTemp = 5000,
				.calibParam = {
					.centLine = {-766376, -642393, -2921800},
					.rgMin = 1086700,
					.rgMax = 3050800,
					.wpRange0 = {
						.wpLCurve = {
							.rg = { 790659, 953496, 1087350, 1277770,
								1427190, 1521530, 1594220, 1791900,
								1965800, 2113520, 2264770, 2450610,
								2600660, 2750720, 2818650,
								3050800 },
							.dist = { 560682, 506246, 480332, 408497,
								256789, 269600, 317996, 416285,
								245149, 80289, -42413, -71649,
								-4396, 111498, 229011, 453350 },
						},
						.wpRCurve = {
							.rg = { 796080, 971478, 1096380, 1295500,
								1423700, 1550280, 1700330, 1850390,
								2000440, 2150500, 2300550, 2450610,
								2630520, 2750720, 2900770,
								3050800 },
							.dist = { -48178, 37442, 108593, 173398,
								161051, 170806, 219416, 259086,
								288821, 304528, 301610, 271649,
								266912, 88502, -67514, -253350 },
						},
					},
					.wpRange1 = {
						.wpLCurve = {
							.rg = { 791651, 1289310, 1508700, 1631550,
								1762950, 1861430, 1976370, 2091310,
								2206250, 2321190, 2490840, 2629990,
								2687900, 2780950, 2895890,
								3010800 },
							.dist = { -178231, 46924, 124591, 108020,
								168323, 171752, 194948, 210209,
								216012, 209423, 225067, 235329,
								111501, -30197, -152124, -292180 },
						},
						.wpRCurve = {
							.rg = { 797383, 1275640, 1482030, 1601860,
								1792600, 1869890, 1959780, 2076980,
								2281190, 2332430, 2431000, 2574250,
								2654400, 2765620, 2839520,
								3010800 },
							.dist = { 449646, 289261, 93411, 142743,
								266621, 286629, 206521, 53599,
								-134047, -144737, -124515, -148586,
								-49479, 73539, 227056, 312180 },
						},
					},
					.illuminant[ILLUMINANT_A] = {
						.illuType = ILLUMINANT_A,
						.colorTemp = 2800,
						.wbGain = {0x124, 0x100, 0x100, 0x205},
					},
					.illuminant[ILLUMINANT_TL84] = {
						.illuType = ILLUMINANT_TL84,
						.colorTemp = 3800,
						.wbGain = {0x15B, 0x100, 0x100, 0x1B5},
					},
					.illuminant[ILLUMINANT_CWF] = {
						.illuType = ILLUMINANT_CWF,
						.colorTemp = 4100,
						.wbGain = {0x178, 0x100, 0x100, 0x1C5},
					},
					.illuminant[ILLUMINANT_D50] = {
						.illuType = ILLUMINANT_D50,
						.colorTemp = 5000,
						.wbGain = {0x182, 0x100, 0x100, 0x181},
					},
					.illuminant[ILLUMINANT_D65] = {
						.illuType = ILLUMINANT_D65,
						.colorTemp = 6500,
						.wbGain = {0x1D4, 0x100, 0x100, 0x140},
					},
				},
			},
		},
#endif /* defined(CONFIG_ISP_LIB_WB_MODULE) */

#if defined(CONFIG_ISP_LIB_DMSC_MODULE)
		.dmsc = {
			.enable = 1,
			.threshold = 10,
			.cacAttr = {
				.enable = 0,
				.hClipMode = 0,
				.vClipMode = 0,
				.hStart = 0,
				.vStart = 0,
				.aBlue = 0,
				.aRed = 0,
				.bBlue = 0,
				.bRed = 0,
				.cBlue = 0,
				.cRed = 0,
				.xNormShift = 7,
				.xNormFactor = 29,
				.yNormShift = 7,
				.yNormFactor = 29,
			},
		},
#endif /* defined(CONFIG_ISP_LIB_DMSC_MODULE) */

#if defined(CONFIG_ISP_LIB_FLT_MODULE)
		.flt = {
			.enable = 1,
			.opType = OP_TYPE_MANUAL,
			.manualAttr = {
				.denoiseLevel = 6,
				.sharpenLevel = 2,
			},
			.autoAttr = {
				.denoiseLevel = {
					0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0
				},
				.sharpenLevel = {
					0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0
				},
			},
		},
#endif /* defined(CONFIG_ISP_LIB_FLT_MODULE) */

#if defined(CONFIG_ISP_LIB_CCM_MODULE)
		.ccm = {
			.opType = OP_TYPE_AUTO,
			.manualAttr = {
				.colorMatrix = {
					128, 0, 0, 0, 128, 0, 0, 0, 128
				},
				.rOffset = 0,
				.gOffset = 0,
				.bOffset = 0,
			},
			.autoAttr = {
				.illuminantCCM[ILLUMINANT_A] = {
					.colorTemp = 2800,
					.colorMatrix = {
						249, -38, -83, -76, 287, -83, -41, -170, 339
					},
					.rOffset = 0,
					.gOffset = 0,
					.bOffset = 0,
				},
				.illuminantCCM[ILLUMINANT_TL84] = {
					.colorTemp = 3800,
					.colorMatrix = {
						292, -85, -79, -71, 273, -74, -17, -125, 269
					},
					.rOffset = 0,
					.gOffset = 0,
					.bOffset = 0,
				},
				.illuminantCCM[ILLUMINANT_CWF] = {
					.colorTemp = 4100,
					.colorMatrix = {
						356, -163, -65, -79, 267, -60, -20, -116, 265
					},
					.rOffset = 0,
					.gOffset = 0,
					.bOffset = 0,
				},
				.illuminantCCM[ILLUMINANT_D50] = {
					.colorTemp = 5000,
					.colorMatrix = {
						277, -86, -64, -55, 274, -91, -8, -137, 273
					},
					.rOffset = 0,
					.gOffset = 0,
					.bOffset = 0,
				},
				.illuminantCCM[ILLUMINANT_D65] = {
					.colorTemp = 6500,
					.colorMatrix = {
						295, -122, -45, -52, 268, -88, -7, -109, 245
					},
					.rOffset = 0,
					.gOffset = 0,
					.bOffset = 0,
				},
			},
		},
#endif /* defined(CONFIG_ISP_LIB_CCM_MODULE) */

#if defined(CONFIG_ISP_LIB_GAMMAOUT_MODULE)
		.gammaOut = {
			.enable = 1,
			.gammaY = {
				0, 142, 230, 305, 373, 436, 495, 557, 629,
				697, 761, 821, 876, 925, 968, 1003, 1023
			},
		},
#endif /* defined(CONFIG_ISP_LIB_GAMMAOUT_MODULE) */

#if defined(CONFIG_ISP_LIB_CSM_MODULE)
		.csm = {
			.type = ISP_CSM_TYPE_601,
			.quantization = ISP_CSM_LIM_RANGE,
		},
#endif /* defined(CONFIG_ISP_LIB_CSM_MODULE) */

	},
};

#ifdef __cplusplus
}
#endif
