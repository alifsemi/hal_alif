/*
 * Copyright (C) 2024 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ospi.h"

#define TX_INTR_MASK  (SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK		   \
			| SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK	   \
			| SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK)  \

#define RX_INTR_MASK  (SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK           \
			| SPI_IMR_RX_FIFO_OVER_FLOW_INTERRUPT_MASK         \
			| SPI_IMR_RX_FIFO_FULL_INTERRUPT_MASK		   \
			| SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK)  \

/* Helper : update CTRL0 Reg with Frame Format and Transer mode bits */
static uint32_t update_ctrl0_frf_tmode(uint32_t reg_val,
					uint32_t frf,
					uint32_t t_mode)
{
	reg_val &= ~(SPI_CTRLR0_SPI_FRF_MASK
			| (SPI_CTRLR0_TMOD_MASK | SPI_CTRLR0_SSTE_MASK));
	reg_val |= ((frf << SPI_CTRLR0_SPI_FRF) | t_mode);

	return reg_val;
}

/* Helper : value to set spi_ctrl0 */
static uint32_t set_spi_ctrlr0_reg(struct ospi_transfer *transfer,
				uint8_t inst_len)
{
	uint32_t val;

	val = SPI_TRANS_TYPE_FRF_DEFINED
		| (SPI_CTRLR0_SPI_RXDS_ENABLE << SPI_CTRLR0_SPI_RXDS_EN_OFFSET)
		| (transfer->ddr << SPI_CTRLR0_SPI_DDR_EN_OFFSET)
		| (inst_len << SPI_CTRLR0_INST_L_OFFSET)
		| (transfer->addr_len << SPI_CTRLR0_ADDR_L_OFFSET)
		| (transfer->dummy_cycle << SPI_CTRLR0_WAIT_CYCLES_OFFSET);

	return val;
}

/* Helper : value to ospi_ctrl0 for XiP */
static uint32_t set_ospi_ctrl0_in_xip(uint8_t frf,
			uint8_t t_mode, uint8_t dfs)
{
	uint32_t val;

	/* Set OSPI CTRL0 */
	val = OSPI_CTRLR0_IS_MST
		|(frf << OSPI_CTRLR0_SPI_FRF_OFFSET)
		|(0 << OSPI_CTRLR0_SCPOL_OFFSET)
		|(0 << OSPI_CTRLR0_SCPH_OFFSET)
		|(0 << OSPI_CTRLR0_SSTE_OFFSET)
		|(t_mode << OSPI_CTRLR0_TMOD_OFFSET)
		|(dfs << OSPI_CTRLR0_DFS_OFFSET);

	return val;
}

/* Helper : value to ospi_xip_ctrl0 for XiP */
static uint32_t set_xip_ctrl(uint8_t wait_cycles, uint8_t rxds_vl_en)
{
	uint32_t val;

	val = (OCTAL << XIP_CTRL_FRF_OFFSET)
	| (0x2 << XIP_CTRL_TRANS_TYPE_OFFSET)
	| (XIP_CTRL_ADDR_LEN_36_BIT << XIP_CTRL_ADDR_L_OFFSET)
	| (XIP_CTRL_INST_LEN_8_BIT << XIP_CTRL_INST_L_OFFSET)
	| (0x0 << XIP_CTRL_MD_BITS_EN_OFFSET)
	| (wait_cycles << XIP_CTRL_WAIT_CYCLES_OFFSET)
	| (0x1 << XIP_CTRL_DFS_HC_OFFSET)
	| (0x1 << XIP_CTRL_DDR_EN_OFFSET)
	| (0x0 << XIP_CTRL_INST_DDR_EN_OFFSET)
	| (0x1 << XIP_CTRL_RXDS_EN_OFFSET)
	| (0x1 << XIP_CTRL_INST_EN_OFFSET)
	| (0x0 << XIP_CTRL_CONT_XFER_EN_OFFSET)
	| (0x0 << XIP_CTRL_XIP_HYPERBUS_EN_OFFSET)
	| (0x0 << XIP_CTRL_RXDS_SIG_EN_OFFSET)
	| (0x0 << XIP_CTRL_XIP_MBL_OFFSET)
	| (0x0 << XIP_CTRL_XIP_PREFETCH_EN_OFFSET)
	| (rxds_vl_en << XIP_CTRL_RXDS_VL_EN_OFFSET);

	return val;
}

/**
 * \fn          void ospi_set_mode(struct ospi_regs *ospi, enum spi_mode mode)
 * \brief       Set the OSPI mode for the OSPI instance.
 * \param[in]   ospi     Pointer to the OSPI register map
 * \param[in]   mode    The mode to be set.
 * \return      none
 */
