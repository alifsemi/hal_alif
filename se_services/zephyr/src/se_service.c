/* Copyright (C) 2024  Alif Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/kernel.h>
#include <zephyr/cache.h>
#include <zephyr/drivers/ipm.h>
#include <se_service.h>
#include <soc_memory_map.h>
#include <zephyr/logging/log.h>
#include <errno.h>
LOG_MODULE_REGISTER(se_service, CONFIG_IPM_LOG_LEVEL);

#define DT_DRV_COMPAT alif_secure_enclave_services

#define CH_ID           0
#define SERVICE_TIMEOUT 10000
#define SYNC_TIMEOUT    500
/* MUTEX_TIMEOUT must be higher than SERVICE_TIMEOUT */
#define MUTEX_TIMEOUT   15000
#define MAX_TRIES       100

static K_SEM_DEFINE(svc_send_sem, 0, 1);
static K_SEM_DEFINE(svc_recv_sem, 0, 1);
static K_MUTEX_DEFINE(svc_mutex);

const struct device *send_dev;
const struct device *recv_dev;
static uint32_t se_toc_version;

/* Manufacturing data for older Ensemble Family revision <= REV_B2 */
typedef struct {
	uint8_t x_loc: 7;
	uint8_t y_loc: 7;
	uint8_t wfr_id: 5;
	uint8_t year: 6;
	uint8_t fab_id: 1;
	uint8_t week: 6;
	uint8_t lot_no: 8;
} mfg_data_v1_t;

/* Manufacturing data for Ensemble Family revision >= REV_B3 */
typedef struct {
	uint8_t year: 6;
	uint8_t zero_1: 2;
	uint8_t wfr_id: 5;
	uint8_t zero_2: 2;
	uint8_t fab_id: 1;
	uint8_t y_loc: 7;
	uint8_t zero_3: 1;
	uint8_t x_loc: 7;
	uint8_t zero_4: 1;
	uint8_t zero_5: 8;
	uint8_t zero_6: 8;
	uint8_t lot_no: 8;
	uint8_t week: 6;
	uint8_t zero_7: 2;
} mfg_data_v2_t;

typedef union {
	service_header_t service_header;
	get_rnd_svc_t get_rnd_svc_d;
	get_se_revision_t get_se_revision_svc_d;
	get_toc_number_svc_t get_toc_number_svc_d;
	get_toc_version_svc_t get_toc_version_svc_d;
	get_device_part_svc_t get_device_part_svc_d;
	otp_data_t read_otp_svc_d;
	get_device_revision_data_t get_device_revision_data_d;
	net_proc_boot_svc_t boot_svc_d;
	net_proc_shutdown_svc_t shutdown_svc_d;
	set_services_capabilities_t set_services_capabilities_d;
	aipm_get_run_profile_svc_t get_run_d;
	aipm_set_run_profile_svc_t set_run_d;
	aipm_set_off_profile_svc_t set_off_d;
	aipm_get_off_profile_svc_t get_off_d;
	control_cpu_svc_t cpu_reboot_d;
	se_sleep_svc_t se_sleep_d;
} se_service_all_svc_t;

static se_service_all_svc_t se_service_all_svc_d;
static uint32_t global_address;
static uint32_t se_service_recv_data;

/**
 * @brief Callback API to make sure MHUv2 messages are received.
 *
 * In send_msg_to_se function, the semaphore svc_recv_sem waits for
 * SYNC_TIMEOUT/SERVICE_TIMEOUT to receive MHUv2 data from Secure enclave (SE).
 * During the SYNC_TIMEOUT/SERVICE_TIMEOUT wait, the callback_for_receive_msg
 * should release the semaphore indicating that the data sent by SE has been
 * received otherwise considered as failure to receive data.
 *
 * parameters,
 * @dev         - Driver instance.
 * @user_data   - pointer to user data.
 * @id          - channel number
 * @data        - data
 */
static void callback_for_receive_msg(const struct device *dev, void *user_data,
					uint32_t id, volatile void *data)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);
	ARG_UNUSED(id);
	ARG_UNUSED(data);
	k_sem_give(&svc_recv_sem);
}
/**
 * @brief Callback API to make sure MHUv2 IPM messages are sent.
 *
 * In send_msg_to_se function, the semaphore svc_send_sem waits for
 * SYNC_TIMEOUT/SERVICE_TIMEOUT after sending MHUv2 data to Secure enclave(SE).
 * During the SYNC_TIMEOUT/SERVICE_TIMEOUT wait, the callback_for_send_msg
 * should release the semaphore indicating that SE has received the data
 * otherwise data sent is considered as failure.
 *
 * parameters,
 * @dev         - Driver instance
 * @user_data   - pointer to user data.
 * @id          - channel number
 * @data        - data
 */
