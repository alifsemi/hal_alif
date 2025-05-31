/*
 * Copyright (C) 2024 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ALIF_HAL_OSPI_H_
#define _ALIF_HAL_OSPI_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct ospi_regs {
	volatile uint32_t  OSPI_CTRLR0;      /* 0x00 Control Reg 0           */
	volatile uint32_t  OSPI_CTRLR1;      /* 0x04 Control Reg 1           */
	volatile uint32_t  OSPI_ENR;         /* 0x08 Enable Reg              */
	volatile uint32_t  RESERVED;
	volatile uint32_t  OSPI_SER;         /* 0x10 Slave Enable            */
	volatile uint32_t  OSPI_BAUDR;       /* 0x14 Baud Rate               */
	volatile uint32_t  OSPI_TXFTLR;      /* 0x18 Tx FIFO Thrshold Lvl    */
	volatile uint32_t  OSPI_RXFTLR;      /* 0x1C Rx FIFO Thrshold Lvl    */
	volatile uint32_t  OSPI_TXFLR;       /* 0x20 Tx FIFO Level Reg       */
	volatile uint32_t  OSPI_RXFLR;       /* 0x24 Receive FIFO Level Reg  */
	volatile uint32_t  OSPI_SR;          /* 0x28 Status Reg              */
	volatile uint32_t  OSPI_IMR;         /* 0x2C Interrupt Mask Reg      */
	volatile uint32_t  OSPI_ISR;         /* 0x30 Interrupt Status Reg    */
	volatile uint32_t  OSPI_RISR;        /* 0x34 Raw Intr Status Reg     */
	volatile uint32_t  OSPI_TXEICR;      /* 0x38 Tx FIFO Err Intr clr    */
	volatile uint32_t  OSPI_RXOICR;      /* 0x3C Rx FIFO o-flow Intr clr */
	volatile uint32_t  OSPI_RXUICR;      /* 0x40 Rx FIFO u-flow Intr clr */
	volatile uint32_t  RESERVED1;
	volatile uint32_t  OSPI_ICR;         /* 0x48 Interrupt Clear         */
	volatile uint32_t  OSPI_DMACR;       /* 0x4C DMA Control Reg         */
	volatile uint32_t  OSPI_DMATDLR;     /* 0x50 DMA Tx Data Level       */
	volatile uint32_t  OSPI_DMARDLR;     /* 0x54 DMA Rx Data Level       */
	volatile uint32_t  OSPI_IDR;         /* 0x58 Reserved                */
	volatile uint32_t  OSPI_VERSION_ID;  /* 0x5C Reserved                */
	volatile uint32_t  OSPI_DR0;         /* 0x60 Data Reg (n)            */
	volatile uint32_t  OSPI_DR1;         /* 0x64 Data Reg (n)            */
	volatile uint32_t  OSPI_DR2;         /* 0x68 Data Reg (n)            */
	volatile uint32_t  OSPI_DR3;         /* 0x6C Data Reg (n)            */
	volatile uint32_t  OSPI_DR4;         /* 0x70 Data Reg (n)            */
	volatile uint32_t  OSPI_DR5;         /* 0x74 Data Reg (n)            */
	volatile uint32_t  OSPI_DR6;         /* 0x78 Data Reg (n)            */
	volatile uint32_t  OSPI_DR7;         /* 0x7C Data Reg (n)            */
	volatile uint32_t  OSPI_DR8;         /* 0x80 Data Reg (n)            */
	volatile uint32_t  OSPI_DR9;         /* 0x84 Data Reg (n)            */
	volatile uint32_t  OSPI_DR10;        /* 0x88 Data Reg (n)            */
	volatile uint32_t  OSPI_DR11;        /* 0x8C Data Reg (n)            */
	volatile uint32_t  OSPI_DR12;        /* 0x90 Data Reg (n)            */
	volatile uint32_t  OSPI_DR13;        /* 0x94 Data Reg (n)            */
	volatile uint32_t  OSPI_DR14;        /* 0x98 Data Reg (n)            */
	volatile uint32_t  OSPI_DR15;        /* 0x9C Data Reg (n)            */
	volatile uint32_t  OSPI_DR16;        /* 0xA0 Data Reg (n)            */
	volatile uint32_t  OSPI_DR17;        /* 0xA4 Data Reg (n)            */
	volatile uint32_t  OSPI_DR18;        /* 0xA8 Data Reg (n)            */
	volatile uint32_t  OSPI_DR19;        /* 0xAC Data Reg (n)            */
	volatile uint32_t  OSPI_DR20;        /* 0xB0 Data Reg (n)            */
	volatile uint32_t  OSPI_DR21;        /* 0xB4 Data Reg (n)            */
	volatile uint32_t  OSPI_DR22;        /* 0xB8 Data Reg (n)            */
	volatile uint32_t  OSPI_DR23;        /* 0xBC Data Reg (n)            */
	volatile uint32_t  OSPI_DR24;        /* 0xC0 Data Reg (n)            */
	volatile uint32_t  OSPI_DR25;        /* 0xC4 Data Reg (n)            */
	volatile uint32_t  OSPI_DR26;        /* 0xC8 Data Reg (n)            */
	volatile uint32_t  OSPI_DR27;        /* 0xCC Data Reg (n)            */
	volatile uint32_t  OSPI_DR28;        /* 0xD0 Data Reg (n)            */
	volatile uint32_t  OSPI_DR29;        /* 0xD4 Data Reg (n)            */
	volatile uint32_t  OSPI_DR30;        /* 0xD8 Data Reg (n)            */
	volatile uint32_t  OSPI_DR31;        /* 0xDC Data Reg (n)            */
	volatile uint32_t  OSPI_DR32;        /* 0xE0 Data Reg (n)            */
	volatile uint32_t  OSPI_DR33;        /* 0xE4 Data Reg (n)            */
	volatile uint32_t  OSPI_DR34;        /* 0xE8 Data Reg (n)            */
	volatile uint32_t  OSPI_DR35;        /* 0xEC Data Reg (n)            */
	volatile uint32_t  OSPI_RX_SAMPLE_DELAY; /* 0x0F0 RX Sample Delay    */
	volatile uint32_t  OSPI_SPI_CTRLR0;	 /* 0x0F4 SPI Control Reg    */
	volatile uint32_t  OSPI_DDR_DRIVE_EDGE;	 /* 0x0F8 Tx Drive Edge      */
	volatile uint32_t  OSPI_XIP_MODE_BITS;	 /* 0x0FC XIP Mode Bits Reg  */
	volatile uint32_t  OSPI_XIP_INCR_INST;	 /* 0x100 XIP INCR Tx Opcode */
	volatile uint32_t  OSPI_XIP_WRAP_INST;	 /* 0x104 XIP WRAP Tx Opcode */
	volatile uint32_t  OSPI_XIP_CTRL;	 /* 0x108 XIP Control Reg    */
	volatile uint32_t  OSPI_XIP_SER;         /* 0x10C XIP Slave Enable   */
	volatile uint32_t  RESERVED2;
	volatile uint32_t  OSPI_XIP_CNT_TIME_OUT; /*0x114 For Continuous Tx  */
	volatile const uint32_t RESERVED3[10];
	volatile uint32_t  OSPI_XIP_WRITE_INCR_INST; /* 0x140 INCR Tx Opcode */
	volatile uint32_t  OSPI_XIP_WRITE_WRAP_INST; /* 0x144 WRAP Tx Opcode */
	volatile uint32_t  OSPI_XIP_WRITE_CTRL;      /* 0x148 XIP Write Ctrl */
};