void ospi_set_mode(struct ospi_regs *ospi, enum spi_mode mode)
{
	uint32_t val;

	ospi_disable(ospi);

	val = ospi->OSPI_CTRLR0;
	val &= ~(SPI_CTRLR0_SCPOL_HIGH | SPI_CTRLR0_SCPH_HIGH);

	switch (mode) {
	/* Clock Polarity 0, Clock Phase 0 */
	case SPI_MODE_0:
		break;

	/* Clock Polarity 0, Clock Phase 1 */
	case SPI_MODE_1:
		val |= (SPI_CTRLR0_SCPOL_LOW | SPI_CTRLR0_SCPH_HIGH);
		break;

	/* Clock Polarity 1, Clock Phase 0 */
	case SPI_MODE_2:
		val |= (SPI_CTRLR0_SCPOL_HIGH | SPI_CTRLR0_SCPH_LOW);
		break;

	/* Clock Polarity 1, Clock Phase 1 */
	case SPI_MODE_3:
		val |= (SPI_CTRLR0_SCPOL_HIGH | SPI_CTRLR0_SCPH_HIGH);
		break;
	}

	ospi->OSPI_CTRLR0 = val;
	ospi_enable(ospi);
}

/**
 * \fn          void ospi_set_dfs(struct ospi_regs *ospi, uint8_t dfs)
 * \brief       Set the data frame size for the OSPI instance.
 * \param[in]   ospi     Pointer to the OSPI register map
 * \param[in]   dfs     The data frame size
 * \return      none
 */
void ospi_set_dfs(struct ospi_regs *ospi, uint8_t dfs)
{
	uint32_t val = 0;

	ospi_disable(ospi);

	val = ospi->OSPI_CTRLR0;
	val &= ~SPI_CTRLR0_DFS_MASK;
	val |= (dfs - 1);
	ospi->OSPI_CTRLR0 = val;

	ospi_enable(ospi);
}

/**
 * \fn          void ospi_set_tmod(struct ospi_regs *spi, enum spi_tmode tmod)
 * \brief       Set the transfer mode for the OSPI instance.
 * \param[in]   ospi    Pointer to the OSPI register map
 * \param[in]   tmod    Transfer mode
 * \return      none
 */
void ospi_set_tmod(struct ospi_regs *ospi, enum spi_tmode tmod)
{
	uint32_t val = 0;

	ospi_disable(ospi);

	val = ospi->OSPI_CTRLR0;
	val &= ~(SPI_CTRLR0_TMOD_MASK);

	switch (tmod) {
	case SPI_TMOD_TX_AND_RX:
	val |= SPI_CTRLR0_TMOD_TRANSFER;
	break;
	case SPI_TMOD_TX:
	val |= SPI_CTRLR0_TMOD_SEND_ONLY;
	break;
	case SPI_TMOD_RX:
	val |= SPI_CTRLR0_TMOD_RECEIVE_ONLY;
	break;
	case SPI_TMOD_EEPROM_READ:
	val |= SPI_CTRLR0_TMOD_EEPROM_READ_ONLY;
	break;
	default:
	break;
	}
	ospi->OSPI_CTRLR0 = val;

	ospi_enable(ospi);
}

/**
 * \fn          void ospi_set_tx_threshold(struct ospi_regs *ospi,
 *                                         uint8_t threshold)
 * \brief       Set Transmit FIFO interrupt threshold for the OSPI instance
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   threshold  Transmit FIFO threshold
 * \return      none
 */
void ospi_set_tx_threshold(struct ospi_regs *ospi, uint8_t threshold)
{
	uint32_t val = ospi->OSPI_TXFTLR;

	val &= ~(SPI_TXFTLR_TFT_MASK);
	val |= threshold << SPI_TXFTLR_TFT_SHIFT;
	ospi->OSPI_TXFTLR = val;
}

/**
 * \fn          void ospi_set_rx_sample_delay(struct ospi_regs *ospi,
 *                                            uint8_t rx_sample_delay)
 * \brief       Set Receive sample delay for the OSPI instance
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   threshold  Receive FIFO threshold
 * \return      none
 */
void ospi_set_rx_sample_delay(struct ospi_regs *ospi, uint8_t rx_sample_delay)
{
	ospi_disable(ospi);
	ospi->OSPI_RX_SAMPLE_DELAY = rx_sample_delay;
	ospi_enable(ospi);
}

/**
 * \fn          void ospi_set_ddr_drive_edge(struct ospi_regs *ospi,
 *                                           uint8_t ddr_drive_edge)
 * \brief       Set DDR drive edge for the OSPI instance
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   threshold  Receive FIFO threshold
 * \return      none
 */
