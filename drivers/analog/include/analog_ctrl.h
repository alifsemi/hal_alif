/*
 * Copyright (C) 2025 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ANALOG_CTRL_H
#define ANALOG_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define REG(addr)	(*(volatile uint32_t *)(addr))

/* Used to scale the input reference to the DAC6 with a step size of 1/16.
 * Ex: DAC6_VREF_SCALE has a value of 0x5, then
 * Scaling factor = Register value/16 => 0x5/16 => 0.3125
 * which means input reference to the DAC6 is scaled by a factor of 5/16
 */
#define DAC6_VREF_SCALE		(0x1U << 27)

/* Output of DAC6 programmable reference voltage.
 * DAC6 Output = (DAC6_VREF_SCALE × DAC6_CONT) / 64
 */
#define DAC6_CONT			(0x20U << 21)

#define DAC6_EN				(0x1U << 20) /* Enable the DAC6 */

/* Used select different voltage reference values for a DAC12
 * 000: 0.500 (or 500 mv)
 * 001: 0.667 (or 667 mV)
 * 010: 0.667 (or 667 mV)
 * 011: 0.750 (or 750 mV)
 * 100: 0.750 (or 750 mV)
 * 101: 0.800 (or 800 mV)
 * 110: 0.800 (or 800 mV)
 * 111: 0.833 (or 833 mV)
 */
#define DAC12_VREF_CONT		(0x4U << 17)

/* 0x0: Vref = 1.8 V
 * 0x1: Vref = 1.5 V
 */
#define ADC_VREF_BUF_RDIV_EN	(0x0U << 16)

/* 0x0: ADC Vref is off
 * 0x1: ADC Vref is on
 */
#define ADC_VREF_BUF_EN		(0x1U << 15)

/* Control the ADC Vref: 1.8 V ± 100 mV, where:
 * 00000: -100 mV
 * 00001: -93 mV
 * 00010: -86 mV
 * ...
 * 11110: 93 mV
 * 11111: 100 mV
 * 10000: 1.8 V = Default (0x10)
 * Step size = 7 mV
 */
#define ADC_VREF_CONT		(0x10U << 10)

/* Analog peripheral LDO (LDO-5) output voltage (VDD_ANA):
 * 0000: 1.6 V
 * 0001: 1.62 V
 * ...
 * 1010: 1.8 V
 * 1111: 1.9 V
 * Step: 20 mV Default (0xA)
 */
#define ANA_PERIPH_LDO_CONT		(0xAU << 6)

/* Calibration for analog peripherals precision bandgap:
 * 0000: 1.140 V
 * ...
 * 1010: 1.2 V
 * 1111: 1.233 V
 * Step: 6 mV Default (0xA)
 */
#define ANA_PERIPH_BG_CONT		(0xAU << 1)

#define CMP_CTRL_CMP0_CLKEN	(1U << 0U) /* Enable CMP0 clock */

#define VBAT_ANA_REG2_VAL	0x00C00000 /* Enable analog peripheral LDO and precision bandgap */

#define DAC6_REF_VAL		(DAC6_VREF_SCALE | DAC6_CONT | DAC6_EN)

#define DAC12_REF_VAL		(DAC12_VREF_CONT | ADC_VREF_BUF_EN | ADC_VREF_BUF_RDIV_EN)

#define ADC_REF_VAL			(ADC_VREF_BUF_RDIV_EN | ADC_VREF_BUF_EN | ADC_VREF_CONT)

extern uint32_t analog_ldo_ref_cnt;
extern uint32_t adc_vref_cnt;
extern uint32_t dac6_vref_cnt;
extern uint32_t dac12_vref_cnt;
extern uint32_t cmp_clk_ref_cnt;

/**
 * @fn         void enable_analog_peripherals(uintptr_t vbat_reg2_addr)
 * @brief      Enable LDO and precision bandgap for analog peripherals
 * @param[in]  vbat_reg2_addr    Base address of Vbat register.
 * @return     none
 */
static inline void enable_analog_peripherals(uintptr_t vbat_reg2_addr)
{
	/* Analog configuration Vbat register2 */
	REG(vbat_reg2_addr) |= VBAT_ANA_REG2_VAL;
	analog_ldo_ref_cnt++;
}

/**
 * @fn          void disable_analog_peripherals(uintptr_t vbat_reg2_addr)
 * @brief       Disable LDO and precision bandgap for analog peripherals
 * @param[in]   vbat_reg2_addr   Base address of Vbat register.
 * @return      none
 */
static inline void disable_analog_peripherals(uintptr_t vbat_reg2_addr)
{
	analog_ldo_ref_cnt--;
	if (analog_ldo_ref_cnt == 0) {
		/* Analog configuration Vbat register2 */
		REG(vbat_reg2_addr) &= ~VBAT_ANA_REG2_VAL;
	}
}

