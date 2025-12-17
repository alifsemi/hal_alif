/*
 * Copyright (C) 2025 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#include "isp_conf.h"
#include "isp_param_conf.h"
#include <zephyr/drivers/video.h>
#include <zephyr/drivers/video-controls.h>
#include <soc_memory_map.h>

#include <zephyr/drivers/video/isp-vsi.h>
#include <zephyr/drivers/video/video_alif.h>

/* ISP lib includes */
#include "vsi_comm_isp.h"
#include "vsios_isp.h"

#include "mpi_isp_calib.h"
#include "vsi_comm_awb.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(isp_wrapper, CONFIG_VIDEO_LOG_LEVEL);

#if defined(CONFIG_ISP_LIB_WB_MODULE)
extern ISP_AWB_FUNC_S vsiAwbAlgo;
#endif /* defined(CONFIG_ISP_LIB_WB_MODULE) */


extern int isp_vsi_init(struct isp_config_params *init_cfg);
extern int isp_vsi_update_cfg(struct isp_config_params *init_cfg);
extern int isp_vsi_uninit(struct isp_config_params *init_cfg);
extern void isp_vsi_bottom_half(struct isp_config_params *init_cfg, uint32_t mi_mis);
extern int isp_vsi_start(struct isp_config_params *init_cfg);
extern int isp_vsi_stop(struct isp_config_params *init_cfg);
extern int isp_vsi_enqueue(struct isp_config_params *init_cfg,
		struct video_buffer *buf);
extern int isp_vsi_dequeue(struct isp_config_params *init_cfg,
		struct video_buffer *buf);
extern void VSI_ISP_IrqProcessFrameEnd(ISP_PORT IspPort);

static inline int isp_err_2_errno(int val)
{
	switch (val) {
	case VSI_ERR_INVALID_DEVID:
		LOG_DBG("Invalid Device-ID");
		return -ENODEV;
	case VSI_ERR_INVALID_PORTID:
		LOG_DBG("Invalid Port-ID");
		return -ENXIO;
	case VSI_ERR_INVALID_CHNID:
		LOG_DBG("Invalid Channel-ID");
		return -ENXIO;
	case VSI_ERR_NULL_PTR:
		LOG_DBG("Null-Pointer passed");
		return -EFAULT;
	case VSI_ERR_ILLEGAL_PARAM:
		LOG_DBG("Illegal parameter passed");
		return -EINVAL;
	case VSI_ERR_NOT_CONFIG:
		LOG_DBG("Not yet configured");
		return -ENOSYS;
	case VSI_ERR_NOT_SUPPORT:
		LOG_DBG("Feature not supported");
		return -ENOTSUP;
	case VSI_ERR_NOT_PERM:
		LOG_DBG("Operation not permitted");
		return -EPERM;
	case VSI_ERR_NOT_READY:
		LOG_DBG("Not ready");
		return -EAGAIN;
	case VSI_ERR_EXIST:
		LOG_DBG("Already exists");
		return -EEXIST;
	case VSI_ERR_UNEXIST:
		LOG_DBG("Does not exists");
		return -ENOENT;
	case VSI_ERR_BUF_EMPTY:
		LOG_DBG("Buffer empty");
		return -EAGAIN;
	case VSI_ERR_BUF_FULL:
		LOG_DBG("Buffer full");
		return -ENOBUFS;
	case VSI_ERR_NOMEM:
		LOG_DBG("Out of memory");
		return -ENOMEM;
	case VSI_ERR_NOBUF:
		LOG_DBG("No buffers available");
		return -ENOBUFS;
	case VSI_ERR_TIMEOUT:
		LOG_DBG("Timed-out");
		return -ETIMEDOUT;
	case VSI_ERR_BUSY:
		LOG_DBG("Resource busy");
		return -EBUSY;
	default:
		LOG_DBG("Unknown isp error!");
		return -EIO;
	}
}

