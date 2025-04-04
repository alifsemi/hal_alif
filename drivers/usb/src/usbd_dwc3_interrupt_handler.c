/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2025 Alif Semiconductor.
 */

/* Include necessary system files.  */

#include "usbd_dwc3.h"
LOG_MODULE_DECLARE(udc_dwc3);

static void usbd_event_buffer_handler(USB_DRIVER *drv, USBD_EVENT_BUFFER *event_buffer)
{
	uint32_t reg;

	SCB_InvalidateDCache_by_Addr(event_buffer->buf, USB_EVENT_BUFFER_SIZE);

	while (event_buffer->count) {
		reg = ((uint32_t)(*((uint32_t *) ((uint8_t *)event_buffer->buf
				+ event_buffer->lpos))));
		/* Check type of event */
		if ((reg & USB_EVENT_TYPE_CHECK) == USB_DEV_EVENT_TYPE) {
			/* process device specific events  */
			usbd_devt_handler(drv, reg);
		} else if ((reg & USB_EVENT_TYPE_CHECK) == USB_EP_EVENT_TYPE) {
			/* process  endpoint specific events  */
			usbd_depevt_handler(drv, reg);
		} else {
			LOG_ERR("Unknown events");
		}
		event_buffer->lpos = (event_buffer->lpos + USB_EVENT_CNT_SIZE) %
							USB_EVENT_BUFFER_SIZE;
		event_buffer->count -= USB_EVENT_CNT_SIZE;
		drv->regs->GEVNTCOUNT0 = USB_EVENT_CNT_SIZE;
	}
	/* Unmask interrupt */
	event_buffer->count = 0;
}

void  usbd_dwc3_interrupt_handler(USB_DRIVER  *drv)
{
	uint32_t pending_interrupt;
	uint32_t mask_interrupt;
	USBD_EVENT_BUFFER  *event_buf;

	/* Get event pointer ...*/
	event_buf = drv->event_buf;
	pending_interrupt = drv->regs->GEVNTCOUNT0;
	pending_interrupt &= USB_GEVNTCOUNT_MASK;
	if (!pending_interrupt) {
		LOG_ERR("no pending irq");
		return;
	}
	event_buf->count = pending_interrupt;

	/* Set the Event Interrupt Mask */
	mask_interrupt = drv->regs->GEVNTSIZ0;
	SET_BIT(mask_interrupt, USB_GEVNTSIZ_INTMASK);
	drv->regs->GEVNTSIZ0 = mask_interrupt;

	/* Processes events in an Event Buffer */
	usbd_event_buffer_handler(drv, event_buf);

	/* Clear the Event Interrupt Mask */
	mask_interrupt = drv->regs->GEVNTSIZ0;
	CLEAR_BIT(mask_interrupt, USB_GEVNTSIZ_INTMASK);
	drv->regs->GEVNTSIZ0 = mask_interrupt;
}

void usbd_prepare_Setup(USB_DRIVER *drv)
{
	USBD_EP_PARAMS params = {0};
	USBD_TRB      *trb_ptr;
	USBD_EP       *ept;
	uint32_t ret;

	/* Setup packet always on EP0 */
	ept = &drv->eps[USB_CONTROL_EP];
	trb_ptr = &drv->endp0_trb;
	SCB_CleanDCache_by_Addr(&drv->setup_data, USB_SETUP_PKT_SIZE);

	trb_ptr->buf_ptr_low =  LOWER_32_BITS(local_to_global(&drv->setup_data));
	trb_ptr->buf_ptr_high = 0;
	trb_ptr->size = USB_TRB_SIZE_LENGTH(USB_SETUP_PKT_SIZE);
	trb_ptr->ctrl = USB_TRBCTL_CONTROL_SETUP;
	SET_BIT(trb_ptr->ctrl, USB_TRB_CTRL_HWO | USB_TRB_CTRL_LST | USB_TRB_CTRL_IOC
			| USB_TRB_CTRL_ISP_IMI);

	SCB_CleanDCache_by_Addr(trb_ptr, sizeof(*trb_ptr));
	params.param1 = (uint32_t)trb_ptr;
	drv->ep0state = EP0_SETUP_PHASE;

	ret = usbd_SendEpCmd(drv, 0U, USB_DEPCMD_STARTTRANSFER, params);
	if (ret) {
		LOG_ERR("Failed in send the command for setup pkt and status: %d", ret);
	}
	SET_BIT(ept->ep_status, USB_EP_BUSY);
	ept->ep_resource_index = usbd_EpGetTransferIndex(drv,
			ept->ep_index,
			ept->ep_dir);
}

