/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2024 Alif Semiconductor.
 */

#include <stdint.h>
#include <stdbool.h>
#include <utimer.h>

#define REG(addr)    (*(volatile uint32_t *)(uint32_t)(addr))

void alif_utimer_enable_timer_clock(uint32_t reg_base, uint8_t timer_id)
{
	REG(UTIMER_GLB_CLOCK_ENABLE(reg_base)) |= (1 << timer_id);
}

void alif_utimer_disable_timer_clock(uint32_t reg_base, uint8_t timer_id)
{
	REG(UTIMER_GLB_CLOCK_ENABLE(reg_base)) &= ~(1 << timer_id);
}

void alif_utimer_enable_counter(uint32_t reg_base)
{
	REG(UTIMER_CNTR_CTRL(reg_base)) |= CNTR_CTRL_EN;
}

void alif_utimer_disable_counter(uint32_t reg_base)
{
	REG(UTIMER_CNTR_CTRL(reg_base)) &= ~CNTR_CTRL_EN;
}

void alif_utimer_set_up_counter(uint32_t reg_base)
{
	REG(UTIMER_CNTR_CTRL(reg_base)) &= ~CNTR_CTRL_DIR_DOWN;
}

void alif_utimer_set_down_counter(uint32_t reg_base)
{
	REG(UTIMER_CNTR_CTRL(reg_base)) |= CNTR_CTRL_DIR_DOWN;
}

void alif_utimer_set_triangular_counter(uint32_t reg_base)
{
	REG(UTIMER_CNTR_CTRL(reg_base)) |= CNTR_CTRL_TRIANGLE_BUF_TROUGH;
}

void alif_utimer_disable_driver(uint32_t reg_base, uint8_t driver)
{
	if (driver) {
		REG(UTIMER_COMPARE_CTRL_B(reg_base)) &= ~COMPARE_CTRL_DRV_DRIVER_EN;
	} else {
		REG(UTIMER_COMPARE_CTRL_A(reg_base)) &= ~COMPARE_CTRL_DRV_DRIVER_EN;
	}
}

void alif_utimer_enable_driver(uint32_t reg_base, uint8_t driver)
{
	if (driver) {
		REG(UTIMER_COMPARE_CTRL_B(reg_base)) |= COMPARE_CTRL_DRV_DRIVER_EN;
	} else {
		REG(UTIMER_COMPARE_CTRL_A(reg_base)) |= COMPARE_CTRL_DRV_DRIVER_EN;
	}
}

void alif_utimer_disable_driver_output(uint32_t reg_base, uint8_t driver,
								uint8_t timer_id)
{
	REG(UTIMER_GLB_DRIVER_OEN(reg_base)) |= ((timer_id * 2) + driver);
}

void alif_utimer_enable_driver_output(uint32_t reg_base, uint8_t driver,
								uint8_t timer_id)
{
	REG(UTIMER_GLB_DRIVER_OEN(reg_base)) &= ~(1 << ((timer_id * 2) + driver));
}

void alif_utimer_disable_timer_output(uint32_t reg_base, uint8_t timer_id)
{
	REG(UTIMER_GLB_DRIVER_OEN(reg_base)) |= (0x3 << (timer_id * 2));
}

void alif_utimer_enable_soft_counter_ctrl(uint32_t reg_base)
{
	REG(UTIMER_START_1_SRC(reg_base)) |= CNTR_SRC1_PGM_EN;
	REG(UTIMER_STOP_1_SRC(reg_base)) |= CNTR_SRC1_PGM_EN;
	REG(UTIMER_CLEAR_1_SRC(reg_base)) |= CNTR_SRC1_PGM_EN;
}

void alif_utimer_disable_soft_counter_ctrl(uint32_t reg_base)
{
	REG(UTIMER_START_1_SRC(reg_base)) &= ~CNTR_SRC1_PGM_EN;
	REG(UTIMER_STOP_1_SRC(reg_base)) &= ~CNTR_SRC1_PGM_EN;
	REG(UTIMER_CLEAR_1_SRC(reg_base)) &= ~CNTR_SRC1_PGM_EN;
}

void alif_utimer_set_counter_reload_value(uint32_t reg_base, uint32_t value)
{
	REG(UTIMER_CNTR_PTR(reg_base)) = value;
}

uint32_t alif_utimer_get_counter_reload_value(uint32_t reg_base)
{
	return REG(UTIMER_CNTR_PTR(reg_base));
}

void alif_utimer_set_counter_value(uint32_t reg_base, uint32_t value)
{
	REG(UTIMER_CNTR(reg_base)) = value;
}

uint32_t alif_utimer_get_counter_value(uint32_t reg_base)
{
	return REG(UTIMER_CNTR(reg_base));
}

void alif_utimer_clear_interrupt(uint32_t reg_base, uint8_t evt_bit)
{
	/* set interrupt bit to clear pending interrupt */
	REG(UTIMER_CHAN_INTERRUPT(reg_base)) |= (1 << evt_bit);
}

void alif_utimer_enable_interrupt(uint32_t reg_base, uint8_t evt_bit)
{
	REG(UTIMER_CHAN_INTERRUPT_MASK(reg_base)) &= ~(1 << evt_bit);
}

void alif_utimer_disable_interrupt(uint32_t reg_base, uint8_t evt_bit)
{
	REG(UTIMER_CHAN_INTERRUPT_MASK(reg_base)) |= (1 << evt_bit);
}