static void callback_for_send_msg(const struct device *dev, void *user_data,
					uint32_t id, volatile void *data)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);
	ARG_UNUSED(id);
	ARG_UNUSED(data);
	k_sem_give(&svc_send_sem);
}

/**
 * @brief Send data to SE through MHUv2.

 * The semaphores svc_recv_sem and svc_send_sem are used with timeout
 * to make sure data is received or sent.
 *
 * parameters,
 * @ptr     - placeholder for data to be sent.
 * @size    - size of data.
 * @timeout - Timeout in milliseconds.
 *
 * returns,
 * 0      - success.
 * err    - unable to send data.
 * -ETIME - semphores are timed out.
 */
static int send_msg_to_se(uint32_t *ptr, uint32_t size, uint32_t timeout)
{
	int err;
	int service_id = ((service_header_t *)ptr)->hdr_service_id;

	global_address = local_to_global(ptr);
	__asm__ volatile("dmb 0xF" ::: "memory");
	sys_cache_data_flush_range(ptr, size);

	if (k_can_yield()) {
		int wait = 0;

		k_sem_reset(&svc_send_sem);
		k_sem_reset(&svc_recv_sem);

		/* Perform transaction in interrupt mode */
		err = ipm_send(send_dev, wait, CH_ID, &global_address, (int)size);
		if (err) {
			LOG_ERR("failed to send request for MSG(error: %d)\n", err);
			return err;
		}

		if (k_sem_take(&svc_send_sem, K_MSEC(timeout)) != 0) {
			LOG_ERR("service %d send is timed out!\n", service_id);
			return -ETIME;
		}
		if (k_sem_take(&svc_recv_sem, K_MSEC(timeout)) != 0) {
			LOG_ERR("service %d response is timed out!\n", service_id);
			return -ETIME;
		}
	} else {
		uint32_t rx_data = 0;

		/* Perform transaction in polling mode */
		/* Disable Rx MHU interrupts */
		ipm_set_enabled(recv_dev, false);

		err = ipm_poll_out(send_dev, CH_ID, &global_address,
					(int)size, K_MSEC(timeout));
		if (err) {
			LOG_ERR("failed to send service %d\n", service_id);
			return err;
		}

		err = ipm_poll_in(recv_dev, CH_ID, &rx_data,
				(int)size, K_MSEC(timeout));
		if (err) {
			LOG_ERR("failed to rcv resp for service %d\n", service_id);
			return err;
		}
		/* Enable Rx MHU interrupts */
		ipm_set_enabled(recv_dev, true);
	}

	sys_cache_data_invd_range(ptr, size);
	return 0;
}

/**
 * @brief Synchronize with SE or wait until SE wakes up by sending
 * multiple SE heartbeat service requests.
 *
 * returns,
 * 0     - On success. SE is woken up to service SE service requests.
 * errno - Unable to get an valid response from SE.
 */
int se_service_sync(void)
{
	int err, i = 0;

	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.service_header.hdr_service_id = SERVICE_MAINTENANCE_HEARTBEAT_ID;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	while (i < MAX_TRIES) {
		err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.service_header,
				     sizeof(se_service_all_svc_d.service_header), SYNC_TIMEOUT);
		if (!err) {
			break;
		}
		/* SE service timed out. Increment count */
		++i;
	}
	k_mutex_unlock(&svc_mutex);
	if (i >= MAX_TRIES) {
		LOG_ERR("Failed to synchronize with SE (errno =%d)\n", err);
		return err;
	}
	return 0;
}

/**
 * @brief Send heartbeat service request to SE to check if SE is alive.

 * Set the service id as SERVICE_MAINTENANCE_HEARTBEAT_ID in the
 * service_header and call send_msg_to_se to send the service request.
 * Use svc_mutex to avoid race condition while sending service request.
 *
 * returns,
 * 0      - success.
 * err    - if unable to send service request.
 * errno  - Unable to unlock mutex.
 */
int se_service_heartbeat(void)
{
	int err;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.service_header.hdr_service_id = SERVICE_MAINTENANCE_HEARTBEAT_ID;
	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.service_header,
			     sizeof(se_service_all_svc_d.service_header), SYNC_TIMEOUT);
	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	return 0;
}

/**
 * @brief Send service request to SE to get random number.

 * Set the service id as SERVICE_CRYPTOCELL_GET_RND in the
 * service_header, set send_rnd_length with required random number length and
 * call send_msg_to_se to send the service request. Use svc_mutex to
 * avoid race condition while sending service request.
 *
 * parameters,
 * @buffer - placeholder for random number.
 * @length - length of requested random number.
 *
 * returns,
 * 0        - success, buffer contains random numbers of length 'length'.
 * err      - if unable to send service request.
 * errno    - unable to unlock mutex.
 * resp_err - error in service response for the requested service.
 */