uint32_t usbd_EpGetTransferIndex(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir)
{
	uint8_t phy_ep;
	uint32_t resource_index;

	phy_ep = USB_GET_PHYSICAL_EP(ep_num, dir);
	/* [22:16]: Transfer Resource Index (XferRscIdx). The hardware-assigned
	 * transfer resource index for the transfer
	 */
	resource_index = drv->regs->USB_ENDPNT_CMD[phy_ep].DEPCMD;

	return USB_DEPCMD_GET_RSC_IDX(resource_index);
}

int32_t usbd_transfer_wakeup(USB_DRIVER *drv)
{
	int32_t    retries = USB_TRANSFER_WAKEUP_RETRY;
	int32_t   ret;
	uint32_t   reg;
	uint32_t   link_state;
	/*
	 * According to the Databook Remote wakeup request should
	 * be issued only when the device is in early suspend state.
	 *
	 * We can check that via USB Link State bits in DSTS register.
	 */
	reg = drv->regs->DSTS;
	link_state = USB_DSTS_USBLNKST(reg);

	switch (link_state) {
	case USB_LINK_STATE_EARLY_SUS:    /* in HS, means Early Suspend */
	case USB_LINK_STATE_L2:        /* in HS, means SUSPEND */
	case USB_LINK_STATE_L1:        /* in HS, means SLEEP */
		break;
	default:
		return USB_LINKSTATE_INVALID;
	}

	ret = usbd_set_link_state(drv, USB_LINK_STATE_RECOV);
	if (ret < 0) {
		LOG_ERR("failed to put link in Recovery");
		return ret;
	}

	/* poll until Link State changes to ON */
	while (retries--) {
		reg = drv->regs->DSTS;
		/* in HS, means ON */
		if (USB_DSTS_USBLNKST(reg) == USB_LINK_STATE_ON)
			break;
	}
	if (USB_DSTS_USBLNKST(reg) != USB_LINK_STATE_ON) {
		LOG_ERR("failed to send remote wakeup: Controller state not changed");
		return USB_REMOTE_WAKEUP_FAILED;
	}

	return USB_SUCCESS;
}

int32_t usbd_set_link_state(USB_DRIVER *drv, USBD_LINK_STATE state)
{
	uint32_t reg;
	int32_t retries = USB_LINK_STATE_RETRY;

	while (--retries) {
		reg = drv->regs->DSTS;
		if (reg & USB_DSTS_DCNRD)
			k_busy_wait(5);
		else
			break;
	}
	if (retries <= 0) {
		LOG_ERR("expired retries");
		return USB_LINKSTATE_TIMEOUT_ERROR;
	}
	reg = drv->regs->DCTL;
	reg &= ~USB_DCTL_ULSTCHNGREQ_MASK;

	/* set requested state */
	reg |= USB_DCTL_ULSTCHNGREQ(state);
	drv->regs->DCTL = reg;
	/* wait for a change in DSTS */
	retries = USB_LINK_STATE_RETRY;
	while (--retries) {
		reg = drv->regs->DSTS;
		if (USB_DSTS_USBLNKST(reg) == state) {
			return USB_SUCCESS;
		}
		k_busy_wait(5);
	}
	return USB_LINKSTATE_SET_FAILED;
}
void usbd_ClearStallAllEp(USB_DRIVER *drv)
{
	uint8_t ep_num;

	for (ep_num = 1U; ep_num < USB_ENDPOINTS_NUM; ep_num++) {
		USBD_EP *ept;

		ept = &drv->eps[ep_num];
		if ((ept->ep_status & USB_EP_ENABLED) == 0U)
			continue;

		if ((ept->ep_status & USB_EP_STALL) == 0U)
			continue;

		usbd_EpClearStall(drv, ept->ep_index,
				ept->ep_dir);
	}
}

