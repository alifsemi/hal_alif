/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2024 Alif Semiconductor.
 */

#ifndef UTIMER_H_
#define UTIMER_H_

#define UTIMER_GLB_CNTR_START(global_addr)	(global_addr + 0x00U)
#define UTIMER_GLB_CNTR_STOP(global_addr)	(global_addr + 0x04U)
#define UTIMER_GLB_CNTR_CLEAR(global_addr)	(global_addr + 0x08U)
#define UTIMER_GLB_CNTR_RUNNING(global_addr)	(global_addr + 0x0CU)
#define UTIMER_GLB_DRIVER_OEN(global_addr)	(global_addr + 0x10U)
#define UTIMER_GLB_CLOCK_ENABLE(global_addr)	(global_addr + 0x20U)

#define UTIMER_START_0_SRC(timer_addr)		(timer_addr + 0x0000U)
#define UTIMER_START_1_SRC(timer_addr)		(timer_addr + 0x0004U)
#define UTIMER_STOP_0_SRC(timer_addr)		(timer_addr + 0x0008U)
#define UTIMER_STOP_1_SRC(timer_addr)		(timer_addr + 0x000CU)
#define UTIMER_CLEAR_0_SRC(timer_addr)		(timer_addr + 0x0010U)
#define UTIMER_CLEAR_1_SRC(timer_addr)		(timer_addr + 0x0014U)
#define UTIMER_UP_0_SRC(timer_addr)		(timer_addr + 0x0018U)
#define UTIMER_UP_1_SRC(timer_addr)		(timer_addr + 0x001CU)
#define UTIMER_DOWN_0_SRC(timer_addr)		(timer_addr + 0x0020U)
#define UTIMER_DOWN_1_SRC(timer_addr)		(timer_addr + 0x0024U)
#define UTIMER_TRIG_CAPTURE_SRC_A_0(timer_addr)	(timer_addr + 0x0028U)
#define UTIMER_TRIG_CAPTURE_SRC_A_1(timer_addr)	(timer_addr + 0x002CU)
#define UTIMER_TRIG_CAPTURE_SRC_B_0(timer_addr)	(timer_addr + 0x0030U)
#define UTIMER_TRIG_CAPTURE_SRC_B_1(timer_addr)	(timer_addr + 0x0034U)
#define UTIMER_DMA_CLEAR_SRC_A_0(timer_addr)	(timer_addr + 0x0038U)
#define UTIMER_DMA_CLEAR_SRC_A_1(timer_addr)	(timer_addr + 0x003CU)
#define UTIMER_DMA_CLEAR_SRC_B_0(timer_addr)	(timer_addr + 0x0040U)
#define UTIMER_DMA_CLEAR_SRC_B_1(timer_addr)	(timer_addr + 0x0044U)
#define UTIMER_CNTR_PAUSE_SRC(timer_addr)	(timer_addr + 0x0048U)
#define UTIMER_CNTR_CTRL(timer_addr)		(timer_addr + 0x0080U)
#define UTIMER_FILTER_CTRL_A(timer_addr)	(timer_addr + 0x0084U)
#define UTIMER_FILTER_CTRL_B(timer_addr)	(timer_addr + 0x0088U)
#define UTIMER_COMPARE_CTRL_A(timer_addr)	(timer_addr + 0x008CU)
#define UTIMER_COMPARE_CTRL_B(timer_addr)	(timer_addr + 0x0090U)
#define UTIMER_BUF_OP_CTRL(timer_addr)		(timer_addr + 0x0094U)
#define UTIMER_CNTR(timer_addr)			(timer_addr + 0x00A0U)
#define UTIMER_CNTR_PTR(timer_addr)		(timer_addr + 0x00A4U)
#define UTIMER_CNTR_PTR_BUF1(timer_addr)	(timer_addr + 0x00A8U)
#define UTIMER_CNTR_PTR_BUF2(timer_addr)	(timer_addr + 0x00ACU)
#define UTIMER_CAPTURE_A(timer_addr)		(timer_addr + 0x00B0U)
#define UTIMER_CAPTURE_A_BUF1(timer_addr)	(timer_addr + 0x00B4U)
#define UTIMER_CAPTURE_A_BUF2(timer_addr)	(timer_addr + 0x00B8U)
#define UTIMER_CAPTURE_B(timer_addr)		(timer_addr + 0x00C0U)
#define UTIMER_CAPTURE_B_BUF1(timer_addr)	(timer_addr + 0x00C4U)
#define UTIMER_CAPTURE_B_BUF2(timer_addr)	(timer_addr + 0x00C8U)
#define UTIMER_COMPARE_A(timer_addr)		(timer_addr + 0x00D0U)
#define UTIMER_COMPARE_A_BUF1(timer_addr)	(timer_addr + 0x00D4U)
#define UTIMER_COMPARE_A_BUF2(timer_addr)	(timer_addr + 0x00D8U)
#define UTIMER_COMPARE_B(timer_addr)		(timer_addr + 0x00E0U)
#define UTIMER_COMPARE_B_BUF1(timer_addr)	(timer_addr + 0x00E4U)
#define UTIMER_COMPARE_B_BUF2(timer_addr)	(timer_addr + 0x00E8U)
#define UTIMER_DT_UP(timer_addr)		(timer_addr + 0x00F0U)
#define UTIMER_DT_UP_BUF1(timer_addr)		(timer_addr + 0x00F4U)
#define UTIMER_DT_DOWN(timer_addr)		(timer_addr + 0x00F8U)
#define UTIMER_DT_DOWN_BUF1(timer_addr)		(timer_addr + 0x00FCU)
#define UTIMER_CHAN_STATUS(timer_addr)		(timer_addr + 0x0114U)
#define UTIMER_CHAN_INTERRUPT(timer_addr)	(timer_addr + 0x0118U)
#define UTIMER_CHAN_INTERRUPT_MASK(timer_addr)	(timer_addr + 0x011CU)
#define UTIMER_DUTY_CYCLE_CTRL(timer_addr)	(timer_addr + 0x0120U)
#define UTIMER_DEAD_TIME_CTRL(timer_addr)	(timer_addr + 0x0124U)
#define UTIMER_INT_CNTR_CTRL(timer_addr)	(timer_addr + 0x0130U)
#define UTIMER_FAULT_CTRL(timer_addr)		(timer_addr + 0x0134U)