int se_service_get_rnd_num(uint8_t *buffer, uint16_t length)
{
	int err, resp_err = -1;

	if (!buffer) {
		LOG_ERR("Invalid argument\n");
		return -EINVAL;
	}

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.get_rnd_svc_d.header.hdr_service_id = SERVICE_CRYPTOCELL_GET_RND;
	se_service_all_svc_d.get_rnd_svc_d.send_rnd_length = length;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.get_rnd_svc_d,
			     sizeof(se_service_all_svc_d.get_rnd_svc_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.get_rnd_svc_d.resp_error_code;
	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	memcpy(buffer, (uint8_t *)se_service_all_svc_d.get_rnd_svc_d.resp_rnd, length);

	return 0;
}

/**
 * @brief Send service request to SE to get number of table of contents (TOC).

 * Set the service id as SERVICE_SYSTEM_MGMT_GET_TOC_NUMBER in the
 * service_header and call send_msg_to_se to send the service request.
 * Use svc_mutex to avoid race condition while sending service request.
 *
 * parameters,
 * @ptoc - placeholder for TOC number.
 *
 * returns,
 * 0        - success, ptoc contains number of TOC.
 * err      - if unable to send service request.
 * errno    - unable to unlock mutex.
 * resp_err - error in service response for the requested service.
 */
int se_service_get_toc_number(uint32_t *ptoc)
{
	int err, resp_err = -1;

	if (!ptoc) {
		LOG_ERR("Invalid argument\n");
		return -EINVAL;
	}

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.get_toc_number_svc_d.header.hdr_service_id =
		SERVICE_SYSTEM_MGMT_GET_TOC_NUMBER;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.get_toc_number_svc_d,
			     sizeof(se_service_all_svc_d.get_toc_number_svc_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.get_toc_number_svc_d.resp_error_code;
	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}

	*ptoc = se_service_all_svc_d.get_toc_number_svc_d.resp_number_of_toc;

	return 0;
}

/**
 * @brief Send service request to SE to get TOC Version.

 * Set the service id as SERVICE_SYSTEM_MGMT_GET_TOC_VERSION in the
 * service_header and call send_msg_to_se to send the service request.
 * Use svc_mutex to avoid race condition while sending service request.
 *
 * parameters,
 * @pversion - placeholder for TOC Version.
 *
 * returns,
 * 0        - success, ptoc contains the TOC version.
 * err      - if unable to send service request.
 * errno    - unable to unlock mutex.
 * resp_err - error in service response for the requested service.
 */
int se_service_get_toc_version(uint32_t *pversion)
{
	int err, resp_err;

	if (!pversion) {
		LOG_ERR("Invalid argument\n");
		return -EINVAL;
	}
	/* Check is TOC version readed */
	if (se_toc_version) {
		*pversion = se_toc_version;
		return 0;
	}

	err = k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT));
	if (err) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", err);
		return err;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.get_toc_version_svc_d.header.hdr_service_id =
		SERVICE_SYSTEM_MGMT_GET_TOC_VERSION;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.get_toc_version_svc_d,
			     sizeof(se_service_all_svc_d.get_toc_version_svc_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.get_toc_version_svc_d.resp_error_code;

	*pversion = se_service_all_svc_d.get_toc_version_svc_d.resp_version;
	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	/* Save Toc Version */
	se_toc_version = se_service_all_svc_d.get_toc_version_svc_d.resp_version;
	LOG_DBG("toc version: %x", se_toc_version);
	return 0;
}
/**
 * @brief Send service request to SE to get revision of SE firmware.
 *
 * Set the service id as SERVICE_APPLICATION_FIRMWARE_VERSION_ID in the
 * service_header and call send_msg_to_se to send the service request.
 * Use svc_mutex to avoid race condition while sending service request.
 *
 * parameters,
 * @prev - placeholder for SE firmware string up to VERSION_RESPONSE_LENGTH
 *        characters.
 *
 * returns,
 * 0        - success, prev contains SE firmware string.
 * err      - if unable to send service request.
 * errno    - unable to unlock mutex.
 * resp_err - error in service response for the requested service.
 */
