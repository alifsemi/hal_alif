/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*******************************************************************************
 * @file     sau_tcm_ns_setup.h
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     6-August-2024
 * @brief    Set TCM Nonsecure Partitioning in SAU and TGU
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#ifndef SAU_TCM_NS_SETUP_H
#define SAU_TCM_NS_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

#define DTCM_BASE DT_REG_ADDR(DT_NODELABEL(dtcm))
#define DTCM_END (DTCM_BASE + DT_REG_SIZE(DT_NODELABEL(dtcm)))

#define ITCM_BASE DT_REG_ADDR(DT_NODELABEL(itcm))
#define ITCM_END (DTCM_BASE + DT_REG_SIZE(DT_NODELABEL(itcm)))

/* NS REGIONs, should be TGU Block size aligned */
#if DT_NODE_EXISTS(DT_NODELABEL(ns))
#define NS_REGION_0_BASE DT_REG_ADDR(DT_NODELABEL(ns))
#define NS_REGION_0_SIZE DT_REG_SIZE(DT_NODELABEL(ns))
#define NS_REGION_0_END  (NS_REGION_0_BASE + NS_REGION_0_SIZE)
#else
#error "Non-secure DTS node not found."
#endif /* DT_NODE_EXISTS(DT_NODELABEL(ns)) */


/*
 * sau_tcm_ns_setup ()
 * Set up the TCM Nonsecure partitioning in SAU and TGU
 */
void sau_tcm_ns_setup(void);

#ifdef __cplusplus
}
#endif

#endif /* SAU_TCM_NS_SETUP_H */