/* Bit definition for TIMER_RegInfo:cntr_start_src_1 register */
#define CNTR_SRC1_PGM_EN_BIT			31U
#define CNTR_SRC1_PGM_EN			(1 << CNTR_SRC1_PGM_EN_BIT)

/* Bit definition for TIMER_RegInfo:cntr_ctrl register */
#define CNTR_CTRL_EN_BIT			0U
#define CNTR_CTRL_EN				(1 << CNTR_CTRL_EN_BIT)
#define CNTR_CTRL_RUNNING_BIT		1U
#define CNTR_CTRL_RUNNING			(1 << CNTR_CTRL_RUNNING_BIT)
#define CNTR_CTRL_SAWTOOTH_BIT		2U
#define CNTR_CTRL_SAWTOOTH			(0 << CNTR_CTRL_SAWTOOTH_BIT)
#define CNTR_CTRL_SAWTOOTH_ONE_SHOT	(1 << CNTR_CTRL_SAWTOOTH_BIT)
#define CNTR_CTRL_TRIANGLE_BUF_TROUGH_BIT	4U
#define CNTR_CTRL_TRIANGLE_BUF_TROUGH		\
				(1 << CNTR_CTRL_TRIANGLE_BUF_TROUGH_BIT)
#define CNTR_CTRL_TRIANGLE_BUF_TROUGH_CREST	((1 << 2) | (1 << 4))
#define CNTR_CTRL_TRIANGLE_ONE_SHOT		((1 << 3) | (1 << 4))
#define CNTR_CTRL_DIR_DOWN_BIT			8U
#define CNTR_CTRL_DIR_DOWN			(1 << CNTR_CTRL_DIR_DOWN_BIT)

/* Bit definition for TIMER_RegInfo:compare_ctrl register */
#define COMPARE_CTRL_DRV_RETAIN_VAL_AT_COMP_MATCH	(~1 << 0)
#define COMPARE_CTRL_DRV_LOW_AT_COMP_MATCH		(1 << 0)
#define COMPARE_CTRL_DRV_HIGH_AT_COMP_MATCH		(1 << 1)
#define COMPARE_CTRL_DRV_TOGGLE_AT_COMP_MATCH		((1 << 0) | (1 << 1))
#define COMPARE_CTRL_DRV_COMP_MATCH_Msk			((1 << 0) | (1 << 1))
#define COMPARE_CTRL_DRV_RETAIN_VAL_AT_CYCLE_END	(0 << 2)
#define COMPARE_CTRL_DRV_LOW_AT_CYCLE_END		(1 << 2)
#define COMPARE_CTRL_DRV_HIGH_AT_CYCLE_END		(1 << 3)
#define COMPARE_CTRL_DRV_TOGGLE_AT_CYCLE_END		((1 << 2) | (1 << 3))
#define COMPARE_CTRL_DRV_CYCLE_END_Msk			((1 << 2) | (1 << 3))
#define COMPARE_CTRL_DRV_START_VAL_HIGH_BIT		4U
#define COMPARE_CTRL_DRV_START_VAL_LOW			 \
				(0 << COMPARE_CTRL_DRV_START_VAL_HIGH_BIT)
