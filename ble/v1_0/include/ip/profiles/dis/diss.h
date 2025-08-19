/**
 ****************************************************************************************
 *
 * @file diss.h
 *
 * @brief Header file - Device Info Service Server - Native API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef DISS_H_
#define DISS_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup DISS_API Device Information Service Server
 * @ingroup DIS_API
 * @brief Description of API for Device Information Service Server
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup DISS_API_COMMON Common
 * @ingroup DISS_API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup DISS_API_NATIVE Native API
 * @ingroup DISS_API
 * @brief Description of Native API for Device Information Service Server
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "dis.h"
#include "co_math.h"

/// @addtogroup DISS_API_COMMON
/// @{

/*
 * DEFINES
 ****************************************************************************************
 */

/// All features are supported
#define DIS_ALL_FEAT_SUP                     (0x01FF)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Attribute Table Indexes
enum diss_val_id
{
    /// Manufacturer Name
    DIS_VAL_MANUFACTURER_NAME = 0,
    /// Model Number
    DIS_VAL_MODEL_NB_STR,
    /// Serial Number
    DIS_VAL_SERIAL_NB_STR,
    /// HW Revision Number
    DIS_VAL_HARD_REV_STR,
    /// FW Revision Number
    DIS_VAL_FIRM_REV_STR,
    /// SW Revision Number
    DIS_VAL_SW_REV_STR,
    /// System Identifier Name
    DIS_VAL_SYSTEM_ID,
    /// IEEE Certificate
    DIS_VAL_IEEE,
    /// Plug and Play Identifier
    DIS_VAL_PNP_ID,

    DIS_VAL_MAX,
};

/// Database Configuration Flags (not used)
enum diss_features_bf
{
    /// Indicate if Manufacturer Name String characteristic is supported
    DIS_MANUFACTURER_NAME_CHAR_SUP_POS = 0,
    DIS_MANUFACTURER_NAME_CHAR_SUP_BIT = CO_BIT(DIS_MANUFACTURER_NAME_CHAR_SUP_POS),

    /// Indicate if Model Number String characteristic is supported
    DIS_MODEL_NB_STR_CHAR_SUP_POS      = 1,
    DIS_MODEL_NB_STR_CHAR_SUP_BIT      = CO_BIT(DIS_MODEL_NB_STR_CHAR_SUP_POS),

    /// Indicate if Serial Number String characteristic is supported
    DIS_SERIAL_NB_STR_CHAR_SUP_POS     = 2,
    DIS_SERIAL_NB_STR_CHAR_SUP_BIT     = CO_BIT(DIS_SERIAL_NB_STR_CHAR_SUP_POS),

    /// Indicate if Hardware Revision String characteristic is supported
    DIS_HARD_REV_STR_CHAR_SUP_POS      = 3,
    DIS_HARD_REV_STR_CHAR_SUP_BIT      = CO_BIT(DIS_HARD_REV_STR_CHAR_SUP_POS),

    /// Indicate if Firmware Revision String characteristic is supported
    DIS_FIRM_REV_STR_CHAR_SUP_POS      = 4,
    DIS_FIRM_REV_STR_CHAR_SUP_BIT      = CO_BIT(DIS_FIRM_REV_STR_CHAR_SUP_POS),

    /// Indicate if Software Revision String characteristic is supported
    DIS_SW_REV_STR_CHAR_SUP_POS        = 5,
    DIS_SW_REV_STR_CHAR_SUP_BIT        = CO_BIT(DIS_SW_REV_STR_CHAR_SUP_POS),

    /// Indicate if System ID characteristic is supported
    DIS_SYSTEM_ID_CHAR_SUP_POS         = 6,
    DIS_SYSTEM_ID_CHAR_SUP_BIT         = CO_BIT(DIS_SYSTEM_ID_CHAR_SUP_POS),

    /// Indicate if IEEE characteristic is supported
    DIS_IEEE_CHAR_SUP_POS              = 7,
    DIS_IEEE_CHAR_SUP_BIT              = CO_BIT(DIS_IEEE_CHAR_SUP_POS),

    /// Indicate if PnP ID characteristic is supported
    DIS_PNP_ID_CHAR_SUP_POS            = 8,
    DIS_PNP_ID_CHAR_SUP_BIT            = CO_BIT(DIS_PNP_ID_CHAR_SUP_POS),
};

/*
 * TYPES DEFINITION
 ****************************************************************************************
 */

/// Parameters for the database creation
typedef struct diss_db_cfg
{
    /// Database configuration see enum #diss_features_bf
    uint16_t features;
} diss_db_cfg_t;

/// @} DISS_API_COMMON

/// @addtogroup DISS_API_NATIVE
/// @{

/*
 * NATIVE API CALLBACKS
 ****************************************************************************************
 */

/// Device Information Service server callback set
typedef struct diss_cb
{
    /**
     ****************************************************************************************
     * @brief This function is called when GATT server user has initiated event send to peer
     *        device or if an error occurs.
     *
     * @param[in] token         Procedure token that must be returned in confirmation function
     * @param[in] val_id        Requested value identifier (see #diss_val_id enumeration)
     ****************************************************************************************
     */
    void (*cb_value_get) (uint32_t token, uint8_t val_id);
} diss_cb_t;

/*
 * NATIVE API FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Store DIS value information that can be used by service.
 *
 * @param[in] val_id   Value identifier (see #diss_val_id enumeration)
 * @param[in] length   Data value length
 * @param[in] p_data   Pointer to the value data
 *
 * @return Function execution status (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t diss_value_set(uint8_t val_id, uint8_t length, const uint8_t* p_data);

/**
 ****************************************************************************************
 * @brief Provide DIS value information requested by a peer device
 *
 * @param[in] token    Procedure token provided in request
 * @param[in] length   Data value length
 * @param[in] p_data   Pointer to the value data
 *
 * @return Function execution status (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t diss_value_cfm(uint32_t token, uint8_t length, const uint8_t* p_data);

/// @} DISS_API_NATIVE

#endif // DISS_H_