int se_service_get_se_revision(uint8_t *prev)
{
	int err, resp_err = -1;

	if (!prev) {
		LOG_ERR("Invalid argument\n");
		return -EINVAL;
	}

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.get_se_revision_svc_d.header.hdr_service_id =
		SERVICE_APPLICATION_FIRMWARE_VERSION_ID;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.get_se_revision_svc_d,
			     sizeof(se_service_all_svc_d.get_se_revision_svc_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.get_se_revision_svc_d.resp_error_code;
	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	memcpy(prev, (uint8_t *)se_service_all_svc_d.get_se_revision_svc_d.resp_se_revision,
	       se_service_all_svc_d.get_se_revision_svc_d.resp_se_revision_length);

	return 0;
}

/**
 * @brief Send service request to SE to get device part number.
 *
 * Set the service id as SERVICE_SYSTEM_MGMT_GET_DEVICE_PART_NUMBER in the
 * service_header and call send_msg_to_se to send the service request.
 * Use svc_mutex to avoid race condition while sending service request.
 *
 * parameters,
 * @pdev_part - placeholder for device part number.
 *
 * returns,
 * 0        - success, pdev_part contains device part number.
 * err      - if unable to send service request.
 * errno    - unable to unlock mutex.
 * resp_err - error in service response for the requested service.
 */
int se_service_get_device_part_number(uint32_t *pdev_part)
{
	int err, resp_err = -1;

	if (!pdev_part) {
		LOG_ERR("Invalid argument\n");
		return -EINVAL;
	}

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.get_device_part_svc_d.header.hdr_service_id =
		SERVICE_SYSTEM_MGMT_GET_DEVICE_PART_NUMBER;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.get_device_part_svc_d,
			     sizeof(se_service_all_svc_d.get_device_part_svc_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.get_device_part_svc_d.resp_error_code;
	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	*pdev_part = se_service_all_svc_d.get_device_part_svc_d.resp_device_string;

	return 0;
}

/**
 * @brief Send service request to SE to get device data
 *
 * Set the service id as SERVICE_SYSTEM_MGMT_GET_DEVICE_REVISION_DATA in the
 * service_header and call send_msg_to_se to send the service request.
 * Use svc_mutex to avoid race condition while sending service request.
 *
 * parameters,
 * @pdev_data - placeholder for device data.
 *
 * returns,
 * 0        - success, pdev_data contains device data containing
 *            SoC revision, SoC part number, various keys,
 *            firmware version, wounding data, DCU settings,
 *            manufacturing data, serial number, SoC lifecycle state.
 * err      - if unable to send service request.
 * errno    - unable to unlock mutex.
 * resp_err - Error in service response for the requested service.
 */
int se_service_system_get_device_data(get_device_revision_data_t *pdev_data)
{
	int err, resp_err = -1;

	if (!pdev_data) {
		LOG_ERR("Invalid argument\n");
		return -EINVAL;
	}

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.get_device_revision_data_d.header.hdr_service_id =
		SERVICE_SYSTEM_MGMT_GET_DEVICE_REVISION_DATA;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.get_device_revision_data_d,
			     sizeof(se_service_all_svc_d.get_device_revision_data_d),
			     SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.get_device_revision_data_d.resp_error_code;
	if (err) {
		k_mutex_unlock(&svc_mutex);
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		k_mutex_unlock(&svc_mutex);
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	pdev_data->revision_id = se_service_all_svc_d.get_device_revision_data_d.revision_id;
	memcpy((uint8_t *)pdev_data->SerialN,
	       (uint8_t *)se_service_all_svc_d.get_device_revision_data_d.SerialN,
	       sizeof(pdev_data->SerialN));
	memcpy((uint8_t *)pdev_data->ALIF_PN,
	       (uint8_t *)se_service_all_svc_d.get_device_revision_data_d.ALIF_PN,
	       sizeof(pdev_data->ALIF_PN));
	memcpy((uint8_t *)pdev_data->HBK0,
	       (uint8_t *)se_service_all_svc_d.get_device_revision_data_d.HBK0,
	       sizeof(pdev_data->HBK0));
	memcpy((uint8_t *)pdev_data->DCU,
	       (uint8_t *)se_service_all_svc_d.get_device_revision_data_d.DCU,
	       sizeof(pdev_data->DCU));
	memcpy((uint8_t *)pdev_data->config,
	       (uint8_t *)se_service_all_svc_d.get_device_revision_data_d.config,
	       sizeof(pdev_data->config));
	memcpy((uint8_t *)pdev_data->HBK1,
	       (uint8_t *)se_service_all_svc_d.get_device_revision_data_d.HBK1,
	       sizeof(pdev_data->HBK1));
	memcpy((uint8_t *)pdev_data->HBK_FW,
	       (uint8_t *)se_service_all_svc_d.get_device_revision_data_d.HBK_FW,
	       sizeof(pdev_data->HBK_FW));
	memcpy((uint8_t *)pdev_data->MfgData,
	       (uint8_t *)se_service_all_svc_d.get_device_revision_data_d.MfgData,
	       sizeof(pdev_data->MfgData));
	memcpy((uint8_t *)pdev_data->external_config,
	       (uint8_t *)se_service_all_svc_d.get_device_revision_data_d.external_config,
	       sizeof(pdev_data->external_config));

	pdev_data->flags2 = se_service_all_svc_d.get_device_revision_data_d.flags2;
	pdev_data->LCS = se_service_all_svc_d.get_device_revision_data_d.LCS;

	k_mutex_unlock(&svc_mutex);
	return 0;
}

/**
 * @brief   Pack manufacturing data into 40 bits for EUI-64
 */
static void get_eui64_extension(mfg_data_v1_t *p_mfg_data, uint8_t *p_data_out)
{
	uint8_t seven_bits_1 = p_mfg_data->x_loc;
	uint8_t seven_bits_2 = p_mfg_data->y_loc;
	uint8_t six_bits_3 = (p_mfg_data->wfr_id << 1) | p_mfg_data->fab_id;
	uint8_t six_bits_4 = p_mfg_data->year;
	uint8_t six_bits_5 = p_mfg_data->week;
	uint8_t eight_bits = p_mfg_data->lot_no;

	/* x x x x x x x y */
	*p_data_out = (seven_bits_1 << 1) | ((seven_bits_2 & 0x40) >> 6);
	p_data_out++;
	/* y y y y y y wf wf */
	*p_data_out = ((seven_bits_2 & 0x3F) << 2) | ((six_bits_3 & 0x30) >> 4);
	p_data_out++;
	/* wf wf wf f yr yr yr yr */
	*p_data_out = ((six_bits_3 & 0x0F) << 4) | ((six_bits_4 & 0x3C) >> 2);
	p_data_out++;
	/* yr yr wk wk wk wk wk wk */
	*p_data_out = ((six_bits_4 & 0x03) << 6) | six_bits_5;
	p_data_out++;
	*p_data_out = eight_bits;
}

/**
 * @brief   Pack manufacturing data into 24 bits for EUI-48
 */
static void get_eui48_extension(mfg_data_v1_t *p_mfg_data, uint8_t *p_data_out)
{
	uint8_t six_bits_1 = p_mfg_data->x_loc & 0x3F;
	uint8_t six_bits_2 = p_mfg_data->y_loc & 0x3F;
	uint8_t six_bits_3 = (p_mfg_data->wfr_id << 1) | (p_mfg_data->lot_no & 0x1);
	uint8_t six_bits_4 = p_mfg_data->week;

	/* x x x x x x y y */
	*p_data_out = (six_bits_1 << 2) | ((six_bits_2 & 0x30) >> 4);
	p_data_out++;
	/* y y y y wf wf wf wf */
	*p_data_out = ((six_bits_2 & 0x0F) << 4) | ((six_bits_3 & 0x3C) >> 2);
	p_data_out++;
	/* wf lt wk wk wk wk wk wk */
	*p_data_out = ((six_bits_3 & 0x03) << 6) | six_bits_4;
}

static void se_service_manufacture_data_parse(get_device_revision_data_t *device_data,
					      mfg_data_v1_t *mfg_data)
{
	if (IS_ENABLED(CONFIG_SOC_FAMILY_ENSEMBLE) && device_data->revision_id < 0x0000b300) {
		mfg_data_v1_t *p_mfg_data = (mfg_data_v1_t *)device_data->MfgData;

		mfg_data->fab_id = p_mfg_data->fab_id;
		mfg_data->lot_no = p_mfg_data->lot_no;
		mfg_data->week = p_mfg_data->week;
		mfg_data->wfr_id = p_mfg_data->wfr_id;
		mfg_data->x_loc = p_mfg_data->x_loc;
		mfg_data->y_loc = p_mfg_data->y_loc;
		mfg_data->year = p_mfg_data->year;
	} else {
		/* Ensemble(rev>=B3) families use new Manufacture data model */
		mfg_data_v2_t *p_mfg_data = (mfg_data_v2_t *)device_data->MfgData;

		mfg_data->fab_id = p_mfg_data->fab_id;
		mfg_data->lot_no = p_mfg_data->lot_no;
		mfg_data->week = p_mfg_data->week;
		mfg_data->wfr_id = p_mfg_data->wfr_id;
		mfg_data->x_loc = p_mfg_data->x_loc;
		mfg_data->y_loc = p_mfg_data->y_loc;
		mfg_data->year = p_mfg_data->year;
	}
}

/**
 * @brief Calculate unique extension values for EUI-48 or EUI-64
 *
 * @fn    uint32_t se_system_get_eui_extension(
 *                                bool is_eui48,
 *                                uint8_t * eui_extension)
 *
 * @param is_eui48       Specify whether EUI-48 or EUI-64 extension is requested
 * @param eui_extension  Buffer to store the calculated extension
 * @return
 */
int se_system_get_eui_extension(bool is_eui48, uint8_t *eui_extension)
{
	get_device_revision_data_t device_data;
	mfg_data_v1_t p_mfg_dat;
	int ret;

	ret = se_service_system_get_device_data(&device_data);
	if (ret) {
		return ret;
	}

	se_service_manufacture_data_parse(&device_data, &p_mfg_dat);

	/* Use Manufactured data  */
	if (is_eui48) {
		get_eui48_extension(&p_mfg_dat, eui_extension);
	} else {
		get_eui64_extension(&p_mfg_dat, eui_extension);
	}

	return ret;
}

/**
 * @brief Send service request to SE to boot ES0
 *
 * At boot the ES0 is not started automatically.
 * This API function will start the core but it is encouraged not to call this
 * from application, separate power manager library should be used instead.
 *
 * parameters,
 * @nvds_buff - placeholder for NVDS data.
 * @nvds_size - length of the passed NVDS buff
 * @clock_select - ES0 uart and main clock selection
 *
 * returns,
 * 0        - success.
 * err      - if unable to send service request.
 * errno    - unable to unlock mutex.
 * resp_err - Error in service response for the requested service.
 */
int se_service_boot_es0(uint8_t *nvds_buff, uint16_t nvds_size, uint32_t clock_select)
{
	int err, resp_err;
	uint32_t version;

	err = se_service_get_toc_version(&version);
	if (err) {
		return err;
	}

	err = k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT));
	if (err) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", err);
		return err;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));

	se_service_all_svc_d.boot_svc_d.header.hdr_service_id = SERVICE_EXTSYS0_BOOT_SET_ARGS;
	se_service_all_svc_d.boot_svc_d.send_nvds_src_addr = local_to_global(nvds_buff);
	se_service_all_svc_d.boot_svc_d.send_nvds_dst_addr = 0x501D0000;
	se_service_all_svc_d.boot_svc_d.send_nvds_copy_len = nvds_size;
	se_service_all_svc_d.boot_svc_d.send_trng_dst_addr = 0x501D0200;
	se_service_all_svc_d.boot_svc_d.send_trng_len = 64;
	if (version > 0x01650000) {
		/* additional fields are added only when SE supports it */
		se_service_all_svc_d.boot_svc_d.send_internal_clock_select = clock_select;
	}

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.boot_svc_d,
			     sizeof(se_service_all_svc_d.boot_svc_d), SERVICE_TIMEOUT);

	resp_err = se_service_all_svc_d.boot_svc_d.resp_error_code;

	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	return 0;
}