struct ospi_aes_regs {
	volatile uint32_t  AES_CTRL;                     /*  0x00 AES Control             */
	volatile uint32_t  AES_INTR;                     /*  0x04 AES Interrupt Control   */
	volatile uint32_t  AES_INTR_MASK;                /*  0x08 AES Interrupt Mask      */
	volatile uint32_t  AES_CLK_DIS;                  /*  0x0C AES clock Disable       */
	volatile uint32_t  AES_ADDR_CONTROL;             /*  0x010 AES Address Control     */
	volatile uint32_t  AES_RES_0;                    /*  0x14 Reserved Register       */
	volatile uint32_t  AES_RES_1;                    /*  0x18 Reserved Register       */
	volatile uint32_t  AES_RES_2;                    /*  0x1C Reserved Register       */
	volatile uint32_t  AES_RXDS_DLY;                 /*  0x20 AES RXDS Delay Reg      */
	uint32_t  RESERVED0[7];
	volatile uint32_t  AES_RXD_DELAY_0;              /*!< 0x40 RXD[3:0] Delay Reg      */
	volatile uint32_t  AES_RXD_DELAY_1;              /*!< 0x44 RXD[7:4] Delay Reg      */
	volatile uint32_t  AES_RXD_DELAY_2;              /*!< 0x48 RXD[11:8] Delay Reg     */
	volatile uint32_t  AES_RXD_DELAY_3;              /*!< 0x4C RXD[15:12] Delay Reg    */
	volatile uint32_t  AES_TXD_DELAY_0;              /*!< 0x50 TXD[3:0] Delay Reg      */
	volatile uint32_t  AES_TXD_DELAY_1;              /*!< 0x54 TXD[7:4] Delay Reg      */
	volatile uint32_t  AES_TXD_DELAY_2;              /*!< 0x58 TXD[11:8] Delay Reg     */
	volatile uint32_t  AES_TXD_DELAY_3;              /*!< 0x5C TXD[15:12] Delay Reg    */
	volatile uint32_t  AES_SSI_OE_N_DELAY_0;         /*!< 0x60 SSI_OE_N[3:0] Delay Reg */
	volatile uint32_t  AES_SSI_OE_N_DELAY_1;         /*!< 0x64 SSI_OE_N[7:4] Delay Reg */
	volatile uint32_t  AES_SSI_OE_N_DELAY_2;         /*!< 0x68 SSI_OE_N[11:8] Delay Reg*/
	volatile uint32_t  AES_SSI_OE_N_DELAY_3;         /*!< 0x6C SSI_OE_N[15:12] Delay Reg*/
	volatile uint32_t  AES_SS_N_DELAY;               /*!< 0x70 SS_N Delay Reg          */
	volatile uint32_t  AES_TXD_DM_DELAY;             /*!< 0x74 TXD_DM Delay Reg        */
	volatile uint32_t  AES_SCLK_DELAY;               /*!< 0x78 SLCK Delay Reg          */
};

/* Bit fields for SPI FRF */
#define SINGLE               0x0
#define DUAL                 0x1
#define QUAD                 0x2
#define OCTAL                0x3

#define OSPI_TX_FIFO_DEPTH   256U
#define OSPI_RX_FIFO_DEPTH   256U

#define OSPI_ENABLE          1
#define OSPI_DISABLE         0

/* SPI Control Register 0 (CTRLR0) bit Definition, Macros, Offsets and Masks
 * these include DFS, FRF, SCPH, SCPOL, TMOD, etc
 */
/* Data Frame DFS bit[4:0]*/
#define SPI_CTRLR0_DFS                 0U
#define SPI_CTRLR0_DFS_8bit            0x07U
#define SPI_CTRLR0_DFS_16bit           0x0FU
#define SPI_CTRLR0_DFS_32bit           0x1FU
#define SPI_CTRLR0_DFS_MASK            (0x1FU << SPI_CTRLR0_DFS)

/* Frame Format FRF bit[7:6] */
#define SPI_CTRLR0_FRF                  6U
#define SPI_CTRLR0_FRF_MASK             (0x3UL << SPI_CTRLR0_FRF)
#define SPI_CTRLR0_FRF_MOTOROLA         0x00U     /* Motorola SPI Frame Format */
#define SPI_CTRLR0_FRF_TI               0x40U     /* Texas Instruments SSP Frame Format */
#define SPI_CTRLR0_FRF_MICROWIRE        0x80U     /* National Semiconductors Microwire Format */

/* Serial Clock Polarity SCPOL | Serial Clock Phase SCPH bit[9:8] */
#define SPI_CTRLR0_SC                           8U
#define SPI_CTRLR0_SC_MASK                      (0x3UL << SPI_CTRLR0_SC)
#define SPI_CTRLR0_SCPH_HIGH                    0x100U     /* 0x1 SPI SCPH high */
#define SPI_CTRLR0_SCPH_LOW                     0x000U     /* 0x0 SPI SCPH low */
#define SPI_CTRLR0_SCPOL_HIGH                   0x200U     /* 0x2 SPI SCPOL high */
#define SPI_CTRLR0_SCPOL_LOW                    0x000U     /* 0x0 SPI SCPOL low */

/* Transfer Mode TMOD bit[11:10] */
#define SPI_CTRLR0_TMOD                         10U
#define SPI_CTRLR0_TMOD_MASK                    (0x3UL << SPI_CTRLR0_TMOD)
#define SPI_CTRLR0_TMOD_TRANSFER                0x000U     /* 0x0 SPI transfer mode */
#define SPI_CTRLR0_TMOD_SEND_ONLY               0x400U     /* 0x1 SPI send only mode */
#define SPI_CTRLR0_TMOD_RECEIVE_ONLY            0x800U     /* 0x2 SPI receive only mode */
#define SPI_CTRLR0_TMOD_EEPROM_READ_ONLY        0xC00U     /* 0x3 SPI EEPROM read only mode */

/* Slave Select toggle Enable bit[14]
 * While operating in SPI mode with clock phase (SCPH) set to
 * 0, this register controls the behavior of the slave select line
 *  between data frames.
 */
