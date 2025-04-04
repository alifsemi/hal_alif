/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2025 Alif Semiconductor.
 */

#ifndef USBD_DWC3_H
#define USBD_DWC3_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/cache.h>
#include "soc_memory_map.h"

#ifdef __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {

#endif

/* Define the macros    */

#define USB_SUCCESS                                    0
#define USB_INIT_ERROR                                -1
#define USB_CORE_INVALID                              -2
#define USB_MODE_MISMATCH                             -3
#define USB_CORE_SFTRST_TIMEOUT_ERROR                 -4
#define USB_CONTROLLER_INIT_FAILED                    -5
#define USB_EP_DIRECTION_WRONG                        -6
#define USB_EP_BUSY_ERROR                             -7
#define USB_EP_BUFF_LENGTH_INVALID                    -8
#define USB_EP_CMD_CMPLT_ERROR                        -9
#define USB_EP_CMD_CMPLT_BUS_EXPIRY_ERROR             -10
#define USB_EP_CMD_CMPLT_NO_RESOURCE_ERROR            -11
#define USB_EP_CMD_CMPLT_STATUS_UNKNOWN               -12
#define USB_EP_CMD_CMPLT_TIMEOUT_ERROR                -13
#define USB_EP_INVALID                                -14
#define USB_LINKSTATE_INVALID                         -15
#define USB_LINKSTATE_RECOVERY_FAILED                 -16
#define USB_REMOTE_WAKEUP_FAILED                      -17
#define USB_LINKSTATE_TIMEOUT_ERROR                   -18
#define USB_LINKSTATE_SET_FAILED                      -19

#define USB_DEVICE_DEFAULT_ADDRESS                     0
#define USB_EVNT_BUFF_ALLOC_ERROR                      100
#define USB_DGCMD_CMPLT_ERROR                          101
#define USB_DGCMD_TIMEOUT_ERROR                        102
#define USB_EP_ENABLE_ERROR                            103
#define USB_EP_RESOURCE_INDEX_INVALID                  105
#define USB_DEVICE_SET_ADDRESS_INVALID                 106
#define USB_DEVICE_ALREADY_CONFIGURED                  107




#define ALIGNMENT_CACHELINE                          __aligned(32)

#define IS_ALIGNED(x, a)                              (((x) & ((uint32_t)(a) - 1U)) == 0U)

#define roundup(x, y)             (((((x) + (uint32_t)(y - 1U)) / \
		(uint32_t)y) * (uint32_t)y))

#define upper_32_bits(n)          ((uint32_t)(((uint32_t)(n) >> 16) \
		>> 16))
#define lower_32_bits(n)                      ((uint32_t)(n))

#define USB_NUM_TRBS                                  8U
#define NO_OF_TRB_PER_EP                              8U
#define USB_ENDPOINTS_NUM                             8U

#define USB_REQUEST_IN                                0x80u
#define USB_SET_ADDRESS_REQ                           0x05
#define USB_DIR_IN                                    1U
#define USB_DIR_OUT                                   0U
#define USB_EVENT_BUFFER_SIZE                         4096
#define USB_SCRATCHPAD_BUF_SIZE                       4096
#define USB_SETUP_PKT_SIZE                            8

#define USB_DCTL_START_TIMEOUT                        500
#define USB_GENERIC_CMD_TIMEOUT                       500
#define USB_DEPCMD_TIMEOUT                            1000
#define USB_DCTL_CSFTRST_TIMEOUT                      1000
#define USB_TRANSFER_WAKEUP_RETRY                     20000
#define USB_LINK_STATE_RETRY                          10000


#define USB_BULK_EP_MAX_PKT                           512
#define USB_CONTROL_EP_MAX_PKT                        64
#define USB_ISOC_EP_MAX_PKT                           1024

#define USB_CONTROL_EP                                0
#define USB_ISOCRONOUS_EP                             1
#define USB_BULK_EP                                   2
#define USB_INTERRUPT_EP                              3

#define USB_TRB_STRUCTURE_SIZE                       16
#define USB_TRBS_PER_EP                               9
/* USB devices address can be assigned addresses from 1 to 127 */
#define USB_DEVICE_MAX_ADDRESS                        127

#define USB_EP_ENABLED                                (0x00000001U << 0U)
#define USB_EP_STALL                                  (0x00000001U << 1U)
#define USB_EP_WEDGE                                  (0x00000001U << 2U)
#define USB_EP_BUSY                                   (0x00000001U << 4U)
#define USB_EP_PENDING_REQUEST                        (0x00000001U << 5U)
#define USB_EP_MISSED_ISOC                            (0x00000001U << 6U)

/* DEPXFERCFG parameter 0 */
#define USB_DEPXFERCFG_NUM_XFER_RES(n)                (n & 0xFFFFU)
/* The EP number goes 0..31 so ep0 is always out and ep1 is always in */
#define USB_DALEPENA_EP(n)                            (0x00000001U << (n))
#define USB_EVT_EPSTATUS_MASK                         12U