#define COMPARE_CTRL_DRV_START_VAL_HIGH			 \
				(1 << COMPARE_CTRL_DRV_START_VAL_HIGH_BIT)
#define COMPARE_CTRL_DRV_STOP_VAL_HIGH_BIT		6U
#define COMPARE_CTRL_DRV_STOP_VAL_HIGH			\
				(1 << COMPARE_CTRL_DRV_STOP_VAL_HIGH_BIT)
#define COMPARE_CTRL_DRV_START_STOP_LEVEL_BIT		7U
#define COMPARE_CTRL_DRV_START_STOP_LEVEL		\
				(1 << COMPARE_CTRL_DRV_START_STOP_LEVEL_BIT)
#define COMPARE_CTRL_DRV_DRIVER_EN_BIT			8U
#define COMPARE_CTRL_DRV_DRIVER_EN			\
				(1 << COMPARE_CTRL_DRV_DRIVER_EN_BIT)
#define COMPARE_CTRL_DRV_DISABLE_VAL_HIGH_BIT		9U
#define COMPARE_CTRL_DRV_DISABLE_VAL_HIGH		\
				(1 << COMPARE_CTRL_DRV_DISABLE_VAL_HIGH_BIT)
#define COMPARE_CTRL_DRV_COMPARE_EN_BIT			11U
#define COMPARE_CTRL_DRV_COMPARE_EN			\
				(1 << COMPARE_CTRL_DRV_COMPARE_EN_BIT)
#define COMPARE_CTRL_DRV_COMPARE_TRIG_EN_BIT		12U
#define COMPARE_CTRL_DRV_COMPARE_TRIG_EN		\
				(1 << COMPARE_CTRL_DRV_COMPARE_TRIG_EN_BIT)
#define COMPARE_CTRL_DRV_DMA_CLEAR_EN_BIT		13U
#define COMPARE_CTRL_DRV_DMA_CLEAR_EN			\
				(1 << COMPARE_CTRL_DRV_DMA_CLEAR_EN_BIT)

/* Bit definition for TIMER_RegInfo:chan_interrupt register */
#define CHAN_INTERRUPT_CAPTURE_A_BIT			0U
#define CHAN_INTERRUPT_CAPTURE_A	\
				(1 << CHAN_INTERRUPT_CAPTURE_A_BIT)
#define CHAN_INTERRUPT_CAPTURE_B_BIT			1U
#define CHAN_INTERRUPT_CAPTURE_B	\
				(1 << CHAN_INTERRUPT_CAPTURE_B_BIT)
#define CHAN_INTERRUPT_COMPARE_A_BUF1_BIT		2U
#define CHAN_INTERRUPT_COMPARE_A_BUF1	\
				(1 << CHAN_INTERRUPT_COMPARE_A_BUF1_BIT)
#define CHAN_INTERRUPT_COMPARE_A_BUF2_BIT		3U
#define CHAN_INTERRUPT_COMPARE_A_BUF2	\
				(1 << CHAN_INTERRUPT_COMPARE_A_BUF2_BIT)
#define CHAN_INTERRUPT_COMPARE_B_BUF1_BIT		4U
#define CHAN_INTERRUPT_COMPARE_B_BUF1	\
				(1 << CHAN_INTERRUPT_COMPARE_B_BUF1_BIT)
#define CHAN_INTERRUPT_COMPARE_B_BUF2_BIT		5U
#define CHAN_INTERRUPT_COMPARE_B_BUF2	\
				(1 << CHAN_INTERRUPT_COMPARE_B_BUF2_BIT)
#define CHAN_INTERRUPT_UNDER_FLOW_BIT			6U
#define CHAN_INTERRUPT_UNDER_FLOW	\
				(1 << CHAN_INTERRUPT_UNDER_FLOW_BIT)
#define CHAN_INTERRUPT_OVER_FLOW_BIT			7U
#define CHAN_INTERRUPT_OVER_FLOW	\
				(1 << CHAN_INTERRUPT_OVER_FLOW_BIT)

