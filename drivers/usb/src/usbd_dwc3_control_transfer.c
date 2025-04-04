/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2025 Alif Semiconductor.
 */

/* Include necessary system files.  */

#include "usbd_dwc3.h"
LOG_MODULE_DECLARE(udc_dwc3);

/**
 *\fn           usbd_dwc3_ep0_recv
 *\brief        This function prepares the TRB for recv control data from host
 *\param[in]    pointer to the controller context structure
 *\param[in]    endpoint number
 *\param[in]    endpoint direction
 *\param[in]    data buffer pointer
 *\param[in]    data buffer length
 *\return       On success 0, failure case returns error.
 */

int32_t usbd_dwc3_ep0_recv(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir,
		uint8_t *bufferptr, uint32_t buf_len)
{
	USBD_EP_PARAMS params = {0};
	USBD_EP *ept;
	USBD_TRB *trb_ptr;
	int32_t ret;
	uint8_t phy_ep;

	phy_ep = USB_GET_PHYSICAL_EP(ep_num, dir);
	ept = &drv->eps[phy_ep];

	if ((ept->ep_status & USB_EP_BUSY) != 0U) {
		LOG_ERR("Endpoint 0 already busy returning");
		return USB_EP_BUSY_ERROR;
	}
	if (buf_len < USB_CONTROL_EP_MAX_PKT)
		buf_len = USB_CONTROL_EP_MAX_PKT;
	ept->ep_requested_bytes = buf_len;
	ept->bytes_txed = 0U;
	trb_ptr = &drv->endp0_trb;
	SCB_CleanDCache_by_Addr(bufferptr, buf_len);

	trb_ptr->buf_ptr_low  =  LOWER_32_BITS(local_to_global((uint32_t *)bufferptr));
	trb_ptr->buf_ptr_high  = 0;
	trb_ptr->size = USB_TRB_SIZE_LENGTH(buf_len);
	trb_ptr->ctrl = USB_TRBCTL_CONTROL_DATA;
	SET_BIT(trb_ptr->ctrl, USB_TRB_CTRL_HWO | USB_TRB_CTRL_LST | USB_TRB_CTRL_ISP_IMI
		| USB_TRB_CTRL_IOC);

	SCB_CleanDCache_by_Addr(trb_ptr, sizeof(*trb_ptr));
	params.param1 = (uint32_t)trb_ptr;
	drv->ep0state = EP0_DATA_PHASE;
	ret = usbd_SendEpCmd(drv, phy_ep, USB_DEPCMD_STARTTRANSFER, params);
	if (ret < USB_SUCCESS) {
		LOG_ERR("Failed to send command over EP0");
		return ret;
	}

	SET_BIT(ept->ep_status, USB_EP_BUSY);
	/* In response to the Start Transfer command, the hardware assigns
	 * transfer a resource index number (XferRscIdx) and returns index in the DEPCMDn register
	 * and in the Command Complete event.
	 * This index must be used in subsequent Update and End Transfer commands.
	 */
	ept->ep_resource_index = usbd_EpGetTransferIndex(drv,
			ept->ep_index,
			ept->ep_dir);

	return ret;
}

/**
 *\fn           usbd_dwc3_ep0_send
 *\brief        This function prepares the TRB for control data send to host
 *\param[in]    pointer to the controller context structure
 *\param[in]    endpoint number
 *\param[in]    endpoint direction
 *\param[in]    data buffer pointer
 *\param[in]    data buffer length
 *\return       On success 0, failure case returns error.
 */

