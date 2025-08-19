/**
 ****************************************************************************************
 *
 * @file basc.h
 *
 * @brief Header file - Battery Service Client - Native API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef BASC_H_
#define BASC_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup BASC_API Battery Service Client
 * @ingroup BAS_API
 * @brief Description of API for Battery Service Client
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup BASC_API_COMMON Common
 * @ingroup BASC_API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup BASC_API_NATIVE Native API
 * @ingroup BASC_API
 * @brief Description of Native API for Battery Service Client
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "bas.h"

/// @addtogroup BASC_API_COMMON
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Battery Service Characteristics
enum basc_char_type
{
    /// Battery Level
    BAS_CHAR_BATT_LEVEL = 0,

    BAS_CHAR_MAX,
};

/// Battery Service Descriptors
enum basc_desc_type
{
    /// Battery Level Characteristic Presentation Format
    BAS_DESC_BATT_LEVEL_PRES_FORMAT = 0,
    /// Battery Level Client Characteristic Configuration
    BAS_DESC_BATT_LEVEL_CFG,

    BAS_DESC_MAX,
};

/*
 * TYPES DEFINITION
 ****************************************************************************************
 */

/// Structure containing the characteristics handles, value handles and descriptors
typedef struct bas_content
{
    /// Service info
    prf_svc_t svc;
    /// Characteristic Info:\n
    ///     - Battery Level
    prf_char_t chars[BAS_CHAR_MAX];
    /// Descriptor handles:\n
    ///     - Battery Level Client Characteristic Configuration\n
    ///     - Battery Level Characteristic Presentation Format
    prf_desc_t descs[BAS_DESC_MAX];
} bas_content_t;

/// @} BASC_API_COMMON

/// @addtogroup BASC_API_NATIVE
/// @{

/*
 * NATIVE API CALLBACKS
 ****************************************************************************************
 */

/// Battery Service client callback set
typedef struct basc_cb
{
    /**
     ****************************************************************************************
     * @brief Completion of Enable procedure
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] bas_nb        Number of BAS that have been found
     * @param[in] p_bas         Pointer to peer database description bond data
     ****************************************************************************************
     */
    void (*cb_enable_cmp)(uint8_t conidx, uint16_t status, uint8_t bas_nb, const bas_content_t* p_bas);

    /**
     ****************************************************************************************
     * @brief Inform that battery level read procedure is over
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] bas_instance  Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
     * @param[in] batt_level    Battery Level
     ****************************************************************************************
     */
    void (*cb_read_batt_level_cmp)(uint8_t conidx, uint16_t status, uint8_t bas_instance, uint8_t batt_level);

    /**
     ****************************************************************************************
     * @brief Inform that Notification configuration read procedure is over
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] bas_instance  Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
     * @param[in] ntf_cfg       Notification Configuration
     ****************************************************************************************
     */
    void (*cb_read_ntf_cfg_cmp)(uint8_t conidx, uint16_t status, uint8_t bas_instance, uint16_t ntf_cfg);

    /**
     ****************************************************************************************
     * @brief Inform that Presentation Format read procedure is over
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] bas_instance  Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
     * @param[in] p_pres_format Pointer to Characteristic Presentation Format value
     ****************************************************************************************
     */
    void (*cb_read_pres_format_cmp)(uint8_t conidx, uint16_t status, uint8_t bas_instance,
                                    const prf_char_pres_fmt_t* p_pres_format);

    /**
     ****************************************************************************************
     * @brief Inform that Notification configuration write procedure is over
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] bas_instance  Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
     ****************************************************************************************
     */
    void (*cb_write_ntf_cfg_cmp)(uint8_t conidx, uint16_t status, uint8_t bas_instance);

    /**
     ****************************************************************************************
     * @brief Inform that battery level update has been received from peer device
     *
     * @param[in] conidx        Connection index
     * @param[in] bas_instance  Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
     * @param[in] batt_level    Battery Level
     ****************************************************************************************
     */
    void (*cb_batt_level_upd)(uint8_t conidx, uint8_t bas_instance, uint8_t batt_level);
} basc_cb_t;

/*
 * NATIVE API FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Restore bond data of a known peer device (at connection establishment)
 *
 * Wait for #basc_cb_t.cb_enable_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 * @param[in] con_type      Connection type
 * @param[in] nb_bas        Number of BAS that have been found
 * @param[in] p_bas         Pointer to peer database description bond data
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t basc_enable(uint8_t conidx, uint8_t con_type, uint8_t nb_bas, const bas_content_t* p_bas);

/**
 ****************************************************************************************
 * @brief Perform battery level read procedure.
 *
 * Wait for #basc_cb_t.cb_read_batt_level_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 * @param[in] bas_instance  Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t basc_read_batt_level(uint8_t conidx, uint8_t bas_instance);

/**
 ****************************************************************************************
 * @brief Perform  Notification configuration read procedure
 *
 * Wait for #basc_cb_t.cb_read_ntf_cfg_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 * @param[in] bas_instance  Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t basc_read_ntf_cfg(uint8_t conidx, uint8_t bas_instance);

/**
 ****************************************************************************************
 * @brief Perform Presentation Format read procedure
 *
 * Wait for #basc_cb_t.cb_read_pres_format_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 * @param[in] bas_instance  Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t basc_read_pres_format(uint8_t conidx, uint8_t bas_instance);

/**
 ****************************************************************************************
 * @brief Perform Notification configuration write procedure
 *
 * Wait for #basc_cb_t.cb_write_ntf_cfg_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 * @param[in] bas_instance  Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
 * @param[in] ntf_cfg       Notification Configuration
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t basc_write_ntf_cfg(uint8_t conidx, uint8_t bas_instance, uint16_t ntf_cfg);

/// @} BASC_API_NATIVE

#endif // BASC_H_