/**
 * @brief Send service request to SE to shutdown ES0
 *
 * ES0 is started using se_service_boot_es0 and when the application does not need
 * the services of ES0 anymore it should be shutdown to save power.
 * This API function will shutdown the core but it is encouraged not to call this
 * from application, separate power manager library should be used instead.
 *
 * parameters,
 * @nvds_buff - placeholder for NVDS data.
 * @nvds_size - length of the passed NVDS buff
 *
 * returns,
 * 0        - success.
 * err      - if unable to send service request.
 * errno    - unable to unlock mutex.
 * resp_err - Error in service response for the requested service.
 */
int se_service_shutdown_es0(void)
{
	int err, resp_err = -1;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.shutdown_svc_d.header.hdr_service_id = SERVICE_EXTSYS0_SHUTDOWN;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.shutdown_svc_d,
			     sizeof(se_service_all_svc_d.shutdown_svc_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.shutdown_svc_d.resp_error_code;

	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	return 0;
}

int se_service_get_run_cfg(run_profile_t *pp)
{
	int err, resp_err = -1;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}

	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.get_run_d.header.hdr_service_id = SERVICE_POWER_GET_RUN_REQ_ID;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.get_run_d,
			     sizeof(se_service_all_svc_d.get_run_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.get_run_d.resp_error_code;

	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		k_mutex_unlock(&svc_mutex);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		k_mutex_unlock(&svc_mutex);
		return resp_err;
	}

	pp->aon_clk_src = se_service_all_svc_d.get_run_d.resp_aon_clk_src;
	pp->run_clk_src = se_service_all_svc_d.get_run_d.resp_run_clk_src;
	pp->cpu_clk_freq = se_service_all_svc_d.get_run_d.resp_cpu_clk_freq;
	pp->scaled_clk_freq = se_service_all_svc_d.get_run_d.resp_scaled_clk_freq;
	pp->dcdc_mode = se_service_all_svc_d.get_run_d.resp_dcdc_mode;
	pp->dcdc_voltage = se_service_all_svc_d.get_run_d.resp_dcdc_voltage;
	pp->memory_blocks = se_service_all_svc_d.get_run_d.resp_memory_blocks;
	pp->ip_clock_gating = se_service_all_svc_d.get_run_d.resp_ip_clock_gating;
	pp->phy_pwr_gating = se_service_all_svc_d.get_run_d.resp_phy_pwr_gating;
	pp->power_domains = se_service_all_svc_d.get_run_d.resp_power_domains;
	pp->vdd_ioflex_3V3 = se_service_all_svc_d.get_run_d.resp_vdd_ioflex_3V3;
	k_mutex_unlock(&svc_mutex);

	return 0;
}