int32_t usbd_StopTransfer(USB_DRIVER *drv, uint8_t ep_num,
		uint8_t dir, uint32_t force_rm)
{
	USBD_EP *ept;
	USBD_EP_PARAMS params = {0};
	uint8_t phy_ep;
	uint32_t cmd;
	uint32_t ret;

	phy_ep = USB_GET_PHYSICAL_EP(ep_num, dir);
	ept = &drv->eps[phy_ep];
	if (ept->ep_resource_index == 0U)
		return USB_EP_RESOURCE_INDEX_INVALID;

	/* Data book says for end transfer  HW needs some
	 * extra time to synchronize with the interconnect
	 * - Issue EndTransfer WITH CMDIOC bit set
	 * - Wait 100us
	 */
	cmd = USB_DEPCMD_ENDTRANSFER;
	cmd |= (force_rm == 1) ? USB_DEPCMD_HIPRI_FORCERM : 0U;
	SET_BIT(cmd, USB_DEPCMD_CMDIOC);
	cmd |= USB_DEPCMD_PARAM(ept->ep_resource_index);

	ret = usbd_SendEpCmd(drv, phy_ep, cmd, params);
	if (ret < 0) {
		LOG_ERR("Failed to send command at END transfer");
		return ret;
	}
	if (force_rm == 1) {
		ept->ep_resource_index = 0U;
	}
	CLEAR_BIT(ept->ep_status, USB_EP_BUSY);
	k_busy_wait(100);

	return USB_SUCCESS;
}

void usbd_EpClearStall(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir)
{
	uint8_t  phy_ep;
	USBD_EP *ept;
	USBD_EP_PARAMS params = {0};
	uint32_t ret;

	phy_ep = USB_GET_PHYSICAL_EP(ep_num, dir);
	ept = &drv->eps[phy_ep];
	ret = usbd_SendEpCmd(drv, phy_ep, USB_DEPCMD_CLEARSTALL, params);
	if (ret < 0) {
		LOG_ERR("Failed to send command at STALL Ep");
		return;
	}
	CLEAR_BIT(ept->ep_status, USB_EP_STALL);
}

void usbd_ep0_end_control_data(USB_DRIVER *drv, USBD_EP *ept)
{
	USBD_EP_PARAMS params = {0};
	uint32_t     cmd;
	uint32_t ret;
	uint8_t phy_ep;

	if (ept->ep_resource_index == 0U)
		return;
	phy_ep = USB_GET_PHYSICAL_EP(ept->ep_index, ept->ep_dir);
	cmd = USB_DEPCMD_ENDTRANSFER;
	SET_BIT(cmd, USB_DEPCMD_CMDIOC);
	cmd |= USB_DEPCMD_PARAM(ept->ep_resource_index);
	ret = usbd_SendEpCmd(drv, phy_ep, cmd, params);
	if (ret < 0) {
		LOG_ERR("Failed to send command at Endcontrol data");
		return;
	}
	ept->ep_resource_index = 0U;
}

void usbd_ep0_stall_restart(USB_DRIVER *drv)
{
	USBD_EP *ept;

	/* stall is always issued on EP0 */
	usbd_ep0_set_stall(drv, 0U, USB_DIR_OUT);
	ept = &drv->eps[USB_CONTROL_EP];
	ept->ep_status = USB_EP_ENABLED;
	drv->ep0state = EP0_SETUP_PHASE;
	usbd_prepare_Setup(drv);
}

void  usbd_ep0_set_stall(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir)
{
	uint8_t  phy_ep;
	USBD_EP *ept = NULL;
	USBD_EP_PARAMS params = {0};
	uint32_t ret;

	phy_ep = USB_GET_PHYSICAL_EP(ep_num, dir);
	ept = &drv->eps[phy_ep];

	ret = usbd_SendEpCmd(drv, phy_ep, USB_DEPCMD_SETSTALL, params);
	if (ret < 0) {
		LOG_ERR("failed to send STALL command");
		return;
	}
	SET_BIT(ept->ep_status, USB_EP_STALL);
}