void ospi_set_ddr_drive_edge(struct ospi_regs *ospi, uint8_t ddr_drive_edge)
{
	ospi_disable(ospi);
	ospi->OSPI_DDR_DRIVE_EDGE = ddr_drive_edge;
	ospi_enable(ospi);
}

/**
 * \fn          void ospi_control_ss(struct ospi_regs *ospi,
 *                     uint8_t slave, enum spi_ss_state state)
 * \brief       Control the slave select line
 * \param[in]   spi    Pointer to the OSPI register map
 * \param[in]   slave  The slave to be selected
 * \param[in]   state  The state of the slave select line
 * \return      none
 */
void ospi_control_ss(struct ospi_regs *ospi,
			uint8_t slave, enum spi_ss_state state)
{
	ospi_disable(ospi);

	if (state == SPI_SS_STATE_ENABLE) {
		ospi->OSPI_SER |= 1 << slave;
	} else {
		ospi->OSPI_SER &= ~(1 << slave);
	}
	ospi_enable(ospi);
}

/**
 * \fn          void ospi_control_xip_ss(struct ospi_regs *ospi,
 *                                      uint8_t slave, enum spi_ss_state state)
 * \brief       Control the XIP slave select line
 * \param[in]   ospi   Pointer to the OSPI register map
 * \param[in]   slave  The slave to be selected
 * \param[in]   state  The state of the slave select line
 * \return      none
 */
void ospi_control_xip_ss(struct ospi_regs *ospi,
			uint8_t slave, enum spi_ss_state state)
{
	ospi_disable(ospi);

	if (state == SPI_SS_STATE_ENABLE) {
		ospi->OSPI_XIP_SER |= 1 << slave;
	} else {
		ospi->OSPI_XIP_SER &= ~(1 << slave);
	}
	ospi_enable(ospi);
}

