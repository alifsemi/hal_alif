/**
 ****************************************************************************************
 *
 * @file disc_msg.h
 *
 * @brief Header file - Device Information Service Client - Message API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef DISC_MSG_H_
#define DISC_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup DISC_API_MSG Message API
 * @ingroup DISC_API
 * @brief Description of Message API for Device Information Service Client
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "disc.h"

/// @addtogroup DISC_API_MSG
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Message IDs for Device Information Service Client
/*@TRACE*/
enum disc_msg_id
{
    /// Start the find me locator profile - at connection\n
    /// See #disc_enable_req_t
    DISC_ENABLE_REQ = MSG_ID(DISC, 0x00),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started\n
    /// See #disc_enable_rsp_t
    DISC_ENABLE_RSP = MSG_ID(DISC, 0x01),
    /// Generic message to read a DIS value\n
    /// See #disc_rd_val_cmd_t
    DISC_RD_VAL_CMD = MSG_ID(DISC, 0x02),
    /// Generic message for read responses for APP\n
    /// See #disc_rd_val_ind_t
    DISC_RD_VAL_IND = MSG_ID(DISC, 0x03),
    /// Complete Event Information\n
    /// See #disc_cmp_evt_t
    DISC_CMP_EVT    = MSG_ID(DISC, 0x04),
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the #DISC_ENABLE_REQ message
typedef struct disc_enable_req
{
    /// Connection Index
    uint8_t conidx;
    ///Connection type
    uint8_t con_type;
    /// Existing handle values
    disc_dis_content_t dis;
} disc_enable_req_t;

/// Parameters of the #DISC_ENABLE_RSP message
typedef struct disc_enable_rsp
{
    /// Connection Index
    uint8_t conidx;
    ///status
    uint16_t status;
    /// DIS handle values and characteristic properties
    disc_dis_content_t dis;
} disc_enable_rsp_t;

///Parameters of the #DISC_RD_VAL_CMD message
typedef struct disc_rd_val_cmd
{
    /// Connection Index
    uint8_t conidx;
    /// Value identifier (see #disc_val_id)
    uint8_t val_id;
} disc_rd_val_cmd_t;

///Parameters of the #DISC_RD_VAL_IND message
typedef struct disc_rd_val_ind
{
    /// Connection Index
    uint8_t conidx;
    /// Value identifier (see #disc_val_id)
    uint8_t val_id;
    /// Attribute length
    uint16_t length;
    /// Attribute value
    uint8_t value[__ARRAY_EMPTY];
} disc_rd_val_ind_t;

///Parameters of the #DISC_CMP_EVT message
typedef struct disc_cmp_evt
{
    /// Connection Index
    uint8_t conidx;
    /// Operation
    uint8_t operation;
    /// Status
    uint16_t status;
} disc_cmp_evt_t;

/// @} DISC_API_MSG

#endif // DISC_MSG_H_