static inline int isp_mode_from_fourcc(uint32_t fourcc)
{
	switch (fourcc) {
	case VIDEO_PIX_FMT_BGGR8:
	case VIDEO_PIX_FMT_GBRG8:
	case VIDEO_PIX_FMT_GRBG8:
	case VIDEO_PIX_FMT_RGGB8:
	case VIDEO_PIX_FMT_BGGR10:
	case VIDEO_PIX_FMT_GBRG10:
	case VIDEO_PIX_FMT_GRBG10:
	case VIDEO_PIX_FMT_RGGB10:
	case VIDEO_PIX_FMT_BGGR12:
	case VIDEO_PIX_FMT_GBRG12:
	case VIDEO_PIX_FMT_GRBG12:
	case VIDEO_PIX_FMT_RGGB12:
	case VIDEO_PIX_FMT_Y10P:
		return ISP_MODE_RAW;
	case VIDEO_PIX_FMT_GREY:
	case VIDEO_PIX_FMT_Y10:
		return ISP_MODE_BT601;
	default:
		return ISP_MODE_BT656;
	}
}

static inline int isp_pixelfmt_from_fourcc(uint32_t fourcc)
{
	switch (fourcc) {
	case VIDEO_PIX_FMT_BGGR8:
		return PIXEL_FORMAT_BGGR8;
	case VIDEO_PIX_FMT_GBRG8:
		return PIXEL_FORMAT_GBRG8;
	case VIDEO_PIX_FMT_GRBG8:
		return PIXEL_FORMAT_GRBG8;
	case VIDEO_PIX_FMT_RGGB8:
		return PIXEL_FORMAT_RGGB8;
	case VIDEO_PIX_FMT_BGGR10:
		return PIXEL_FORMAT_BGGR10;
	case VIDEO_PIX_FMT_GBRG10:
		return PIXEL_FORMAT_GBRG10;
	case VIDEO_PIX_FMT_GRBG10:
		return PIXEL_FORMAT_GRBG10;
	case VIDEO_PIX_FMT_RGGB10:
		return PIXEL_FORMAT_RGGB10;
	case VIDEO_PIX_FMT_BGGR12:
		return PIXEL_FORMAT_BGGR12;
	case VIDEO_PIX_FMT_GBRG12:
		return PIXEL_FORMAT_GBRG12;
	case VIDEO_PIX_FMT_GRBG12:
		return PIXEL_FORMAT_GRBG12;
	case VIDEO_PIX_FMT_RGGB12:
		return PIXEL_FORMAT_RGGB12;
	case VIDEO_PIX_FMT_BGGR14:
		return PIXEL_FORMAT_BGGR14;
	case VIDEO_PIX_FMT_GBRG14:
		return PIXEL_FORMAT_GBRG14;
	case VIDEO_PIX_FMT_GRBG14:
		return PIXEL_FORMAT_GRBG14;
	case VIDEO_PIX_FMT_RGGB14:
		return PIXEL_FORMAT_RGGB14;
	case VIDEO_PIX_FMT_BGGR16:
		return PIXEL_FORMAT_BGGR16;
	case VIDEO_PIX_FMT_GBRG16:
		return PIXEL_FORMAT_GBRG16;
	case VIDEO_PIX_FMT_GRBG16:
		return PIXEL_FORMAT_GRBG16;
	case VIDEO_PIX_FMT_RGGB16:
		return PIXEL_FORMAT_RGGB16;
	case VIDEO_PIX_FMT_GREY:
		return PIXEL_FORMAT_RAW8;
	case VIDEO_PIX_FMT_Y10P:
		/*
		 * This should have been PIXEL_FORMAT_RAW10, but lib does not support it.
		 * TODO: Fill-in correct equivalent format for RAW8/RAW10 and RAW12
		 */
		return PIXEL_FORMAT_GRBG10;
	case VIDEO_PIX_FMT_Y12P:
		return PIXEL_FORMAT_RAW12;
	case VIDEO_PIX_FMT_NV12:
		return PIXEL_FORMAT_NV12;
	case VIDEO_PIX_FMT_NV21:
		return PIXEL_FORMAT_NV21;
	case VIDEO_PIX_FMT_NV16:
		return PIXEL_FORMAT_NV16;
	case VIDEO_PIX_FMT_NV61:
		return PIXEL_FORMAT_NV61;
	case VIDEO_PIX_FMT_NV24:
		return PIXEL_FORMAT_NV24;
	case VIDEO_PIX_FMT_NV42:
		return PIXEL_FORMAT_NV42;
	case VIDEO_PIX_FMT_YUV422P:
		return PIXEL_FORMAT_YUV422P;
	case VIDEO_PIX_FMT_YUV420:
		return PIXEL_FORMAT_YUV420P;
	case VIDEO_PIX_FMT_YUYV:
		return PIXEL_FORMAT_YUYV;
	case VIDEO_PIX_FMT_VYUY:
		return PIXEL_FORMAT_VYUY;
	case VIDEO_PIX_FMT_UYVY:
		return PIXEL_FORMAT_UYVY;
	case VIDEO_PIX_FMT_RGB888_PLANAR_PRIVATE:
		return PIXEL_FORMAT_RGB888P;
	default:
		return -1;
	}
}