/* Define USB DWC3 endpoint transfer status definition.  */

#define USB_ED_TRANSFER_STATUS_IDLE                      0
#define USB_ED_TRANSFER_STATUS_SETUP                     1
#define USB_ED_TRANSFER_STATUS_IN_COMPLETION             2
#define USB_ED_TRANSFER_STATUS_OUT_COMPLETION            3

#define USB_DEPEVT_XFERCOMPLETE                          0x01
#define USB_DEPEVT_XFERINPROGRESS                        0x02
#define USB_DEPEVT_XFERNOTREADY                          0x03
#define USB_DEPEVT_RXTXFIFOEVT                           0x04
#define USB_DEPEVT_STREAMEVT                             0x06
#define USB_DEPEVT_EPCMDCMPLT                            0x07

/* Control-only Status */
#define USB_DEPEVT_STATUS_CONTROL_DATA                   1U
#define USB_DEPEVT_STATUS_CONTROL_STATUS                 2U
#define USB_DEPEVT_STATUS_CONTROL_DATA_INVALTRB          9U
#define USB_DEPEVT_STATUS_CONTROL_STATUS_INVALTRB        0xAU


/*
 * return Physical EP number as dwc3 mapping
 */
#define USB_PhysicalEp(epnum, direction)                 (((epnum) << 1U) | (direction))

#define USB_DEPCMD_CMD(x)                                ((x) & 0xf)

/* Device endpoint specific events */
#define USB_GET_DEPEVT_TYPE(reg)                         ((reg & 0x3C0) >> 6)
/* Device endpoint num  */
#define USB_GET_DEPEVT_EP_NUM(reg)                       ((reg & 0x3E) >> 1)
/* Device specific events */
#define USB_DEVT_TYPE(reg)                               ((reg & 0x1f00) >> 8)
/* device Link state change event info  */
#define USB_DEVT_LINK_STATE_INFO(reg)                    ((reg & 0xf0000) >> 16)
/* Device EP event status   */
#define USB_GET_EP_EVENT_STATUS(reg)                     ((reg >> USB_EVT_EPSTATUS_MASK) & 0xf)

/* In response to Start Transfer */
#define USB_DEPEVT_TRANSFER_NO_RESOURCE                  1
#define USB_DEPEVT_TRANSFER_BUS_EXPIRY                   2
#define USB_DEPEVT_CMD_SUCCESS                           0

/* Device Configuration Register */
#define USB_DCFG_DEVADDR(addr)                           ((addr) << 3)
#define USB_DCFG_DEVADDR_MASK                            USB_DCFG_DEVADDR(0x7f)
#define USB_DCFG_SPEED_MASK                              7U
#define USB_DCFG_HIGHSPEED                               0U
#define USB_DCFG_LOWSPEED                                1U
#define USB_DCFG_NUMP_SHIFT                              17
#define USB_DCFG_NUMP(n)                                 (((n) >> USB_DCFG_NUMP_SHIFT) & 0x1f)
#define USB_DCFG_NUMP_MASK                               (0x1f << USB_DCFG_NUMP_SHIFT)
#define USB_DCFG_LPM_CAP                                 BIT(22)


/* Device Generic Command Register */
#define USB_DGCMD_SET_LMP                                0x01
#define USB_DGCMD_SET_PERIODIC_PAR                       0x02
#define USB_DGCMD_XMIT_FUNCTION                          0x03
#define USB_DGCMD_CMDACT                                 BIT(10)
#define USB_DGCMD_STATUS(n)                              (((uint32_t)(n) >> 12U) & 0x0fU)
#define USB_DGCMD_CMDIOC                                 BIT(8)
#define USB_DGCMD_SET_SCRATCHPAD_ADDR_LO                 0x04
#define USB_DGCMD_SET_SCRATCHPAD_ADDR_HI                 0x05


/* Global HWPARAMS0 Register */
#define USB_GHWPARAMS0_MODE(n)                           ((n) & 0x3)
#define USB_GHWPARAMS0_MODE_DEVICE                       0
#define USB_GHWPARAMS0_MODE_HOST                         1
#define USB_GHWPARAMS0_MODE_DRD                          2
#define USB_GHWPARAMS0_MBUS_TYPE(n)                      (((n) >> 3) & 0x7)
#define USB_GHWPARAMS0_SBUS_TYPE(n)                      (((n) >> 6) & 0x3)
#define USB_GHWPARAMS0_MDWIDTH(n)                        (((n) >> 8) & 0xff)
#define USB_GHWPARAMS0_SDWIDTH(n)                        (((n) >> 16) & 0xff)
#define USB_GHWPARAMS0_AWIDTH(n)                         (((n) >> 24) & 0xff)