/* Slave Select toggle Enable SSTE bit[14] */
#define SPI_CTRLR0_SSTE                 14U
#define SPI_CTRLR0_SSTE_MASK            (1U << SPI_CTRLR0_SSTE)
#define SPI_CTRLR0_SSTE_ENABLE          0x4000U     /* 0x1 SPI slave select toggle enable */
#define SPI_CTRLR0_SSTE_DISABLE         0x0000U     /* 0x0 SPI slave select toggle disable */

/* Control Frame Size for the Microwire frame format CFS bit[19:16]*/
#define SPI_CTRLR0_CFS                  16U
#define SPI_CTRLR0_CFS_MASK             (0xFU << SPI_CTRLR0_CFS)

#define SPI_FRF_DUAL                    0x1
#define SPI_FRF_QUAD                    0x2
#define SPI_FRF_OCTAL                   0x3

/* SPI Frame Format SPI_FRF bit[23:22]*/
#define SPI_CTRLR0_SPI_FRF              22U
#define SPI_CTRLR0_SPI_FRF_MASK         (0x3U << SPI_CTRLR0_SPI_FRF)
#define SPI_CTRLR0_SPI_FRF_STANDRAD     0x000000U    /* 0x0 Standard SPI Format */
#define SPI_CTRLR0_SPI_FRF_DUAL         0x400000U    /* 0x1 Dual SPI Format */
#define SPI_CTRLR0_SPI_FRF_QUAD         0x800000U    /* 0X2 Quad SPI Format */
#define SPI_CTRLR0_SPI_FRF_OCTAL        0xC00000U    /* 0X2 Octal SPI Format */

/* SPI Hyperbus Frame format enable SPI_HYPERBUS_EN bit[24] */
#define SPI_CTRLR0_SPI_HYPERBUS_EN              24
#define SPI_CTRLR0_SPI_HYPERBUS_EN_SSTE_MASK    (1 << SPI_CTRLR0_SPI_HYPERBUS_EN)
#define SPI_CTRLR0_SPI_HYPERBUS_ENABLE          0x1000000U   /* 0x1 SPI Hyperbus enable */
#define SPI_CTRLR0_SPI_HYPERBUS_DISABLE         0x0000000U   /* 0x0 SPI Hyperbus disable */

/* SPI is working in Master or Slave SSI_IS_MST bit[31] */
#define SPI_CTRLR0_SSI_IS_MST                           31U
#define SPI_CTRLR0_SSI_IS_MST_MASK                      (0x1UL << SPI_CTRLR0_SSI_IS_MST)
#define SPI_CTRLR0_SSI_IS_MST_MASTER                    0x80000000U  /* 0x1 SPI master */
#define SPI_CTRLR0_SSI_IS_MST_SLAVE                     0x00000000U  /* 0x0 SPI slave  */

/* Quad SPI MODE Macros */
#define SPI_CTRLR0_SPI_QUAD_ENABLE                     (0x2UL << SPI_CTRLR0_SPI_FRF)
#define SPI_CTRLR0_SPI_QUAD_TX_MODE                     SPI_TMOD_SEND_ONLY
#define SPI_CTRLR0_SPI_QUAD_RX_MODE                     SPI_TMOD_RECEIVE_ONLY

/* Octal SPI MODE Macros */
#define SPI_CTRLR0_SPI_OCTAL_ENABLE                     (0x3UL << SPI_CTRLR0_SPI_FRF)
#define SPI_CTRLR0_SPI_OCTAL_TX_RX_MODE                 SPI_TMOD_TRANSFER
#define SPI_CTRLR0_SPI_OCTAL_TX_MODE                    SPI_TMOD_SEND_ONLY
#define SPI_CTRLR0_SPI_OCTAL_RX_MODE                    SPI_TMOD_RECEIVE_ONLY

#define SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK            0x00000001U  /* Tx fifo empty intr */
#define SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK        0x00000002U  /* Tx fifo overflow intr */
#define SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK       0x00000004U  /* Rx fifo underflow intr */
#define SPI_IMR_RX_FIFO_OVER_FLOW_INTERRUPT_MASK        0x00000008U  /* Rx fifo Overflow intr */
#define SPI_IMR_RX_FIFO_FULL_INTERRUPT_MASK             0x00000010U  /* Rx fifo full intr */
#define SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK  0x00000020U  /* Multi-Master contention */

#define SPI_TXFTLR_TFT_SHIFT                            0U
#define SPI_TXFTLR_TFT_MASK                             (0xFFFFU << SPI_TXFTLR_TFT_SHIFT)
#define SPI_TXFTLR_TXFTHR_SHIFT                         16U
#define SPI_TXFTLR_TXFTHR_MASK                          (0xFFFFU << SPI_TXFTLR_TXFTHR_SHIFT)

/* Bit fields in OSPI_CTRLR0 */
#define OSPI_CTRLR0_IS_MST                      (1U << 31)
#define OSPI_CTRLR0_HE_OFFSET                   24U
#define OSPI_CTRLR0_SPI_FRF_OFFSET              22U
#define OSPI_CTRLR0_SSTE_OFFSET                 14U
#define OSPI_CTRLR0_SRL_OFFSET                  13U
#define OSPI_CTRLR0_TMOD_OFFSET                 10U
#define OSPI_CTRLR0_TMOD_MASK                   (3U << OSPI_CTRLR0_TMOD_OFFSET)
#define OSPI_CTRLR0_SCPOL_OFFSET                9U
#define OSPI_CTRLR0_SCPH_OFFSET                 8U
#define OSPI_CTRLR0_FRF_OFFSET                  6U
#define OSPI_CTRLR0_DFS_OFFSET                  0U

/* Bit fields in SPI_CTRLR0 */
#define SPI_CTRLR0_XIP_PREFETCH_EN_OFFSET       29U
#define SPI_CTRLR0_XIP_MBL_OFFSET               26U
#define SPI_CTRLR0_SPI_RXDS_SIG_EN_OFFSET       25U
#define SPI_CTRLR0_SPI_DM_EN_OFFSET             24U
#define SPI_CTRLR0_XIP_CONT_EN_OFFSET           21U
#define SPI_CTRLR0_XIP_INST_EN_OFFSET           20U
#define SPI_CTRLR0_XIP_DFS_HC_OFFSET            19U
#define SPI_CTRLR0_SPI_RXDS_EN_OFFSET           18U
#define SPI_CTRLR0_INST_DDR_EN_OFFSET           17U
#define SPI_CTRLR0_SPI_DDR_EN_OFFSET            16U
#define SPI_CTRLR0_WAIT_CYCLES_OFFSET           11U
#define SPI_CTRLR0_INST_L_OFFSET                8U
#define SPI_CTRLR0_XIP_MD_EN_OFFSET             7U
#define SPI_CTRLR0_ADDR_L_OFFSET                2U
#define SPI_CTRLR0_TRANS_TYPE_OFFSET            0U

#define SPI_CTRLR0_TRANS_TYPE_MASK              3U
#define SPI_TRANS_TYPE_STANDARD                 0U
#define SPI_TRANS_TYPE_FRF_DEFINED              2U  /* CTRLR0.SPI_FRF :Standard/Dual/Quad/Octal */
#define SPI_TRANS_TYPE_FRF_DUAL_OCTAL           3U

