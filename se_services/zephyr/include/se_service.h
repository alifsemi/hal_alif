#ifndef SE_SERVICES_ZEPHYR_INCLUDE_SE_SERVICE_H_
#define SE_SERVICES_ZEPHYR_INCLUDE_SE_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <services_lib_api.h>
#include <services_lib_ids.h>

/**
 * @brief Send heartbeat to check if SE is alive.
 *
 * @retval 0 Success.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 */
int se_service_heartbeat(void);

/**
 * @brief Synchronize with SE by sending multiple heartbeat requests.
 *
 * Wakes up the SE and waits until it is ready to service requests.
 *
 * @retval 0 Success. SE is ready to service requests.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 */
int se_service_sync(void);

/**
 * @brief Enable or disable SE services debug output.
 *
 * @param debug_enable true to enable debug, false to disable.
 * @retval 0 Success.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_system_set_services_debug(bool debug_enable);

/**
 * @brief Get random number from SE.
 *
 * @param buffer Pointer to buffer to store the random number.
 * @param length Length of the requested random number in bytes.
 * @retval 0 Success.
 * @retval -EINVAL @p buffer is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_get_rnd_num(uint8_t *buffer, uint16_t length);

/**
 * @brief Get number of table of contents (TOC) entries.
 *
 * @param ptoc Pointer to store the TOC count.
 * @retval 0 Success.
 * @retval -EINVAL @p ptoc is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_get_toc_number(uint32_t *ptoc);

/**
 * @brief Get SE firmware revision string.
 *
 * @param prev Buffer to store the firmware revision string
 *             (up to VERSION_RESPONSE_LENGTH characters).
 * @retval 0 Success.
 * @retval -EINVAL @p prev is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_get_se_revision(uint8_t *prev);

/**
 * @brief Get TOC version.
 *
 * The result is cached after the first successful query.
 *
 * @param pversion Pointer to store the TOC version.
 * @retval 0 Success.
 * @retval -EINVAL @p pversion is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_get_toc_version(uint32_t *pversion);

/**
 * @brief Get device part number.
 *
 * @param pdev_part Pointer to store the device part number.
 * @retval 0 Success.
 * @retval -EINVAL @p pdev_part is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_get_device_part_number(uint32_t *pdev_part);

/**
 * @brief Get device revision and identification data.
 *
 * On success, @p pdev_data contains SoC revision, part number, keys,
 * firmware version, wounding data, DCU settings, manufacturing data,
 * serial number, and SoC lifecycle state.
 *
 * @param pdev_data Pointer to store the device data.
 * @retval 0 Success.
 * @retval -EINVAL @p pdev_data is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_system_get_device_data(get_device_revision_data_t *pdev_data);

/**
 * @brief Calculate unique EUI-48 or EUI-64 extension from manufacturing data.
 *
 * Internally calls se_service_system_get_device_data() and inherits
 * its error codes.
 *
 * @param is_eui48      true for EUI-48 extension, false for EUI-64.
 * @param eui_extension Buffer to store the calculated extension.
 * @retval 0 Success.
 * @retval -EINVAL Invalid argument.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_system_get_eui_extension(bool is_eui48, uint8_t *eui_extension);

/**
 * @brief Send service request to SE to boot ES0.
 *
 * Starts the ES0 core with the given NVDS data and clock configuration.
 * It is encouraged to use the power manager library instead of calling
 * this directly from the application.
 *
 * @param nvds_buff    NVDS configuration defined by user.
 * @param nvds_size    Size of the NVDS data.
 * @param clock_select ES0 UART and main clock selection.
 * @param hpa_mode     If true, HPA mode is used, otherwise LPA mode is used.
 * @retval 0 Success.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_boot_es0(uint8_t *nvds_buff, uint16_t nvds_size, uint32_t clock_select,
			bool hpa_mode);

/**
 * @brief Send service request to SE to shutdown ES0.
 *
 * Shuts down the ES0 core previously started with se_service_boot_es0().
 * It is encouraged to use the power manager library instead of calling
 * this directly from the application.
 *
 * @retval 0 Success.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_shutdown_es0(void);

/**
 * @brief Get current run profile.
 *
 * Queries the SE firmware for the current run profile.
 *
 * @param pp Pointer to run_profile_t to receive the profile data.
 * @retval 0 Success.
 * @retval -EINVAL @p pp is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_get_run_cfg(run_profile_t *pp);

/**
 * @brief Get the last run configuration set by this core.
 *
 * Returns the locally cached run profile that was set via
 * se_service_set_run_cfg(). Faster than querying SE firmware and
 * suitable for read-modify-write operations.
 *
 * @note The cached profile reflects what this core has set, not the
 * actual SE firmware state. The state may differ if other cores have
 * made changes, the system has been suspended/resumed, or the profile
 * was never initialized on this core.
 *
 * @param pp Pointer to run_profile_t to receive cached data.
 * @retval 0 Success.
 * @retval -EINVAL @p pp is NULL.
 * @retval -ENODATA Profile cache is not initialized.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 */