/* Global USB2 PHY Configuration Register */
#define USB_GUSB2PHYCFG_PHYSOFTRST                       BIT(31)
#define USB_GUSB2PHYCFG_U2_FREECLK_EXISTS                BIT(30)
#define USB_GUSB2PHYCFG_SUSPHY                           BIT(6)
#define USB_GUSB2PHYCFG_ULPI_UTMI                        BIT(4)
#define USB_GUSB2PHYCFG_ENBLSLPM                         BIT(8)
#define USB_GUSB2PHYCFG_PHYIF(n)                         (n << 3)
#define USB_GUSB2PHYCFG_PHYIF_MASK                       USB_GUSB2PHYCFG_PHYIF(1)
#define USB_GUSB2PHYCFG_USBTRDTIM(n)                     (n << 10)
#define USB_GUSB2PHYCFG_USBTRDTIM_MASK                   USB_GUSB2PHYCFG_USBTRDTIM(0xf)
#define USBTRDTIM_UTMI_8_BIT                             9
#define USBTRDTIM_UTMI_16_BIT                            5
#define UTMI_PHYIF_16_BIT                                1
#define UTMI_PHYIF_8_BIT                                 0
#define USB_GUSB2PHYCFG_ULPIAUTORES                      1U << 15

/* Global HWPARAMS3 Register */

#define USB_GHWPARAMS3_HSPHY_IFC(n)                      (((n) & (3 << 2)) >> 2)
#define USB_GHWPARAMS3_HSPHY_IFC_DIS                     0
#define USB_GHWPARAMS3_HSPHY_IFC_UTMI                    1
#define USB_GHWPARAMS3_HSPHY_IFC_ULPI                    2
#define USB_GHWPARAMS3_HSPHY_IFC_UTMI_ULPI               3
#define USB_GHWPARAMS3_FSPHY_IFC(n)                      (((n) & (3 << 4)) >> 4)
#define USB_GHWPARAMS3_FSPHY_IFC_DIS                     0
#define USB_GHWPARAMS3_FSPHY_IFC_ENA                     1

#define USB_NUM_IN_EPS_MASK                              (0x1f << 18)
#define USB_NUM_EPS_MASK                                 (0x3f << 12)
#define USB_NUM_EPS(ep_num)                              ((ep_num & \
		(USB_NUM_EPS_MASK)) >> 12)
#define USB_IN_EPS(p)                                    ((p & \
		(USB_NUM_IN_EPS_MASK)) >> 18)


/* TRB Control */
#define USB_TRB_CTRL_HWO                                 (1U << 0U)
#define USB_TRB_CTRL_LST                                 (0x00000001U << 1U)
#define USB_TRB_CTRL_CHN                                 (0x00000001U << 2U)
#define USB_TRB_CTRL_CSP                                 (0x00000001U << 3U)
#define USB_TRB_CTRL_TRBCTL(n)                           (((n) & 0x3FU) << 4U)
#define USB_TRB_CTRL_ISP_IMI                             0x00000400U /* bit 10 */
#define USB_TRB_CTRL_IOC                                 0x00000800U /* bit 11 */
#define USB_TRB_CTRL_SID_SOFN(n)                         (((n) & (u32)0xFFFFU) << 14U)

#define USB_TRBCTL_TYPE(n)                               ((n) & (0x3f << 4))
#define USB_TRBCTL_NORMAL                                USB_TRB_CTRL_TRBCTL(1)
#define USB_TRBCTL_CONTROL_SETUP                         USB_TRB_CTRL_TRBCTL(2)
#define USB_TRBCTL_CONTROL_STATUS2                       USB_TRB_CTRL_TRBCTL(3)
#define USB_TRBCTL_CONTROL_STATUS3                       USB_TRB_CTRL_TRBCTL(4)
#define USB_TRBCTL_CONTROL_DATA                          USB_TRB_CTRL_TRBCTL(5)
#define USB_TRBCTL_ISOCHRONOUS_FIRST                     USB_TRB_CTRL_TRBCTL(6)
#define USB_TRBCTL_ISOCHRONOUS                           USB_TRB_CTRL_TRBCTL(7)
#define USB_TRBCTL_LINK_TRB                              USB_TRB_CTRL_TRBCTL(8)
#define USB_TRBCTL_NORMAL_ZLP                            USB_TRB_CTRL_TRBCTL(9)

/* TRB Length, PCM and Status */

#define USB_TRB_SIZE_MASK                                (0x00ffffff)
#define USB_TRB_SIZE_LENGTH(n)                           ((n) & USB_TRB_SIZE_MASK)
#define USB_TRB_SIZE_PCM1(n)                             (((n) & 0x03) << 24)
#define USB_TRB_SIZE_TRBSTS(n)                           (((n) & (0x0f << 28)) >> 28)

#define USB_TRBSTS_OK                                    0
#define USB_TRBSTS_MISSED_ISOC                           1
#define USB_TRBSTS_SETUP_PENDING                         2
#define USB_TRB_STS_XFER_IN_PROG                         4