#define SPI_CTRLR0_SPI_RXDS_ENABLE              1U
#define SPI_CTRLR0_SPI_RXDS_DISABLE             0U

#define SPI_CTRLR0_INST_L_0bit                  0x0U
#define SPI_CTRLR0_INST_L_4bit                  0x1U
#define SPI_CTRLR0_INST_L_8bit                  0x2U
#define SPI_CTRLR0_INST_L_16bit                 0x3U

#define SPI_DMACR_TDMAE                         2U
#define SPI_DMACR_RDMAE                         1U

/* Bit fields in XIP_CTRL */
#define XIP_CTRL_RXDS_VL_EN_OFFSET              30U
#define XIP_CTRL_XIP_PREFETCH_EN_OFFSET         29U
#define XIP_CTRL_XIP_MBL_OFFSET                 26U
#define XIP_CTRL_RXDS_SIG_EN_OFFSET             25U
#define XIP_CTRL_XIP_HYPERBUS_EN_OFFSET         24U
#define XIP_CTRL_CONT_XFER_EN_OFFSET            23U
#define XIP_CTRL_INST_EN_OFFSET                 22U
#define XIP_CTRL_RXDS_EN_OFFSET                 21U
#define XIP_CTRL_INST_DDR_EN_OFFSET             20U
#define XIP_CTRL_DDR_EN_OFFSET                  19U
#define XIP_CTRL_DFS_HC_OFFSET                  18U
#define XIP_CTRL_WAIT_CYCLES_OFFSET             13U
#define XIP_CTRL_MD_BITS_EN_OFFSET              12U
#define XIP_CTRL_INST_L_OFFSET                  9U
#define XIP_CTRL_ADDR_L_OFFSET                  4U
#define XIP_CTRL_TRANS_TYPE_OFFSET              2U
#define XIP_CTRL_FRF_OFFSET                     0U

#define XIP_WRITE_CTRL_XIPWR_DFS_HC_OFFSET      21U
#define XIP_WRITE_CTRL_XIPWR_WAIT_CYCLES        16U
#define XIP_WRITE_CTRL_XIPWR_DM_EN_OFFSET       14U
#define XIP_WRITE_CTRL_XIPWR_RXDS_SIG_EN_OFFSET 13U
#define XIP_WRITE_CTRL_XIPWR_HYPERBUS_EN_OFFSET 12U
#define XIP_WRITE_CTRL_WR_INST_DDR_EN_OFFSET    11U
#define XIP_WRITE_CTRL_WR_SPI_DDR_EN_OFFSET     10U
#define XIP_WRITE_CTRL_WR_INST_L_OFFSET         8U
#define XIP_WRITE_CTRL_WR_ADDR_L_OFFSET         4U
#define XIP_WRITE_CTRL_WR_TRANS_TYPE_OFFSET     2U
#define XIP_WRITE_CTRL_WR_FRF_OFFSET            0U

#define SPI_SR_TX_FIFO_EMPTY                    0x4U
#define SPI_SR_TFNF                             0x2U
#define SPI_SR_BUSY                             0x1U

#define SPI_TX_FIFO_EMPTY_EVENT                 0x01U  /* Tx fifo empty interrupt mask*/
#define SPI_TX_FIFO_OVER_FLOW_EVENT             0x02U  /* Tx fifo overflow interrupt mask*/
#define SPI_RX_FIFO_UNDER_FLOW_EVENT            0x04U  /* Rx fifo underflow interrupt mask*/
#define SPI_RX_FIFO_OVER_FLOW_EVENT             0x08U  /* Rx fifo Overflow interrupt mask*/
#define SPI_RX_FIFO_FULL_EVENT                  0x10U  /* Rx fifo full interrupt mask*/
#define SPI_MULTI_MASTER_CONTENTION_EVENT       0x20U  /* Multi-Master contention intr mask*/

#define TMODE_TX_AND_RX                 0x0
#define TMODE_TX_ONLY                   0x1
#define TMODE_RD_ONLY                   0x2
#define TMODE_EPROM_READ                0x3

#define XIP_CTRL_INST_LEN_0_BIT         0x0
#define XIP_CTRL_INST_LEN_4_BIT         0x1
#define XIP_CTRL_INST_LEN_8_BIT         0x2
#define XIP_CTRL_INST_LEN_16_BIT        0x3

#define XIP_CTRL_ADDR_LEN_0_BIT         0x0
#define XIP_CTRL_ADDR_LEN_4_BIT         0x1
#define XIP_CTRL_ADDR_LEN_8_BIT         0x2
#define XIP_CTRL_ADDR_LEN_16_BIT        0x3
#define XIP_CTRL_ADDR_LEN_20_BIT        0x4
#define XIP_CTRL_ADDR_LEN_24_BIT        0x5
#define XIP_CTRL_ADDR_LEN_28_BIT        0x6
#define XIP_CTRL_ADDR_LEN_32_BIT        0x7
#define XIP_CTRL_ADDR_LEN_36_BIT        0x8

/* AES_CONTROL fields */
#define AES_CONTROL_LD_KEY              (1U << 7)
#define AES_CONTROL_XIP_EN              (1U << 4)
#define AES_CONTROL_DECRYPT_EN          (1U << 0)

/* AES_INTR_MASK fields */
#define AES_INTR_MASK_BAUD2_DELAY       30U
#define AES_INTR_MASK_BAUD2_DELAY_MASK  (1U << AES_INTR_MASK_BAUD2_DELAY)

/**
 * enum spi_frame_format.
 * SPI frame formats.
 */
enum spi_frame_format {
	SPI_FRAME_FORMAT_STANDARD,          /* Standard SPI frame format */
	SPI_FRAME_FORMAT_DUAL,              /* Dual SPI frame format */
	SPI_FRAME_FORMAT_QUAD,              /* Quad SPI frame format */
	SPI_FRAME_FORMAT_OCTAL              /* Octal SPI frame format */
};

/**
 * enum spi_mode.
 * SPI modes.
 */
enum spi_mode {
	SPI_MODE_0,                 /* SPI Mode - 0 : CPHA = 0, CPOL = 0 */
	SPI_MODE_1,                 /* SPI Mode - 1 : CPHA = 1, CPOL = 0 */
	SPI_MODE_2,                 /* SPI Mode - 2 : CPHA = 0, CPOL = 1 */
	SPI_MODE_3                  /* SPI Mode - 3 : CPHA = 1, CPOL = 1 */
};

/**
 * enum spi_tmode.
 * SPI transfer modes.
 */
enum spi_tmode {
	SPI_TMOD_TX_AND_RX     = TMODE_TX_AND_RX,       /* Tx and Rx */
	SPI_TMOD_TX            = TMODE_TX_ONLY,         /* Transmit only  */
	SPI_TMOD_RX            = TMODE_RD_ONLY,         /* Receive only */
	SPI_TMOD_EEPROM_READ   = TMODE_EPROM_READ       /* EEPROM read  */
};

