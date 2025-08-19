/**
 ****************************************************************************************
 *
 * @file findl_msg.h
 *
 * @brief Header file - Find Me Locator - Message API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef FINDL_MSG_H_
#define FINDL_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup FINDL_API_MSG Message API
 * @ingroup FINDL_API
 * @brief Description of Message API for Find Me Profile Locator
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "findl.h"

/// @addtogroup FINDL_API_MSG
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Message IDs for Find Me Profile Locator
/*@TRACE*/
enum findl_msg_id
{
    /// Start the find me locator profile - at connection\n
    /// See #findl_enable_req_t
    FINDL_ENABLE_REQ    = MSG_ID(FINDL, 0x00),
    /// Enable confirm message, containing IAS attribute details if discovery connection type\n
    /// See #findl_enable_rsp_t
    FINDL_ENABLE_RSP    = MSG_ID(FINDL, 0x01),
    /// Alert level set request\n
    /// See #findl_set_alert_cmd_t
    FINDL_SET_ALERT_CMD = MSG_ID(FINDL, 0x02),
    /// Complete Event Information\n
    /// See #findl_cmp_evt_t
    FINDL_CMP_EVT       = MSG_ID(FINDL, 0x03),
};

/// Operation codes
enum findl_op_codes
{
    /// Reserved
    FINDL_RESERVED_OP_CODE = 0,
    /// Set Alert level Operation Code
    FINDL_SET_ALERT_CMD_OP_CODE,
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the #FINDL_ENABLE_REQ message
typedef struct findl_enable_req
{
    /// Connection index
    uint8_t conidx;
    /// Connection type
    uint8_t con_type;
    /// Discovered IAS details if any
    ias_content_t ias;
} findl_enable_req_t;

/// Parameters of the #FINDL_ENABLE_RSP message
typedef struct findl_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status
    uint16_t status;
    /// Discovered IAS details if any
    ias_content_t ias;
} findl_enable_rsp_t;

/// Parameters of the #FINDL_SET_ALERT_CMD message
typedef struct findl_set_alert_cmd
{
    /// Connection index
    uint8_t conidx;
    /// Alert level
    uint8_t alert_lvl;
} findl_set_alert_cmd_t;

///Parameters of the #FINDL_CMP_EVT message
typedef struct findl_cmp_evt
{
    /// Connection index
    uint8_t conidx;
    /// Operation
    uint8_t operation;
    /// Status
    uint16_t status;
} findl_cmp_evt_t;

/// @} FINDL_API_MSG

#endif // FINDL_MSG_H_
