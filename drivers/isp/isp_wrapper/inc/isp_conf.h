/*
 * Copyright (C) 2025 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ISP_ISP_WRAPPER_INC_ISP_CONF_H_
#define _ISP_ISP_WRAPPER_INC_ISP_CONF_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ISP_DEV_CNT 1
#define ISP_PORT_CNT 1
#define ISP_CHN_CNT 1

#ifdef ISP_ARY_CROP_V10
#undef ISP_ARY_CROP_V10
#endif

#ifdef ISP_ACQ_NR_NUMBERS
#undef ISP_ACQ_NR_NUMBERS
#endif

#define ISP_PICO              1
#define ISPPICO_V2101         1
#define ISP_IRQ_HANDLER_ALIF  1

#if defined(CONFIG_ISP_LIB_SCALAR_MODULE)
#define ISP_SCALE_V10 1
#define ISP_MP_SCALE 1
#endif /* defined(CONFIG_ISP_LIB_SCALAR_MODULE) */

#if defined(CONFIG_ISP_LIB_AE_MODULE)
#define ISP_AE_V10 1
#define ISP_AE_ALGO_V10 1
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE */

#if defined(CONFIG_ISP_LIB_WB_MODULE)
#define ISP_WB_V10_1 1
#define ISP_AWB_ALGO_V10 1
#endif /* defined(CONFIG_ISP_LIB_WB_MODULE) */

#if defined(CONFIG_ISP_LIB_BINNING_MODULE)
#define ISP_BINNING_V10 1
#endif /* defined(CONFIG_ISP_LIB_BINNING_MODULE) */

#if defined(CONFIG_ISP_LIB_BLS_MODULE)
#define ISP_BLS_V10 1
#endif /* defined(CONFIG_ISP_LIB_BLS_MODULE) */

#if defined(CONFIG_ISP_LIB_DMSC_MODULE)
#define ISP_DMSC_V10 1
#endif /* defined(CONFIG_ISP_LIB_DMSC_MODULE) */

#if defined(CONFIG_ISP_LIB_FLT_MODULE)
#define ISP_FLT_V10 1
#endif /* defined(CONFIG_ISP_LIB_FLT_MODULE) */

#if defined(CONFIG_ISP_LIB_CCM_MODULE)
#define ISP_CCM_V10 1
#endif /* defined(CONFIG_ISP_LIB_CCM_MODULE) */

#if defined(CONFIG_ISP_LIB_GAMMAOUT_MODULE)
#define ISP_GAMMA_OUT_V10 1
#endif /* defined(CONFIG_ISP_LIB_GAMMAOUT_MODULE) */

#if defined(CONFIG_ISP_LIB_EXPOSUREM_MODULE)
#define ISP_EXPM_V10 1
#endif /* defined(CONFIG_ISP_LIB_EXPOSUREM_MODULE) */

#if defined(CONFIG_ISP_LIB_CSM_MODULE)
#define ISP_CSM_V10 1
#endif /* defined(CONFIG_ISP_LIB_CSM_MODULE) */

#if defined(ISP_LIB_WBM_MODULE)
#define ISP_WBM_V10 1
#endif /* defined(ISP_LIB_WBM_MODULE) */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _ISP_ISP_WRAPPER_INC_ISP_CONF_H_ */