/**
 * enum spi_ss_state.
 * SPI Slave Select States.
 */
enum spi_ss_state {
	SPI_SS_STATE_DISABLE,                   /* Slave select disabled */
	SPI_SS_STATE_ENABLE,                    /* Slave select Enabled */
};

/**
 * enum spi_transfer_status.
 * Status of an ongoing SPI transfer.
 */
enum spi_transfer_status {
	SPI_TRANSFER_STATUS_NONE,               /* Status none */
	SPI_TRANSFER_STATUS_COMPLETE,           /* Status complete */
	SPI_TRANSFER_STATUS_OVERFLOW,           /* Status Tx/Rx overflow */
	SPI_TRANSFER_STATUS_MASTER_CONTENTION,  /* Status master contention */
	SPI_TRANSFER_STATUS_RX_UNDERFLOW,       /* Status Rx underflow */
};

/**
 * struct ospi_transfer.
 * Information about an ongoing OSPI transfer.
 */
struct ospi_transfer {
	uint32_t            tx_current_cnt;     /* Current Tx Transfer count */
	uint32_t            rx_current_cnt;     /* Current Rx Transfer count */
	uint32_t            tx_total_cnt;       /* Total count to transmit */
	uint32_t            rx_total_cnt;       /* Total count to receive */
	const uint32_t      *tx_buff;           /* Pointer to TX buffer */
	void                *rx_buff;           /* Pointer to Rx buffer */
	uint32_t            tx_default_val;     /* Default value to Transfer */
	uint32_t            spi_frf;            /* Standard/Dual/Quad/Octal  */
	uint32_t            addr_len;           /* Address length  */
	uint32_t            inst_len;           /* Instruction length */
	uint32_t            dummy_cycle;        /* Dummy cycles    */
	uint32_t            ddr;                /* DDR / SDR mode  */
	bool                tx_default_enable;  /* Enable Tx default */
	enum spi_tmode      mode;               /* SPI transfer mode */
	enum spi_transfer_status    status;    /* transfer status */

	/**XiP Configuration*/
	uint16_t            wrap_cmd;           /* WRAP OpCode */
	uint16_t            incr_cmd;           /* INCR OpCode */
};

/**
 * struct ospi_xip_config.
 * configuration for xip setup.
 */
struct ospi_xip_config {
	int16_t                 xip_cs_pin;          /* CS PIN   */
	uint16_t                wrap_cmd;            /* WRAP cmd */
	uint16_t                incr_cmd;            /* INCR cmd */
	uint16_t                rx_smpl_dlay;        /* Delay on Sampling */
	uint16_t                aes_rx_ds_dlay;      /* AES Dly */
	uint16_t                xip_mod_bits;        /* XIP Mod */
	uint16_t                xip_cnt_time_out;    /* Timeout value */
	uint16_t                xip_wait_cycles;     /* Dummy cycles*/
	uint16_t                xip_rxds_vl_en;      /* Enable RxDS_VL_EN bit */
};

/**
 * \fn          static inline void ospi_disable(struct ospi_regs *spi)
 * \brief       Disable the OSPI instance
 * \param[in]   ospi     Pointer to the OSPI register map
 * \return      none
 */
static inline void ospi_disable(struct ospi_regs *ospi)
{
	ospi->OSPI_ENR = OSPI_DISABLE;
}

/**
 * \fn          static inline void ospi_enable(struct ospi_regs *spi)
 * \brief       Enable the OSPI instance
 * \param[in]   ospi     Pointer to the OSPI register map
 * \return      none
 */
static inline void ospi_enable(struct ospi_regs *ospi)
{
	ospi->OSPI_ENR = OSPI_ENABLE;
}

/***
 * \fn          static inline void ospi_mode_master(struct ospi_regs *ospi)
 * \brief       Enable master mode in the OSPI instance.
 * \param[in]   ospi     Pointer to the OSPI register map
 * \return      none
 */
static inline void ospi_mode_master(struct ospi_regs *ospi)
{
	ospi_disable(ospi);
	ospi->OSPI_CTRLR0 |= SPI_CTRLR0_SSI_IS_MST_MASTER;
	ospi_enable(ospi);
}

/***
 * \fn          static inline void ospi_mode_slave(struct ospi_regs *ospi)
 * \brief       Enable slave mode in the OSPI instance.
 * \param[in]   ospi     Pointer to the OSPI register map
 * \return      none
 */
static inline void ospi_mode_slave(struct ospi_regs *ospi)
{
	ospi_disable(ospi);
	ospi->OSPI_CTRLR0 &= ~SPI_CTRLR0_SSI_IS_MST_MASTER;
	ospi_enable(ospi);
}

/***
 * \fn          static inline void ospi_set_bus_speed(struct ospi_regs *ospi,
 *                                       uint32_t speed, uint32_t clk)
 * \brief       Set the bus speed for the OSPI instance.
 * \param[in]   ospi    Pointer to the OSPI register map
 * \param[in]   speed   The bus speed to be set
 * \param[in]   clk     OSPI input clk
 * \return      none
 */
static inline void ospi_set_bus_speed(struct ospi_regs *ospi,
					uint32_t speed, uint32_t clk)
{
	ospi_disable(ospi);
	ospi->OSPI_BAUDR = (clk / speed);
	ospi_enable(ospi);
}

/***
 * \fn          static inline void ospi_aes_set_baud2_delay(struct ospi_aes_regs *aes,
 *                                                          bool baud2_delay_en)
 * \brief       Enable or disable BAUD2 delay
 * \param[in]   ospi    Pointer to the AES register map
 * \param[in]   baud2_delay_en  Enable \c true or disable \c false
 * \return      none
 */
static inline void ospi_aes_set_baud2_delay(struct ospi_aes_regs *aes,
	bool baud2_delay_en)
{
#if defined(CONFIG_SOC_SERIES_ENSEMBLE_E1C) || defined(CONFIG_SOC_SERIES_BALLETTO_B1)
	if (baud2_delay_en) {
		aes->AES_INTR_MASK |= AES_INTR_MASK_BAUD2_DELAY_MASK;
	} else {
		aes->AES_INTR_MASK &= ~AES_INTR_MASK_BAUD2_DELAY_MASK;
	}
#else
	(void)aes;
	(void)baud2_delay_en;
#endif
}