/* Device specific commmands, DEPCMD register is used */
#define USB_DEPCMD_DEPSTARTCFG                           (0x09 << 0)
#define USB_DEPCMD_ENDTRANSFER                           (0x08 << 0)
#define USB_DEPCMD_UPDATETRANSFER                        (0x07 << 0)
#define USB_DEPCMD_STARTTRANSFER                         (0x06 << 0)
#define USB_DEPCMD_CLEARSTALL                            (0x05 << 0)
#define USB_DEPCMD_SETSTALL                              (0x04 << 0)
#define USB_DEPCMD_GETEPSTATE                            (0x03 << 0)
#define USB_DEPCMD_SETTRANSFRESOURCE                     (0x02 << 0)
#define USB_DEPCMD_SETEPCONFIG                           (0x01 << 0)

#define USB_DEPCMD_CLEARPENDIN                           BIT(11)
#define USB_DEPCMD_CMDACT                                BIT(10)
#define USB_DEPCMD_CMDIOC                                BIT(8)

/* Define DWC3 USB device control endpoint ep0-out command configuration config values */
#define USB_DEPCMD_EP0_OUT_PAR1                          0x00000500
#define USB_DEPCMD_EP0_OUT_PAR0                          0x00000200

/* Define DWC3 USB device control endpoint ep1-in command configuration config values */

#define USB_DEPCMD_EP1_IN_PAR1                           0x06000500
#define USB_DEPCMD_EP1_IN_PAR0                           0x00000200

/* Define DWC3 USB device endpoint command config for ep/0/1/2/3/in/out */
#define USB_DEPCMD_EP_XFERCFG_PAR0                       0x00000001
#define USB_DEPCMD_EP_XFERCFG                            0x00000402

#define USB_DEPCMD_EP0_XFERCFG_PAR1                      0x02041000
#define USB_DEPCMD_EP0_XFERCFG_PAR0                      0x00000000
#define USB_DEPCMD_XFERCFG_EP0                           0x00000506

/* Device Endpoint Command Register */
#define USB_DEPCMD_PARAM_SHIFT                           16
#define USB_DEPCMD_PARAM(x)                              ((x) << USB_DEPCMD_PARAM_SHIFT)
#define USB_DEPCMD_GET_RSC_IDX(x)                        (((x) >> USB_DEPCMD_PARAM_SHIFT) & 0x7f)
#define USB_DEPCMD_STATUS(x)                             (((x) >> 12) & 0x0F)
#define USB_DEPCMD_HIPRI_FORCERM                         BIT(11)


/* DEPCFG parameter 0 */
#define USB_DEPCFG_EP_TYPE(n)                            ((n) << 1U)
#define USB_DEPCFG_MAX_PACKET_SIZE(n)                    ((n) << 3U)
#define USB_DEPCFG_FIFO_NUMBER(n)                        (((n) & 0x1f) << 17)
#define USB_DEPCFG_BURST_SIZE(n)                         ((n) << 22U)
#define USB_DEPCFG_DATA_SEQ_NUM(n)                       ((n) << 26U)

/* bit 30 and 31 is Config action of endpoint
 * value 0 for Initialize endpoint state
 * value 1 for Restore endpoint state
 * value 2 for Modify endpoint state
 */
#define USB_DEPCFG_ACTION_INIT                           (0 << 30)
#define USB_DEPCFG_ACTION_RESTORE                        (1 << 30)
#define USB_DEPCFG_ACTION_MODIFY                         (2 << 30)

#define USB_DEPCFG_INT_NUM(n)                            ((n) << 0U)
#define USB_DEPCFG_XFER_COMPLETE_EN                      0x00000100U /* bit 8 */
#define USB_DEPCFG_XFER_IN_PROGRESS_EN                   0x00000200U /* bit 9 */
#define USB_DEPCFG_XFER_NOT_READY_EN                     0x00000400U /* bit 10 */
#define USB_DEPCFG_FIFO_ERROR_EN                         0x00000800U /* bit 11 */
#define USB_DEPCFG_STREAM_EVENT_EN                       0x00002000U /* bit 13 */
#define USB_DEPCFG_BINTERVAL_M1(n)                       (((n) & 0xFF) << 16U)
#define USB_DEPCFG_STREAM_CAPABLE                        0x01000000U /* bit 24 */
#define USB_DEPCFG_EP_NUMBER(n)                          ((n) << 25U)
#define USB_DEPCFG_BULK_BASED                            0x40000000U /* bit 30 */
#define USB_DEPCFG_FIFO_BASED                            0x80000000U /* bit 31 */


/* Define DWC3 USB device event configuration config values */

/* USB device Disconnect event   */
#define USB_DEV_DISSCONNEVTEN                            1U << 0
/* USB device reset event   */
#define USB_DEV_USBRSTEVTEN                              1U << 1
/* USB device connection done event */
#define USB_DEV_CONNECTDONEEVTEN                         1U << 2
/* USB device link state change event  */
#define USB_DEV_EVENT_ULSTCNGEN                          1U << 3

/* Dwc3 device status register      */
#define USB_DSTS_DWC3_USBLNKST_MASK                      (0x0f << 18)
#define USB_DSTS_DWC3_USBLNKST(n)                        ((n & USB_DSTS_DWC3_USBLNKST_MASK) >> 18)
#define USB_DSTS_DEVCTRLHLT                              BIT(22)
#define USB_DSTS_DWC3_DCNRD                              BIT(29)