/**
 * \fn          void ospi_send(struct ospi_regs *spi,
 *                             struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for transmission
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_send(struct ospi_regs *ospi, struct ospi_transfer *transfer)
{
	ospi_disable(ospi);

	ospi->OSPI_CTRLR0 = update_ctrl0_frf_tmode(ospi->OSPI_CTRLR0,
					transfer->spi_frf,
					SPI_CTRLR0_TMOD_SEND_ONLY);

	ospi->OSPI_CTRLR1 = 0;

	ospi->OSPI_SPI_CTRLR0 = set_spi_ctrlr0_reg(transfer,
						SPI_CTRLR0_INST_L_8bit);

	//Unmask Tx interrupts.
	ospi->OSPI_IMR = TX_INTR_MASK;

	ospi_enable(ospi);
}

/**
 * \fn          void ospi_receive(struct ospi_regs *ospi,
 *                                struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for reception
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_receive(struct ospi_regs *ospi, struct ospi_transfer *transfer)
{
	ospi_disable(ospi);

	ospi->OSPI_CTRLR0 = update_ctrl0_frf_tmode(ospi->OSPI_CTRLR0,
					transfer->spi_frf,
					SPI_CTRLR0_TMOD_RECEIVE_ONLY);

	ospi->OSPI_CTRLR1 = transfer->rx_total_cnt - 1;

	ospi->OSPI_SPI_CTRLR0 = set_spi_ctrlr0_reg(transfer,
						SPI_CTRLR0_INST_L_0bit);

	//Unmask Tx and Rx interrupts.
	ospi->OSPI_IMR = TX_INTR_MASK | RX_INTR_MASK;

	ospi_enable(ospi);
}

/**
 * \fn          void ospi_transfer(struct ospi_regs *spi,
 *                                 struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for transfer
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_transfer(struct ospi_regs *ospi, struct ospi_transfer *transfer)
{
	ospi_disable(ospi);

	ospi->OSPI_CTRLR0 = update_ctrl0_frf_tmode(ospi->OSPI_CTRLR0,
					transfer->spi_frf,
					SPI_CTRLR0_TMOD_RECEIVE_ONLY);

	ospi->OSPI_CTRLR1 = transfer->rx_total_cnt - 1;

	ospi->OSPI_SPI_CTRLR0 = set_spi_ctrlr0_reg(transfer,
						SPI_CTRLR0_INST_L_8bit);

	ospi->OSPI_IMR = TX_INTR_MASK | RX_INTR_MASK;

	ospi_enable(ospi);
}

/**
 * \fn          void ospi_dma_send(struct ospi_regs *spi,
 *                                  struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for transmission with DMA support
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_dma_send(struct ospi_regs *ospi, struct ospi_transfer *transfer)
{
	ospi_disable(ospi);

	ospi->OSPI_CTRLR0 = update_ctrl0_frf_tmode(ospi->OSPI_CTRLR0,
					transfer->spi_frf,
					SPI_CTRLR0_TMOD_SEND_ONLY);

	ospi->OSPI_SPI_CTRLR0 = set_spi_ctrlr0_reg(transfer,
						SPI_CTRLR0_INST_L_8bit);

	ospi->OSPI_IMR = SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK;

	ospi->OSPI_TXFTLR &= ~(0xFFU << SPI_TXFTLR_TXFTHR_SHIFT);

	ospi->OSPI_TXFTLR |=
		((transfer->tx_total_cnt - 1U) << SPI_TXFTLR_TXFTHR_SHIFT);

	ospi_enable_tx_dma(ospi);

	ospi_enable(ospi);
}

/**
 * \fn          void ospi_dma_transfer(struct ospi_regs *spi,
 *                                     struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for transfer with DMA support
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_dma_transfer(struct ospi_regs *ospi, struct ospi_transfer *transfer)
{
	ospi_disable(ospi);

	ospi->OSPI_CTRLR0 = update_ctrl0_frf_tmode(ospi->OSPI_CTRLR0,
					transfer->spi_frf,
					SPI_CTRLR0_TMOD_RECEIVE_ONLY);

	ospi->OSPI_CTRLR1 = transfer->rx_total_cnt - 1;

	ospi->OSPI_SPI_CTRLR0 = set_spi_ctrlr0_reg(transfer,
						SPI_CTRLR0_INST_L_8bit);

	ospi->OSPI_TXFTLR &= ~(0xFFU << SPI_TXFTLR_TXFTHR_SHIFT);

	ospi->OSPI_TXFTLR |=
		((transfer->tx_total_cnt - 1U) << SPI_TXFTLR_TXFTHR_SHIFT);

	ospi->OSPI_IMR = SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK
			| SPI_IMR_RX_FIFO_OVER_FLOW_INTERRUPT_MASK
			| SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK;

	ospi_enable_tx_dma(ospi);

	ospi_enable_rx_dma(ospi);

	ospi_enable(ospi);
}

/**
  \fn          void ospi_hyperbus_xip_init(struct ospi_regs *ospi, uint8_t wait_cycles,
					bool is_dual_octal)
  \brief       Initialize hyperbus XIP configuration for the OSPI instance
  \param[in]   ospi        Pointer to the OSPI register map
  \param[in]   wait_cycles Wait cycles needed by the hyperbus device
  \param[in]   is_dual_octal OSPI transfer type is Dual Octal
  \return      none
*/
void ospi_hyperbus_xip_init(struct ospi_regs *ospi, uint8_t wait_cycles, bool is_dual_octal)
{
	uint8_t trans_type;

	if (is_dual_octal) {
		trans_type = SPI_TRANS_TYPE_FRF_DUAL_OCTAL;
	} else {
		trans_type = SPI_TRANS_TYPE_STANDARD;
	}

	ospi_disable(ospi);

	ospi->OSPI_SPI_CTRLR0 = 1 << SPI_CTRLR0_SPI_DM_EN_OFFSET;

	ospi->OSPI_XIP_CTRL = (1 << XIP_CTRL_XIP_HYPERBUS_EN_OFFSET)
			| (1 << XIP_CTRL_RXDS_SIG_EN_OFFSET)
			| (wait_cycles << XIP_CTRL_WAIT_CYCLES_OFFSET)
			| (1 << XIP_CTRL_DFS_HC_OFFSET)
			| (trans_type << XIP_CTRL_TRANS_TYPE_OFFSET);

	ospi->OSPI_XIP_WRITE_CTRL = (1 << XIP_WRITE_CTRL_XIPWR_HYPERBUS_EN_OFFSET)
			| (1 << XIP_WRITE_CTRL_XIPWR_DM_EN_OFFSET)
			| (1 << XIP_WRITE_CTRL_XIPWR_RXDS_SIG_EN_OFFSET)
#if (defined(CONFIG_SOC_SERIES_ENSEMBLE_E1C) || defined(CONFIG_SOC_SERIES_BALLETTO_B1) \
		|| defined(CONFIG_SOC_SERIES_ENSEMBLE_E8))
			| (1 << XIP_WRITE_CTRL_XIPWR_DFS_HC_OFFSET)
#endif
			| (trans_type << XIP_WRITE_CTRL_WR_TRANS_TYPE_OFFSET)
			| (wait_cycles << XIP_WRITE_CTRL_XIPWR_WAIT_CYCLES);

	ospi_enable(ospi);
}

