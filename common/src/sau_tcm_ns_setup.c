/*
 * Copyright (c) 2025 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "soc.h"
#include "sau_tcm_ns_setup.h"
#include "tgu.h"

#if !(defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))

typedef struct {
	__IOM uint32_t CTRL;       /*!< Offset: 0x000 (R/W)  SAU Control Register */

	__IM  uint32_t TYPE;       /*!< Offset: 0x004 (R/ )  SAU Type Register */

	__IOM uint32_t RNR;        /*!< Offset: 0x008 (R/W)  SAU Region Number Register */
	__IOM uint32_t RBAR;       /*!< Offset: 0x00C (R/W)  SAU Region Base Address Register */
	__IOM uint32_t RLAR;       /*!< Offset: 0x010 (R/W)  SAU Region Limit Address Register */
	__IOM uint32_t SFSR;       /*!< Offset: 0x014 (R/W)  Secure Fault Status Register */
	__IOM uint32_t SFAR;       /*!< Offset: 0x018 (R/W)  Secure Fault Address Register */
} SAU_Type;

#define SAU_CTRL_ENABLE_Pos   0U                                   /*!< SAU CTRL: ENABLE Position */
#define SAU_CTRL_ENABLE_Msk   (1UL /*<< SAU_CTRL_ENABLE_Pos*/)     /*!< SAU CTRL: ENABLE Mask */

/* SAU Region Base Address Register Definitions */
#define SAU_RBAR_BADDR_Pos    5U                                   /*!< SAU RBAR: BADDR Position */
#define SAU_RBAR_BADDR_Msk    (0x7FFFFFFUL << SAU_RBAR_BADDR_Pos)  /*!< SAU RBAR: BADDR Mask */

/* SAU Region Limit Address Register Definitions */
#define SAU_RLAR_LADDR_Pos    5U                                   /*!< SAU RLAR: LADDR Position */
#define SAU_RLAR_LADDR_Msk    (0x7FFFFFFUL << SAU_RLAR_LADDR_Pos)  /*!< SAU RLAR: LADDR Mask */

#define SAU_RLAR_ENABLE_Pos   0U                                   /*!< SAU RLAR: ENABLE Position */
#define SAU_RLAR_ENABLE_Msk   (1UL /*<< SAU_RLAR_ENABLE_Pos*/)     /*!< SAU RLAR: ENABLE Mask */

#define SAU_BASE              (SCS_BASE +  0x0DD0UL)               /*!< Security Attribution Unit */
#define SAU                   ((SAU_Type       *)     SAU_BASE)    /*!< Security Attribution Unit */

__STATIC_INLINE void SAU_TCM_NS_Setup(void)
{
	SAU->RNR = 0;
	SAU->RBAR = (uint32_t)NS_REGION_0_BASE & SAU_RBAR_BADDR_Msk;
	SAU->RLAR = (((uint32_t)NS_REGION_0_END - 1) & SAU_RLAR_LADDR_Msk) | SAU_RLAR_ENABLE_Msk;

	SAU->CTRL = ((1U << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk);
}

void sau_tcm_ns_setup(void)
{
	/* SAU and TGU Setup */
	SAU_TCM_NS_Setup();
	TGU_Setup();
}
#endif