int se_service_get_last_set_run_cfg(run_profile_t *pp);

/**
 * @brief Set run profile.
 *
 * Sends the run profile to SE. Skips the SE call if the profile
 * has not changed from the cached value.
 *
 * @param pp Pointer to run_profile_t with the desired profile.
 * @retval 0 Success.
 * @retval -EINVAL @p pp is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_set_run_cfg(run_profile_t *pp);

/**
 * @brief Get current off profile.
 *
 * @param wp Pointer to off_profile_t to receive the profile data.
 * @retval 0 Success.
 * @retval -EINVAL @p wp is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_get_off_cfg(off_profile_t *wp);

/**
 * @brief Set off profile.
 *
 * @param wp Pointer to off_profile_t with the desired profile.
 * @retval 0 Success.
 * @retval -EINVAL @p wp is NULL.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_set_off_cfg(off_profile_t *wp);

/**
 * @brief Request SE to enter sleep mode.
 *
 * After a successful call, the SE ready flag is cleared so the next
 * SE service call will re-synchronize (wake up) the SE.
 *
 * @param param Sleep parameter passed to SE.
 * @retval 0 Success.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_se_sleep_req(uint32_t param);

/**
 * @brief Request SE to reset the entire SoC.
 *
 * @retval 0 Success.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 */
int se_service_boot_reset_soc(void);

/**
 * @brief Request SE to reset a specific CPU.
 *
 * @param cpu_id Identifier of the CPU to reset.
 * @retval 0 Success.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_boot_reset_cpu(uint32_t cpu_id);

/**
 * @brief Update the ALIF STOC using the provided image.
 *
 * @param img_addr Pointer to the image data.
 * @param img_size Size of the image in bytes (must be non-zero).
 * @retval 0 Success.
 * @retval -EINVAL @p img_addr is NULL or @p img_size is zero.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_update_stoc(uint8_t *img_addr, uint32_t img_size);

/**
 * @brief Send service request to SE to set the value of a clock divider.
 *
 * Selects the value of a clock divider.
 *
 * @param divider Which divider to set – CPUPLL, SYSPLL, ACLK (Corstone), HCLK, PCLK (Alif).
 * @param value Divider value. 0x0 to 0x1F for Corstone dividers, 0x0 to 0x2 for Alif dividers.
 * @retval 0 Success.
 * @retval -EAGAIN Operation timed out. Retry after a delay.
 * @retval -EBUSY SE is busy. Retry after a delay.
 * @return Positive error code returned by SE for a failed service request.
 */
int se_service_clock_set_divider(clock_divider_t divider, uint32_t value);
#ifdef __cplusplus
}
#endif
#endif /* SE_SERVICES_ZEPHYR_INCLUDE_SE_SERVICE_H_ */
