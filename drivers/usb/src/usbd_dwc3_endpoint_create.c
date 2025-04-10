/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2025 Alif Semiconductor.
 */

/* Include necessary system files.  */

#include "usbd_dwc3.h"
LOG_MODULE_DECLARE(udc_dwc3);
/**
 *\fn           usbd_dwc3_endpoint_create
 *\brief        This function will creates the all types of endpoints.
 *\param[in]    pointer to the controller context structure
 *\param[in]    endpoint type
 *\param[in]    endpoint num
 *\param[in]    endpoint direction
 *\param[in]    max endpoint pkt size
 *\return       On success 0, failure case returns error.
 */

int32_t  usbd_dwc3_endpoint_create(USB_DRIVER *drv, uint8_t ep_type, uint8_t ep_num, uint8_t dir,
		uint16_t ep_max_packet_size, uint8_t ep_interval)
{
	int32_t status;

	switch (ep_type) {

	case USB_CONTROL_EP:
		/* Enable the control endpoint  */
		status = usbd_dwc3_ep_enable(drv, ep_num, dir, ep_type,
				ep_max_packet_size, ep_interval);
		if (status) {
			LOG_ERR("Failed to enable control ep num : %d direction: %d", ep_num, dir);
			return status;
		}
		break;
	case USB_BULK_EP:
		status = usbd_dwc3_ep_enable(drv, ep_num, dir, ep_type,
				ep_max_packet_size, ep_interval);
		if (status) {
			LOG_ERR("Failed to enable bulk ep num %d: direction: %d", ep_num, dir);
			return status;
		}
		break;
	case USB_ISOCRONOUS_EP:

		/* Not yet implemented   */
		break;
	case USB_INTERRUPT_EP:
		/* Enable the Interrupt endpoint  */
		status = usbd_dwc3_ep_enable(drv, ep_num, dir, ep_type,
				ep_max_packet_size, ep_interval);
		if (status) {
			LOG_ERR("failed to enable interrupt ep num %d: direction: %d", ep_num, dir);
			return status;
		}
		break;
	default:
		LOG_ERR("Invalid Endpoint");
		return USB_EP_INVALID;
	}
	/* Return successful completion.  */
	return status;
}

uint32_t usbd_dwc3_ep_enable(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir, uint8_t ep_type,
		uint16_t ep_max_packet_size, uint8_t ep_interval)
{
	USBD_EP *Ept;
	uint32_t reg;
	uint8_t phy_ep;
	uint32_t ret;

	phy_ep = USB_PhysicalEp(ep_num, dir);
	Ept = &drv->eps[phy_ep];
	Ept->ep_index = ep_num;
	Ept->ep_dir = dir;
	Ept->ep_maxpacket = ep_max_packet_size;
	Ept->phy_ep   = phy_ep;
	if (!(Ept->ep_status & USB_EP_ENABLED)) {
		ret =  usbd_start_ep_config(drv, ep_num, dir);
		if (ret) {
			LOG_ERR("Start EPConfig failed");
			return ret;
		}
	}
	ret = usbd_set_ep_config(drv, ep_num, dir, ep_type, ep_max_packet_size, ep_interval);
	if (ret) {
		LOG_ERR("SetEP failed");
		return ret;
	}

	if (!(Ept->ep_status & USB_EP_ENABLED)) {
		ret = usbd_set_xferResource(drv, phy_ep);
		if (ret) {
			LOG_ERR("Set XferResource failed");
			return ret;
		}
	}
	if (!(Ept->ep_status & USB_EP_ENABLED)) {
		Ept->ep_status |= USB_EP_ENABLED;

		reg = drv->regs->DALEPENA;
		reg |=  USB_DALEPENA_EP(Ept->phy_ep);
		drv->regs->DALEPENA = reg;
		if (phy_ep > 1) {
			USBD_TRB *TrbStHw, *TrbLink;
			/* Initialize TRB ring   */
			Ept->trb_enqueue = 0;
			Ept->trb_dequeue = 0;
			TrbStHw = &Ept->EpTrb[0U];
			/* Link TRB. The HWO bit is never reset */
			TrbLink = &Ept->EpTrb[NO_OF_TRB_PER_EP];
			memset(TrbLink, 0x0, sizeof(USBD_TRB));

			TrbLink->BufferPtrLow = lower_32_bits(local_to_global(TrbStHw));
			TrbLink->BufferPtrHigh =  upper_32_bits(local_to_global(TrbStHw));
			TrbLink->Ctrl |= USB_TRBCTL_LINK_TRB;
			TrbLink->Ctrl |= USB_TRB_CTRL_HWO;

			return USB_SUCCESS;
		}

		return USB_SUCCESS;
	}

	return USB_EP_ENABLE_ERROR;

}
uint32_t usbd_set_ep_config(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir, uint8_t ep_type,
		uint16_t ep_max_packet_size, uint8_t ep_interval)
{
	USBD_EP_PARAMS Params = {0};
	uint8_t phy_ep;

	phy_ep = USB_PhysicalEp(ep_num, dir);

	Params.Param0 = USB_DEPCFG_EP_TYPE(ep_type)
			| USB_DEPCFG_MAX_PACKET_SIZE(ep_max_packet_size);

	Params.Param0 |= USB_DEPCFG_ACTION_INIT;
	Params.Param1 = USB_DEPCFG_XFER_COMPLETE_EN | USB_DEPCFG_XFER_NOT_READY_EN;

	/*
	 * We are doing 1:1 mapping for endpoints, meaning
	 * Physical Endpoints 2 maps to Logical Endpoint 2 and
	 * so on. We consider the direction bit as part of the physical
	 * endpoint number. So USB endpoint 0x81 is 0x03.
	 */
	Params.Param1 |= USB_DEPCFG_EP_NUMBER(phy_ep);

	if (dir != USB_DIR_OUT) {
		Params.Param0 |= USB_DEPCFG_FIFO_NUMBER(phy_ep >> 1U);
	}
	if (ep_type != 0)
		Params.Param1 |= USB_DEPCFG_XFER_IN_PROGRESS_EN;

	if (ep_interval) {
		Params.Param1 |= USB_DEPCFG_BINTERVAL_M1(ep_interval - 1);
	}

	return usbd_SendEpCmd(drv, phy_ep, USB_DEPCMD_SETEPCONFIG, Params);
}