#define USB_DSTS_CONNECTSPD                              (7 << 0)
#define USB_DSTS_HIGHSPEED                               (0 << 0)

/* Device control register */
#define USB_DCTL_CSFTRST                                 1 << 30
#define USB_DCTL_START                                   1U << 31
#define USB_DCTL_KEEP_CONNECT                            BIT(19) /* bit 19 */
#define USB_DCTL_INITU2ENA                               BIT(12)
#define USB_DCTL_ACCEPTU2ENA                             BIT(11)
#define USB_DCTL_INITU1ENA                               BIT(10)
#define USB_DCTL_ACCEPTU1ENA                             BIT(9)
#define USB_DCTL_TSTCTRL_MASK                            (0xf << 1)
#define USB_DCTL_ULSTCHNGREQ_MASK                        (0x0f << 5)
#define USB_DCTL_ULSTCHNGREQ(n)                          (((n) << 5) & USB_DCTL_ULSTCHNGREQ_MASK)

#define USB_DCTL_L1_HIBER_EN                             0x00040000U /* bit 18 */
#define USB_DCTL_HIRD_THRES_MASK                         (0x1f << 24)
#define USB_DCTL_HIRD_THRES(n)                           ((n) << 24)

/* Global Event Size Registers */
#define USB_GEVNTSIZ_INTMASK                             BIT(31)
#define USB_GEVNTSIZ_SIZE(n)                             ((n) & 0xffff)
#define USB_GEVNTCOUNT_MASK                              0xfffc

#define USB_EP_EVENT_TYPE                                0
#define USB_DEV_EVENT_TYPE                               0x1
#define USB_EVENT_TYPE_CHECK                             0x1

/* Device specific events */
#define USB_EVENT_DISCONNECT                             0
#define USB_EVENT_RESET                                  1
#define USB_EVENT_CONNECT_DONE                           2
#define USB_EVENT_LINK_STATUS_CHANGE                     3
#define USB_EVENT_WAKEUP                                 4
#define USB_EVENT_HIBER_REQ                              5
#define USB_EVENT_EOPF                                   6
#define USB_EVENT_SOF                                    7
#define USB_EVENT_ERRATIC_ERROR                          9
#define USB_EVENT_CMD_CMPL                               10
#define USB_EVENT_OVERFLOW                               11

/* dwc3 global control register bits   */
#define USB_GCTL_CORESOFTRESET                           BIT(11)
#define USB_GCTL_SOFITPSYNC                              0x00000400U /* bit 10 */
#define USB_GCTL_SCALEDOWN(n)                            ((n) << 4U)
#define USB_GCTL_SCALEDOWN_MASK                          USB_GCTL_SCALEDOWN(3U)
#define USB_GCTL_DISSCRAMBLE                             (1U << 3U)
#define USB_GCTL_U2EXIT_LFPS                             (1U << 2U)
#define USB_GCTL_GBLHIBERNATIONEN                        (1U << 1U)
#define USB_GCTL_DSBLCLKGTNG                             (1U << 0U)
#define USB_GCTL_PRTCAP(n)                               (((n) & (3 << 12)) >> 12)
#define USB_GCTL_PRTCAPDIR(n)                            ((n) << 12)
#define USB_GCTL_PRTCAP_HOST                             1
#define USB_GCTL_PRTCAP_DEVICE                           2
#define USB_GCTL_PRTCAP_OTG                              3

/* Define DWC3 USB device controller global soc bus config values */

#define USB_GSBUSCFG0_INCR256BRSTENA                    (1 << 7) /* INCR256 burst */
#define USB_GSBUSCFG0_INCR128BRSTENA                    (1 << 6) /* INCR128 burst */
#define USB_GSBUSCFG0_INCR64BRSTENA                     (1 << 5) /* INCR64 burst */
#define USB_GSBUSCFG0_INCR32BRSTENA                     (1 << 4) /* INCR32 burst */
#define USB_GSBUSCFG0_INCR16BRSTENA                     (1 << 3) /* INCR16 burst */
#define USB_GSBUSCFG0_INCR8BRSTENA                      (1 << 2) /* INCR8 burst */
#define USB_GSBUSCFG0_INCR4BRSTENA                      (1 << 1) /* INCR4 burst */
#define USB_GSBUSCFG0_INCRBRSTENA                       (1 << 0) /* undefined length enable */
#define USB_GSBUSCFG0_INCRBRST_MASK                     0xff

/* Global Frame Length Adjustment Register */
#define USB_GFLADJ_30MHZ_SDBND_SEL                       (1 << 7)
#define USB_GFLADJ_30MHZ_MASK                            0x3f
#define USB_GFLADJ_DEFAULT_VALUE                         0x20
/* Global User Control Register */
#define USB_GUCTL_HSTINAUTORETRY                         (1 << 14)

/* Global Synopsys ID Register  */
/* bit [31:16] for Core Identification Number */
#define USB_GSNPSID_MASK                                 0xffff0000
/* bit [15:0] for Core release number */
#define USB_GSNPSREV_MASK                                0xffff