/**
 * \fn          void ospi_psram_xip_init(struct ospi_regs *ospi,
 *                    struct ospi_xip_config *xip_cfg, bool is_dual_octal)
 * \brief       Initialize XIP configuration for the OSPI instance
 * \param[in]   ospi     Pointer to the OSPI register map
 * \param[in]   xip_cfg  Pointer to the xip config structure
 * \param[in]   is_dual_octal OSPI transfer type is Dual Octal
 * \return      none
 */
void ospi_psram_xip_init(struct ospi_regs *ospi,
		struct ospi_xip_config *xip_cfg, bool is_dual_octal)
{
	uint8_t trans_type;
	uint32_t val;

	if (is_dual_octal) {
		trans_type = SPI_TRANS_TYPE_FRF_DUAL_OCTAL;
	} else {
		trans_type = SPI_TRANS_TYPE_FRF_DEFINED;
	}

	ospi_disable(ospi);

	val = (SPI_FRAME_FORMAT_OCTAL << XIP_CTRL_FRF_OFFSET)
			| (trans_type << XIP_CTRL_TRANS_TYPE_OFFSET)
			| (0x8 << XIP_CTRL_ADDR_L_OFFSET)
			| (0x2 << XIP_CTRL_INST_L_OFFSET)
			| (0x0 << XIP_CTRL_MD_BITS_EN_OFFSET)
			| (xip_cfg->xip_wait_cycles << XIP_CTRL_WAIT_CYCLES_OFFSET)
			| (0x1 << XIP_CTRL_DFS_HC_OFFSET)
			| (0x1 << XIP_CTRL_DDR_EN_OFFSET)
			| (0x0 << XIP_CTRL_INST_DDR_EN_OFFSET)
			| (0x1 << XIP_CTRL_RXDS_EN_OFFSET)
			| (0x1 << XIP_CTRL_INST_EN_OFFSET)
			| (0x0 << XIP_CTRL_CONT_XFER_EN_OFFSET)
			| (0x0 << XIP_CTRL_XIP_HYPERBUS_EN_OFFSET)
			| (0x0 << XIP_CTRL_RXDS_SIG_EN_OFFSET)
			| (0x0 << XIP_CTRL_XIP_MBL_OFFSET)
			| (0x0 << XIP_CTRL_XIP_PREFETCH_EN_OFFSET)
			| (0x1 << XIP_CTRL_RXDS_VL_EN_OFFSET);

	ospi->OSPI_XIP_CTRL = val;

	val = (SPI_FRAME_FORMAT_OCTAL << XIP_WRITE_CTRL_WR_FRF_OFFSET)
			| (0x2 << XIP_WRITE_CTRL_WR_TRANS_TYPE_OFFSET)
			| (0x8 << XIP_WRITE_CTRL_WR_ADDR_L_OFFSET)
			| (0x2 << XIP_WRITE_CTRL_WR_INST_L_OFFSET)
			| (0x1 << XIP_WRITE_CTRL_WR_SPI_DDR_EN_OFFSET)
			| (0x0 << XIP_WRITE_CTRL_WR_INST_DDR_EN_OFFSET)
			| (0x0 << XIP_WRITE_CTRL_XIPWR_HYPERBUS_EN_OFFSET)
			| (0x0 << XIP_WRITE_CTRL_XIPWR_RXDS_SIG_EN_OFFSET)
			| (0x1 << XIP_WRITE_CTRL_XIPWR_DM_EN_OFFSET)
			| (xip_cfg->xip_wait_cycles << XIP_WRITE_CTRL_XIPWR_WAIT_CYCLES)
			| (0x1 << XIP_WRITE_CTRL_XIPWR_DFS_HC_OFFSET);

	ospi->OSPI_XIP_WRITE_CTRL = val;

	ospi->OSPI_RX_SAMPLE_DELAY = 0;

	ospi->OSPI_XIP_MODE_BITS = xip_cfg->xip_mod_bits;
	ospi->OSPI_XIP_INCR_INST = xip_cfg->incr_cmd;
	ospi->OSPI_XIP_WRAP_INST = xip_cfg->wrap_cmd;
	ospi->OSPI_XIP_WRITE_INCR_INST = xip_cfg->write_incr_cmd;
	ospi->OSPI_XIP_WRITE_WRAP_INST = xip_cfg->write_wrap_cmd;
	ospi->OSPI_XIP_CNT_TIME_OUT = xip_cfg->xip_cnt_time_out;

	ospi_enable(ospi);
}

