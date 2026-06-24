/*
 * Copyright (C) 2025 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <stdarg.h>

#include "isp_conf.h"
#include "isp_param_conf.h"

#if defined(CONFIG_ISP_LIB_AE_MODULE)
#include "sensor_attributes.h"
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

#include <zephyr/drivers/video.h>
#include <zephyr/drivers/video-controls.h>
#include <soc_memory_map.h>

#include <zephyr/drivers/video/isp-vsi.h>
#include <zephyr/drivers/video/video_alif.h>

/* ISP lib includes */
#include "vsi_comm_isp.h"
#include "vsios_isp.h"

#include "mpi_isp_calib.h"
#include "mpi_isp.h"
#include "vsi_comm_awb.h"
#include "vsi_comm_sns.h"

#if defined(CONFIG_ISP_LIB_BLS_MODULE)
#include "mpi_isp_bls.h"
#endif
#if defined(CONFIG_ISP_LIB_EXPOSUREM_MODULE)
#include "mpi_isp_expm.h"
#endif
#if defined(CONFIG_ISP_LIB_AE_MODULE)
#include "mpi_isp_ae.h"
#endif
#if defined(CONFIG_ISP_LIB_WB_MODULE)
#include "mpi_isp_wb.h"
#endif
#if defined(CONFIG_ISP_LIB_WBM_MODULE)
#include "mpi_isp_wbm.h"
#endif
#if defined(CONFIG_ISP_LIB_DMSC_MODULE)
#include "mpi_isp_dmsc.h"
#endif
#if defined(CONFIG_ISP_LIB_FLT_MODULE)
#include "mpi_isp_flt.h"
#endif
#if defined(CONFIG_ISP_LIB_CCM_MODULE)
#include "mpi_isp_ccm.h"
#endif
#if defined(CONFIG_ISP_LIB_GAMMAOUT_MODULE)
#include "mpi_isp_gamma_out.h"
#endif
#if defined(CONFIG_ISP_LIB_CSM_MODULE)
#include "mpi_isp_csm.h"
#endif

#include "vsios_log.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(isp_wrapper, CONFIG_VIDEO_LOG_LEVEL);

#if defined(CONFIG_ISP_LIB_WB_MODULE)
extern ISP_AWB_FUNC_S vsiAwbAlgo;
#endif /* defined(CONFIG_ISP_LIB_WB_MODULE) */

#if defined(CONFIG_ISP_LIB_AE_MODULE)
extern ISP_AE_FUNC_S vsiAeAlgo;
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

extern int isp_vsi_init(struct isp_config_params *init_cfg);
extern int isp_vsi_update_cfg(struct isp_config_params *init_cfg);
extern int isp_vsi_uninit(struct isp_config_params *init_cfg);
extern void isp_vsi_bottom_half(const struct device *dev,
		struct isp_config_params *init_cfg, uint32_t mi_mis);
extern int isp_vsi_start(struct isp_config_params *init_cfg);
extern int isp_vsi_stop(struct isp_config_params *init_cfg);
extern int isp_vsi_enqueue(struct isp_config_params *init_cfg,
		struct video_buffer *buf);
extern int isp_vsi_dequeue(struct isp_config_params *init_cfg,
		struct video_buffer *buf);
extern void VSI_ISP_IrqProcessFrameEnd(ISP_PORT IspPort);

#if defined(CONFIG_ISP_LIB_AE_MODULE)
static struct sensor_config {
	vsi_u32_t aGain;
	vsi_u32_t dGain;
	vsi_u32_t intLine;
} sns_config = {0}, cached_sns_config = {0};
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

extern int log_level(void);

static int isp_lib_log_print(const char *fmt, ...)
{
	char buf[512];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	LOG_INF("%s", buf);
	return 0;
}

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
	case VIDEO_PIX_FMT_GREY:
		return ISP_MODE_RAW;
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
		/*
		 * This should have been PIXEL_FORMAT_RAW10 and Grey, but lib does not support it.
		 * TODO: Fill-in correct equivalent format for RAW8/RAW10 and RAW12
		 */
		return PIXEL_FORMAT_BGGR8;
	case VIDEO_PIX_FMT_Y10P:
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

#if defined(CONFIG_ISP_LIB_AE_MODULE)
static int vsi_get_ae_default(ISP_PORT IspPort, AE_SNS_DEFAULT_S *pAeSnsDft)
{
	ARG_UNUSED(IspPort);

	if (!pAeSnsDft) {
		return -EFAULT;
	}

	memcpy(pAeSnsDft, &sensor_attributes, sizeof(sensor_attributes));

	pAeSnsDft->linesPer500ms = (vsi_u32_t)(
		(vsi_u64_t)pAeSnsDft->fullLines * pAeSnsDft->fps / (2 * ISP_SNS_FPS_ACCU));

	return 0;
}

static int vsi_int_time_update(ISP_PORT IspPort, vsi_u32_t *pIntLine)
{
	sns_config.intLine = *pIntLine;

	return 0;
}

static int vsi_gain_update(ISP_PORT IspPort, vsi_u32_t *pAgain, vsi_u32_t *pDgain)
{
	sns_config.aGain = *pAgain;
	sns_config.dGain = *pDgain;

	return 0;
}

static AE_SNS_FUNC_S aeSnsFunc = {
	.pfnGetAeDefault = vsi_get_ae_default,
	.pfnIntTimeUpdate = vsi_int_time_update,
	.pfnGainUpdate = vsi_gain_update,
};
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

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

	VsiLogLevelSet(&log_level, &isp_lib_log_print);

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
	ret = VSI_MPI_ISP_AwbRegCallBack(isp_port_id, &vsiAwbAlgo);
	if (ret) {
		LOG_ERR("AWB Algorithm setup in library failed");
		return isp_err_2_errno(ret);
	}
