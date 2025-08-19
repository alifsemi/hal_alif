/**
 ****************************************************************************************
 *
 * @file iasc_msg.h
 *
 * @brief Header file - Immediate Alert Service Client - Message API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef IASC_MSG_H_
#define IASC_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup IASC_API_MSG Message API
 * @ingroup IASC_API
 * @brief Description of Message API for Immediate Alert Service Client
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "iasc.h"
#include "findl_msg.h"

/// @addtogroup IASC_API_MSG
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Message IDs
enum iasc_msg_id
{
    /// Start use of the Immediate Alert Service as Client - at connection\n
    /// See #iasc_enable_req_t
    IASC_ENABLE_REQ          = MSG_ID(IASC, 0x00),
    /// Enable response message, containing IAS attribute details if discovery connection type\n
    /// See #iasc_enable_rsp_t
    IASC_ENABLE_RSP          = MSG_ID(IASC, 0x01),
    /// Alert level set request\n
    /// See #iasc_set_alert_level_cmd_t
    IASC_SET_ALERT_LEVEL_CMD = MSG_ID(IASC, 0x02),
    /// Complete Event Information\n
    /// See #iasc_cmp_evt_t
    IASC_CMP_EVT             = MSG_ID(IASC, 0x03),
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the #IASC_ENABLE_REQ message\n
typedef findl_enable_req_t iasc_enable_req_t;

/// Parameters of the #IASC_ENABLE_RSP message
typedef findl_enable_rsp_t iasc_enable_rsp_t;

/// Parameters of the #IASC_SET_ALERT_LEVEL_CMD message
typedef findl_set_alert_cmd_t iasc_set_alert_level_cmd_t;

///Parameters of the #IASC_CMP_EVT message
typedef findl_cmp_evt_t iasc_cmp_evt_t;

/// @} IASC_API_MSG

#endif // IASC_MSG_H_