/**
  \fn          void ospi_hyperbus_send(struct ospi_regs *spi, struct ospi_transfer *transfer)
  \brief       Prepare the OSPI Hyperbus for transmission
  \param[in]   ospi       Pointer to the OSPI register map
  \param[in]   transfer   Transfer parameters
  \return      none
*/
void ospi_hyperbus_send(struct ospi_regs *ospi, struct ospi_transfer *transfer)
{
	uint32_t val, tx_count, curr_fifo_level;

	ospi_disable(ospi);

	val = ospi->OSPI_CTRLR0;
	val &= ~(SPI_CTRLR0_SPI_FRF_MASK | (SPI_CTRLR0_TMOD_MASK | SPI_CTRLR0_SSTE_MASK));
	val |= ((transfer->spi_frf << SPI_CTRLR0_SPI_FRF)
			| SPI_CTRLR0_TMOD_SEND_ONLY
			| SPI_CTRLR0_SPI_HYPERBUS_ENABLE);
	ospi->OSPI_CTRLR0 = val;

	val = SPI_TRANS_TYPE_FRF_DEFINED
			| (transfer->ddr << SPI_CTRLR0_SPI_DDR_EN_OFFSET)
			| (transfer->inst_len << SPI_CTRLR0_INST_L_OFFSET)
			| (transfer->addr_len << SPI_CTRLR0_ADDR_L_OFFSET)
			| (transfer->dummy_cycle << SPI_CTRLR0_WAIT_CYCLES_OFFSET);

	ospi->OSPI_SPI_CTRLR0 = val;

	ospi_enable(ospi);

	/* transmitting data in polling mode */
	while (transfer->tx_total_cnt != transfer->tx_current_cnt) {
		while ((ospi->OSPI_SR & SPI_SR_TFNF) == 0) {
		}

		curr_fifo_level = ospi->OSPI_TXFLR;

		if (transfer->tx_total_cnt >=
				(transfer->tx_current_cnt + OSPI_TX_FIFO_DEPTH - curr_fifo_level)) {
			tx_count = (OSPI_TX_FIFO_DEPTH - curr_fifo_level);
		} else {
			tx_count = (transfer->tx_total_cnt - transfer->tx_current_cnt);
		}

		ospi->OSPI_TXFTLR |= ((tx_count - 1U) << SPI_TXFTLR_TXFTHR_SHIFT);

		for (int i = 0; i < tx_count; i++) {
			ospi->OSPI_DR0 = transfer->tx_buff[0];
			transfer->tx_buff = (transfer->tx_buff + 1);
			transfer->tx_current_cnt++;
		}
	}

	while (ospi_busy(ospi)) {
	}
}

/**
 * \fn          void ospi_irq_handler(struct ospi_regs *ospi,
 *                                    struct ospi_transfer *transfer)
 * \brief       Handle interrupts for the OSPI instance.
 * \param[in]   ospi      Pointer to the OSPI register map
 * \param[in]   transfer  The transfer structure for the OSPI instance
 * \return      none
 */