/**
 * @fn      void enable_dac6_ref_voltage_alias_mode(uintptr_t adc_vref_base,
 *             uintptr_t dac6_reg)
 * @brief   Enable DAC6 as a negative input reference for HSCMP.
 *          This function sets the DAC6 configuration register to output
 *          a specific voltage (0.9V) that can be used as a negative
 *          input for the High-Speed Comparator (aliasing mode).
 * @param[in] adc_vref_base Base address of ADC VREF register.
 * @param[in] dac6_reg      Base address of DAC6 register.
 * @return   none
 */
static inline void enable_dac6_ref_voltage_alias_mode(uintptr_t adc_vref_base,
							uintptr_t dac6_reg)
{
	REG(adc_vref_base) |= ADC_VREF_BUF_EN;
	REG(dac6_reg) |= DAC6_REF_VAL;

	dac6_vref_cnt++;
}

/**
 * @fn      void enable_dac6_ref_voltage(uintptr_t cmp_reg2)
 * @brief   Enable DAC6 as a negative input reference for HSCMP.
 *          This function sets the DAC6 configuration register to output
 *          a specific voltage (0.9V) that can be used as a negative
 *          input for the High-Speed Comparator (non-aliasing mode).
 * @param[in] cmp_reg2 Base address of HSCMP register.
 * @return   none
 */
static inline void enable_dac6_ref_voltage(uintptr_t cmp_reg2)
{
	REG(cmp_reg2) |= (DAC6_REF_VAL | ADC_VREF_BUF_EN);

	dac6_vref_cnt++;
}

/**
 * @fn       void disable_dac6_ref_voltage_alias_mode(uintptr_t adc_vref_base,
 *                                                  uintptr_t dac6_reg)
 * @brief    Disable DAC6 as a negative input reference for HSCMP
 *           (aliasing mode).
 * @param[in] adc_vref_base  Base address of ADC VREF register.
 * @param[in] dac6_reg       Base address of DAC6 register.
 * @return   none
 */
static inline void disable_dac6_ref_voltage_alias_mode(uintptr_t adc_vref_base,
							uintptr_t dac6_reg)
{
	dac6_vref_cnt--;

	if (dac6_vref_cnt == 0) {
		REG(adc_vref_base) &= ~ADC_VREF_BUF_EN;
		REG(dac6_reg) &= ~DAC6_REF_VAL;
	}
}

/**
 * @fn       void disable_dac6_ref_voltage(uintptr_t cmp_reg2)
 * @brief    Disable DAC6 as a negative input reference for HSCMP
 *           (non-aliasing mode).
 * @param[in] cmp_reg2  Base address of HSCMP register.
 * @return   none
 */
static inline void disable_dac6_ref_voltage(uintptr_t cmp_reg2)
{
	dac6_vref_cnt--;

	if (dac6_vref_cnt == 0) {
		REG(cmp_reg2) &= ~(DAC6_REF_VAL | ADC_VREF_BUF_EN);
	}
}

/**
 * @fn       void enable_dac12_ref_voltage_alias_mode(uintptr_t cmp_reg2,
 *                                                 uintptr_t adc_vref_base)
 * @brief    Enable DAC12 voltage reference and internal buffer
 *           for DAC operation (aliasing mode).
 * @param[in] cmp_reg2       Base address of the comparator control register.
 * @param[in] adc_vref_base  Base address of the ADC VREF register.
 * @return   none
 */
static inline void enable_dac12_ref_voltage_alias_mode(uintptr_t cmp_reg2,
							uintptr_t adc_vref_base)
{
	REG(adc_vref_base) |= (ADC_VREF_BUF_EN | ADC_VREF_BUF_RDIV_EN);
	REG(cmp_reg2) |= DAC12_VREF_CONT;
	dac12_vref_cnt++;
}

/**
 * @fn       void enable_dac12_ref_voltage(uintptr_t cmp_reg2)
 * @brief    Enable DAC12 voltage reference and internal buffer
 *           for DAC operation (non-aliasing mode).
 * @param[in] cmp_reg2  Base address of the comparator control register.
 * @return   none
 */
static inline void enable_dac12_ref_voltage(uintptr_t cmp_reg2)
{
	REG(cmp_reg2) |= DAC12_REF_VAL;
	dac12_vref_cnt++;
}

/**
 * @fn       void disable_dac12_ref_voltage(uintptr_t cmp_reg2)
 * @brief    Disables DAC12 voltage reference and internal buffer
 *             for DAC operation (non-aliasing mode).
 * @param[in] cmp_reg2	   Base address of the comparator control register.
 * @return    none
 */