void usbd_ep0_data_done(USB_DRIVER *drv, uint8_t endp_number)
{
	USBD_EP       *ept;
	USBD_TRB      *trb_ptr;
	uint32_t     status;
	uint8_t     length;

	ept = &drv->eps[endp_number];
	trb_ptr = &drv->endp0_trb;
	drv->actual_length = 0;
	SCB_InvalidateDCache_by_Addr(trb_ptr, sizeof(*trb_ptr));

	status = USB_TRB_SIZE_TRBSTS(trb_ptr->size);

	if (status == USB_TRBSTS_SETUP_PENDING) {
		drv->setup_packet_pending = 1;
		LOG_ERR("TRB transmission pending in control DATA_PHASE");
		return;
	}
	length = trb_ptr->size & USB_TRB_SIZE_MASK;
	ept->bytes_txed = ept->ep_requested_bytes - length;
	drv->actual_length = ept->bytes_txed;
	if (endp_number != 0) {
		if (drv->usbd_dwc3_data_in_cb != NULL)
			drv->usbd_dwc3_data_in_cb(drv, 0 | USB_REQUEST_IN);
	} else {
		if (drv->usbd_dwc3_data_out_cb != NULL)
			drv->usbd_dwc3_data_out_cb(drv, 0);
	}
}

int32_t usbd_ep0_start_status(USB_DRIVER *drv, uint8_t endp_number)
{
	USBD_EP       *ept;
	USBD_TRB      *trb_ptr;
	USBD_EP_PARAMS params = {0};
	uint32_t type;
	uint32_t ret;
	uint8_t dir;

	ept = &drv->eps[endp_number];

	if ((ept->ep_status & USB_EP_BUSY) != 0U) {
		LOG_ERR("Ep is busy");
		return USB_EP_BUSY_ERROR;
	}

	type = (drv->three_stage_setup != 0U) ?
			USB_TRBCTL_CONTROL_STATUS3
			: USB_TRBCTL_CONTROL_STATUS2;
	trb_ptr = &drv->endp0_trb;

	SCB_CleanDCache_by_Addr(&drv->setup_data, USB_SETUP_PKT_SIZE);
	/* we use same trb_ptr for setup packet */
	trb_ptr->buf_ptr_low = LOWER_32_BITS(local_to_global(&drv->setup_data));
	trb_ptr->buf_ptr_high = 0;
	trb_ptr->size = USB_TRB_SIZE_LENGTH(0U);
	trb_ptr->ctrl = type;
	SET_BIT(trb_ptr->ctrl, USB_TRB_CTRL_HWO | USB_TRB_CTRL_LST | USB_TRB_CTRL_IOC
			| USB_TRB_CTRL_ISP_IMI);

	SCB_CleanDCache_by_Addr(trb_ptr, sizeof(*trb_ptr));
	params.param1 = (uint32_t)trb_ptr;
	drv->ep0state = EP0_STATUS_PHASE;
	/*
	 * Control OUT transfer - Status stage happens on EP0 IN - EP1
	 * Control IN transfer - Status stage happens on EP0 OUT - EP0
	 */
	dir = !drv->ep0_expect_in;
	ret = usbd_SendEpCmd(drv, 0U|dir, USB_DEPCMD_STARTTRANSFER, params);

	if (ret < USB_SUCCESS) {
		LOG_ERR("failed to execute the command at control status");
		return ret;
	}

	SET_BIT(ept->ep_status, USB_EP_BUSY);
	ept->ep_resource_index = usbd_EpGetTransferIndex(drv,
			ept->ep_index,
			ept->ep_dir);

	return ret;
}

void usbd_ep0_status_done(USB_DRIVER *drv)
{
	USBD_TRB *trb_ptr;
	uint32_t status;

	trb_ptr = &drv->endp0_trb;
	status = USB_TRB_SIZE_TRBSTS(trb_ptr->size);
	if (status == USB_TRBSTS_SETUP_PENDING) {
		drv->setup_packet_pending = 1;
		LOG_ERR("status pending at status done");
	}
	drv->actual_length = 0;
	drv->ep0state = EP0_SETUP_PHASE;
	usbd_prepare_Setup(drv);
}