int se_service_set_run_cfg(run_profile_t *pp)
{
	int err, resp_err = -1;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));

	se_service_all_svc_d.set_run_d.header.hdr_service_id = SERVICE_POWER_SET_RUN_REQ_ID;
	se_service_all_svc_d.set_run_d.send_aon_clk_src = pp->aon_clk_src;
	se_service_all_svc_d.set_run_d.send_run_clk_src = pp->run_clk_src;
	se_service_all_svc_d.set_run_d.send_cpu_clk_freq = pp->cpu_clk_freq;
	se_service_all_svc_d.set_run_d.send_scaled_clk_freq = pp->scaled_clk_freq;
	se_service_all_svc_d.set_run_d.send_dcdc_mode = pp->dcdc_mode;
	se_service_all_svc_d.set_run_d.send_dcdc_voltage = pp->dcdc_voltage;
	se_service_all_svc_d.set_run_d.send_memory_blocks = pp->memory_blocks;
	se_service_all_svc_d.set_run_d.send_ip_clock_gating = pp->ip_clock_gating;
	se_service_all_svc_d.set_run_d.send_phy_pwr_gating = pp->phy_pwr_gating;
	se_service_all_svc_d.set_run_d.send_power_domains = pp->power_domains;
	se_service_all_svc_d.set_run_d.send_vdd_ioflex_3V3 = pp->vdd_ioflex_3V3;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.set_run_d,
			     sizeof(se_service_all_svc_d.set_run_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.set_run_d.resp_error_code;

	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	return 0;
}