uint32_t usbd_start_ep_config(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir)
{
	USBD_EP_PARAMS Params = {0};
	uint32_t     cmd;
	uint8_t phy_ep;
	uint32_t ret;

	phy_ep = USB_PhysicalEp(ep_num, dir);

	if (phy_ep != 1U) {
		cmd = USB_DEPCMD_DEPSTARTCFG;
		/* XferRscIdx == 0 for EP0 and 2 for the remaining */
		if (phy_ep > 1U) {
			if (drv->IsConfigDone != 0U) {
				return USB_SUCCESS;
			}
			drv->IsConfigDone = 1U;
			cmd |= USB_DEPCMD_PARAM(2U);
		}
		ret  = usbd_SendEpCmd(drv, 0, cmd, Params);
		if (ret) {
			LOG_ERR("Ep cmd failed");
			return ret;
		} else {
			return ret;
		}
	}

	return USB_SUCCESS;
}
uint32_t usbd_set_xferResource(USB_DRIVER *drv, uint8_t phy_ep)
{
	USBD_EP_PARAMS Params = {0};
	/* Set Endpoint Transfer Resource configuration parameter
	 * This field must be set to 1
	 */
	Params.Param0 = USB_DEPXFERCFG_NUM_XFER_RES(1U);
	return usbd_SendEpCmd(drv, phy_ep, USB_DEPCMD_SETTRANSFRESOURCE, Params);
}

int32_t usbd_dwc3_ep_disable(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir)
{
	USBD_EP *Ept;
	uint32_t reg;
	uint8_t phy_ep;

	phy_ep = USB_PhysicalEp(ep_num, dir);
	Ept = &drv->eps[phy_ep];
	reg = drv->regs->DALEPENA;
	reg &= ~USB_DALEPENA_EP(phy_ep);
	drv->regs->DALEPENA = reg;
	Ept->ep_status &= ~USB_EP_ENABLED;
	return USB_SUCCESS;
}


int32_t usbd_dwc3_ep_stall(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir)
{
	uint8_t  phy_ep;
	USBD_EP *Ept = NULL;
	USBD_EP_PARAMS Params = {0};
	int32_t ret;

	phy_ep = USB_PhysicalEp(ep_num, dir);
	Ept = &drv->eps[phy_ep];
	ret = usbd_SendEpCmd(drv, phy_ep, USB_DEPCMD_SETSTALL, Params);
	if (ret < USB_SUCCESS) {
		LOG_ERR("failed to send STALL command");
		return ret;
	}
	Ept->ep_status |= USB_EP_STALL;

	return ret;
}

int32_t usbd_dwc3_ep_clearstall(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir)
{
	uint8_t  phy_ep;
	USBD_EP *Ept = NULL;
	USBD_EP_PARAMS Params = {0};
	int32_t ret;

	phy_ep = USB_PhysicalEp(ep_num, dir);
	Ept = &drv->eps[phy_ep];
	ret = usbd_SendEpCmd(drv, phy_ep, USB_DEPCMD_CLEARSTALL, Params);
	if (ret < USB_SUCCESS) {
		LOG_ERR("failed to send CLEARSTALL command");
		return ret;
	}
	Ept->ep_status  &= ~(USB_EP_STALL | USB_EP_WEDGE);

	return ret;
}