void usbd_EpXferComplete(USB_DRIVER *drv, uint8_t endp_number)
{
	USBD_TRB      *trb_ptr;
	USBD_EP       *ept;
	uint32_t     length;
	uint8_t      dir;
	uint32_t status;

	ept = &drv->eps[endp_number];
	dir = ept->ep_dir;
	trb_ptr = &ept->ep_trb[ept->trb_dequeue];
	SCB_InvalidateDCache_by_Addr(trb_ptr, sizeof(*trb_ptr));
	status = USB_TRB_SIZE_TRBSTS(trb_ptr->size);
	if (status == USB_TRBSTS_SETUP_PENDING) {
		drv->setup_packet_pending = 1;
		LOG_ERR("TRB transmission pending in BULK DATA");
		return;
	}
	ept->trb_dequeue++;
	if (ept->trb_dequeue == NO_OF_TRB_PER_EP) {
		ept->trb_dequeue = 0U;
	}
	length = trb_ptr->size & USB_TRB_SIZE_MASK;
	if (length == 0U) {
		ept->bytes_txed = ept->ep_requested_bytes;
	} else {
		if (dir == USB_DIR_IN) {
			ept->bytes_txed = ept->ep_requested_bytes - length;
		} else {
			if (ept->unaligned_txed == 1U) {
				ept->bytes_txed = ROUND_UP(ept->ep_requested_bytes,
						ept->ep_maxpacket);
				ept->bytes_txed -= length;
				ept->unaligned_txed = 0U;
			} else {
				/* Get the actual number of bytes transmitted by host */
				ept->bytes_txed = ept->ep_requested_bytes - length;
			}
		}
	}
	drv->num_bytes =  ept->bytes_txed;
	if ((drv->usbd_dwc3_data_in_cb != NULL) && dir) {
		drv->usbd_dwc3_data_in_cb(drv, ept->ep_index | USB_REQUEST_IN);
	} else {
		if (drv->usbd_dwc3_data_out_cb != NULL)
			drv->usbd_dwc3_data_out_cb(drv, ept->ep_index);
	}
}

void usbd_clear_trb(USB_DRIVER *drv, uint8_t endp_number)
{
	USBD_TRB      *trb_ptr;
	USBD_EP       *ept;

	ept = &drv->eps[endp_number];
	trb_ptr = &ept->ep_trb[ept->trb_dequeue];
	if (trb_ptr->ctrl & USB_TRB_CTRL_HWO) {
		CLEAR_BIT(trb_ptr->ctrl, USB_TRB_CTRL_HWO);
	}
}
/* Device specific events   */
void usbd_devt_handler(USB_DRIVER *drv, uint32_t reg)
{
	uint32_t  event_info;
	uint32_t event_type;

	/* link state event info  */
	event_info = USB_DEVT_LINK_STATE_INFO(reg);

	/* Device specific events */
	event_type = USB_DEVT_TYPE(reg);
	switch (event_type) {
	case USB_EVENT_WAKEUP:
		break;
	case USB_EVENT_DISCONNECT:
		if (drv->usbd_dwc3_disconnect_cb != NULL)
			drv->usbd_dwc3_disconnect_cb(drv);
		break;
	case USB_EVENT_EOPF:
		break;
	case USB_EVENT_RESET:
		usbd_reset_event(drv);

		if (drv->usbd_dwc3_device_reset_cb != NULL)
			drv->usbd_dwc3_device_reset_cb(drv);
		break;
	case USB_EVENT_CONNECT_DONE:
		usbd_connectionDone_event(drv);

		if (drv->usbd_dwc3_connect_cb != NULL)
			drv->usbd_dwc3_connect_cb(drv);

		usbd_prepare_Setup(drv);
		drv->IsConfigDone = 0;
		break;
	case USB_EVENT_LINK_STATUS_CHANGE:
		drv->link_state = event_info;
		usbd_linksts_change_event(drv, event_info);
		break;
	case USB_EVENT_HIBER_REQ:
		break;
	default:
		break;
	}
}

void usbd_linksts_change_event(USB_DRIVER *drv, uint32_t event_info)
{
	/* not yet implemented fully  */
	switch (event_info) {
	case USB_LINK_STATE_ON: /* ON state */
		break;
	case USB_LINK_STATE_L1: /* Sleep mode  */
		break;

	case USB_LINK_STATE_L2:/* Suspend  */
		break;
	case USB_LINK_STATE_DIS: /*Disconnected  */
		break;
	case USB_LINK_STATE_EARLY_SUS:/* early Suspend  */
		break;
	case USB_LINK_STATE_RESET: /* Disconnected  */
		break;
	case USB_LINK_STATE_RESUME: /* Reseume  */
		break;
	default:
		break;
	}
}