int se_service_get_off_cfg(off_profile_t *wp)
{
	int err, resp_err = -1;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.get_off_d.header.hdr_service_id = SERVICE_POWER_GET_OFF_REQ_ID;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.get_off_d,
			     sizeof(se_service_all_svc_d.get_off_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.get_off_d.resp_error_code;

	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		k_mutex_unlock(&svc_mutex);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		k_mutex_unlock(&svc_mutex);
		return resp_err;
	}

	wp->dcdc_voltage = se_service_all_svc_d.get_off_d.resp_dcdc_voltage;
	wp->memory_blocks = se_service_all_svc_d.get_off_d.resp_memory_blocks;
	wp->power_domains = se_service_all_svc_d.get_off_d.resp_power_domains;
	wp->aon_clk_src = se_service_all_svc_d.get_off_d.resp_aon_clk_src;
	wp->stby_clk_src = se_service_all_svc_d.get_off_d.resp_stby_clk_src;
	wp->stby_clk_freq = se_service_all_svc_d.get_off_d.resp_stby_clk_freq;
	wp->ip_clock_gating = se_service_all_svc_d.get_off_d.resp_ip_clock_gating;
	wp->phy_pwr_gating = se_service_all_svc_d.get_off_d.resp_phy_pwr_gating;
	wp->vdd_ioflex_3V3 = se_service_all_svc_d.get_off_d.resp_vdd_ioflex_3V3;
	wp->vtor_address = se_service_all_svc_d.get_off_d.resp_vtor_address;
	wp->vtor_address_ns = se_service_all_svc_d.get_off_d.resp_vtor_address_ns;
	wp->wakeup_events = se_service_all_svc_d.get_off_d.resp_wakeup_events;
	wp->ewic_cfg = se_service_all_svc_d.get_off_d.resp_ewic_cfg;
	k_mutex_unlock(&svc_mutex);
	return 0;
}

int se_service_set_off_cfg(off_profile_t *wp)
{
	int err, resp_err = -1;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.set_off_d.header.hdr_service_id = SERVICE_POWER_SET_OFF_REQ_ID;
	se_service_all_svc_d.set_off_d.send_dcdc_voltage = wp->dcdc_voltage;
	se_service_all_svc_d.set_off_d.send_memory_blocks = wp->memory_blocks;
	se_service_all_svc_d.set_off_d.send_power_domains = wp->power_domains;
	se_service_all_svc_d.set_off_d.send_aon_clk_src = wp->aon_clk_src;
	se_service_all_svc_d.set_off_d.send_stby_clk_src = wp->stby_clk_src;
	se_service_all_svc_d.set_off_d.send_stby_clk_freq = wp->stby_clk_freq;
	se_service_all_svc_d.set_off_d.send_ip_clock_gating = wp->ip_clock_gating;
	se_service_all_svc_d.set_off_d.send_phy_pwr_gating = wp->phy_pwr_gating;
	se_service_all_svc_d.set_off_d.send_vdd_ioflex_3V3 = wp->vdd_ioflex_3V3;
	se_service_all_svc_d.set_off_d.send_vtor_address = wp->vtor_address;
	se_service_all_svc_d.set_off_d.send_vtor_address_ns = wp->vtor_address_ns;
	se_service_all_svc_d.set_off_d.send_wakeup_events = wp->wakeup_events;
	se_service_all_svc_d.set_off_d.send_ewic_cfg = wp->ewic_cfg;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.set_off_d,
			     sizeof(se_service_all_svc_d.set_off_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.set_off_d.resp_error_code;

	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	return 0;
}

int se_service_se_sleep_req(uint32_t param)
{
	int err, resp_err = -1;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));

	se_service_all_svc_d.se_sleep_d.send_param = param;
	se_service_all_svc_d.se_sleep_d.header.hdr_service_id = SERVICE_POWER_SE_SLEEP_REQ_ID;

	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.se_sleep_d,
				sizeof(se_service_all_svc_d.se_sleep_d), SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.se_sleep_d.resp_error_code;

	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}
	return 0;
}