/* Bit definition for TIMER_RegInfo:chan_status register */
#define CHAN_STATUS_INTERRUPT_MASK				0xC3U

/* Bit definition for TIMER_RegInfo:glb_cntr_start register */
#define GLB_CNTR_START				((uint32_t)0x0000FFFF)

/* Bit definition for TIMER_RegInfo:glb_cntr_stop register */
#define GLB_CNTR_STOP				((uint32_t)0x0000FFFF)

/* Bit definition for TIMER_RegInfo:glb_cntr_clear register */
#define GLB_CNTR_CLEAR				((uint32_t)0x000007FF)

/* Bit definition for TIMER_RegInfo:glb_cntr_running register */
#define GLB_CNTR_RUNNING			((uint32_t)0x0000FFFF)

/* Bit definition for TIMER_RegInfo:glb_driver_oen register */
#define GLB_DRIVER_CHAN_A_OEN_BIT	0U
#define GLB_DRIVER_CHAN_A_OEN		(1 << GLB_DRIVER_CHAN_A_OEN_BIT)
#define GLB_DRIVER_CHAN_B_OEN_BIT	1U
#define GLB_DRIVER_CHAN_B_OEN		(1 << GLB_DRIVER_CHAN_B_OEN_BIT)
#define GLB_DRIVER_CHAN_OEN		((1 << 0) | 1 << 1)

/* Bit definition for TIMER_RegInfo:glb_clk_en register */
#define GLB_CLK_EN				((uint32_t)0x0000FFFF)

/**
 * \fn        void alif_utimer_enable_timer_clock(uint32_t reg_base,
 *                             uint8_t timer_id)
 * \brief     enable clock for specified timer.
 * \param[in] reg_base  register base address
 * \param[in] timer_id  timer instance
 * \return    none
 */
void alif_utimer_enable_timer_clock(uint32_t reg_base, uint8_t timer_id);

/**
 * \fn        void alif_utimer_disable_timer_clock(uint32_t reg_base,
 *                             uint8_t timer_id)
 * \brief     disable clock for specified timer.
 * \param[in] reg_base  register base address
 * \param[in] timer_id  timer instance
 * \return    none
 */
void alif_utimer_disable_timer_clock(uint32_t reg_base, uint8_t timer_id);

/**
 * \fn        void alif_utimer_enable_counter(uint32_t reg_base)
 * \brief     enable timer counter.
 * \param[in] reg_base  register base address
 * \return    none
 */
void alif_utimer_enable_counter(uint32_t reg_base);

/**
 * \fn        void alif_utimer_disable_counter(uint32_t reg_base)
 * \brief     disable timer counter.
 * \param[in] reg_base  register base address
 * \return    none
 */
void alif_utimer_disable_counter(uint32_t reg_base);

/**
 * \fn        void alif_utimer_set_up_counter(uint32_t reg_base)
 * \brief     set counter counter as up counter.
 * \param[in] reg_base  register base address
 * \return    none
 */
void alif_utimer_set_up_counter(uint32_t reg_base);

/**
 * \fn        void alif_utimer_set_down_counter(uint32_t reg_base)
 * \brief     set counter counter as down counter.
 * \param[in] reg_base  register base address
 * \return    none
 */
void alif_utimer_set_down_counter(uint32_t reg_base);

/**
 * \fn        void alif_utimer_set_triangular_counter(uint32_t reg_base)
 * \brief     set counter counter as triangular counter.
 * \param[in] reg_base  register base address
 * \return    none
 */
void alif_utimer_set_triangular_counter(uint32_t reg_base);

/**
 * \fn        void alif_utimer_enable_soft_counter_ctrl(uint32_t reg_base)
 * \brief     enable counter program control.
 * \param[in] reg_base  register base address
 * \return    none
 */
void alif_utimer_enable_soft_counter_ctrl(uint32_t reg_base);

/**
 * \fn        void alif_utimer_disable_soft_counter_ctrl(uint32_t reg_base)
 * \brief     disable counter program control.
 * \param[in] reg_base  register base address
 * \return    none
 */
void alif_utimer_disable_soft_counter_ctrl(uint32_t reg_base);

/**
 * \fn        uint32_t alif_utimer_get_counter_reload_value(uint32_t reg_base)
 * \brief     read counter pointer register value.
 * \param[in] reg_base  register base address
 * \return    counter pointer register value
 */