/* Reset the USB device */
void usbd_reset_event(USB_DRIVER *drv)
{
	uint32_t reg;
	uint32_t index;

	reg = drv->regs->DCTL;
	reg &= ~USB_DCTL_TSTCTRL_MASK;
	drv->regs->DCTL = reg;
	/* Clear STALL on all endpoints */
	usbd_ClearStallAllEp(drv);
	for (index = 0U; index < (drv->out_eps + drv->in_eps); index++) {
		drv->eps[index].ep_status = 0U;
	}
	drv->IsConfigDone = 0U;

	/* Reset device address to zero */
	reg = drv->regs->DCFG;
	reg &= ~(USB_DCFG_DEVADDR_MASK);
	drv->regs->DCFG = reg;

}
void usbd_connectionDone_event(USB_DRIVER *drv)
{
	uint32_t reg;
	uint32_t speed;

	reg = drv->regs->DSTS;
	/* if speed value is 0 then it's HIGH SPEED */
	speed = reg & USB_DSTS_CONNECTSPD;
	/* Enable USB2 LPM Capability */
	reg = drv->regs->DCFG;
	SET_BIT(reg, USB_DCFG_LPM_CAP);
	drv->regs->DCFG = reg;
	reg = drv->regs->DCTL;
	reg |= USB_DCTL_HIRD_THRES_MASK;
	drv->regs->DCTL = reg;

}

/* Endpoint specific events */
void usbd_depevt_handler(USB_DRIVER *drv, uint32_t reg)
{
	USBD_EP *ept;
	uint8_t endp_number;
	USBD_EP      *ed;
	uint32_t event_status;
	uint32_t event_type;

	/* Event status from bit[15:12]  */
	event_status = USB_GET_EP_EVENT_STATUS(reg);
	endp_number = USB_GET_DEPEVT_EP_NUM(reg);
	drv->endp_number = endp_number;
	ept = &drv->eps[endp_number];
	if (!(ept->ep_status & USB_EP_ENABLED)) {
		LOG_ERR("endpoint has not enabled");
		return;
	}
	/*  Get the event type  */
	event_type = USB_GET_DEPEVT_TYPE(reg);
	/* Event type can be used for Debugging purpose */
	drv->event_type = event_type;
	if (endp_number == 0 || endp_number == 1) {
		SCB_InvalidateDCache_by_Addr(&drv->setup_data, USB_SETUP_PKT_SIZE);

		/* Get the physical endpoint associated with this endpoint.  */
		ed =  &drv->eps[endp_number];

		/* Reset the endpoint transfer status. */
		ed->ep_transfer_status =  USB_ED_TRANSFER_STATUS_IDLE;

		/* Process the ep0 interrupts bases on event_type. */
		if (event_type == USB_DEPEVT_XFERNOTREADY) {
			if (event_status == USB_DEPEVT_STATUS_CONTROL_DATA) {
				/* We already have a DATA transfer in the controller's cache,
				 * if we receive a XferNotReady(DATA) we will ignore it, unless
				 * it's for the wrong direction.
				 *
				 * In that case, we must issue END_TRANSFER command to the Data
				 * Phase we already have started and issue SetStall on the
				 * control endpoint.
				 */
				if (endp_number != drv->ep0_expect_in) {
					LOG_ERR("unexpected direction for the data phase");
					usbd_ep0_end_control_data(drv, ept);
					usbd_ep0_stall_restart(drv);
				}

			} else if (event_status == USB_DEPEVT_STATUS_CONTROL_STATUS) {

				if (drv->setup_data.bRequest == USB_SET_ADDRESS_REQ) {

					usbd_SetDeviceAddress(drv, drv->setup_data.wValue);
				}
				usbd_ep0_start_status(drv, endp_number);
			} else {
				/* Do nothing  */
			}
		} else if (event_type == USB_DEPEVT_XFERCOMPLETE) {
			USBD_CTRL_REQUEST *setup;

			setup = &drv->setup_data;
			ept = &drv->eps[endp_number];
			CLEAR_BIT(ept->ep_status, USB_EP_BUSY);
			ept->ep_resource_index = 0U;
			drv->setup_packet_pending = 0;
			switch (drv->ep0state) {
			case EP0_SETUP_PHASE:
				ed->ep_transfer_status =  USB_ED_TRANSFER_STATUS_SETUP;
				if (setup->wLength == 0U) {
					drv->three_stage_setup = 0;
					drv->ep0_expect_in = USB_DIR_OUT;
				} else {
					drv->three_stage_setup = 1U;
					drv->ep0_expect_in = !!(setup->bRequestType
							& USB_REQUEST_IN);
				}

				if (drv->usbd_dwc3_setupstage_cb != NULL)
					drv->usbd_dwc3_setupstage_cb(drv);

				break;

			case EP0_DATA_PHASE:
				ed->ep_transfer_status =  USB_ED_TRANSFER_STATUS_OUT_COMPLETION;
				usbd_ep0_data_done(drv, endp_number);

				break;
			case EP0_STATUS_PHASE:
				usbd_ep0_status_done(drv);

				break;
			default:
				break;
			}
		} else {
			LOG_ERR("some other events");
		}
	} else {

		/* BULK IN and BULK OUT events */
		ed =  &drv->eps[endp_number];
		switch (event_type) {
		case USB_DEPEVT_XFERINPROGRESS:

			usbd_EpXferComplete(drv, endp_number);
			break;
		case USB_DEPEVT_XFERCOMPLETE:
			break;
		case USB_DEPEVT_XFERNOTREADY:
			break;
		default:
			break;
		}
	}
}


