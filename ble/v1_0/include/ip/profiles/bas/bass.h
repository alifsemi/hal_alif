/**
 ****************************************************************************************
 *
 * @file bass.h
 *
 * @brief Header file - Battery Service Server - Native API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef BASS_H_
#define BASS_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup BASS_API Battery Service Server
 * @ingroup BAS_API
 * @brief Description of API for Battery Service Server
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup BASS_API_COMMON Common
 * @ingroup BASS_API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup BASS_API_NATIVE Native API
 * @ingroup BASS_API
 * @brief Description of Native API for Battery Service Server
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "bas.h"

/// @addtogroup BASS_API_COMMON
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Features Flag Masks
enum bass_features
{
    /// Battery Level Characteristic doesn't support notifications
    BAS_BATT_LVL_NTF_NOT_SUP = 0,
    /// Battery Level Characteristic support notifications
    BAS_BATT_LVL_NTF_SUP,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Parameters for the database creation
typedef struct bass_db_cfg
{
    /// Number of BAS to add
    uint8_t             bas_nb;
    /// Features of each BAS instance
    uint8_t             features[BASS_NB_BAS_INSTANCES_MAX];
    /// Battery Level Characteristic Presentation Format - Should not change during connection
    prf_char_pres_fmt_t batt_level_pres_format[BASS_NB_BAS_INSTANCES_MAX];
} bass_db_cfg_t;

/// @} BASS_API_COMMON

/// @addtogroup BASS_API_NATIVE
/// @{

/*
 * NATIVE API CALLBACKS
 ****************************************************************************************
 */

/// Battery Service server callback set
typedef struct bass_cb
{
    /**
     ****************************************************************************************
     * @brief Completion of battery level update
     *
     * @param[in] status Status of the procedure execution (see enum #hl_err)
     ****************************************************************************************
     */
    void (*cb_batt_level_upd_cmp)(uint16_t status);

    /**
     ****************************************************************************************
     * @brief Inform that Bond data updated for the connection.
     *
     * @param[in] conidx        Connection index
     * @param[in] ntf_cfg       Notification Configuration
     ****************************************************************************************
     */
    void (*cb_bond_data_upd)(uint8_t conidx, uint8_t ntf_ind_cfg);
} bass_cb_t;

/*
 * NATIVE API FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Restore bond data of a known peer device (at connection establishment)
 *
 * @param[in] conidx          Connection index
 * @param[in] ntf_cfg         Notification Configuration
 * @param[in] p_old_batt_lvl  Old Battery Level used to decide if notification should be triggered
 *                            Array of BASS_NB_BAS_INSTANCES_MAX size.
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t bass_enable(uint8_t conidx, uint8_t ntf_cfg, const uint8_t* p_old_batt_lvl);

/**
 ****************************************************************************************
 * @brief Update a battery level
 *
 * Wait for #bass_cb_t.cb_batt_level_upd_cmp execution before starting a new procedure
 *
 * @param[in] bas_instance  Battery service instance
 * @param[in] batt_level    New Battery level
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t bass_batt_level_upd(uint8_t bas_instance, uint8_t batt_level);

/**
 ****************************************************************************************
 * @return Return current battery level exposed for a BAS instance
 *
 * @param[in] bas_instance  Battery Service instance
 ****************************************************************************************
 */
uint8_t bass_get_level(uint8_t bas_instance);

/**
 ****************************************************************************************
 * @return Current client characteristic configuration for a given connection
 *
 * @param[in] conidx        Connection index
 ****************************************************************************************
 */
uint8_t bass_get_client_cfg(uint8_t conidx);

/// @} BASS_API_NATIVE

#endif // BASS_H_