int se_service_system_set_services_debug(bool debug_enable)
{
	int err, resp_err = -1;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));

	se_service_all_svc_d.set_services_capabilities_d.header.hdr_service_id =
		SERVICE_POWER_SET_OFF_REQ_ID;
	se_service_all_svc_d.set_services_capabilities_d.send_services_debug = debug_enable;
	err = send_msg_to_se((uint32_t *)&se_service_all_svc_d.set_services_capabilities_d,
			     sizeof(se_service_all_svc_d.set_services_capabilities_d),
			     SERVICE_TIMEOUT);
	resp_err = se_service_all_svc_d.set_services_capabilities_d.resp_error_code;

	k_mutex_unlock(&svc_mutex);
	if (err) {
		LOG_ERR("%s failed with %d\n", __func__, err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("%s: received response error = %d\n", __func__, resp_err);
		return resp_err;
	}

	return 0;
}

int se_service_boot_reset_soc(void)
{
	int err, i = 0;

	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.service_header.hdr_service_id =
					SERVICE_BOOT_RESET_SOC;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	while (i < MAX_TRIES) {
		err = send_msg_to_se((uint32_t *)
			&se_service_all_svc_d.service_header,
			sizeof(se_service_all_svc_d.service_header),
			SERVICE_TIMEOUT);
		if (!err)
			break;
		/* SE service timed out. Increment count */
		++i;
	}
	k_mutex_unlock(&svc_mutex);
	if (i >= MAX_TRIES) {
		LOG_ERR("Failed to soc reset SoC with SE (errno =%d)\n", err);
		return err;
	}
	return 0;
}

int se_service_boot_reset_cpu(uint32_t cpu_id)
{
	int err, i = 0, resp_err = -1;

	memset(&se_service_all_svc_d, 0, sizeof(se_service_all_svc_d));
	se_service_all_svc_d.cpu_reboot_d.header.hdr_service_id =
					SERVICE_BOOT_RESET_CPU;

	se_service_all_svc_d.cpu_reboot_d.send_cpu_id = cpu_id;

	if (k_mutex_lock(&svc_mutex, K_MSEC(MUTEX_TIMEOUT))) {
		LOG_ERR("Unable to lock mutex (errno = %d)\n", errno);
		return errno;
	}
	while (i < MAX_TRIES) {
		err = send_msg_to_se((uint32_t *)
			&se_service_all_svc_d.service_header,
			sizeof(se_service_all_svc_d.service_header),
			SERVICE_TIMEOUT);
		if (!err)
			break;
		/* SE service timed out. Increment count */
		++i;
	}
	resp_err = se_service_all_svc_d.cpu_reboot_d.resp_error_code;
	k_mutex_unlock(&svc_mutex);
	if (i >= MAX_TRIES) {
		LOG_ERR("Failed to soc reset cpu with SE (errno =%d)\n", err);
		return err;
	}
	if (resp_err) {
		LOG_ERR("received response error = %d\n", resp_err);
		return resp_err;
	}
	return 0;
}

/**
 * @brief Check the MHUv2 devices are ready and initialize callbacks for
 * the received and send data.
 *
 * returns,
 * 0       - success.
 * -ENODEV - if the MHUv2 devices are not ready.
 */
static int se_service_mhuv2_nodes_init(void)
{
	send_dev = DEVICE_DT_GET_OR_NULL(DT_PHANDLE(DT_NODELABEL(se_service), mhuv2_send_node));
	recv_dev = DEVICE_DT_GET_OR_NULL(DT_PHANDLE(DT_NODELABEL(se_service), mhuv2_recv_node));

	if (!device_is_ready(recv_dev) || !device_is_ready(send_dev)) {
		printk("MHU devices not ready\n");
		return -ENODEV;
	}

	ipm_register_callback(recv_dev, callback_for_receive_msg, &se_service_recv_data);
	ipm_register_callback(send_dev, callback_for_send_msg, NULL);

	ipm_set_enabled(recv_dev, true);

	return 0;
}

SYS_INIT(se_service_mhuv2_nodes_init, PRE_KERNEL_1, CONFIG_SE_SERVICE_INIT_PRIORITY);
