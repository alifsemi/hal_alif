/**
 ****************************************************************************************
 *
 * @file findt.h
 *
 * @brief Header file - Find Me Target - Native API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef FINDT_H_
#define FINDT_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup FINDT_API Find Me Profile Target
 * @ingroup FMP_API
 * @brief Description of API for Find Me Profile Target
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup FINDT_API_COMMON Common
 * @ingroup FINDT_API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup FINDT_API_NATIVE Native API
 * @ingroup FINDT_API
 * @brief Description of Native API for Find Me Profile Target
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "find.h"

/// @addtogroup FINDT_API_COMMON
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Alert levels
enum findt_alert_lvl
{
    /// No alert
    FINDT_ALERT_NONE = 0x00,
    /// Mild alert
    FINDT_ALERT_MILD = 0x01,
    /// High alert
    FINDT_ALERT_HIGH = 0x02,
};

/// @} FINDT_API_COMMON

/// @addtogroup FINDT_API_NATIVE
/// @{

/*
 * NATIVE API CALLBACKS
 ****************************************************************************************
 */

/// Find Me Target server callback set
typedef struct findt_cb
{
    /**
     ****************************************************************************************
     * @brief This function is called when a Find me locator update the Alert level
     *
     * @param[in] conidx        Connection Index
     * @param[in] alert_lvl     Alert Level (see #findt_alert_lvl enumeration)
     ****************************************************************************************
     */
    void (*cb_alert_upd) (uint8_t conidx, uint8_t alert_lvl);
} findt_cb_t;

/// @} FINDT_API_NATIVE

#endif // FINDT_H_