int32_t usbd_dwc3_ep0_send(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir,
		uint8_t *bufferptr, uint32_t buf_len)
{
	USBD_EP_PARAMS params = {0};
	USBD_EP *ept;
	USBD_TRB *trb_ptr;
	int32_t ret;
	uint8_t phy_ep;

	if (buf_len == 0) {
		return USB_EP_BUFF_LENGTH_INVALID;
	}
	/* Control IN - EP1 */
	phy_ep = USB_GET_PHYSICAL_EP(ep_num, dir);
	ept = &drv->eps[phy_ep];

	if ((ept->ep_status & USB_EP_BUSY) != 0U) {
		LOG_ERR("Endpoint 1 already busy returning");
		return USB_EP_BUSY_ERROR;
	}

	ept->ep_requested_bytes = buf_len;
	ept->bytes_txed = 0U;
	trb_ptr = &drv->endp0_trb;
	SCB_CleanDCache_by_Addr(bufferptr, buf_len);

	trb_ptr->buf_ptr_low  =  LOWER_32_BITS(local_to_global((uint32_t *)bufferptr));
	trb_ptr->buf_ptr_high  = 0;
	trb_ptr->size = USB_TRB_SIZE_LENGTH(buf_len);
	trb_ptr->ctrl = USB_TRBCTL_CONTROL_DATA;
	SET_BIT(trb_ptr->ctrl, USB_TRB_CTRL_HWO | USB_TRB_CTRL_LST | USB_TRB_CTRL_ISP_IMI
			| USB_TRB_CTRL_IOC);

	SCB_CleanDCache_by_Addr(trb_ptr, sizeof(*trb_ptr));
	params.param1 = (uint32_t)trb_ptr;
	drv->ep0state = EP0_DATA_PHASE;
	ret = usbd_SendEpCmd(drv, phy_ep, USB_DEPCMD_STARTTRANSFER, params);
	if (ret < USB_SUCCESS) {
		LOG_ERR("Failed in sending data over EP1");
		return ret;
	}

	SET_BIT(ept->ep_status, USB_EP_BUSY);
	ept->ep_resource_index = usbd_EpGetTransferIndex(drv,
			ept->ep_index,
			ept->ep_dir);

	return ret;
}

/**
 *\fn           usbd_SendEpCmd
 *\brief        This function sends the controller endpoint commands.
 *\param[in]    pointer to our controller context structure
 *\param[in]    physical endpoint number
 *\param[in]    command to be send
 *\param[in]    endpoint params which contain trb address
 *\return       On success 0, failure case returns error.
 */

int32_t usbd_SendEpCmd(USB_DRIVER *drv, uint8_t phy_ep, uint32_t cmd,
		USBD_EP_PARAMS params)
{
	int32_t ret;
	int32_t      cmd_status = USB_EP_CMD_CMPLT_ERROR;
	int32_t      timeout = USB_DEPCMD_TIMEOUT;

	/* Check usb2phy config before issuing DEPCMD  */
	usbd_dwc3_usb2phy_config_check(drv);
	if (USB_DEPCMD_CMD(cmd) == USB_DEPCMD_UPDATETRANSFER) {
		CLEAR_BIT(cmd, USB_DEPCMD_CMDIOC | USB_DEPCMD_CMDACT);
	} else {
		SET_BIT(cmd, USB_DEPCMD_CMDACT);
	}

	if (USB_DEPCMD_CMD(cmd) == USB_DEPCMD_STARTTRANSFER) {
		drv->regs->USB_ENDPNT_CMD[phy_ep].DEPCMDPAR1 =
				(uint32_t)local_to_global((void *)(params.param1));
	} else {
		drv->regs->USB_ENDPNT_CMD[phy_ep].DEPCMDPAR1 = params.param1;
	}
	/* Issuing DEPCFG Command for appropriate endpoint */
	drv->regs->USB_ENDPNT_CMD[phy_ep].DEPCMDPAR0 = params.param0;
	drv->regs->USB_ENDPNT_CMD[phy_ep].DEPCMD = cmd;
	do {
		/* Read the device endpoint Command register.  */
		ret = drv->regs->USB_ENDPNT_CMD[phy_ep].DEPCMD;
		if (!(ret & USB_DEPCMD_CMDACT)) {
			cmd_status = USB_DEPCMD_STATUS(ret);
			switch (cmd_status) {
			case USB_DEPEVT_CMD_SUCCESS:
				ret = cmd_status;
				break;
			case USB_DEPEVT_TRANSFER_NO_RESOURCE:
				ret = USB_EP_CMD_CMPLT_NO_RESOURCE_ERROR;
				break;
			case USB_DEPEVT_TRANSFER_BUS_EXPIRY:
				ret = USB_EP_CMD_CMPLT_BUS_EXPIRY_ERROR;
				break;
			default:
				LOG_ERR("Unknown cmd status");
				ret = USB_EP_CMD_CMPLT_STATUS_UNKNOWN;
				break;
			}
			break;
		}
	} while (--timeout);

	if (timeout == 0) {
		ret = USB_EP_CMD_CMPLT_TIMEOUT_ERROR;
		LOG_ERR("timeout");
	}
	/* Restore the USB2 phy state  */
	usbd_dwc3_usb2phy_config_reset(drv);

	return ret;
}
