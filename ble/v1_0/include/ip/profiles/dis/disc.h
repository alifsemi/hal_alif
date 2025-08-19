/**
 ****************************************************************************************
 *
 * @file disc.h
 *
 * @brief Header file - Device Information Service Client - Native API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef DISC_H_
#define DISC_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup DISC_API Device Information Service Client
 * @ingroup DIS_API
 * @brief Description of API for Device Information Service Client
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup DISC_API_COMMON Common
 * @ingroup DISC_API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup DISC_API_NATIVE Native API
 * @ingroup DISC_API
 * @brief Description of Native API for Device Information Service Client
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "dis.h"

/// @addtogroup DISC_API_COMMON
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Value identifier
enum disc_val_id
{
    /// Manufacturer Name String
    DISC_VAL_MANUFACTURER_NAME = 0,
    /// Model Number String
    DISC_VAL_MODEL_NB_STR,
    /// Serial Number String
    DISC_VAL_SERIAL_NB_STR,
    /// Hardware Revision String
    DISC_VAL_HARD_REV_STR,
    /// Firmware Revision String
    DISC_VAL_FIRM_REV_STR,
    /// Software Revision String
    DISC_VAL_SW_REV_STR,
    /// System ID
    DISC_VAL_SYSTEM_ID,
    /// IEEE 11073-20601 Regulatory Certification Data List
    DISC_VAL_IEEE,
    /// PnP ID
    DISC_VAL_PNP_ID,

    DISC_VAL_MAX,
};

/*
 * TYPES DEFINITION
 ****************************************************************************************
 */

/// Structure containing the characteristics handles, value handles and descriptors for the Device Information
/// Service
typedef struct disc_dis_content
{
    /// Service information
    prf_svc_t svc;
    /// Characteristic information
    prf_char_t vals[DISC_VAL_MAX];
} disc_dis_content_t;

/// @} DISC_API_COMMON

/// @addtogroup DISC_API_NATIVE
/// @{

/*
 * NATIVE API CALLBACKS
 ****************************************************************************************
 */

/// Device Information Service client callback set
typedef struct disc_cb
{
    /**
     ****************************************************************************************
     * @brief This function is called when GATT server user has initiated event send to peer
     *        device or if an error occurs.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Client Enable status (see enum #hl_err)
     * @param[in] p_dis         Pointer to bond data information that describe peer database
     ****************************************************************************************
     */
    void (*cb_enable_cmp) (uint8_t conidx, uint16_t status, const disc_dis_content_t* p_dis);


    /**
     ****************************************************************************************
     * @brief This function is called when GATT server user has initiated event send to peer
     *        device or if an error occurs.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Read status (see enum #hl_err)
     * @param[in] val_id        Value identifer read (see enum #disc_val_id)
     * @param[in] length        Value data length
     * @param[in] p_data        Pointer to value data
     ****************************************************************************************
     */
    void (*cb_read_val_cmp) (uint8_t conidx, uint16_t status, uint8_t val_id, uint16_t length, const uint8_t* p_data);
} disc_cb_t;

/*
 * NATIVE API FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable DIS client profile by doing a discovery or restoring bond data
 *
 * @param[in] conidx        Connection index
 * @param[in] con_type      Connection type (see enum #prf_con_type)
 * @param[in] p_dis         Pointer to bond data information (valid if con_type == PRF_CON_NORMAL)
 *
 * @return Function execution status (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t disc_enable(uint8_t conidx, uint8_t con_type, const disc_dis_content_t* p_dis);

/**
 ****************************************************************************************
 * @brief Read a DIS value in peer database
 *
 * @param[in] conidx   Connection index
 * @param[in] val_id   Value identifier (see enum #disc_val_id)
 *
 * @return Function execution status (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t disc_read_val(uint8_t conidx, uint8_t val_id);

/// @} DISC_API_NATIVE

#endif // DISC_H_
