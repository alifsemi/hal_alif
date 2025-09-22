/*
 * Copyright (c) 2025 Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc.h"
#include "sau_tcm_ns_setup.h"
#include "tgu.h"

/*----------------------------------------------------------------------------
 * WICCONTROL register
 *----------------------------------------------------------------------------
 * WIC bit positions in WICCONTROL
 *
 */
#define WICCONTROL_WIC_Pos          (8U)
#define WICCONTROL_WIC_Msk          (1U << WICCONTROL_WIC_Pos)

#if defined(RTSS_HP)
#define WICCONTROL                  (AON->RTSS_HP_CTRL)
#elif defined(RTSS_HE)
#define WICCONTROL                  (AON->RTSS_HE_CTRL)
#endif

/*----------------------------------------------------------------------------
 *Default Handler for Spurious wakeup
 *----------------------------------------------------------------------------
 */
	__attribute__ ((weak))
void System_HandleSpuriousWakeup(void)
{
	/*
	 * pm.c has the implementation to handle the spurious wakeup.
	 * User may override and can have their own implementation.
	 */
}


/*----------------------------------------------------------------------------
 *System initialization function
 *----------------------------------------------------------------------------
 */
void SystemInit(void)
{
	__set_FAULTMASK(0);

	/* Disable the MPU */
	ARM_MPU_Disable();

	/* Enable UsageFault, BusFault, MemFault and SecurityFault exceptions */
	/* Otherwise all you see is HardFault, even in the debugger */
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
		SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_SECUREFAULTENA_Msk;

#if (defined(__FPU_USED) && (__FPU_USED == 1U)) || \
	(defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
	SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
			(3U << 11U*2U));         /* enable CP11 Full Access */
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
	SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

	/*
	 * Enable caches now, for speed, but we will have to clean
	 * after scatter-loading, in _platform_pre_stackheap_init

	 * We do not use the CMSIS functions, as these manually invalidate the
	 * cache - this is not required on the M55, as it is auto-invalidated
	 * (and we implicitly rely on this already before activating, if booting
	 * from MRAM).
	 * Enable Loop and branch info cache
	 */
	SCB->CCR |= SCB_CCR_IC_Msk | SCB_CCR_DC_Msk | SCB_CCR_LOB_Msk;

	/* Enable limited static branch prediction using low overhead loops */
	ICB->ACTLR &= ~ICB_ACTLR_DISLOBR_Msk;

	__DSB();
	__ISB();

#if DT_NODE_HAS_STATUS(DT_NODELABEL(ns), okay)
	sau_tcm_ns_setup();
#endif

}
