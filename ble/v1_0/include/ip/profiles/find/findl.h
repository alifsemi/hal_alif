/**
 ****************************************************************************************
 *
 * @file findl.h
 *
 * @brief Header file - Find Me Locator - Native API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef FINDL_H_
#define FINDL_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup FINDL_API Find Me Profile Locator
 * @ingroup FMP_API
 * @brief Description of API for Find Me Profile Locator
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup FINDL_API_COMMON Common
 * @ingroup FINDL_API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup FINDL_API_NATIVE Native API
 * @ingroup FINDL_API
 * @brief Description of Native API for Find Me Profile Locator
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "find.h"

/// @addtogroup FINDL_API_COMMON
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Alert levels
enum findl_alert_lvl
{
    /// No alert
    FINDL_ALERT_NONE = 0x00,
    /// Mild alert
    FINDL_ALERT_MILD = 0x01,
    /// High alert
    FINDL_ALERT_HIGH = 0x02,
};

/*
 * TYPES DEFINITIONS
 ****************************************************************************************
 */

/// Immediate Alert service details container
typedef struct ias_content
{
    /// Service info
    prf_svc_t svc;
    /// Characteristic info:
    /// - Alert Level
    prf_char_t alert_lvl_char;
} ias_content_t;

/// @} FINDL_API_COMMON

/// @addtogroup FINDL_API_NATIVE
/// @{

/*
 * NATIVE API CALLBACKS
 ****************************************************************************************
 */

/// Find Me client callback set
typedef struct findl_cb
{
    /**
     ****************************************************************************************
     * @brief This function is called when GATT server user has initiated event send to peer
     *        device or if an error occurs.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Client Enable status (see enum #hl_err)
     * @param[in] p_ias         Pointer to bond data information that describe peer database
     ****************************************************************************************
     */
    void (*cb_enable_cmp) (uint8_t conidx, uint16_t status, const ias_content_t* p_ias);

    /**
     ****************************************************************************************
     * @brief This function is called when Alert update procedure is over.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Read status (see enum #hl_err)
     ****************************************************************************************
     */
    void (*cb_alert_upd_cmp) (uint8_t conidx, uint16_t status);
} findl_cb_t;

/*
 * NATIVE API FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable client profile by doing a discovery or restoring bond data
 *
 * @param[in] conidx        Connection index
 * @param[in] con_type      Connection type (see enum #prf_con_type)
 * @param[in] p_ias         Pointer to bond data information (valid if con_type == PRF_CON_NORMAL)
 *
 * @return Function execution status (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t findl_enable(uint8_t conidx, uint8_t con_type, const ias_content_t* p_ias);

/**
 ****************************************************************************************
 * @brief Update peer device alert level
 *
 * @param[in] conidx     Connection index
 * @param[in] alert_lvl  Alert level (see enum findl_alert_lvl)
 *
 * @return Function execution status (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t findl_alert_upd(uint8_t conidx, uint8_t alert_lvl);

/// @} FINDL_API_NATIVE

#endif // FINDL_H_