void ospi_irq_handler(struct ospi_regs *ospi, struct ospi_transfer *transfer)
{
	uint32_t event, tx_data, index, rx_count, tx_count;
	uint16_t frame_size;

	event = ospi->OSPI_ISR;

	if (event & SPI_TX_FIFO_EMPTY_EVENT) {
		frame_size = (SPI_CTRLR0_DFS_MASK & ospi->OSPI_CTRLR0);

		/* Calculate data count to transfer */
		if (transfer->tx_total_cnt >=
			(transfer->tx_current_cnt + OSPI_TX_FIFO_DEPTH)) {
			tx_count = (uint16_t) OSPI_TX_FIFO_DEPTH;
		} else {
			tx_count = (transfer->tx_total_cnt
					- transfer->tx_current_cnt);
		}

		ospi->OSPI_TXFTLR &= ~(0xFFU << SPI_TXFTLR_TXFTHR_SHIFT);
		ospi->OSPI_TXFTLR |=
				((tx_count - 1U) << SPI_TXFTLR_TXFTHR_SHIFT);

		for (index = 0; index < tx_count; index++) {
			tx_data = 0;

			if (transfer->tx_buff == NULL) {
				/* Is the default buffer transmit enabled */
				if (transfer->tx_default_enable == true) {
					tx_data = transfer->tx_default_val;
				}
			} else {
				tx_data = transfer->tx_buff[0];
				transfer->tx_buff = (transfer->tx_buff + 1);
			}

			ospi->OSPI_DR0 = tx_data;
			transfer->tx_current_cnt++;
		}
	}

	if (event & SPI_RX_FIFO_FULL_EVENT) {
		frame_size = (SPI_CTRLR0_DFS_MASK & ospi->OSPI_CTRLR0);

		rx_count = ospi->OSPI_RXFLR;

		if (frame_size > SPI_CTRLR0_DFS_16bit) {
			for (index = 0; index < rx_count; index++) {
				*((uint32_t *) transfer->rx_buff) =
							ospi->OSPI_DR0;

				transfer->rx_buff =
					(uint8_t *) transfer->rx_buff
						+ sizeof(uint32_t);
				transfer->rx_current_cnt++;
			}
		} else if (frame_size > SPI_CTRLR0_DFS_8bit) {
			for (index = 0; index < rx_count; index++) {
				*((uint16_t *) transfer->rx_buff) =
					(uint16_t) (ospi->OSPI_DR0);

				transfer->rx_buff =
					(uint8_t *) transfer->rx_buff
						+ sizeof(uint16_t);
				transfer->rx_current_cnt++;
			}
		} else {
			for (index = 0; index < rx_count; index++) {
				/*
				 * It is observed that with DFS set to 8,
				 * the controller reads in 16bit frames.
				 * Workaround this by making two valid
				 * 8bit frames out of the DR content.
				 */
				uint32_t val = ospi->OSPI_DR0;

				*((uint8_t *) transfer->rx_buff) =
					(uint8_t) ((val >> 8) & 0xff);
				transfer->rx_buff =
					(uint8_t *) transfer->rx_buff
						+ sizeof(uint8_t);
				transfer->rx_current_cnt++;

				if (transfer->rx_current_cnt ==
					transfer->rx_total_cnt)
					break;

				*((uint8_t *) transfer->rx_buff) =
					(uint8_t) (val & 0xff);
				transfer->rx_buff =
					(uint8_t *) transfer->rx_buff
						+ sizeof(uint8_t);
				transfer->rx_current_cnt++;
			}
		}
	}

	if (event &
		(SPI_RX_FIFO_OVER_FLOW_EVENT | SPI_TX_FIFO_OVER_FLOW_EVENT)) {
		/* Disabling and Enabling the OSPI will Reset the FIFO */
		ospi_disable(ospi);
		ospi_enable(ospi);

		transfer->status = SPI_TRANSFER_STATUS_OVERFLOW;
	}

	/* Rx FIFO was accessed when it was empty */
	if (event & SPI_RX_FIFO_UNDER_FLOW_EVENT) {
		transfer->status = SPI_TRANSFER_STATUS_RX_UNDERFLOW;
		ospi->OSPI_IMR &= ~(SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK);
	}

	/* SEND ONLY mode : check if the transfer is finished */
	if ((transfer->mode == SPI_TMOD_TX) &&
		(transfer->tx_total_cnt == transfer->tx_current_cnt)) {
		/* Wait for the transfer to complete */
		if ((ospi->OSPI_SR &
			(SPI_SR_BUSY | SPI_SR_TX_FIFO_EMPTY)) ==
				SPI_SR_TX_FIFO_EMPTY) {
			/* Mask the TX interrupts */
			ospi->OSPI_IMR &= ~(TX_INTR_MASK);

			transfer->tx_current_cnt = 0;
			transfer->status = SPI_TRANSFER_STATUS_COMPLETE;
		}
	}
	/* RECEIVE ONLY mode : check if the transfer is finished */
	if ((transfer->mode == SPI_TMOD_RX) &&
		(transfer->rx_total_cnt == transfer->rx_current_cnt)) {
		/* Mask the RX interrupts */
		ospi->OSPI_IMR &= ~(RX_INTR_MASK);

		transfer->rx_current_cnt = 0;
		transfer->status = SPI_TRANSFER_STATUS_COMPLETE;
	}

	if ((transfer->mode == SPI_TMOD_TX_AND_RX) &&
		(transfer->rx_total_cnt == (transfer->rx_current_cnt))) {
		/* Mask all the interrupts */
		ospi->OSPI_IMR = 0;

		ospi_disable(ospi);

		transfer->rx_current_cnt = 0;
		transfer->status = SPI_TRANSFER_STATUS_COMPLETE;
	}

	if ((transfer->mode == SPI_TMOD_TX_AND_RX) &&
		(transfer->tx_total_cnt == transfer->tx_current_cnt)) {
		if ((ospi->OSPI_SR & SPI_SR_TX_FIFO_EMPTY) ==
			 SPI_SR_TX_FIFO_EMPTY) {
			/* Reset the Tx FIFO start level */
			ospi->OSPI_TXFTLR &=
				~(0xFFU << SPI_TXFTLR_TXFTHR_SHIFT);

			/* Mask the TX interrupts */
			ospi->OSPI_IMR &= ~(TX_INTR_MASK);
		}
	}

	/* Read interrupt clear registers */
	(void) ospi->OSPI_TXEICR;
	(void) ospi->OSPI_RXOICR;
	(void) ospi->OSPI_RXUICR;
	(void) ospi->OSPI_ICR;
}