#if defined(CONFIG_SOC_SERIES_ENSEMBLE_E8)
#define AES_SIGNAL_0_DELAY_POS                 (0)
#define AES_SIGNAL_1_DELAY_POS                 (8)
#define AES_SIGNAL_2_DELAY_POS                 (16)
#define AES_SIGNAL_3_DELAY_POS                 (24)
#define AES_SIGNAL_4_DELAY_POS                 (0)
#define AES_SIGNAL_5_DELAY_POS                 (8)
#define AES_SIGNAL_6_DELAY_POS                 (16)
#define AES_SIGNAL_7_DELAY_POS                 (24)
#define AES_SIGNAL_8_DELAY_POS                 (0)
#define AES_SIGNAL_9_DELAY_POS                 (8)
#define AES_SIGNAL_10_DELAY_POS                (16)
#define AES_SIGNAL_11_DELAY_POS                (24)
#define AES_SIGNAL_12_DELAY_POS                (0)
#define AES_SIGNAL_13_DELAY_POS                (8)
#define AES_SIGNAL_14_DELAY_POS                (16)
#define AES_SIGNAL_15_DELAY_POS                (24)
#define AES_TXD_DM_0_DELAY_POS                 (0)
#define AES_TXD_DM_1_DELAY_POS                 (8)
#define AES_DM_OE_N_DELAY_0_POS                (16)
#define AES_DM_OE_N_DELAY_1_POS                (24)
#define AES_SCLK_DELAY_POS                     (0)
#define AES_SCLK_N_DELAY_POS                   (8)

/***
 * \fn          static inline void aes_set_rxds_delay
 *                   (struct ospi_aes_regs *aes, uint8_t rxds_delay)
 * \brief       Set OSPI RXDS Delay
 * \param[in]   aes        Pointer to the AES register map
 * \param[in]   rxds_delay rxds value
 * \return      none
 */
static inline void aes_set_rxds_delay(struct ospi_aes_regs *aes, uint8_t delay)
{
	aes->AES_RXDS_DLY = ((delay << AES_SIGNAL_0_DELAY_POS) | (delay << AES_SIGNAL_1_DELAY_POS));
}

/***
 * \fn          static inline void aes_set_signal_delay
 *                   (struct ospi_aes_regs *aes, uint8_t delay)
 * \brief       Set OSPI Signal Delay for BAUD2 speed
 * \param[in]   aes        Pointer to the AES register map
 * \param[in]   delay      Delay value for signals
 * \return      none
 */
static inline void aes_set_signal_delay(struct ospi_aes_regs *aes, uint8_t delay)
{
	aes->AES_RXD_DELAY_0 = ((delay << AES_SIGNAL_0_DELAY_POS)
				| (delay << AES_SIGNAL_1_DELAY_POS)
				| (delay << AES_SIGNAL_2_DELAY_POS)
				| (delay << AES_SIGNAL_3_DELAY_POS));
	aes->AES_RXD_DELAY_1 = ((delay << AES_SIGNAL_4_DELAY_POS)
				| (delay << AES_SIGNAL_5_DELAY_POS)
				| (delay << AES_SIGNAL_6_DELAY_POS)
				| (delay << AES_SIGNAL_7_DELAY_POS));
	aes->AES_RXD_DELAY_2 = ((delay << AES_SIGNAL_8_DELAY_POS)
				| (delay << AES_SIGNAL_9_DELAY_POS)
				| (delay << AES_SIGNAL_10_DELAY_POS)
				| (delay << AES_SIGNAL_11_DELAY_POS));
	aes->AES_RXD_DELAY_3 = ((delay << AES_SIGNAL_12_DELAY_POS)
				| (delay << AES_SIGNAL_13_DELAY_POS)
				| (delay << AES_SIGNAL_14_DELAY_POS)
				| (delay << AES_SIGNAL_15_DELAY_POS));

	aes->AES_TXD_DELAY_0 = ((delay << AES_SIGNAL_0_DELAY_POS)
				| (delay << AES_SIGNAL_1_DELAY_POS)
				| (delay << AES_SIGNAL_2_DELAY_POS)
				| (delay << AES_SIGNAL_3_DELAY_POS));
	aes->AES_TXD_DELAY_1 = ((delay << AES_SIGNAL_4_DELAY_POS)
				| (delay << AES_SIGNAL_5_DELAY_POS)
				| (delay << AES_SIGNAL_6_DELAY_POS)
				| (delay << AES_SIGNAL_7_DELAY_POS));
	aes->AES_TXD_DELAY_2 = ((delay << AES_SIGNAL_8_DELAY_POS)
				| (delay << AES_SIGNAL_9_DELAY_POS)
				| (delay << AES_SIGNAL_10_DELAY_POS)
				| (delay << AES_SIGNAL_11_DELAY_POS));
	aes->AES_TXD_DELAY_3 = ((delay << AES_SIGNAL_12_DELAY_POS)
				| (delay << AES_SIGNAL_13_DELAY_POS)
				| (delay << AES_SIGNAL_14_DELAY_POS)
				| (delay << AES_SIGNAL_15_DELAY_POS));

	aes->AES_SSI_OE_N_DELAY_0 = ((delay << AES_SIGNAL_0_DELAY_POS)
				   | (delay << AES_SIGNAL_1_DELAY_POS)
				   | (delay << AES_SIGNAL_2_DELAY_POS)
				   | (delay << AES_SIGNAL_3_DELAY_POS));
	aes->AES_SSI_OE_N_DELAY_1 = ((delay << AES_SIGNAL_4_DELAY_POS)
				   | (delay << AES_SIGNAL_5_DELAY_POS)
				   | (delay << AES_SIGNAL_6_DELAY_POS)
				   | (delay << AES_SIGNAL_7_DELAY_POS));
	aes->AES_SSI_OE_N_DELAY_2 = ((delay << AES_SIGNAL_8_DELAY_POS)
				   | (delay << AES_SIGNAL_9_DELAY_POS)
				   | (delay << AES_SIGNAL_10_DELAY_POS)
				   | (delay << AES_SIGNAL_11_DELAY_POS));
	aes->AES_SSI_OE_N_DELAY_3 = ((delay << AES_SIGNAL_12_DELAY_POS)
				   | (delay << AES_SIGNAL_13_DELAY_POS)
				   | (delay << AES_SIGNAL_14_DELAY_POS)
				   | (delay << AES_SIGNAL_15_DELAY_POS));

	aes->AES_SS_N_DELAY = ((delay << AES_SIGNAL_0_DELAY_POS)
				| (delay << AES_SIGNAL_1_DELAY_POS));

	aes->AES_TXD_DM_DELAY = ((delay << AES_TXD_DM_0_DELAY_POS)
				| (delay << AES_TXD_DM_1_DELAY_POS)
				|(delay << AES_DM_OE_N_DELAY_0_POS)
				| (delay << AES_DM_OE_N_DELAY_1_POS));

	aes->AES_SCLK_DELAY = ((delay << AES_SCLK_DELAY_POS) | (delay << AES_SCLK_N_DELAY_POS));
}
#else

/***
 * \fn          static inline void aes_set_rxds_delay
 *                   (struct ospi_aes_regs *aes, uint8_t rxds_delay)
 * \brief       Set OSPI RXDS Delay
 * \param[in]   aes        Pointer to the AES register map
 * \param[in]   rxds_delay RXDS delay value
 * \return      none
 */