int isp_vsi_init(struct isp_config_params *init_cfg)
{
	struct channel_parameters *channel;
	struct port_parameters *port;

	/* ISP library data-structures */
	ISP_DEV_ATTR_S devAttr = {};

	ISP_DEV isp_dev_id;
	ISP_PORT isp_port_id;
	ISP_CHN isp_chn_id;

	int ret;

	if (!init_cfg) {
		LOG_ERR("Empty parameters!");
		return -EINVAL;
	}

	port = &(init_cfg->port);
	channel = &(init_cfg->channel);

	if (port->isp_idx > ISP_DEV_CNT - 1) {
		LOG_ERR("More ISP device have be observed than configured!");
		return -ENODEV;
	}

	isp_dev_id = port->isp_idx;

	isp_port_id.devId = port->isp_idx;
	isp_port_id.portId = port->port_id;

	isp_chn_id.devId = port->isp_idx;
	isp_chn_id.portId = port->port_id;
	isp_chn_id.chnId = channel->channel_idx;

	ret = VSI_MPI_ISP_Init(isp_dev_id);
	if (ret) {
		LOG_ERR("ISP library Init failed!");
		return isp_err_2_errno(ret);
	}

	devAttr.ispWorkMode = WORK_MODE_NORMAL;
	ret = VSI_MPI_ISP_SetDevAttr(isp_dev_id, &devAttr);
	if (ret) {
		LOG_ERR("ISP set work-mode failed!");
		return isp_err_2_errno(ret);
	}

	/* Program the function pointers for AWB Algorithm in library */
#if defined(CONFIG_ISP_LIB_WB_MODULE)
	if (isp_calib_param.modules.wb.opType == OP_TYPE_AUTO) {
		ret = VSI_MPI_ISP_AwbRegCallBack(isp_port_id, &vsiAwbAlgo);
		if (ret) {
			LOG_ERR("AWB Algorithm setup in library failed");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_WB_MODULE) */

	/* Configure the ISP as per the calibration parameters */
	ret = VSI_MPI_ISP_SetCalib(isp_port_id, &isp_calib_param);
	if (ret) {
		LOG_ERR("Programming ISP calibration data in library failed!");
		return isp_err_2_errno(ret);
	}

	return 0;
}

int isp_vsi_update_cfg(struct isp_config_params *init_cfg)
{
	struct channel_parameters *channel;
	struct port_parameters *port;

	/* ISP library data-structures */
	ISP_PORT_ATTR_S isp_port_config = {};
	ISP_CHN_ATTR_S isp_chan_attr = {};

	ISP_DEV isp_dev_id;
	ISP_PORT isp_port_id;
	ISP_CHN isp_chn_id;

	int ret;

	if (!init_cfg) {
		LOG_ERR("Empty parameters!");
		return -EINVAL;
	}

	port = &(init_cfg->port);
	channel = &(init_cfg->channel);

	if (port->isp_idx > ISP_DEV_CNT - 1) {
		LOG_ERR("More ISP device have be observed than configured!");
		return -ENODEV;
	}

	isp_dev_id = port->isp_idx;

	isp_port_id.devId = port->isp_idx;
	isp_port_id.portId = port->port_id;

	isp_chn_id.devId = port->isp_idx;
	isp_chn_id.portId = port->port_id;
	isp_chn_id.chnId = channel->channel_idx;


	ret = VSI_MPI_ISP_GetPortAttr(isp_port_id, &isp_port_config);
	if (ret) {
		LOG_ERR("Getting the port config failed!");
		return isp_err_2_errno(ret);
	}

	isp_port_config.snsRect.width = port->port_fmt.width;
	isp_port_config.snsRect.height = port->port_fmt.height;
	isp_port_config.ispInputType = (port->input == INPUT_SENSOR) ?
				       INPUT_TYPE_SENSOR :
				       INPUT_TYPE_TPG;
	isp_port_config.ispMode =
		isp_mode_from_fourcc(port->port_fmt.pixelformat);
	switch (port->hdr) {
	case LINEAR:
		isp_port_config.hdrMode = HDR_MODE_LINEAR;
		break;
	case STITCH:
		isp_port_config.hdrMode = HDR_MODE_ISP_STICH;
		break;
	case SENSOR_STITCH:
		isp_port_config.hdrMode = HDR_MODE_SENSOR_STICH;
		break;
	default:
		LOG_ERR("Unknown HDR mode!");
		return -EINVAL;
	}

	switch (port->seq) {
	case YCBYCR:
		isp_port_config.syncCfg.ccirSeq = CCIR_YCBYCR;
		break;
	case YCRYCB:
		isp_port_config.syncCfg.ccirSeq = CCIR_YCRYCB;
		break;
	case CBYCRY:
		isp_port_config.syncCfg.ccirSeq = CCIR_CBYCRY;
		break;
	case CRYCBY:
		isp_port_config.syncCfg.ccirSeq = CCIR_CRYCBY;
		break;
	}

	isp_port_config.pixelFormat =
		isp_pixelfmt_from_fourcc(port->port_fmt.pixelformat);

	if (isp_port_config.pixelFormat == -1) {
		LOG_ERR("Unsupported pixel format");
		return -ENOTSUP;
	}

	/* This is what sensor is transmitting. */
	isp_port_config.snsRect.top    = port->sns_rect.top;
	isp_port_config.snsRect.left   = port->sns_rect.left;
	isp_port_config.snsRect.width  = port->sns_rect.width;
	isp_port_config.snsRect.height = port->sns_rect.height;

	isp_port_config.inFormRect.top    = port->in_form_rect.top;
	isp_port_config.inFormRect.left   = port->in_form_rect.left;
	isp_port_config.inFormRect.width  = port->in_form_rect.width;
	isp_port_config.inFormRect.height = port->in_form_rect.height;

	isp_port_config.outFormRect.top    = port->out_form_rect.top;
	isp_port_config.outFormRect.left   = port->out_form_rect.left;
	isp_port_config.outFormRect.width  = port->out_form_rect.width;
	isp_port_config.outFormRect.height = port->out_form_rect.height;

	isp_port_config.iSRect.top    = port->image_stabilization_rect.top;
	isp_port_config.iSRect.left   = port->image_stabilization_rect.left;
	isp_port_config.iSRect.width  = port->image_stabilization_rect.width;
	isp_port_config.iSRect.height = port->image_stabilization_rect.height;

	isp_port_config.snsFps = port->snsfps;

	switch (port->field) {
	case BOTH:
		isp_port_config.syncCfg.filedSelection = FIELDSEL_BOTH;
		break;
	case EVEN:
		isp_port_config.syncCfg.filedSelection = FIELDSEL_EVEN;
		break;
	case ODD:
		isp_port_config.syncCfg.filedSelection = FIELDSEL_ODD;
		break;
	default:
		LOG_ERR("Unknown Field Sampling parameter!");
		return -EINVAL;
	}

	switch (port->mode) {
	case COSITED:
		isp_port_config.syncCfg.con422 = CONV422_COSITED;
		break;
	case INTER:
		isp_port_config.syncCfg.con422 = CONV422_INTER;
		break;
	case NON_COSITED:
		isp_port_config.syncCfg.con422 = CONV422_NOCOSITED;
		break;
	default:
		LOG_ERR("Unknown Sub-sampling mode!");
		return -EINVAL;
	}

	/*
	 * TODO: Add support for tpg_img_idx in library to switch between
	 * different forms of TPG formats. Right now only 3x3 color block
	 * is supported in library.
	 */

	ret = VSI_MPI_ISP_SetPortAttr(isp_port_id, &isp_port_config);
	if (ret) {
		LOG_ERR("Setting the port config failed!");
		return isp_err_2_errno(ret);
	}

	switch (channel->trans_bus) {
	case ONLINE:
		isp_chan_attr.transBus = TRANS_BUS_ONLINE;
		break;
	case FLEXA:
		isp_chan_attr.transBus = TRANS_BUS_FLEXA;
		break;
	case DMA:
		isp_chan_attr.transBus = TRANS_BUS_DMA;
		break;
	default:
		LOG_ERR("Invalid Transmission bus mode for channel!");
		return -EINVAL;
	}

	isp_chan_attr.chnFormat.width = channel->output_fmt.width;
	isp_chan_attr.chnFormat.height = channel->output_fmt.height;
	isp_chan_attr.chnFormat.pixelFormat =
		isp_pixelfmt_from_fourcc(channel->output_fmt.pixelformat);

	if (isp_chan_attr.chnFormat.pixelFormat == -1) {
		LOG_ERR("Unsupported Channel pixel format!");
		return -ENOTSUP;
	}

	ret = VSI_MPI_ISP_SetChnAttr(isp_chn_id, &isp_chan_attr);
	if (ret) {
		LOG_ERR("Setting the Channel config failed!");
		return isp_err_2_errno(ret);
	}

	return 0;
}

int isp_vsi_uninit(struct isp_config_params *init_cfg)
{
	struct channel_parameters *channel;
	struct port_parameters *port;

	/* ISP library data-structures */
	ISP_DEV isp_dev_id;
	ISP_PORT isp_port_id;

	int ret;

	port = &(init_cfg->port);
	channel = &(init_cfg->channel);

	isp_port_id.devId = port->isp_idx;
	isp_port_id.portId = port->port_id;

	isp_dev_id = port->isp_idx;

#if defined(CONFIG_ISP_LIB_WB_MODULE)
	ret = VSI_MPI_ISP_AwbUnRegCallBack(isp_port_id);
	if (ret) {
		LOG_ERR("Failed to Un-register AWB call-back!");
		return isp_err_2_errno(ret);
	}
#endif /* defined(CONFIG_ISP_LIB_WB_MODULE) */

	ret = VSI_MPI_ISP_Exit(isp_dev_id);
	if (ret) {
		LOG_ERR("Failed to unregister ISP!");
		return isp_err_2_errno(ret);
	}
	return 0;
}

void isp_vsi_bottom_half(struct isp_config_params *init_cfg, uint32_t mi_mis)
{
	struct port_parameters *port;
	ISP_PORT isp_port_id;

	port = &(init_cfg->port);

	isp_port_id.devId = port->isp_idx;
	isp_port_id.portId = port->port_id;

	VSI_ISP_IrqProcessFrameEnd(isp_port_id);
}

int isp_vsi_start(struct isp_config_params *init_cfg)
{
	struct channel_parameters *channel;
	struct port_parameters *port;

	/* ISP library data-structures */
	ISP_DEV isp_dev_id;
	ISP_PORT isp_port_id;
	ISP_CHN isp_chn_id;

	int ret;

	port = &(init_cfg->port);
	channel = &(init_cfg->channel);

	isp_dev_id = port->isp_idx;

	isp_port_id.devId = port->isp_idx;
	isp_port_id.portId = port->port_id;

	isp_chn_id.devId = port->isp_idx;
	isp_chn_id.portId = port->port_id;
	isp_chn_id.chnId = channel->channel_idx;

	ret = VSI_MPI_ISP_EnableDev(isp_dev_id);
	if (ret) {
		LOG_ERR("Failed to enagle ISP Device!");
		return isp_err_2_errno(ret);
	}

	ret = VSI_MPI_ISP_EnablePort(isp_port_id);
	if (ret) {
		LOG_ERR("Failed to enable ISP Port!");
		return isp_err_2_errno(ret);
	}

	ret = VSI_MPI_ISP_EnableChn(isp_chn_id);
	if (ret) {
		LOG_ERR("Failed to enable ISP Channel!");
		return isp_err_2_errno(ret);
	}

	return 0;
}

int isp_vsi_stop(struct isp_config_params *init_cfg)
{
	struct channel_parameters *channel;
	struct port_parameters *port;

	/* ISP library data-structures */
	ISP_DEV isp_dev_id;
	ISP_PORT isp_port_id;
	ISP_CHN isp_chn_id;

	int ret = 0;

	if (!init_cfg) {
		LOG_ERR("Invalid parameters!");
		return -EINVAL;
	}

	port = &(init_cfg->port);
	channel = &(init_cfg->channel);

	isp_dev_id = port->isp_idx;

	isp_port_id.devId = port->isp_idx;
	isp_port_id.portId = port->port_id;

	isp_chn_id.devId = port->isp_idx;
	isp_chn_id.portId = port->port_id;
	isp_chn_id.chnId = channel->channel_idx;

	ret = VSI_MPI_ISP_DisableChn(isp_chn_id);
	if (ret) {
		LOG_ERR("Failed to disable ISP Channel!");
		return isp_err_2_errno(ret);
	}

	ret = VSI_MPI_ISP_DisableDev(isp_dev_id);
	if (ret) {
		LOG_ERR("Failed to disable ISP Device!");
		return isp_err_2_errno(ret);
	}

	ret = VSI_MPI_ISP_DisablePort(isp_port_id);
	if (ret) {
		LOG_ERR("Failed to disable ISP Port!");
		return isp_err_2_errno(ret);
	}

	return 0;
}

int isp_vsi_enqueue(struct isp_config_params *init_cfg, struct video_buffer *buf)
{
	struct channel_parameters *channel;
	struct port_parameters *port;

	ISP_CHN isp_chn_id;
	VIDEO_BUF_S isp_buf;

	size_t tmp;
	int ret;

	if (!buf || !init_cfg) {
		LOG_ERR("Invalid parameters!");
		return -EINVAL;
	}

	port = &(init_cfg->port);
	channel = &(init_cfg->channel);

	isp_chn_id.devId = port->isp_idx;
	isp_chn_id.portId = port->port_id;
	isp_chn_id.chnId = channel->channel_idx;

	isp_buf.index = 0;
	isp_buf.imageSize = buf->size;
	isp_buf.numPlanes =
		fourcc_to_numplanes(channel->output_fmt.pixelformat);
	if (isp_buf.numPlanes == 0) {
		LOG_ERR("Unsupported format");
		return -ENOTSUP;
	}

	LOG_DBG("Image size: %d, #Planes: %d, buffer local address - 0x%08x",
			buf->size, isp_buf.numPlanes, (uint32_t) buf->buffer);
	for (int i = 0; i < isp_buf.numPlanes; i++) {
		tmp = fourcc_to_plane_size(channel->output_fmt.pixelformat, i, buf->size);
		if (tmp == UINT_MAX || tmp == 0) {
			LOG_ERR("Illegal format!, plane-id: %d", i);
			return -EINVAL;
		}

		isp_buf.planes[i].dmaPhyAddr = (i) ?
			(tmp + isp_buf.planes[i - 1].dmaPhyAddr) :
			POINTER_TO_UINT(local_to_global(buf->buffer));
	}

	ret = VSI_MPI_ISP_QBUF(isp_chn_id, &isp_buf);
	if (ret) {
		LOG_ERR("Failed to Queue buffer to ISP library. "
			"Buffer address - 0x%08x", isp_buf.planes[0].dmaPhyAddr);
		return isp_err_2_errno(ret);
	}

	return 0;
}

int isp_vsi_dequeue(struct isp_config_params *init_cfg, struct video_buffer *buf)
{
	struct channel_parameters *channel;
	struct port_parameters *port;

	ISP_CHN isp_chn_id;
	VIDEO_BUF_S isp_buf;

	int ret;

	if (!buf || !init_cfg) {
		LOG_ERR("Invalid parameters!");
		return -EINVAL;
	}

	port = &(init_cfg->port);
	channel = &(init_cfg->channel);

	isp_chn_id.devId = port->isp_idx;
	isp_chn_id.portId = port->port_id;
	isp_chn_id.chnId = channel->channel_idx;

	ret = VSI_MPI_ISP_DQBUF(isp_chn_id, &isp_buf, 0);
	if (ret) {
		LOG_ERR("Failed to De-Queue buffer from ISP library");
		return isp_err_2_errno(ret);
	}

	buf->size = isp_buf.imageSize;
	buf->buffer = (uint8_t *)isp_buf.planes[0].dmaPhyAddr;

	return 0;
}