bool alif_utimer_check_interrupt_enabled(uint32_t reg_base, uint8_t evt_bit)
{
	bool ret;
	(!(REG(UTIMER_CHAN_INTERRUPT_MASK(reg_base)) & (1 << evt_bit))) ?
			(ret = true) : (ret = false);
	return ret;
}

bool alif_utimer_counter_running(uint32_t reg_base, uint8_t timer_id)
{
	bool ret;
	(REG(UTIMER_GLB_CNTR_RUNNING(reg_base)) & (1 << timer_id)) ?
			(ret = true) : (ret = false);
	return ret;
}

void alif_utimer_start_counter(uint32_t reg_base, uint8_t timer_id)
{
	REG(UTIMER_GLB_CNTR_START(reg_base)) |= (1 << timer_id);
}

void alif_utimer_stop_counter(uint32_t reg_base, uint8_t timer_id)
{
	REG(UTIMER_GLB_CNTR_STOP(reg_base)) |= (1 << timer_id);
}

uint32_t alif_utimer_get_pending_interrupt(uint32_t reg_base)
{
	uint32_t value;

	value = REG(UTIMER_CHAN_INTERRUPT(reg_base));
	return value;
}

void alif_utimer_enable_compare_match(uint32_t reg_base, uint8_t driver)
{
	if (driver) {
		REG(UTIMER_COMPARE_CTRL_B(reg_base)) |= COMPARE_CTRL_DRV_COMPARE_EN;
	} else {
		REG(UTIMER_COMPARE_CTRL_A(reg_base)) |= COMPARE_CTRL_DRV_COMPARE_EN;
	}
}

void alif_utimer_disable_compare_match(uint32_t reg_base, uint8_t driver)
{
	if (driver) {
		REG(UTIMER_COMPARE_CTRL_B(reg_base)) &= ~COMPARE_CTRL_DRV_COMPARE_EN;
	} else {
		REG(UTIMER_COMPARE_CTRL_A(reg_base)) &= ~COMPARE_CTRL_DRV_COMPARE_EN;
	}
}

bool alif_utimer_driver_enabled(uint32_t reg_base, uint8_t driver)
{
	bool ret;

	if (driver) {
		(REG(UTIMER_COMPARE_CTRL_B(reg_base)) & COMPARE_CTRL_DRV_DRIVER_EN) ?
			(ret = true) : (ret = false);
	} else {
		(REG(UTIMER_COMPARE_CTRL_A(reg_base)) & COMPARE_CTRL_DRV_DRIVER_EN) ?
			(ret = true) : (ret = false);
	}

	return ret;
}

bool alif_utimer_comp_match_enabled(uint32_t reg_base, uint8_t driver)
{
	bool ret;

	if (driver) {
		(REG(UTIMER_COMPARE_CTRL_B(reg_base)) & COMPARE_CTRL_DRV_COMPARE_EN) ?
			(ret = true) : (ret = false);
	} else {
		(REG(UTIMER_COMPARE_CTRL_A(reg_base)) & COMPARE_CTRL_DRV_COMPARE_EN) ?
			(ret = true) : (ret = false);
	}

	return ret;
}

void alif_utimer_set_compare_value(uint32_t reg_base, uint8_t driver,
				uint32_t compare_value)
{
	if (driver) {
		REG(UTIMER_COMPARE_B(reg_base)) = compare_value;
	} else {
		REG(UTIMER_COMPARE_A(reg_base)) = compare_value;
	}
}

void alif_utimer_config_driver_output(uint32_t reg_base, uint8_t driver,
				uint32_t value)
{
	uint32_t reg;

	if (driver) {
		reg = REG(UTIMER_COMPARE_CTRL_B(reg_base));
		reg &= ~(COMPARE_CTRL_DRV_COMP_MATCH_Msk | COMPARE_CTRL_DRV_CYCLE_END_Msk);
		reg |= value;
		REG(UTIMER_COMPARE_CTRL_B(reg_base)) = reg;
	} else {
		reg = REG(UTIMER_COMPARE_CTRL_A(reg_base));
		reg &= ~(COMPARE_CTRL_DRV_COMP_MATCH_Msk | COMPARE_CTRL_DRV_CYCLE_END_Msk);
		reg |= value;
		REG(UTIMER_COMPARE_CTRL_A(reg_base)) = reg;
	}
}

void alif_utimer_set_driver_disable_val_high(uint32_t reg_base, uint8_t driver)
{
	if (driver) {
		REG(UTIMER_COMPARE_CTRL_B(reg_base)) |= COMPARE_CTRL_DRV_DISABLE_VAL_HIGH;
	} else {
		REG(UTIMER_COMPARE_CTRL_A(reg_base)) |= COMPARE_CTRL_DRV_DISABLE_VAL_HIGH;
	}
}

void alif_utimer_set_driver_disable_val_low(uint32_t reg_base, uint8_t driver)
{
	if (driver) {
		REG(UTIMER_COMPARE_CTRL_B(reg_base)) &= ~COMPARE_CTRL_DRV_DISABLE_VAL_HIGH;
	} else {
		REG(UTIMER_COMPARE_CTRL_A(reg_base)) &= ~COMPARE_CTRL_DRV_DISABLE_VAL_HIGH;
	}
}