/* save the current phy state and disable the lpm and suspend  */
void  usbd_dwc3_usb2phy_config_check(USB_DRIVER *drv)
{
	uint32_t reg;

	drv->endp_config = 0;
	reg = drv->regs->GUSB2PHYCFG0;

	if (reg & USB_GUSB2PHYCFG_SUSPHY) {
		SET_BIT(drv->endp_config, USB_GUSB2PHYCFG_SUSPHY);
		CLEAR_BIT(reg, USB_GUSB2PHYCFG_SUSPHY);
	}

	if (reg & USB_GUSB2PHYCFG_ENBLSLPM) {
		SET_BIT(drv->endp_config, USB_GUSB2PHYCFG_ENBLSLPM);
		CLEAR_BIT(reg, USB_GUSB2PHYCFG_ENBLSLPM);
	}

	if (drv->endp_config)
		drv->regs->GUSB2PHYCFG0 = reg;
}
/* Restore the phy state */
void  usbd_dwc3_usb2phy_config_reset(USB_DRIVER *drv)
{
	uint32_t reg;

	if (drv->endp_config) {
		reg = drv->regs->GUSB2PHYCFG0;
		reg |= drv->endp_config;
		drv->regs->GUSB2PHYCFG0 = reg;
	}
}

/**
 *\fn			usbd_SetDeviceAddress
 *\brief		This function is used to set device address
 *\param[in]	pointer to controller context structure
 *\param[in]	Address to be set
 *\return		On success 0, failure cases error
 */
uint32_t usbd_SetDeviceAddress(USB_DRIVER *drv, uint8_t  addr)
{
	uint32_t reg;

	if (addr > USB_DEVICE_MAX_ADDRESS) {
		LOG_ERR("Invalid address");
		return USB_DEVICE_SET_ADDRESS_INVALID;
	}
	if (drv->config_state == USBD_DWC3_STATE_CONFIGURED) {
		LOG_ERR("address can't set from Configured State");
		return USB_DEVICE_ALREADY_CONFIGURED;
	}
	reg = drv->regs->DCFG;
	reg &= ~(USB_DCFG_DEVADDR_MASK);
	reg |= USB_DCFG_DEVADDR(addr);
	drv->regs->DCFG = reg;

	if (addr > USB_DEVICE_DEFAULT_ADDRESS) {
		drv->config_state = USB_DWC3_STATE_ADDRESS;
	} else {
		drv->config_state = USB_DWC3_STATE_DEFAULT;
	}

	return USB_SUCCESS;
}
