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
int se_service_get_rnd_num(uint8_t *,uint16_t);
int se_service_get_toc_number(uint32_t *);
int se_service_get_se_revision(uint8_t *);
int se_service_get_device_part_number(uint32_t *);
int se_service_system_get_device_data(get_device_revision_data_t *pdev_data);
int se_system_get_eui_extension(bool is_eui48, uint8_t *eui_extension);
int se_service_get_run_cfg(run_profile_t *pp);
int se_service_set_run_cfg(run_profile_t *pp);
int se_service_get_off_cfg(off_profile_t *wp);
int se_service_set_off_cfg(off_profile_t *wp);


#ifdef __cplusplus
}
#endif
#endif /* SE_SERVICES_ZEPHYR_INCLUDE_SE_UTILITIES_H_ */