typedef enum USBD_LINK_STATE {

	USB_LINK_STATE_ON              = 0x00, /* in HS, means ON */
	USB_LINK_STATE_L1              = 0x02, /* in HS, means SLEEP */
	USB_LINK_STATE_L2              = 0x03, /* in HS, means SUSPEND */
	USB_LINK_STATE_DIS             = 0x04, /* Disconnected state (Default state) */
	USB_LINK_STATE_EARLY_SUS       = 0x05, /* in HS, means Early Suspend */
	USB_LINK_STATE_RECOV           = 0x08,  /* Recover */
	USB_LINK_STATE_RESET           = 0x0e,
	USB_LINK_STATE_RESUME          = 0x0f,
} USBD_LINK_STATE;


/*
 * Endpoint Parameters
 */
typedef struct USBD_EP_PARAMS_STRUCT {
	uint32_t     Param2;         /**< Parameter 2 */
	uint32_t     Param1;         /**< Parameter 1 */
	uint32_t     Param0;         /**< Parameter 0 */
} USBD_EP_PARAMS;

typedef struct USBD_TRB_STRUCT {
	uint32_t             BufferPtrLow;
	uint32_t             BufferPtrHigh;
	uint32_t             Size;
	uint32_t             Ctrl;
} USBD_TRB;

/* Define USB DWC3 physical endpoint structure.  */
typedef struct USBD_EP_STRUCT {
	USBD_TRB EpTrb[NO_OF_TRB_PER_EP + 1]ALIGNMENT_CACHELINE;
	uint32_t  ep_status;
	uint8_t   ep_index;
	uint32_t  ep_transfer_status;
	uint8_t   ep_dir;
	uint32_t  ep_maxpacket;
	uint32_t  ep_resource_index;
	uint32_t  ep_requested_bytes;
	uint32_t  trb_enqueue;
	uint32_t  trb_dequeue;
	uint8_t   phy_ep;
	uint32_t  BytesTxed;
	uint32_t  UnalignedTx;
} USBD_EP;


typedef enum USB_DEVICE_STATE {
	USB_DWC3_STATE_NOTATTACHED = 0,

	/* chapter 9 and authentication (wireless) device states */
	USB_DWC3_STATE_ATTACHED,
	USB_DWC3_STATE_POWERED,                      /* wired */
	USB_DWC3_STATE_RECONNECTING,                 /* auth */
	USB_DWC3_STATE_UNAUTHENTICATED,              /* auth */
	USB_DWC3_STATE_DEFAULT,                      /* limited function */
	USB_DWC3_STATE_ADDRESS,
	USBD_DWC3_STATE_CONFIGURED,                   /* most functions */
	USB_DWC3_STATE_SUSPENDED

	/* NOTE:  there are actually four different SUSPENDED
	 * states, returning to POWERED, DEFAULT, ADDRESS, or
	 * CONFIGURED respectively when SOF tokens flow again.
	 * At this level there's no difference between L1 and L2
	 * suspend states.  (L2 being original USB 1.1 suspend.)
	 */
} USB_DEVICE_STATE;

typedef enum USBD_EP0_STATE {
	EP0_UNCONNECTED  = 0,
	EP0_SETUP_PHASE,
	EP0_DATA_PHASE,
	EP0_STATUS_PHASE,
} USBD_EP0_STATE;


typedef enum USB_DR_MODE {
	USB_DR_MODE_UNKNOWN,
	USB_DR_MODE_HOST,
	USB_DR_MODE_PERIPHERAL,
	USB_DR_MODE_OTG,
} USB_DR_MODE;

typedef enum _DWC3_PHY_INTERFACE {
	USB_PHY_INTERFACE_MODE_UNKNOWN,
	USB_PHY_INTERFACE_MODE_UTMI,
	USB_PHY_INTERFACE_MODE_UTMIW,
	USB_PHY_INTERFACE_MODE_ULPI,
	USB_PHY_INTERFACE_MODE_SERIAL,
	USB_PHY_INTERFACE_MODE_HSIC,
} DWC3_PHY_INTERFACE;

typedef struct USB_CTRL_REQUEST_STRUCT {
	uint8_t bRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} USBD_CTRL_REQUEST;

/* Define USB Event structure definition. */

typedef struct USBD_EVENT_BUFFER_STRUCT {
	void    *buf;
	uint32_t   length;
	uint32_t   lpos;
	uint32_t   count;
} USBD_EVENT_BUFFER;


/**
 * @brief USB_USB_ENDPNT_CMD [USB_ENDPNT_CMD] ([0..7])
 */
typedef struct {
	volatile uint32_t  DEPCMDPAR2;
	volatile uint32_t  DEPCMDPAR1;
	volatile uint32_t  DEPCMDPAR0;
	volatile uint32_t  DEPCMD;
} USB_USB_ENDPNT_CMD_Type;

/**
 * @brief USB (USB)
 */