static inline void disable_dac12_ref_voltage(uintptr_t cmp_reg2)
{
	dac12_vref_cnt--;
	if (dac12_vref_cnt == 0) {
		REG(cmp_reg2) &= ~DAC12_REF_VAL;
	}
}

/**
 * @fn       void disable_dac12_ref_voltage_alias_mode(uintptr_t cmp_reg2,
 *                                                   uintptr_t adc_vref_base)
 * @brief    Disable DAC12 voltage reference and internal buffer
 *           (aliasing mode).
 * @param[in] cmp_reg2       Base address of the comparator control register.
 * @param[in] adc_vref_base  Base address of the ADC VREF register.
 * @return   none
 */
static inline void disable_dac12_ref_voltage_alias_mode(uintptr_t cmp_reg2, uintptr_t adc_vref_base)
{
	dac12_vref_cnt--;
	if (dac12_vref_cnt == 0) {
		REG(adc_vref_base) &= ~(ADC_VREF_BUF_EN | ADC_VREF_BUF_RDIV_EN);
		REG(cmp_reg2) &= ~DAC12_VREF_CONT;
	}
}

/**
 * @fn      void enable_adc_ref_voltage(uintptr_t cmp_reg2)
 * @brief   Enable ADC reference voltage (non-aliasing mode).
 *          Configures the comparator control register to enable the
 *          reference divider, turn on the VREF buffer, and set the
 *          appropriate control level for the ADC reference voltage.
 * @param[in] cmp_reg2  Base address of the comparator control register.
 * @return   none
 */
static inline void enable_adc_ref_voltage(uintptr_t cmp_reg2)
{
	REG(cmp_reg2) |= ADC_REF_VAL;
	adc_vref_cnt++;
}

/**
 * @fn      void enable_adc_ref_voltage_alias_mode(uintptr_t adc_vref_base)
 * @brief   Enable ADC reference voltage (aliasing mode).
 *          Configures the ADC VREF register to enable the
 *          reference divider, turn on the VREF buffer, and set the
 *          appropriate control level for the ADC reference voltage.
 * @param[in] adc_vref_base  Base address of the ADC VREF register.
 * @return   none
 */
static inline void enable_adc_ref_voltage_alias_mode(uintptr_t adc_vref_base)
{
	REG(adc_vref_base) |= ADC_REF_VAL;
	adc_vref_cnt++;
}

/**
 * @fn        void disable_adc_ref_voltage(uintptr_t cmp_reg2)
 * @brief     Disable the reference divider, VREF buffer,
 *            and ADC reference voltage (non-aliasing mode).
 * @param[in] cmp_reg2       Base address of the comparator control register.
 * @return     none
 */
static inline void disable_adc_ref_voltage(uintptr_t cmp_reg2)
{
	adc_vref_cnt--;
	if (adc_vref_cnt == 0) {
		REG(cmp_reg2) &= ~ADC_REF_VAL;
	}
}

/**
 * @fn        void disable_adc_ref_voltage_alias_mode(uintptr_t adc_vref_base)
 * @brief     Disable the reference divider, VREF buffer,
 *            and ADC reference voltage (aliasing mode).
 * @param[in] adc_vref_base  Base address of the ADC VREF register.
 * @return     none
 */
static inline void disable_adc_ref_voltage_alias_mode(uintptr_t adc_vref_base)
{
	adc_vref_cnt--;
	if (adc_vref_cnt == 0) {
		REG(adc_vref_base) &= ~ADC_REF_VAL;
	}
}

/**
 * @fn         static inline void enable_analog_periph_clk(uintptr_t cmp_clk_reg)
 * @brief      Enable Analog peripheral clock.
 * @param[in]  cmp_clk_reg  Register base address of comparator clock control
 * @return     none.
 */
static inline void enable_analog_periph_clk(uintptr_t cmp_clk_reg)
{
	REG(cmp_clk_reg) |= CMP_CTRL_CMP0_CLKEN;
	cmp_clk_ref_cnt++;
}

/**
 * @fn        static inline void disable_analog_periph_clk(uintptr_t cmp_clk_reg)
 * @brief     Disable Analog peripheral clock.
 * @param[in] cmp_clk_reg  Register base address of comparator clock control
 * @return    none.
 */
static inline void disable_analog_periph_clk(uintptr_t cmp_clk_reg)
{
	cmp_clk_ref_cnt--;
	if (cmp_clk_ref_cnt == 0) {
		REG(cmp_clk_reg) &= ~CMP_CTRL_CMP0_CLKEN;
	}
}

#ifdef __cplusplus
}
#endif

#endif /* ANALOG_CTRL_H */