uint32_t alif_utimer_get_counter_reload_value(uint32_t reg_base);

/**
 * \fn        void alif_utimer_set_counter_reload_value(uint32_t reg_base,
 *                             uint32_t value)
 * \brief     set counter pointer value.
 * \param[in] reg_base  register base address
 * \param[in] value  counter pointer register value to be set
 * \return    none
 */
void alif_utimer_set_counter_reload_value(uint32_t reg_base, uint32_t value);

/**
 * \fn        uint32_t alif_utimer_get_counter_value(uint32_t reg_base)
 * \brief     read current counter value.
 * \param[in] reg_base  register base address
 * \return    counter value
 */
uint32_t alif_utimer_get_counter_value(uint32_t reg_base);

/**
 * \fn        void alif_utimer_set_counter_value(uint32_t reg_base,
 *                             uint32_t value)
 * \brief     set counter value.
 * \param[in] reg_base  register base address
 * \param[in] value  counter value to be set
 * \return    none
 */
void alif_utimer_set_counter_value(uint32_t reg_base, uint32_t value);

/**
 * \fn        void alif_utimer_clear_interrupt(uint32_t reg_base,
 *                       uint8_t evt_bit)
 * \brief     clear specified interrupt.
 * \param[in] reg_base  register base address
 * \param[in] evt_bit  interrupt bit
 * \return    none
 */
void alif_utimer_clear_interrupt(uint32_t reg_base, uint8_t evt_bit);

/**
 * \fn        void alif_utimer_enable_interrupt(uint32_t reg_base,
 *                       uint8_t evt_bit)
 * \brief     enable interrupt.
 * \param[in] reg_base  register base address
 * \param[in] evt_bit  interrupt bit
 * \return    none
 */
void alif_utimer_enable_interrupt(uint32_t reg_base, uint8_t evt_bit);

/**
 * \fn        void alif_utimer_disable_interrupt(uint32_t reg_base,
 *                       uint8_t evt_bit)
 * \brief     disable interrupt.
 * \param[in] reg_base  register base address
 * \param[in] evt_bit  interrupt bit
 * \return    none
 */
void alif_utimer_disable_interrupt(uint32_t reg_base, uint8_t evt_bit);

/**
 * \fn        bool alif_utimer_check_interrupt_enabled(uint32_t reg_base,
 *                       uint8_t evt_bit)
 * \brief     check interrupt status.
 * \param[in] reg_base  register base address
 * \param[in] evt_bit  interrupt bit
 * \return    result
 */
bool alif_utimer_check_interrupt_enabled(uint32_t reg_base, uint8_t evt_bit);

/**
 * \fn        bool alif_utimer_counter_running(uint32_t reg_base,
 *                             uint8_t timer_id)
 * \brief     check timer counter is running.
 * \param[in] reg_base  register base address
 * \param[in] timer_id  timer instance
 * \return    result
 */
bool alif_utimer_counter_running(uint32_t reg_base, uint8_t timer_id);

/**
 * \fn        void alif_utimer_start_counter(uint32_t reg_base,
 *                             uint8_t timer_id)
 * \brief     start timer counter.
 * \param[in] reg_base  register base address
 * \param[in] timer_id  timer instance
 * \return    none
 */
void alif_utimer_start_counter(uint32_t reg_base, uint8_t timer_id);

/**
 * \fn        void alif_utimer_stop_counter(uint32_t reg_base,
 *                             uint8_t timer_id)
 * \brief     stop timer counter.
 * \param[in] reg_base  register base address
 * \param[in] timer_id  timer instance
 * \return    none
 */
void alif_utimer_stop_counter(uint32_t reg_base, uint8_t timer_id);

/**
 * \fn        uint32_t alif_utimer_get_pending_interrupt(uint32_t reg_base)
 * \brief     stop timer counter.
 * \param[in] reg_base  register base address
 * \return    enabled interrupts which are pending
 */
uint32_t alif_utimer_get_pending_interrupt(uint32_t reg_base);

/**
 * \fn        void alif_utimer_enable_compare_match(uint32_t reg_base,
 *                             uint8_t driver)
 * \brief     enable compare match for the driver.
 * \param[in] reg_base  register base address
 * \param[in] driver  driver type
 * \return    none
 */
void alif_utimer_enable_compare_match(uint32_t reg_base, uint8_t driver);

/**
 * \fn        void alif_utimer_disable_compare_match(uint32_t reg_base,
 *                             uint8_t driver)
 * \brief     disable compare match for the driver.
 * \param[in] reg_base  register base address
 * \param[in] driver  driver type
 * \return    none
 */