typedef struct {     /*!< (@ 0x48200000) USB Structure */
	volatile const  uint32_t  CAPLENGTH;
	volatile const  uint32_t  HCSPARAMS1;
	volatile const  uint32_t  HCSPARAMS2;
	volatile const  uint32_t  HCSPARAMS3;
	volatile const  uint32_t  HCCPARAMS1;
	volatile const  uint32_t  DBOFF;
	volatile const  uint32_t  RTSOFF;
	volatile const  uint32_t  HCCPARAMS2;
	volatile const  uint32_t  RESERVED[12344];
	volatile uint32_t  GSBUSCFG0;
	volatile uint32_t  GSBUSCFG1;
	volatile const  uint32_t  RESERVED1[2];
	volatile uint32_t  GCTL;
	volatile const  uint32_t  RESERVED2;
	volatile uint32_t  GSTS;
	volatile uint32_t  GUCTL1;
	volatile const  uint32_t  GSNPSID;
	volatile const  uint32_t  RESERVED3;
	volatile uint32_t  GUID;
	volatile uint32_t  GUCTL;
	volatile const  uint32_t  GBUSERRADDRLO;
	volatile const  uint32_t  GBUSERRADDRHI;
	volatile const  uint32_t  RESERVED4[2];
	volatile const  uint32_t  GHWPARAMS0;
	volatile const  uint32_t  GHWPARAMS1;
	volatile const  uint32_t  GHWPARAMS2;
	volatile const  uint32_t  GHWPARAMS3;
	volatile const  uint32_t  GHWPARAMS4;
	volatile const  uint32_t  GHWPARAMS5;
	volatile const  uint32_t  GHWPARAMS6;
	volatile const  uint32_t  GHWPARAMS7;
	volatile const  uint32_t  RESERVED5[8];
	volatile uint32_t  GPRTBIMAP_HSLO;
	volatile uint32_t  GPRTBIMAP_HSHI;
	volatile uint32_t  GPRTBIMAP_FSLO;
	volatile uint32_t  GPRTBIMAP_FSHI;
	volatile const  uint32_t  RESERVED6[3];
	volatile uint32_t  GUCTL2;
	volatile const  uint32_t  RESERVED7[24];
	volatile uint32_t  GUSB2PHYCFG0;
	volatile const  uint32_t  RESERVED8[63];
	volatile uint32_t  GTXFIFOSIZ[4];
	volatile const  uint32_t  RESERVED9[28];
	volatile uint32_t  GRXFIFOSIZ[4];
	volatile const  uint32_t  RESERVED10[28];
	volatile uint32_t  GEVNTADRLO0;
	volatile uint32_t  GEVNTADRHI0;
	volatile uint32_t  GEVNTSIZ0;
	volatile uint32_t  GEVNTCOUNT0;
	volatile const  uint32_t  RESERVED11[124];
	volatile const  uint32_t  GHWPARAMS8;
	volatile const  uint32_t  RESERVED12[3];
	volatile uint32_t  GTXFIFOPRIDEV;
	volatile const  uint32_t  RESERVED13;
	volatile uint32_t  GTXFIFOPRIHST;
	volatile uint32_t  GRXFIFOPRIHST;
	volatile const  uint32_t  RESERVED14[4];
	volatile uint32_t  GFLADJ;
	volatile const  uint32_t  RESERVED15[3];
	volatile uint32_t  GUSB2RHBCTL0;
	volatile const  uint32_t  RESERVED16[47];
	volatile uint32_t  DCFG;
	volatile uint32_t  DCTL;
	volatile uint32_t  DEVTEN;
	volatile uint32_t  DSTS;
	volatile uint32_t  DGCMDPAR;
	volatile uint32_t  DGCMD;
	volatile const  uint32_t  RESERVED17[2];
	volatile uint32_t  DALEPENA;
	volatile const  uint32_t  RESERVED18[55];
	volatile USB_USB_ENDPNT_CMD_Type USB_ENDPNT_CMD[8];
	volatile const  uint32_t  RESERVED19[96];
	volatile uint32_t  DEV_IMOD0;
} USB_Type;  /*!< Size = 51716 (0xca04)  */

/*
 * dwc3_hwparams - copy of HWPARAMS registers
 */
typedef struct USB_HWPARAMS_STRUCT {
	uint32_t  hwparams0;
	uint32_t  hwparams1;
	uint32_t  hwparams2;
	uint32_t  hwparams3;
	uint32_t  hwparams4;
	uint32_t  hwparams5;
	uint32_t  hwparams6;
	uint32_t  hwparams7;
	uint32_t  hwparams8;
} USB_HWPARAMS;