static inline void aes_set_rxds_delay(struct ospi_aes_regs *aes, uint8_t rxds_delay)
{
	aes->AES_RXDS_DLY = rxds_delay;
}
#endif

/***
 * \fn          static inline void aes_enable_xip(struct ospi_aes_regs *aes)
 * \brief       Enable OSPI XIP
 * \param[in]   aes     Pointer to the AES register map
 * \return      none
 */
static inline void aes_enable_xip(struct ospi_aes_regs *aes)
{
	aes->AES_CTRL |= AES_CONTROL_XIP_EN;
}

/***
 * \fn          static inline void aes_disable_xip(struct ospi_aes_regs *aes)
 * \brief       Disable OSPI XIP
 * \param[in]   aes     Pointer to the AES register map
 * \return      none
 */
static inline void aes_disable_xip(struct ospi_aes_regs *aes)
{
	aes->AES_CTRL &= ~AES_CONTROL_XIP_EN;
}

/***
 * \fn          static inline uint32_t ospi_get_bus_speed(
 *                                         struct ospi_regs *ospi, uint32_t clk)
 * \brief       Get the current bus speed of the OSPI instance.
 * \param[in]   ospi    Pointer to the OSPI register map
 * \param[in]   clk     OSPI input clk
 * \return      Current bus speed
 */
static inline uint32_t ospi_get_bus_speed(struct ospi_regs *ospi, uint32_t clk)
{
	return clk / ospi->OSPI_BAUDR;
}

/***
 * \fn          static inline void ospi_mask_interrupts(struct ospi_regs *spi)
 * \brief       Mask all the interrupts for the OSPI instance
 * \param[in]   ospi       Pointer to the SPI register map
 * \return      none
 */
static inline void ospi_mask_interrupts(struct ospi_regs *ospi)
{
	ospi->OSPI_IMR = 0;
}

/***
 * \fn          static inline uint32_t ospi_get_dma_addr(struct ospi_regs *ospi)
 * \brief       Get the Data(FIFO) register address for the OSPI instance
 * \param[in]   ospi     Pointer to the OSPI register map
 * \return      TXDMA register address.
 */
static inline volatile uint32_t *ospi_get_dma_addr(struct ospi_regs *ospi)
{
	return &ospi->OSPI_DR0;
}

/***
 * \fn          void ospi_set_tx_dma_data_level(struct ospi_regs *ospi,
 *                                              uint8_t level)
 * \brief       Set Tx DMA trigger level for the OSPI instance.
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   trig_level Tx DMA trigger level
 * \return      none
 */
static inline void ospi_set_tx_dma_data_level(struct ospi_regs *ospi,
					uint8_t level)
{
	ospi->OSPI_DMATDLR = level;
}

/***
 * \fn          void ospi_set_rx_dma_data_level(struct ospi_regs *ospi,
 *                                              uint8_t level)
 * \brief       Set Tx DMA trigger level for the OSPI instance.
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   trig_level Tx DMA trigger level
 * \return      none
 */
static inline void ospi_set_rx_dma_data_level(struct ospi_regs *ospi,
					uint8_t level)
{
	ospi->OSPI_DMARDLR = level;
}

/**
 * \fn          void ospi_enable_tx_dma(struct ospi_regs *ospi)
 * \brief       Enable Tx DMA for the OSPI instance.
 * \param[in]   ospi       Pointer to the OSPI register map
 * \return      none
 */
static inline void ospi_enable_tx_dma(struct ospi_regs *ospi)
{
	ospi->OSPI_DMACR |= SPI_DMACR_TDMAE;
}

/**
 * \fn          void ospi_enable_rx_dma(struct ospi_regs *ospi)
 * \brief       Enable Tx DMA for the OSPI instance.
 * \param[in]   ospi       Pointer to the OSPI register map
 * \return      none
 */
static inline void ospi_enable_rx_dma(struct ospi_regs *ospi)
{
	ospi->OSPI_DMACR |= SPI_DMACR_RDMAE;
}

/**
 * \fn          void ospi_disable_tx_dma(struct ospi_regs *ospi)
 * \brief       Disable Tx DMA for the OSPI instance.
 * \param[in]   ospi       Pointer to the OSPI register map
 * \return      none
 */
static inline void ospi_disable_tx_dma(struct ospi_regs *ospi)
{
	ospi->OSPI_DMACR &= ~SPI_DMACR_TDMAE;
}

/**
 * \fn          void ospi_disable_rx_dma(struct ospi_regs *ospi)
 * \brief       Disable Tx DMA for the OSPI instance.
 * \param[in]   ospi       Pointer to the OSPI register map
 * \return      none
 */
static inline void ospi_disable_rx_dma(struct ospi_regs *ospi)
{
	ospi->OSPI_DMACR &= ~SPI_DMACR_RDMAE;
}

/**
 * \fn          bool ospi_busy(struct ospi_regs *ospi)
 * \brief       Get the busy status of the OSPI instance.
 * \param[in]   ospi       Pointer to the OSPI register map
 * \return      True/False based on the busy status
 */
static inline bool ospi_busy(struct ospi_regs *ospi)
{
	return (ospi->OSPI_SR &
	     (SPI_SR_BUSY | SPI_SR_TX_FIFO_EMPTY)) != SPI_SR_TX_FIFO_EMPTY;
}

/**
 * \fn          uint32_t ospi_get_dfs(struct ospi_regs *ospi)
 * \brief       Get the data frame size for the OSPI instance.
 * \param[in]   ospi     Pointer to the OSPI register map
 * \return      current dfs configuration
 */
static inline uint32_t ospi_get_dfs(struct ospi_regs *ospi)
{
	return (ospi->OSPI_CTRLR0 & SPI_CTRLR0_DFS_MASK) + 1;
}

/**
 * \fn          void ospi_set_mode(struct ospi_regs *ospi, enum spi_mode mode)
 * \brief       Set the OSPI mode for the OSPI instance.
 * \param[in]   ospi     Pointer to the OSPI register map
 * \param[in]   mode    The mode to be set.
 * \return      none
 */
void ospi_set_mode(struct ospi_regs *ospi, enum spi_mode mode);

/**
 * \fn          void ospi_set_dfs(struct ospi_regs *ospi, uint8_t dfs)
 * \brief       Set the data frame size for the OSPI instance.
 * \param[in]   ospi    Pointer to the SPI register map
 * \param[in]   dfs     The data frame size
 * \return      none
 */
void ospi_set_dfs(struct ospi_regs *ospi, uint8_t dfs);

/**
 * \fn          void ospi_set_tmode(struct ospi_regs *ospi, enum spi_tmode tmod)
 * \brief       Set the transfer mode for the OSPI instance.
 * \param[in]   ospi    Pointer to the OSPI register map
 * \param[in]   tmod    Transfer mode
 * \return      none
 */
void ospi_set_tmod(struct ospi_regs *ospi, enum spi_tmode tmod);