#endif /* defined(CONFIG_ISP_LIB_WB_MODULE) */

#if defined(CONFIG_ISP_LIB_AE_MODULE)
	ret = VSI_MPI_ISP_InitAeSnsFunc(isp_port_id, &aeSnsFunc);
	if (ret) {
		LOG_ERR("AE Sensor function setup in library failed");
		return isp_err_2_errno(ret);
	}

	ret = VSI_MPI_ISP_AeRegCallBack(isp_port_id, &vsiAeAlgo);
	if (ret) {
		LOG_ERR("AE Algorithm setup in library failed");
		return isp_err_2_errno(ret);
	}
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

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

	/* Configure the ISP as per the calibration parameters */
	ret = VSI_MPI_ISP_SetCalib(isp_port_id, &isp_calib_param);
	if (ret) {
		LOG_ERR("Programming ISP calibration data in library failed!");
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

#if defined(CONFIG_ISP_LIB_AE_MODULE)
	ret = VSI_MPI_ISP_AeUnRegCallBack(isp_port_id);
	if (ret) {
		LOG_ERR("Failed to Un-register AE call-back!");
		return isp_err_2_errno(ret);
	}
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

	ret = VSI_MPI_ISP_Exit(isp_dev_id);
	if (ret) {
		LOG_ERR("Failed to unregister ISP!");
		return isp_err_2_errno(ret);
	}
	return 0;
}

void isp_vsi_bottom_half(const struct device *dev,
		struct isp_config_params *init_cfg, uint32_t mi_mis)
{
	struct port_parameters *port;
	ISP_PORT isp_port_id;
#if defined(CONFIG_ISP_LIB_AE_MODULE)
	ISP_EXPOSURE_INFO_S info;
	vsi_u32_t totalGain;
	int ret;
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

	port = &(init_cfg->port);

	isp_port_id.devId = port->isp_idx;
	isp_port_id.portId = port->port_id;

	VSI_ISP_IrqProcessFrameEnd(isp_port_id);

#if defined(CONFIG_ISP_LIB_AE_MODULE)
	if ((cached_sns_config.aGain != sns_config.aGain) ||
	    (cached_sns_config.dGain != sns_config.dGain)) {
		totalGain = ((vsi_u64_t)sns_config.aGain *
				sns_config.dGain) / ISP_SNS_GAIN_ACCU;

		ret = video_set_ctrl(dev, VIDEO_CID_GAIN, (void *)&totalGain);
		if (ret) {
			LOG_ERR("Failed to write Total Gain to the sensor!");
		} else {
			cached_sns_config.aGain = sns_config.aGain;
			cached_sns_config.dGain = sns_config.dGain;
		}
	}
	if (cached_sns_config.intLine != sns_config.intLine) {
		ret = video_set_ctrl(dev, VIDEO_CID_EXPOSURE,
				(void *)&sns_config.intLine);
		if (ret) {
			LOG_ERR("Failed to write Exposure to the sensor!");
		} else {
			cached_sns_config.intLine = sns_config.intLine;
		}
	}

