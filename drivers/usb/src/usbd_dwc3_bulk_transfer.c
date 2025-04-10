/* SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2025 Alif Semiconductor.
 */

/* Include necessary system files.  */

#include "usbd_dwc3.h"
LOG_MODULE_DECLARE(udc_dwc3);

/**
 *\fn           usbd_dwc3_bulk_send
 *\brief        This function will prepare the TRB for sending bulk data to the host.
 *\param[in]    pointer to the controller context structure
 *\param[in]    endpoint number
 *\param[in]    endpoint direction
 *\param[in]    buffer pointer
 *\param[in]    buffer length
 *\return       On success 0, failure case returns error.
 */
int32_t usbd_dwc3_bulk_send(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir,
		uint8_t *bufferptr, uint32_t buf_len)
{
	USBD_EP *Ept;
	USBD_TRB      *TrbPtr;
	USBD_EP_PARAMS Params = {0};
	uint8_t      phy_ep;
	int32_t     ret;
	uint32_t      cmd;

	phy_ep = USB_PhysicalEp(ep_num, dir);
	Ept = &drv->eps[phy_ep];
	if (Ept->ep_dir != USB_DIR_IN) {
		LOG_ERR("Direction is wrong returning");
		return USB_EP_DIRECTION_WRONG;
	}
	Ept->BytesTxed = 0U;
	Ept->ep_requested_bytes = buf_len;

	TrbPtr = &Ept->EpTrb[Ept->trb_enqueue];
	Ept->trb_enqueue++;
	if (Ept->trb_enqueue == NO_OF_TRB_PER_EP) {
		Ept->trb_enqueue = 0U;
	}
	SCB_CleanDCache_by_Addr(bufferptr, buf_len);
	TrbPtr->BufferPtrLow  = lower_32_bits(local_to_global((uint32_t *)bufferptr));
	TrbPtr->BufferPtrHigh  = 0;
	TrbPtr->Size = USB_TRB_SIZE_LENGTH(buf_len);
	if (buf_len == 0) {
	/* Normal ZLP(BULK IN) - set to 9 for BULK IN TRB for zero
	 * length packet termination
	 */
		TrbPtr->Ctrl = USB_TRBCTL_NORMAL_ZLP;
	} else {
		/* For Bulk TRB control(TRBCTL) as Normal  */
		TrbPtr->Ctrl = USB_TRBCTL_NORMAL;
	}

	TrbPtr->Ctrl |= (USB_TRB_CTRL_HWO | USB_TRB_CTRL_IOC);
	SCB_CleanDCache_by_Addr(TrbPtr, sizeof(*TrbPtr));
	Params.Param1 = (uint32_t)TrbPtr;
	if ((Ept->ep_status & USB_EP_BUSY) != 0U) {
		cmd = USB_DEPCMD_UPDATETRANSFER;
		cmd |= USB_DEPCMD_PARAM(Ept->ep_resource_index);
	} else {
		cmd = USB_DEPCMD_STARTTRANSFER;
	}
	ret = usbd_SendEpCmd(drv, phy_ep, cmd, Params);
	if (ret < USB_SUCCESS) {
		LOG_ERR("failed to send the command");
		return ret;
	}
	if ((Ept->ep_status & USB_EP_BUSY) == 0U) {
		Ept->ep_resource_index = usbd_EpGetTransferIndex(drv,
				Ept->ep_index, Ept->ep_dir);

		Ept->ep_status |= USB_EP_BUSY;
	}

	return ret;
}
/**
 *\fn           usbd_dwc3_bulk_recv
 *\brief        This function will prepare the TRB for receiving bulk data from host.
 *\param[in]    pointer to the controller context structure
 *\param[in]    endpoint number
 *\param[in]    endpoint direction
 *\param[in]    buffer pointer
 *\param[in]    buffer length
 *\return       On success 0, failure case returns error.
 */

int32_t usbd_dwc3_bulk_recv(USB_DRIVER *drv, uint8_t ep_num, uint8_t dir,
		uint8_t *bufferptr, uint32_t buf_len)
{
	USBD_EP *Ept;
	USBD_TRB   *TrbPtr;
	USBD_EP_PARAMS Params = {0};
	uint8_t      phy_ep;
	uint32_t size;
	uint32_t cmd;
	int32_t     ret;

	phy_ep = USB_PhysicalEp(ep_num, dir);

	Ept = &drv->eps[phy_ep];
	if (Ept->ep_dir != dir) {
		LOG_ERR("Wrong BULK endpoint direction");
		return USB_EP_DIRECTION_WRONG;
	}
	if (buf_len < Ept->ep_maxpacket) {
		size = Ept->ep_maxpacket;
		Ept->ep_requested_bytes = Ept->ep_maxpacket;
	} else {
		size = buf_len;
		Ept->ep_requested_bytes = buf_len;
	}
	Ept->BytesTxed = 0U;
	/*
	 * An OUT transfer size (Total TRB buffer allocation)
	 * must be a multiple of MaxPacketSize even if software is expecting a
	 * fixed non-multiple of MaxPacketSize transfer from the Host.
	 */
	if (IS_ALIGNED(buf_len, Ept->ep_maxpacket)) {
		size = roundup(buf_len, Ept->ep_maxpacket);
		Ept->UnalignedTx = 1U;
	}

	TrbPtr = &Ept->EpTrb[Ept->trb_enqueue];

	Ept->trb_enqueue++;
	if (Ept->trb_enqueue == NO_OF_TRB_PER_EP) {
		Ept->trb_enqueue = 0U;
	}
	SCB_CleanDCache_by_Addr(bufferptr, buf_len);

	TrbPtr->BufferPtrLow  = lower_32_bits(local_to_global((uint32_t *)bufferptr));
	TrbPtr->BufferPtrHigh  = 0;

	TrbPtr->Size = USB_TRB_SIZE_LENGTH(size);
	TrbPtr->Ctrl = USB_TRBCTL_NORMAL;
	TrbPtr->Ctrl |= (USB_TRB_CTRL_CSP | USB_TRB_CTRL_IOC
			| USB_TRB_CTRL_ISP_IMI
			| USB_TRB_CTRL_HWO);

	SCB_CleanDCache_by_Addr(TrbPtr, sizeof(*TrbPtr));

	Params.Param1 = (uint32_t)TrbPtr;
	if ((Ept->ep_status & USB_EP_BUSY) != 0U) {
		cmd = USB_DEPCMD_UPDATETRANSFER;
		cmd |= USB_DEPCMD_PARAM(Ept->ep_resource_index);
	} else {
		cmd = USB_DEPCMD_STARTTRANSFER;
	}
	ret = usbd_SendEpCmd(drv, phy_ep, cmd, Params);
	if (ret < USB_SUCCESS) {
		LOG_ERR("SendEpCmd failed");
		return ret;
	}

	if ((Ept->ep_status & USB_EP_BUSY) == 0U) {
		Ept->ep_resource_index = usbd_EpGetTransferIndex(drv,
				Ept->ep_index,
				Ept->ep_dir);

		Ept->ep_status |= USB_EP_BUSY;
	}

	return ret;
}