void alif_utimer_disable_compare_match(uint32_t reg_base, uint8_t driver);

/**
 * \fn        void alif_utimer_set_compare_value(uint32_t reg_base,
 *                   uint8_t driver, uint32_t compare_value)
 * \brief     stop timer counter.
 * \param[in] reg_base  register base address
 * \param[in] driver  driver type
 * \param[in] compare_value value for compare register
 * \return    none
 */
void alif_utimer_set_compare_value(uint32_t reg_base,
		uint8_t driver, uint32_t compare_value);

/**
 * \fn     void alif_utimer_disable_driver(uint32_t reg_base, uint8_t driver)
 * \brief     disable output channel.
 * \param[in] reg_base  register base address
 * \param[in] driver  driver type
 * \return    none
 */
void alif_utimer_disable_driver(uint32_t reg_base, uint8_t driver);

/**
 * \fn     void alif_utimer_enable_driver(uint32_t reg_base, uint8_t driver)
 * \brief     enable output channel.
 * \param[in] reg_base  register base address
 * \param[in] driver  driver type
 * \return    none
 */
void alif_utimer_enable_driver(uint32_t reg_base, uint8_t driver);

/**
 * \fn     void alif_utimer_config_driver_output(uint32_t reg_base,
 *            uint8_t driver, uint32_t value)
 * \brief     configure output driver.
 * \param[in] reg_base  register base address
 * \param[in] driver  driver type
 * \param[in] value   driver output value
 * \return    none
 */
void alif_utimer_config_driver_output(uint32_t reg_base, uint8_t driver,
				uint32_t value);

/**
 * \fn        bool alif_utimer_driver_enabled(uint32_t reg_base,
 *                             uint8_t driver)
 * \brief     enable output driver.
 * \param[in] reg_base  register base address
 * \param[in] driver  driver type
 * \return    none
 */
bool alif_utimer_driver_enabled(uint32_t reg_base, uint8_t driver);

/**
 * \fn        bool alif_utimer_comp_match_enabled(uint32_t reg_base,
 *                             uint8_t driver)
 * \brief     enable driver compare match feature for.
 * \param[in] reg_base  register base address
 * \param[in] driver  driver type
 * \return    none
 */
bool alif_utimer_comp_match_enabled(uint32_t reg_base, uint8_t driver);

/**
 * \fn        void alif_utimer_disable_driver_output(uint32_t reg_base,
 *                                 uint8_t driver, uint8_t timer_id)
 * \brief     disable timer driver output.
 * \param[in] reg_base  register base address
 * \param[in] driver    driver type
 * \param[in] timer_id  timer instance
 * \return    none
 */
void alif_utimer_disable_driver_output(uint32_t reg_base, uint8_t driver,
					uint8_t timer_id);

/**
 * \fn        void alif_utimer_enable_driver_output(uint32_t reg_base,
 *                                 uint8_t driver, uint8_t timer_id)
 * \brief     enable timer driver output.
 * \param[in] reg_base  register base address
 * \param[in] driver    driver type
 * \param[in] timer_id  timer instance
 * \return    none
 */
void alif_utimer_enable_driver_output(uint32_t reg_base, uint8_t driver,
					uint8_t timer_id);

/**
 * \fn        void alif_utimer_disable_timer_output(uint32_t reg_base,
 *                                      uint8_t timer_id)
 * \brief     disable timer output.
 * \param[in] reg_base  register base address
 * \param[in] timer_id  timer instance
 * \return    none
 */
void alif_utimer_disable_timer_output(uint32_t reg_base, uint8_t timer_id);

/**
 * \fn        void alif_utimer_set_driver_disable_val_high(uint32_t reg_base,
 *                                      uint8_t driver)
 * \brief     set output as high during driver disable state.
 * \param[in] reg_base  register base address
 * \param[in] driver    driver type
 * \return    none
 */
void alif_utimer_set_driver_disable_val_high(uint32_t reg_base,
			uint8_t driver);

/**
 * \fn        void alif_utimer_set_driver_disable_val_low(uint32_t reg_base,
 *                                      uint8_t driver)
 * \brief     set output as low during driver disable state.
 * \param[in] reg_base  register base address
 * \param[in] driver    driver type
 * \return    none
 */
void alif_utimer_set_driver_disable_val_low(uint32_t reg_base,
			uint8_t driver);


#endif /* UTIMER_H_ */