	if (init_cfg->ae_status_cb) {
		ret = VSI_MPI_ISP_QueryExposureInfo(isp_port_id, &info);
		if (ret) {
			LOG_ERR("Failed to Query Exposure Info");
		} else {
			init_cfg->ae_status_cb(dev, info.isStable,
					init_cfg->ae_status_user_data);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */
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
		if (!i) {
			isp_buf.planes[i].dmaPhyAddr =
				POINTER_TO_UINT(local_to_global(buf->buffer));
		} else {
			tmp = fourcc_to_plane_size(channel->output_fmt.pixelformat,
					i - 1,
					buf->size);
			if (tmp == UINT_MAX || tmp == 0) {
				LOG_ERR("Illegal format!, plane-id: %d", i);
				return -EINVAL;
			}

			isp_buf.planes[i].dmaPhyAddr =
				(tmp + isp_buf.planes[i - 1].dmaPhyAddr);
		}
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

/* TODO: Modularize the following function implementation. */
int isp_vsi_set_param(struct isp_config_params *init_cfg,
		      const struct isp_params *params)
{
	ISP_PORT isp_port_id;
	struct port_parameters *port;
	int ret = 0;

	if (!init_cfg || !params) {
		return -EINVAL;
	}

	port = &init_cfg->port;
	isp_port_id.devId  = port->isp_idx;
	isp_port_id.portId = port->port_id;

#if defined(CONFIG_ISP_LIB_AE_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_AE) {
		const struct isp_ae_param *ae = &params->ae;
		ISP_EXPOSURE_ATTR_S exp_attr = {};

		exp_attr.opType = (ae->op_mode == ISP_OP_AUTO) ?
				  OP_TYPE_AUTO : OP_TYPE_MANUAL;
		exp_attr.manualAttr.intTime        = ae->int_time;
		exp_attr.manualAttr.again          = ae->again;
		exp_attr.manualAttr.dgain          = ae->dgain;
		exp_attr.autoAttr.expTimeRange.min = ae->int_time_min;
		exp_attr.autoAttr.expTimeRange.max = ae->int_time_max;
		exp_attr.autoAttr.againRange.min   = ae->again_min;
		exp_attr.autoAttr.againRange.max   = ae->again_max;
		exp_attr.autoAttr.dgainRange.min   = ae->dgain_min;
		exp_attr.autoAttr.dgainRange.max   = ae->dgain_max;
		exp_attr.autoAttr.aeTarget         = ae->ae_target;
		exp_attr.autoAttr.dampOver         = ae->damp_over;
		exp_attr.autoAttr.dampUnder        = ae->damp_under;
		exp_attr.autoAttr.tolerance        = ae->tolerance;
		exp_attr.autoAttr.aeRunInterval    = ae->run_interval;

		exp_attr.autoAttr.antiflicker.enable     = ae->antiflicker_enable;
		exp_attr.autoAttr.antiflicker.flickerFreq = ae->antiflicker_freq;

		exp_attr.autoAttr.gainThreshold              = ae->gain_threshold;
		exp_attr.autoAttr.aeMode                     = (ISP_AE_MODE_E)ae->ae_mode;
		exp_attr.autoAttr.aeDelayAttr.blackDelayFrame = ae->ae_delay_attr.black_delay_frame;
		exp_attr.autoAttr.aeDelayAttr.whiteDelayFrame = ae->ae_delay_attr.white_delay_frame;

		for (int r = 0; r < 5; r++) {
			for (int c = 0; c < 5; c++) {
				exp_attr.autoAttr.weight[r][c] = ae->weight[r][c];
			}
		}

		exp_attr.autoAttr.aeRoute.totalNum =
			MIN(ae->ae_route_total_nodes, ISP_AE_ROUTE_MAX_NODES);
		for (int i = 0; i < ae->ae_route_total_nodes; i++) {
			exp_attr.autoAttr.aeRoute.routeNode[i].intTime =
				ae->ae_route[i].int_time;
			exp_attr.autoAttr.aeRoute.routeNode[i].again =
				ae->ae_route[i].again;
			exp_attr.autoAttr.aeRoute.routeNode[i].dgain =
				ae->ae_route[i].dgain;
		}

		ret = VSI_MPI_ISP_SetExposureAttr(isp_port_id, &exp_attr);
		if (ret) {
			LOG_ERR("Failed to set AE attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

#if defined(CONFIG_ISP_LIB_WB_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_WB) {
		const struct isp_wb_param *wb = &params->wb;
		ISP_WB_ATTR_S wb_attr = {};

		wb_attr.enable = wb->enable;
		wb_attr.opType = (wb->op_mode == ISP_OP_AUTO) ?
				 OP_TYPE_AUTO : OP_TYPE_MANUAL;
		wb_attr.manualAttr.wbGain.rGain  = wb->r_gain;
		wb_attr.manualAttr.wbGain.grGain = wb->gr_gain;
		wb_attr.manualAttr.wbGain.gbGain = wb->gb_gain;
		wb_attr.manualAttr.wbGain.bGain  = wb->b_gain;

		wb_attr.autoAttr.runInterval    = wb->run_interval;
		wb_attr.autoAttr.speed          = wb->speed;
		wb_attr.autoAttr.tolerance      = wb->tolerance;
		wb_attr.autoAttr.initColorTemp  = wb->init_color_temp;

		wb_attr.autoAttr.calibParam.centLine.rgParam   =
			wb->calib.center_line.rg_param;
		wb_attr.autoAttr.calibParam.centLine.bgParam   =
			wb->calib.center_line.bg_param;
		wb_attr.autoAttr.calibParam.centLine.distParam =
			wb->calib.center_line.dist_param;
		wb_attr.autoAttr.calibParam.rgMin = wb->calib.rg_min;
		wb_attr.autoAttr.calibParam.rgMax = wb->calib.rg_max;

		for (int i = 0; i < 16; i++) {
			wb_attr.autoAttr.calibParam.wpRange0.wpLCurve.rg[i]   =
				wb->calib.wp_range0.left.rg[i];
			wb_attr.autoAttr.calibParam.wpRange0.wpLCurve.dist[i] =
				wb->calib.wp_range0.left.dist[i];
			wb_attr.autoAttr.calibParam.wpRange0.wpRCurve.rg[i]   =
				wb->calib.wp_range0.right.rg[i];
			wb_attr.autoAttr.calibParam.wpRange0.wpRCurve.dist[i] =
				wb->calib.wp_range0.right.dist[i];
			wb_attr.autoAttr.calibParam.wpRange1.wpLCurve.rg[i]   =
				wb->calib.wp_range1.left.rg[i];
			wb_attr.autoAttr.calibParam.wpRange1.wpLCurve.dist[i] =
				wb->calib.wp_range1.left.dist[i];
			wb_attr.autoAttr.calibParam.wpRange1.wpRCurve.rg[i]   =
				wb->calib.wp_range1.right.rg[i];
			wb_attr.autoAttr.calibParam.wpRange1.wpRCurve.dist[i] =
				wb->calib.wp_range1.right.dist[i];
		}

		for (int i = 0; i < ISP_ILLUMINANT_COUNT; i++) {
			wb_attr.autoAttr.calibParam.illuminant[i].illuType =
				(ISP_ILLUMINANT_TYPE_E)wb->calib.illuminant[i].illu_type;
			wb_attr.autoAttr.calibParam.illuminant[i].colorTemp =
				wb->calib.illuminant[i].color_temp;
			wb_attr.autoAttr.calibParam.illuminant[i].wbGain.rGain  =
				wb->calib.illuminant[i].r_gain;
			wb_attr.autoAttr.calibParam.illuminant[i].wbGain.grGain =
				wb->calib.illuminant[i].gr_gain;
			wb_attr.autoAttr.calibParam.illuminant[i].wbGain.gbGain =
				wb->calib.illuminant[i].gb_gain;
			wb_attr.autoAttr.calibParam.illuminant[i].wbGain.bGain  =
				wb->calib.illuminant[i].b_gain;
		}

		ret = VSI_MPI_ISP_SetWbAttr(isp_port_id, &wb_attr);
		if (ret) {
			LOG_ERR("Failed to set WB attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_WB_MODULE) */

#if defined(CONFIG_ISP_LIB_BLS_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_BLS) {
		const struct isp_bls_param *bls = &params->bls;
		ISP_BLS_ATTR_S bls_attr = {};

		bls_attr.enable = bls->enable;
		bls_attr.opType = (bls->op_mode == ISP_OP_AUTO) ?
				  OP_TYPE_AUTO : OP_TYPE_MANUAL;
		bls_attr.manualAttr.blackLevel[0] = bls->black_level[0];
		bls_attr.manualAttr.blackLevel[1] = bls->black_level[1];
		bls_attr.manualAttr.blackLevel[2] = bls->black_level[2];
		bls_attr.manualAttr.blackLevel[3] = bls->black_level[3];

		for (int i = 0; i < ISP_AUTO_STRENGTH_NUN; i++) {
			bls_attr.autoAttr.blackLevel[i][0] =
				bls->auto_black_level[i][0];
			bls_attr.autoAttr.blackLevel[i][1] =
				bls->auto_black_level[i][1];
			bls_attr.autoAttr.blackLevel[i][2] =
				bls->auto_black_level[i][2];
			bls_attr.autoAttr.blackLevel[i][3] =
				bls->auto_black_level[i][3];
		}

		ret = VSI_MPI_ISP_SetBlsAttr(isp_port_id, &bls_attr);
		if (ret) {
			LOG_ERR("Failed to set BLS attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_BLS_MODULE) */

#if defined(CONFIG_ISP_LIB_DMSC_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_DMSC) {
		const struct isp_dmsc_param *dmsc = &params->dmsc;
		ISP_DMSC_ATTR_S dmsc_attr = {};

		dmsc_attr.enable    = dmsc->enable;
		dmsc_attr.threshold = dmsc->threshold;
		dmsc_attr.cacAttr.enable      = dmsc->cac_enable;
		dmsc_attr.cacAttr.hClipMode   = dmsc->cac_h_clip_mode;
		dmsc_attr.cacAttr.vClipMode   = dmsc->cac_v_clip_mode;
		dmsc_attr.cacAttr.hStart      = dmsc->cac_h_start;
		dmsc_attr.cacAttr.vStart      = dmsc->cac_v_start;
		dmsc_attr.cacAttr.aBlue       = dmsc->cac_a_blue;
		dmsc_attr.cacAttr.aRed        = dmsc->cac_a_red;
		dmsc_attr.cacAttr.bBlue       = dmsc->cac_b_blue;
		dmsc_attr.cacAttr.bRed        = dmsc->cac_b_red;
		dmsc_attr.cacAttr.cBlue       = dmsc->cac_c_blue;
		dmsc_attr.cacAttr.cRed        = dmsc->cac_c_red;
		dmsc_attr.cacAttr.xNormShift  = dmsc->cac_x_norm_shift;
		dmsc_attr.cacAttr.xNormFactor = dmsc->cac_x_norm_factor;
		dmsc_attr.cacAttr.yNormShift  = dmsc->cac_y_norm_shift;
		dmsc_attr.cacAttr.yNormFactor = dmsc->cac_y_norm_factor;

		ret = VSI_MPI_ISP_SetDmscAttr(isp_port_id, &dmsc_attr);
		if (ret) {
			LOG_ERR("Failed to set DMSC attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_DMSC_MODULE) */

#if defined(CONFIG_ISP_LIB_FLT_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_FLT) {
		const struct isp_flt_param *flt = &params->flt;
		ISP_FLT_ATTR_S flt_attr = {};

		flt_attr.enable  = flt->enable;
		flt_attr.opType  = (flt->op_mode == ISP_OP_AUTO) ?
				   OP_TYPE_AUTO : OP_TYPE_MANUAL;
		flt_attr.manualAttr.denoiseLevel = flt->denoise_level;
		flt_attr.manualAttr.sharpenLevel = flt->sharpen_level;

		for (int i = 0; i < ISP_AUTO_STRENGTH_NUN; i++) {
			flt_attr.autoAttr.denoiseLevel[i] =
				flt->auto_denoise_level[i];
			flt_attr.autoAttr.sharpenLevel[i] =
				flt->auto_sharpen_level[i];
		}

		ret = VSI_MPI_ISP_SetFltAttr(isp_port_id, &flt_attr);
		if (ret) {
			LOG_ERR("Failed to set FLT attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_FLT_MODULE) */

#if defined(CONFIG_ISP_LIB_CCM_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_CCM) {
		const struct isp_ccm_param *ccm = &params->ccm;
		ISP_CCM_ATTR_S ccm_attr = {};

		ccm_attr.opType = (ccm->op_mode == ISP_OP_AUTO) ?
				  OP_TYPE_AUTO : OP_TYPE_MANUAL;
		memcpy(ccm_attr.manualAttr.colorMatrix, ccm->color_matrix,
		       sizeof(ccm_attr.manualAttr.colorMatrix));
		ccm_attr.manualAttr.rOffset = ccm->r_offset;
		ccm_attr.manualAttr.gOffset = ccm->g_offset;
		ccm_attr.manualAttr.bOffset = ccm->b_offset;

		for (int i = 0; i < ISP_ILLUMINANT_COUNT; i++) {
			memcpy(ccm_attr.autoAttr.illuminantCCM[i].colorMatrix,
			       ccm->auto_ccm[i].color_matrix,
			       sizeof(ccm_attr.autoAttr.illuminantCCM[i].colorMatrix));
			ccm_attr.autoAttr.illuminantCCM[i].rOffset   =
				ccm->auto_ccm[i].r_offset;
			ccm_attr.autoAttr.illuminantCCM[i].gOffset   =
				ccm->auto_ccm[i].g_offset;
			ccm_attr.autoAttr.illuminantCCM[i].bOffset   =
				ccm->auto_ccm[i].b_offset;
			ccm_attr.autoAttr.illuminantCCM[i].colorTemp =
				ccm->auto_ccm[i].color_temp;
		}

		ret = VSI_MPI_ISP_SetCcmAttr(isp_port_id, &ccm_attr);
		if (ret) {
			LOG_ERR("Failed to set CCM attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_CCM_MODULE) */

#if defined(CONFIG_ISP_LIB_GAMMAOUT_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_GAMMA_OUT) {
		const struct isp_gamma_param *gamma = &params->gamma_out;
		ISP_GAMMA_OUT_ATTR_S gamma_attr = {};

		gamma_attr.enable = gamma->enable;
		memcpy(gamma_attr.gammaY, gamma->gamma_y,
		       sizeof(gamma_attr.gammaY));

		ret = VSI_MPI_ISP_SetGammaOutAttr(isp_port_id, &gamma_attr);
		if (ret) {
			LOG_ERR("Failed to set GammaOut attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_GAMMAOUT_MODULE) */

#if defined(CONFIG_ISP_LIB_CSM_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_CSM) {
		const struct isp_csm_param *csm = &params->csm;
		ISP_CSM_ATTR_S csm_attr = {};

		csm_attr.type         = (ISP_CSM_TYPE_E)csm->type;
		csm_attr.quantization = (ISP_CSM_QUANTIZATION_E)csm->range;
		memcpy(csm_attr.coef, csm->coef, sizeof(csm_attr.coef));

		ret = VSI_MPI_ISP_SetCsmAttr(isp_port_id, &csm_attr);
		if (ret) {
			LOG_ERR("Failed to set CSM attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_CSM_MODULE) */

#if defined(CONFIG_ISP_LIB_EXPOSUREM_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_AEM) {
		const struct isp_aem_param *aem = &params->aem;
		ISP_EXPM_ATTR_S expm_attr = {};

		expm_attr.enable         = aem->enable;
		expm_attr.expAltMode     = aem->alt_mode;
		expm_attr.blockWin.hOffs = aem->h_offs;
		expm_attr.blockWin.vOffs = aem->v_offs;
		expm_attr.blockWin.hSize = aem->h_size;
		expm_attr.blockWin.vSize = aem->v_size;

		ret = VSI_MPI_ISP_SetExpmAttr(isp_port_id, &expm_attr);
		if (ret) {
			LOG_ERR("Failed to set AEM attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_EXPOSUREM_MODULE) */

#if defined(CONFIG_ISP_LIB_WBM_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_WBM) {
		const struct isp_wbm_param *wbm = &params->wbm;
		ISP_WBM_ATTR_S wbm_attr = {};

		wbm_attr.enable             = wbm->enable;
		wbm_attr.measMode           = (ISP_WBM_MODE_E)wbm->meas_mode;
		wbm_attr.measRect.hOffs     = wbm->h_offs;
		wbm_attr.measRect.vOffs     = wbm->v_offs;
		wbm_attr.measRect.hSize     = wbm->h_size;
		wbm_attr.measRect.vSize     = wbm->v_size;
		wbm_attr.wpRange.maxY       = wbm->max_y;
		wbm_attr.wpRange.refCr_MaxR = wbm->ref_cr_max_r;
		wbm_attr.wpRange.minY_MaxG  = wbm->min_y_max_g;
		wbm_attr.wpRange.refCb_MaxB = wbm->ref_cb_max_b;
		wbm_attr.wpRange.maxCSum    = wbm->max_c_sum;
		wbm_attr.wpRange.minC       = wbm->min_c;

		ret = VSI_MPI_ISP_SetWbmAttr(isp_port_id, &wbm_attr);
		if (ret) {
			LOG_ERR("Failed to set WBM attr");
			return isp_err_2_errno(ret);
		}
	}
#endif /* defined(CONFIG_ISP_LIB_WBM_MODULE) */

	if (params->valid_mask & ISP_PARAM_MASK_AUTO_ROUTE) {
		ISP_AUTO_ROUTE_S route = {};

		for (int i = 0; i < ISP_AUTO_STRENGTH_NUN; i++) {
			route.autoRoute[i] =
				params->auto_route.auto_route[i];
		}

		ret = VSI_MPI_ISP_SetAutoRoute(isp_port_id, &route);
		if (ret) {
			LOG_ERR("Failed to set AutoRoute");
			return isp_err_2_errno(ret);
		}
	}

	return 0;
}

/* TODO: Modularize the following function implementation. */
int isp_vsi_get_param(struct isp_config_params *init_cfg,
		      struct isp_params *params)
{
	ISP_PORT isp_port_id;
	struct port_parameters *port;
	int ret = 0;

	if (!init_cfg || !params) {
		return -EINVAL;
	}

	port = &init_cfg->port;
	isp_port_id.devId  = port->isp_idx;
	isp_port_id.portId = port->port_id;

#if defined(CONFIG_ISP_LIB_AE_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_AE) {
		struct isp_ae_param *ae = &params->ae;
		ISP_EXPOSURE_ATTR_S exp_attr = {};

		ret = VSI_MPI_ISP_GetExposureAttr(isp_port_id, &exp_attr);
		if (ret) {
			LOG_ERR("Failed to get AE attr");
			return isp_err_2_errno(ret);
		}

		ae->op_mode      = (exp_attr.opType == OP_TYPE_AUTO) ?
				   ISP_OP_AUTO : ISP_OP_MANUAL;
		ae->int_time     = exp_attr.manualAttr.intTime;
		ae->again        = exp_attr.manualAttr.again;
		ae->dgain        = exp_attr.manualAttr.dgain;
		ae->int_time_min = exp_attr.autoAttr.expTimeRange.min;
		ae->int_time_max = exp_attr.autoAttr.expTimeRange.max;
		ae->again_min    = exp_attr.autoAttr.againRange.min;
		ae->again_max    = exp_attr.autoAttr.againRange.max;
		ae->dgain_min    = exp_attr.autoAttr.dgainRange.min;
		ae->dgain_max    = exp_attr.autoAttr.dgainRange.max;
		ae->ae_target    = exp_attr.autoAttr.aeTarget;
		ae->damp_over    = exp_attr.autoAttr.dampOver;
		ae->damp_under   = exp_attr.autoAttr.dampUnder;
		ae->tolerance    = exp_attr.autoAttr.tolerance;
		ae->run_interval = exp_attr.autoAttr.aeRunInterval;

		ae->antiflicker_enable = exp_attr.autoAttr.antiflicker.enable;
		ae->antiflicker_freq   = exp_attr.autoAttr.antiflicker.flickerFreq;

		ae->gain_threshold                  = exp_attr.autoAttr.gainThreshold;
		ae->ae_mode                         = (enum isp_ae_mode)exp_attr.autoAttr.aeMode;
		ae->ae_delay_attr.black_delay_frame = exp_attr.autoAttr.aeDelayAttr.blackDelayFrame;
		ae->ae_delay_attr.white_delay_frame = exp_attr.autoAttr.aeDelayAttr.whiteDelayFrame;

		for (int r = 0; r < 5; r++) {
			for (int c = 0; c < 5; c++) {
				ae->weight[r][c] = exp_attr.autoAttr.weight[r][c];
			}
		}

		ae->ae_route_total_nodes =
			MIN(exp_attr.autoAttr.aeRoute.totalNum, ISP_AE_ROUTE_MAX_NODES);
		for (int i = 0; i < exp_attr.autoAttr.aeRoute.totalNum; i++) {
			ae->ae_route[i].int_time =
				exp_attr.autoAttr.aeRoute.routeNode[i].intTime;
			ae->ae_route[i].again =
				exp_attr.autoAttr.aeRoute.routeNode[i].again;
			ae->ae_route[i].dgain =
				exp_attr.autoAttr.aeRoute.routeNode[i].dgain;
		}
	}
#endif /* defined(CONFIG_ISP_LIB_AE_MODULE) */

#if defined(CONFIG_ISP_LIB_WB_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_WB) {
		struct isp_wb_param *wb = &params->wb;
		ISP_WB_ATTR_S wb_attr = {};

		ret = VSI_MPI_ISP_GetWbAttr(isp_port_id, &wb_attr);
		if (ret) {
			LOG_ERR("Failed to get WB attr");
			return isp_err_2_errno(ret);
		}

		wb->enable  = wb_attr.enable;
		wb->op_mode = (wb_attr.opType == OP_TYPE_AUTO) ?
			      ISP_OP_AUTO : ISP_OP_MANUAL;
		wb->r_gain  = wb_attr.manualAttr.wbGain.rGain;
		wb->gr_gain = wb_attr.manualAttr.wbGain.grGain;
		wb->gb_gain = wb_attr.manualAttr.wbGain.gbGain;
		wb->b_gain  = wb_attr.manualAttr.wbGain.bGain;

		wb->run_interval    = wb_attr.autoAttr.runInterval;
		wb->speed           = wb_attr.autoAttr.speed;
		wb->tolerance       = wb_attr.autoAttr.tolerance;
		wb->init_color_temp = wb_attr.autoAttr.initColorTemp;

		wb->calib.center_line.rg_param   =
			wb_attr.autoAttr.calibParam.centLine.rgParam;
		wb->calib.center_line.bg_param   =
			wb_attr.autoAttr.calibParam.centLine.bgParam;
		wb->calib.center_line.dist_param =
			wb_attr.autoAttr.calibParam.centLine.distParam;
		wb->calib.rg_min = wb_attr.autoAttr.calibParam.rgMin;
		wb->calib.rg_max = wb_attr.autoAttr.calibParam.rgMax;

		for (int i = 0; i < 16; i++) {
			wb->calib.wp_range0.left.rg[i]   =
				wb_attr.autoAttr.calibParam.wpRange0.wpLCurve.rg[i];
			wb->calib.wp_range0.left.dist[i] =
				wb_attr.autoAttr.calibParam.wpRange0.wpLCurve.dist[i];
			wb->calib.wp_range0.right.rg[i]   =
				wb_attr.autoAttr.calibParam.wpRange0.wpRCurve.rg[i];
			wb->calib.wp_range0.right.dist[i] =
				wb_attr.autoAttr.calibParam.wpRange0.wpRCurve.dist[i];
			wb->calib.wp_range1.left.rg[i]   =
				wb_attr.autoAttr.calibParam.wpRange1.wpLCurve.rg[i];
			wb->calib.wp_range1.left.dist[i] =
				wb_attr.autoAttr.calibParam.wpRange1.wpLCurve.dist[i];
			wb->calib.wp_range1.right.rg[i]   =
				wb_attr.autoAttr.calibParam.wpRange1.wpRCurve.rg[i];
			wb->calib.wp_range1.right.dist[i] =
				wb_attr.autoAttr.calibParam.wpRange1.wpRCurve.dist[i];
		}

		for (int i = 0; i < ISP_ILLUMINANT_COUNT; i++) {
			wb->calib.illuminant[i].illu_type = (enum isp_illuminant_type)
				wb_attr.autoAttr.calibParam.illuminant[i].illuType;
			wb->calib.illuminant[i].color_temp =
				wb_attr.autoAttr.calibParam.illuminant[i].colorTemp;
			wb->calib.illuminant[i].r_gain  =
				wb_attr.autoAttr.calibParam.illuminant[i].wbGain.rGain;
			wb->calib.illuminant[i].gr_gain =
				wb_attr.autoAttr.calibParam.illuminant[i].wbGain.grGain;
			wb->calib.illuminant[i].gb_gain =
				wb_attr.autoAttr.calibParam.illuminant[i].wbGain.gbGain;
			wb->calib.illuminant[i].b_gain  =
				wb_attr.autoAttr.calibParam.illuminant[i].wbGain.bGain;
		}
	}
#endif /* defined(CONFIG_ISP_LIB_WB_MODULE) */

#if defined(CONFIG_ISP_LIB_BLS_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_BLS) {
		struct isp_bls_param *bls = &params->bls;
		ISP_BLS_ATTR_S bls_attr = {};

		ret = VSI_MPI_ISP_GetBlsAttr(isp_port_id, &bls_attr);
		if (ret) {
			LOG_ERR("Failed to get BLS attr");
			return isp_err_2_errno(ret);
		}

		bls->enable         = bls_attr.enable;
		bls->op_mode        = (bls_attr.opType == OP_TYPE_AUTO) ?
				      ISP_OP_AUTO : ISP_OP_MANUAL;
		bls->black_level[0] = bls_attr.manualAttr.blackLevel[0];
		bls->black_level[1] = bls_attr.manualAttr.blackLevel[1];
		bls->black_level[2] = bls_attr.manualAttr.blackLevel[2];
		bls->black_level[3] = bls_attr.manualAttr.blackLevel[3];

		for (int i = 0; i < ISP_AUTO_STRENGTH_NUN; i++) {
			bls->auto_black_level[i][0] =
				bls_attr.autoAttr.blackLevel[i][0];
			bls->auto_black_level[i][1] =
				bls_attr.autoAttr.blackLevel[i][1];
			bls->auto_black_level[i][2] =
				bls_attr.autoAttr.blackLevel[i][2];
			bls->auto_black_level[i][3] =
				bls_attr.autoAttr.blackLevel[i][3];
		}
	}
#endif /* defined(CONFIG_ISP_LIB_BLS_MODULE) */

#if defined(CONFIG_ISP_LIB_DMSC_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_DMSC) {
		struct isp_dmsc_param *dmsc = &params->dmsc;
		ISP_DMSC_ATTR_S dmsc_attr = {};

		ret = VSI_MPI_ISP_GetDmscAttr(isp_port_id, &dmsc_attr);
		if (ret) {
			LOG_ERR("Failed to get DMSC attr");
			return isp_err_2_errno(ret);
		}

		dmsc->enable           = dmsc_attr.enable;
		dmsc->threshold        = dmsc_attr.threshold;
		dmsc->cac_enable       = dmsc_attr.cacAttr.enable;
		dmsc->cac_h_clip_mode  = dmsc_attr.cacAttr.hClipMode;
		dmsc->cac_v_clip_mode  = dmsc_attr.cacAttr.vClipMode;
		dmsc->cac_h_start      = dmsc_attr.cacAttr.hStart;
		dmsc->cac_v_start      = dmsc_attr.cacAttr.vStart;
		dmsc->cac_a_blue       = dmsc_attr.cacAttr.aBlue;
		dmsc->cac_a_red        = dmsc_attr.cacAttr.aRed;
		dmsc->cac_b_blue       = dmsc_attr.cacAttr.bBlue;
		dmsc->cac_b_red        = dmsc_attr.cacAttr.bRed;
		dmsc->cac_c_blue       = dmsc_attr.cacAttr.cBlue;
		dmsc->cac_c_red        = dmsc_attr.cacAttr.cRed;
		dmsc->cac_x_norm_shift  = dmsc_attr.cacAttr.xNormShift;
		dmsc->cac_x_norm_factor = dmsc_attr.cacAttr.xNormFactor;
		dmsc->cac_y_norm_shift  = dmsc_attr.cacAttr.yNormShift;
		dmsc->cac_y_norm_factor = dmsc_attr.cacAttr.yNormFactor;
	}
#endif /* defined(CONFIG_ISP_LIB_DMSC_MODULE) */

#if defined(CONFIG_ISP_LIB_FLT_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_FLT) {
		struct isp_flt_param *flt = &params->flt;
		ISP_FLT_ATTR_S flt_attr = {};

		ret = VSI_MPI_ISP_GetFltAttr(isp_port_id, &flt_attr);
		if (ret) {
			LOG_ERR("Failed to get FLT attr");
			return isp_err_2_errno(ret);
		}

		flt->enable        = flt_attr.enable;
		flt->op_mode       = (flt_attr.opType == OP_TYPE_AUTO) ?
				     ISP_OP_AUTO : ISP_OP_MANUAL;
		flt->denoise_level = flt_attr.manualAttr.denoiseLevel;
		flt->sharpen_level = flt_attr.manualAttr.sharpenLevel;

		for (int i = 0; i < ISP_AUTO_STRENGTH_NUN; i++) {
			flt->auto_denoise_level[i] =
				flt_attr.autoAttr.denoiseLevel[i];
			flt->auto_sharpen_level[i] =
				flt_attr.autoAttr.sharpenLevel[i];
		}
	}
#endif /* defined(CONFIG_ISP_LIB_FLT_MODULE) */

#if defined(CONFIG_ISP_LIB_CCM_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_CCM) {
		struct isp_ccm_param *ccm = &params->ccm;
		ISP_CCM_ATTR_S ccm_attr = {};

		ret = VSI_MPI_ISP_GetCcmAttr(isp_port_id, &ccm_attr);
		if (ret) {
			LOG_ERR("Failed to get CCM attr");
			return isp_err_2_errno(ret);
		}

		ccm->op_mode = (ccm_attr.opType == OP_TYPE_AUTO) ?
			       ISP_OP_AUTO : ISP_OP_MANUAL;
		memcpy(ccm->color_matrix, ccm_attr.manualAttr.colorMatrix,
		       sizeof(ccm->color_matrix));
		ccm->r_offset = ccm_attr.manualAttr.rOffset;
		ccm->g_offset = ccm_attr.manualAttr.gOffset;
		ccm->b_offset = ccm_attr.manualAttr.bOffset;

		for (int i = 0; i < ISP_ILLUMINANT_COUNT; i++) {
			memcpy(ccm->auto_ccm[i].color_matrix,
			       ccm_attr.autoAttr.illuminantCCM[i].colorMatrix,
			       sizeof(ccm->auto_ccm[i].color_matrix));
			ccm->auto_ccm[i].r_offset   =
				ccm_attr.autoAttr.illuminantCCM[i].rOffset;
			ccm->auto_ccm[i].g_offset   =
				ccm_attr.autoAttr.illuminantCCM[i].gOffset;
			ccm->auto_ccm[i].b_offset   =
				ccm_attr.autoAttr.illuminantCCM[i].bOffset;
			ccm->auto_ccm[i].color_temp =
				ccm_attr.autoAttr.illuminantCCM[i].colorTemp;
		}
	}
#endif /* defined(CONFIG_ISP_LIB_CCM_MODULE) */

#if defined(CONFIG_ISP_LIB_GAMMAOUT_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_GAMMA_OUT) {
		struct isp_gamma_param *gamma = &params->gamma_out;
		ISP_GAMMA_OUT_ATTR_S gamma_attr = {};

		ret = VSI_MPI_ISP_GetGammaOutAttr(isp_port_id, &gamma_attr);
		if (ret) {
			LOG_ERR("Failed to get GammaOut attr");
			return isp_err_2_errno(ret);
		}

		gamma->enable = gamma_attr.enable;
		memcpy(gamma->gamma_y, gamma_attr.gammaY,
		       sizeof(gamma->gamma_y));
	}
#endif /* defined(CONFIG_ISP_LIB_GAMMAOUT_MODULE) */

#if defined(CONFIG_ISP_LIB_CSM_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_CSM) {
		struct isp_csm_param *csm = &params->csm;
		ISP_CSM_ATTR_S csm_attr = {};

		ret = VSI_MPI_ISP_GetCsmAttr(isp_port_id, &csm_attr);
		if (ret) {
			LOG_ERR("Failed to get CSM attr");
			return isp_err_2_errno(ret);
		}

		csm->type  = (enum isp_csm_type)csm_attr.type;
		csm->range = (enum isp_csm_range)csm_attr.quantization;
		memcpy(csm->coef, csm_attr.coef, sizeof(csm->coef));
	}
#endif /* defined(CONFIG_ISP_LIB_CSM_MODULE) */

#if defined(CONFIG_ISP_LIB_EXPOSUREM_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_AEM) {
		struct isp_aem_param *aem = &params->aem;
		ISP_EXPM_ATTR_S expm_attr = {};

		ret = VSI_MPI_ISP_GetExpmAttr(isp_port_id, &expm_attr);
		if (ret) {
			LOG_ERR("Failed to get AEM attr");
			return isp_err_2_errno(ret);
		}

		aem->enable   = expm_attr.enable;
		aem->alt_mode = expm_attr.expAltMode;
		aem->h_offs   = expm_attr.blockWin.hOffs;
		aem->v_offs   = expm_attr.blockWin.vOffs;
		aem->h_size   = expm_attr.blockWin.hSize;
		aem->v_size   = expm_attr.blockWin.vSize;
	}
#endif /* defined(CONFIG_ISP_LIB_EXPOSUREM_MODULE) */

#if defined(CONFIG_ISP_LIB_WBM_MODULE)
	if (params->valid_mask & ISP_PARAM_MASK_WBM) {
		struct isp_wbm_param *wbm = &params->wbm;
		ISP_WBM_ATTR_S wbm_attr = {};

		ret = VSI_MPI_ISP_GetWbmAttr(isp_port_id, &wbm_attr);
		if (ret) {
			LOG_ERR("Failed to get WBM attr");
			return isp_err_2_errno(ret);
		}

		wbm->enable       = wbm_attr.enable;
		wbm->meas_mode    = (enum isp_wbm_mode)wbm_attr.measMode;
		wbm->h_offs       = wbm_attr.measRect.hOffs;
		wbm->v_offs       = wbm_attr.measRect.vOffs;
		wbm->h_size       = wbm_attr.measRect.hSize;
		wbm->v_size       = wbm_attr.measRect.vSize;
		wbm->max_y        = wbm_attr.wpRange.maxY;
		wbm->ref_cr_max_r = wbm_attr.wpRange.refCr_MaxR;
		wbm->min_y_max_g  = wbm_attr.wpRange.minY_MaxG;
		wbm->ref_cb_max_b = wbm_attr.wpRange.refCb_MaxB;
		wbm->max_c_sum    = wbm_attr.wpRange.maxCSum;
		wbm->min_c        = wbm_attr.wpRange.minC;
	}
#endif /* defined(CONFIG_ISP_LIB_WBM_MODULE) */

	if (params->valid_mask & ISP_PARAM_MASK_AUTO_ROUTE) {
		ISP_AUTO_ROUTE_S route = {};

		ret = VSI_MPI_ISP_GetAutoRoute(isp_port_id, &route);
		if (ret) {
			LOG_ERR("Failed to get AutoRoute");
			return isp_err_2_errno(ret);
		}

		for (int i = 0; i < ISP_AUTO_STRENGTH_NUN; i++) {
			params->auto_route.auto_route[i] = route.autoRoute[i];
		}
	}

	return 0;
}
