#ifndef SE_SERVICES_ZEPHYR_INCLUDE_SE_SERVICE_H_
#define SE_SERVICES_ZEPHYR_INCLUDE_SE_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/device.h>
#include <services_lib_api.h>
#include <services_lib_ids.h>

int se_service_heartbeat(void);
int se_service_sync(void);
int se_service_system_set_services_debug(bool debug_enable);
int se_service_get_rnd_num(uint8_t *buffer, uint16_t length);
int se_service_get_toc_number(uint32_t *ptoc);
int se_service_get_se_revision(uint8_t *prev);
int se_service_get_toc_version(uint32_t *pversion);
int se_service_get_device_part_number(uint32_t *pdev_part);
int se_service_system_get_device_data(get_device_revision_data_t *pdev_data);
int se_system_get_eui_extension(bool is_eui48, uint8_t *eui_extension);

int se_service_boot_es0(uint8_t *nvds_buff, uint16_t nvds_size, uint32_t clock_select,
			bool hpa_mode);
int se_service_shutdown_es0(void);
int se_service_get_run_cfg(run_profile_t *pp);
int se_service_get_last_set_run_cfg(run_profile_t *pp);
int se_service_set_run_cfg(run_profile_t *pp);
int se_service_get_off_cfg(off_profile_t *wp);
int se_service_set_off_cfg(off_profile_t *wp);
int se_service_se_sleep_req(uint32_t param);
int se_service_boot_reset_soc(void);
int se_service_boot_reset_cpu(uint32_t cpu_id);
int se_service_update_stoc(uint8_t *img_addr, uint32_t img_size);

/**
 * @brief Send service request to SE to set the value of a clock divider.
 *
 * Selects the value of a clock divider.
 *
 * @param divider Which divider to set – CPUPLL, SYSPLL, ACLK (Corstone), HCLK, PCLK (Alif)
 * @param value Divider value. 0x0 to 0x1F for Corstone dividers, 0x0 to 0x2 for Alif divider
 *
 * @return 0 on success, non-zero on failure
 */
int se_service_clock_set_divider(clock_divider_t divider, uint32_t value);
#ifdef __cplusplus
}
#endif
#endif /* SE_SERVICES_ZEPHYR_INCLUDE_SE_UTILITIES_H_ */