typedef struct USB_DRIVER_STRUCT {
	USB_Type *regs;
	void (*usbd_dwc3_device_reset_cb)(struct USB_DRIVER_STRUCT *drv);
	void (*usbd_dwc3_connect_cb)(struct USB_DRIVER_STRUCT *drv);
	void (*usbd_dwc3_disconnect_cb)(struct USB_DRIVER_STRUCT *drv);
	void (*usbd_dwc3_setupstage_cb)(struct USB_DRIVER_STRUCT *drv);
	void (*usbd_dwc3_data_in_cb)(struct USB_DRIVER_STRUCT *drv, uint8_t ep_num);
	void (*usbd_dwc3_data_out_cb)(struct USB_DRIVER_STRUCT *drv, uint8_t ep_num);

	USBD_CTRL_REQUEST         SetupData ALIGNMENT_CACHELINE;
	USBD_TRB     endp0_trb ALIGNMENT_CACHELINE;
	USBD_EP    eps[USB_ENDPOINTS_NUM]; /**< Endpoints */
	USBD_EVENT_BUFFER    *event_buf;
	USBD_EP0_STATE       ep0state;
	USB_DEVICE_STATE    config_state;
	USBD_LINK_STATE      link_state;
	USB_HWPARAMS   hwparams;
	uint32_t       NumInEps;
	uint32_t       NumBytes;
	uint32_t       ConnectionDone;
	uint32_t       NumOutEps;
	uint32_t       IsConfigDone;
	uint32_t       endp_config;
	uint32_t       setup_packet_pending:1;
	uint32_t       three_stage_setup:1;
	uint32_t       ep0_expect_in:1;
	uint32_t       revision;
	uint32_t        current_dr_role;
	uint8_t     endp_number;
	uint32_t dr_mode;
	uint32_t hsphy_mode;
	uint32_t fladj;
	uint32_t event_type;
	uint32_t actual_length;
} USB_DRIVER;

void dwc3_reset_cb(USB_DRIVER *Drv);
void dwc3_setupstage_cb(USB_DRIVER *Drv);
void dwc3_disconnect_cb(USB_DRIVER *Drv);
void dwc3_connect_cb(USB_DRIVER *Drv);
void dwc3_data_in_cb(USB_DRIVER *Drv, uint8_t ep_num);
void dwc3_data_out_cb(USB_DRIVER *Drv, uint8_t ep_num);


void  usbd_dwc3_interrupt_handler(USB_DRIVER  *drv);
void usbd_prepare_Setup(USB_DRIVER *drv);
void  usbd_dwc3_usb2phy_config_check(USB_DRIVER *drv);
uint32_t usbd_SetDeviceAddress(USB_DRIVER *drv, uint8_t addr);
void  usbd_dwc3_usb2phy_config_reset(USB_DRIVER *drv);
void usbd_connectionDone_event(USB_DRIVER *drv);
void usbd_reset_event(USB_DRIVER *drv);
void usbd_linksts_change_event(USB_DRIVER *drv, uint32_t event_info);
void usbd_devt_handler(USB_DRIVER *drv, uint32_t reg);
void  usbd_ep0_set_stall(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir);
void usbd_EpClearStall(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir);
int32_t usbd_set_link_state(USB_DRIVER *drv, USBD_LINK_STATE state);
uint32_t usbd_EpGetTransferIndex(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir);
int32_t usbd_SendEpCmd(USB_DRIVER *drv, uint8_t phy_ep, uint32_t cmd,
		USBD_EP_PARAMS Params);

uint32_t usbd_set_xferResource(USB_DRIVER *drv, uint8_t phy_ep);
uint32_t   usbd_start_ep_config(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir);
uint32_t usbd_set_ep_config(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir, uint8_t ep_type,
		uint16_t ep_max_packet_size, uint8_t ep_interval);
uint32_t usbd_dwc3_ep_enable(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir, uint8_t ep_type,
		uint16_t ep_max_packet_size, uint8_t ep_interval);
int32_t usbd_dwc3_ep_disable(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir);
int32_t  usbd_dwc3_endpoint_create(USB_DRIVER *drv, uint8_t ep_type, uint8_t ep_num,
		uint8_t dir, uint16_t ep_max_packet_size, uint8_t ep_interval);
int32_t usbd_dwc3_ep_stall(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir);
int32_t usbd_dwc3_ep_clearstall(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir);
int32_t usbd_StopTransfer(USB_DRIVER *drv, uint8_t ep_num,
		uint8_t dir, uint32_t ForceRM);

void usbd_depevt_handler(USB_DRIVER *drv, uint32_t reg);
int32_t usbd_dwc3_ep0_send(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir,
		uint8_t *bufferptr, uint32_t buf_len);
int32_t usbd_dwc3_ep0_recv(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir,
		uint8_t *bufferptr, uint32_t buf_len);
int32_t usbd_dwc3_bulk_recv(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir,
		uint8_t *bufferptr, uint32_t buf_len);
int32_t usbd_dwc3_bulk_send(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir,
		uint8_t *bufferptr, uint32_t buf_len);
int32_t usbd_ep_transfer_abort(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir);
int32_t  usbd_dwc3_initialize(USB_DRIVER *drv);
int32_t usbd_dwc3_connect(USB_DRIVER *drv);
void usbd_dwc3_disconnect(USB_DRIVER *drv);

#ifdef __cplusplus

}

#endif

#endif
