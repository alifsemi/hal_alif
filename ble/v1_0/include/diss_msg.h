/**
 ****************************************************************************************
 *
 * @file diss_msg.h
 *
 * @brief Header file - Device Information Service Server - Message API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef DISS_MSG_H_
#define DISS_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup DISS_API_MSG Message API
 * @ingroup DISS_API
 * @brief Description of Message API for Device Information Service Server
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "diss.h"

/// @addtogroup DISS_API_MSG
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Messages for Device Information Service Server
/*@TRACE*/
enum diss_msg_id
{
    ///Set the value of an attribute - Request
    DISS_SET_VALUE_REQ = MSG_ID(DISS, 0x00),
    ///Set the value of an attribute - Response
    DISS_SET_VALUE_RSP = MSG_ID(DISS, 0x01),
    /// Peer device request to get profile attribute value
    DISS_VALUE_REQ_IND = MSG_ID(DISS, 0x02),
    /// Peer device confirm value of requested attribute
    DISS_VALUE_CFM     = MSG_ID(DISS, 0x03),
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the #DISS_SET_VALUE_REQ message
typedef struct diss_set_value_req
{
    /// Value identifier (see #diss_val_id enumeration)
    uint8_t val_id;
    /// Value length
    uint8_t length;
    /// Value data
    uint8_t data[__ARRAY_EMPTY];
} diss_set_value_req_t;

/// Parameters of the #DISS_SET_VALUE_RSP message
typedef struct diss_set_value_rsp
{
    /// Value identifier (see #diss_val_id enumeration)
    uint8_t val_id;
    /// status of the request
    uint16_t status;
} diss_set_value_rsp_t;

/// Parameters of the #DISS_VALUE_REQ_IND message
typedef struct diss_value_req_ind
{
    /// Token value that must be returned in confirmation
    uint32_t token;
    /// Value identifier (see #diss_val_id enumeration)
    uint8_t val_id;
} diss_value_req_ind_t;

/// Parameters of the #DISS_VALUE_CFM message
typedef struct diss_value_cfm
{
    /// Token value provided in request
    uint32_t token;
    /// Value identifier (see #diss_val_id enumeration)
    uint8_t val_id;
    /// Data length
    uint8_t length;
    /// Value data
    uint8_t data[__ARRAY_EMPTY];
} diss_value_cfm_t;

/// @} DISS_API_MSG

#endif // DISS_MSG_H_