/**
 * \fn          void ospi_xip_enable(struct ospi_regs *ospi,
 *                                   struct ospi_aes_regs *aes,
 *                                   struct ospi_xip_config *xfg)
 * \brief       Enable XiP Mode
 * \param[in]   ospi Pointer to the OSPI register map
 * \param[in]   aes  Pointer to the AES register map
 * \param[in]   xfg  configurations for XiP mode
 * \return      none
 */
void ospi_xip_enable(struct ospi_regs *ospi, struct ospi_aes_regs *aes,
			struct ospi_xip_config *xfg)
{
	ospi_disable(ospi);

	/* Set OSPI CTRL0 */
	ospi->OSPI_CTRLR0 = set_ospi_ctrl0_in_xip(OCTAL,
					TMODE_RD_ONLY, SPI_CTRLR0_DFS_16bit);

	/* Set OSPI XIP CTRL */
	ospi->OSPI_XIP_CTRL = set_xip_ctrl(xfg->xip_wait_cycles,
					xfg->xip_rxds_vl_en);

	ospi->OSPI_XIP_INCR_INST = xfg->incr_cmd;
	ospi->OSPI_XIP_WRAP_INST = xfg->wrap_cmd;
	ospi->OSPI_XIP_MODE_BITS = xfg->xip_mod_bits;
	ospi->OSPI_RX_SAMPLE_DELAY = xfg->rx_smpl_dlay;

#ifndef CONFIG_FLASH_ADDRESS_IN_SINGLE_FIFO_LOCATION
	ospi_control_xip_ss(ospi, xfg->xip_cs_pin, SPI_SS_STATE_ENABLE);
#endif
	ospi_enable(ospi);

	/* Enable XiP */
	aes->AES_CTRL |= AES_CONTROL_XIP_EN;
}


/**
 * \fn          void ospi_xip_disable(struct ospi_regs *ospi,
 *                              struct ospi_aes_regs *aes,
 *                              struct ospi_transfer *transfer,
 *                              struct ospi_xip_config *xfg)
 * \brief       Disable XiP Mode
 * \param[in]   ospi Pointer to the OSPI register map
 * \param[in]   aes  Pointer to the AES register map
 * \param[in]   transfer  The transfer structure for the OSPI instance
 * \param[in]   xfg  configurations for XiP mode
 * \return      none
 */
void ospi_xip_disable(struct ospi_regs *ospi, struct ospi_aes_regs *aes,
		struct ospi_transfer *transfer, struct ospi_xip_config *xfg)
{
	uint32_t val;

	ospi_disable(ospi);

	/* Set OSPI CTRL0 */
	ospi->OSPI_CTRLR0 = set_ospi_ctrl0_in_xip(OCTAL,
					TMODE_RD_ONLY, SPI_CTRLR0_DFS_32bit);

	val = SPI_TRANS_TYPE_FRF_DEFINED
		|((transfer->ddr) << SPI_CTRLR0_SPI_DDR_EN_OFFSET)
		|(2 << SPI_CTRLR0_XIP_MBL_OFFSET)
		|(1 << SPI_CTRLR0_XIP_DFS_HC_OFFSET)
		|(1 << SPI_CTRLR0_XIP_INST_EN_OFFSET)
		|(SPI_CTRLR0_INST_L_8bit << SPI_CTRLR0_INST_L_OFFSET)
		|(transfer->addr_len) << (SPI_CTRLR0_ADDR_L_OFFSET)
		|(transfer->dummy_cycle << SPI_CTRLR0_WAIT_CYCLES_OFFSET);

	ospi->OSPI_SPI_CTRLR0 = val;

	ospi->OSPI_XIP_MODE_BITS = 0x1;
	ospi->OSPI_XIP_WRITE_INCR_INST = xfg->incr_cmd;
	ospi->OSPI_XIP_WRAP_INST = xfg->wrap_cmd;

	ospi->OSPI_XIP_CNT_TIME_OUT = 100;

#ifndef CONFIG_FLASH_ADDRESS_IN_SINGLE_FIFO_LOCATION
	ospi_control_xip_ss(ospi, xfg->xip_cs_pin, SPI_SS_STATE_ENABLE);
#endif
	ospi_enable(ospi);

	/* XiP Disable */
	aes->AES_CTRL &= ~AES_CONTROL_XIP_EN;
}