/**
 * \fn          void ospi_set_tx_threshold(struct ospi_regs *ospi,
 *                                         uint8_t threshold)
 * \brief       Set Transmit FIFO interrupt threshold for the OSPI instance
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   threshold  Transmit FIFO threshold
 * \return      none
 */
void ospi_set_tx_threshold(struct ospi_regs *ospi, uint8_t threshold);

/**
 * \fn          void ospi_set_rx_threshold(struct ospi_regs *ospi,
 *                                         uint8_t threshold)
 * \brief       Set Receive FIFO interrupt threshold for the OSPI instance
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   threshold  Receive FIFO threshold
 * \return      none
 */
static inline void ospi_set_rx_threshold(struct ospi_regs *ospi,
					uint8_t threshold)
{
	ospi->OSPI_RXFTLR = threshold;
}

/**
 * \fn          void ospi_set_tx_fifo_start_level(struct ospi_regs *spi,
 *                                                uint16_t level)
 * \brief       Set Transmit FIFO start level
 * \param[in]   ospi   Pointer to the OSPI register map
 * \param[in]   level  Transmit FIFO start level
 * \return      none
 */
void ospi_set_tx_fifo_start_level(struct ospi_regs *ospi, uint16_t level);

/**
 * \fn          void ospi_set_rx_sample_delay(struct ospi_regs *ospi,
 *                                            uint8_t rx_sample_delay)
 * \brief       Set Receive sample delay for the OSPI instance
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   threshold  Receive FIFO threshold
 * \return      none
 */
void ospi_set_rx_sample_delay(struct ospi_regs *ospi, uint8_t rx_sample_delay);

/**
 * \fn          void ospi_set_ddr_drive_edge(struct ospi_regs *ospi,
 *                                           uint8_t ddr_drive_edge)
 * \brief       Set DDR drive edge for the OSPI instance
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   threshold  Receive FIFO threshold
 * \return      none
 */
void ospi_set_ddr_drive_edge(struct ospi_regs *ospi, uint8_t ddr_drive_edge);

/**
 * \fn          void ospi_control_ss(struct ospi_regs *ospi, uint8_t slave,
 *                                   enum spi_ss_state state)
 * \brief       Control the slave select line
 * \param[in]   ospi   Pointer to the OSPI register map
 * \param[in]   slave  The slave to be selected
 * \param[in]   state  The state of the slave select line
 * \return      none
 */
void ospi_control_ss(struct ospi_regs *ospi, uint8_t slave,
			enum spi_ss_state state);

/**
 * \fn          void ospi_control_xip_ss(struct ospi_regs *ospi, uint8_t slave,
 *                                       enum spi_ss_state state)
 * \brief       Control the XIP slave select line
 * \param[in]   ospi   Pointer to the OSPI register map
 * \param[in]   slave  The slave to be selected
 * \param[in]   state  The state of the slave select line
 * \return      none
 */
void ospi_control_xip_ss(struct ospi_regs *ospi, uint8_t slave,
			enum spi_ss_state state);

/**
 * \fn          void ospi_send(struct ospi_regs *spi,
 *                             struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for transmission
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_send(struct ospi_regs *ospi, struct ospi_transfer *transfer);

/**
 * \fn          void ospi_receive(struct ospi_regs *ospi,
 *                                struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for reception
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_receive(struct ospi_regs *ospi, struct ospi_transfer *transfer);

/**
 * \fn          void ospi_transfer(struct ospi_regs *spi,
 *                                 struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for transfer
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_transfer(struct ospi_regs *ospi, struct ospi_transfer *transfer);

/**
 * \fn          void ospi_dma_send(struct ospi_regs *spi,
 *                                 struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for transmission with DMA support
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_dma_send(struct ospi_regs *ospi, struct ospi_transfer *transfer);

/**
 * \fn          void ospi_dma_transfer(struct ospi_regs *spi,
 *                                     struct ospi_transfer *transfer)
 * \brief       Prepare the OSPI instance for transfer with DMA support
 * \param[in]   ospi       Pointer to the OSPI register map
 * \param[in]   transfer   Transfer parameters
 * \return      none
 */
void ospi_dma_transfer(struct ospi_regs *ospi, struct ospi_transfer *transfer);

/**
  \fn          void ospi_hyperbus_xip_init(struct ospi_regs *ospi, uint8_t wait_cycles,
						bool is_dual_octal)
  \brief       Initialize hyperbus XIP configuration for the OSPI instance
  \param[in]   ospi        Pointer to the OSPI register map
  \param[in]   wait_cycles Wait cycles needed by the hyperbus device
  \param[in]   is_dual_octal OSPI transfer type is Dual Octal
  \return      none
*/
void ospi_hyperbus_xip_init(struct ospi_regs *ospi, uint8_t wait_cycles, bool is_dual_octal);

/**
  \fn          void ospi_hyperbus_send(struct ospi_regs *spi, ospi_transfer *transfer)
  \brief       Prepare the OSPI instance for transmission
  \param[in]   ospi       Pointer to the OSPI register map
  \param[in]   transfer   Transfer parameters
  \return      none
*/
void ospi_hyperbus_send(struct ospi_regs *ospi, struct ospi_transfer *transfer);


/**
 * \fn          void ospi_irq_handler(struct ospi_regs *ospi,
 *                                    struct ospi_transfer *transfer)
 * \brief       Handle interrupts for the OSPI instance.
 * \param[in]   ospi      Pointer to the OSPI register map
 * \param[in]   transfer  The transfer structure for the SPI instance
 * \return      none
 */
void ospi_irq_handler(struct ospi_regs *ospi, struct ospi_transfer *transfer);


/**
 * \fn          void ospi_xip_enable(struct ospi_regs *ospi,
 *                                    struct ospi_aes_regs *aes,
 *                                    struct ospi_xip_config *xfg)
 * \brief       Enable XiP Mode
 * \param[in]   ospi Pointer to the OSPI register map
 * \param[in]   aes  Pointer to the AES register map
 * \param[in]   xfg  configurations for XiP mode
 * \return      none
 */
void ospi_xip_enable(struct ospi_regs *ospi, struct ospi_aes_regs *aes,
		struct ospi_xip_config *xip_configs);

/**
 * \fn          void ospi_xip_disable(struct ospi_regs *ospi,
 *                            struct ospi_aes_regs *aes,
 *                            struct ospi_transfer *transfer,
 *                            struct ospi_xip_config *xfg)
 * \brief       Disable XiP Mode
 * \param[in]   ospi Pointer to the OSPI register map
 * \param[in]   aes  Pointer to the AES register map
 * \param[in]   transfer  The transfer structure for the OSPI instance
 * \param[in]   xfg  configurations for XiP mode
 * \return      none
 */
void ospi_xip_disable(struct ospi_regs *ospi, struct ospi_aes_regs *aes,
		struct ospi_transfer *transfer, struct ospi_xip_config *xfg);

#ifdef __cplusplus
}
#endif

#endif /* _ALIF_HAL_OSPI_H_ */
