/**
 ****************************************************************************************
 *
 * @file basc_msg.h
 *
 * @brief Header file - Battery Service Client - Message API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef BASC_MSG_H_
#define BASC_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup BASC_API_MSG Message API
 * @ingroup BASC_API
 * @brief Description of Message API for Battery Service Client
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "basc.h"

/// @addtogroup BASC_API_MSG
/// @{

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Message IDs for Battery Service Client
/*@TRACE*/
enum basc_msg_id
{
    /// Start the Battery Service Client Role - at connection\n
    /// See #basc_enable_req_t
    BASC_ENABLE_REQ             = MSG_ID(BASC, 0x00),
    /// Confirm that cfg connection has finished with discovery results, or that normal cnx started\n
    /// See #basc_enable_rsp_t
    BASC_ENABLE_RSP             = MSG_ID(BASC, 0x01),
    /// Read Characteristic Value Request\n
    /// See #basc_read_info_req_t
    BASC_READ_INFO_REQ          = MSG_ID(BASC, 0x02),
    /// Read Characteristic Value Request\n
    /// See #basc_read_info_rsp_t
    BASC_READ_INFO_RSP          = MSG_ID(BASC, 0x03),
    /// Write Battery Level Notification Configuration Value request\n
    /// See #basc_batt_level_ntf_cfg_req_t
    BASC_BATT_LEVEL_NTF_CFG_REQ = MSG_ID(BASC, 0x04),
    /// Write Battery Level Notification Configuration Value response\n
    /// See #basc_batt_level_ntf_cfg_rsp_t
    BASC_BATT_LEVEL_NTF_CFG_RSP = MSG_ID(BASC, 0x05),
    /// Indicate to APP that the Battery Level value has been received\n
    /// See #basc_batt_level_ind_t
    BASC_BATT_LEVEL_IND         = MSG_ID(BASC, 0x06),
};

/// Peer battery info that can be read
enum basc_info
{
    /// Battery Level value
    BASC_BATT_LVL_VAL = 0,
    /// Battery Level Client Characteristic Configuration
    BASC_NTF_CFG,
    /// Battery Level Characteristic Presentation Format
    BASC_BATT_LVL_PRES_FORMAT,

    BASC_INFO_MAX,
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the #BASC_ENABLE_REQ message
typedef struct basc_enable_req
{
    /// Connection Index
    uint8_t conidx;
    /// Connection type
    uint8_t con_type;
    /// Number of BAS instances that have previously been found
    uint8_t bas_nb;
    /// Existing handle values bas
    bas_content_t bas[BASC_NB_BAS_INSTANCES_MAX];
} basc_enable_req_t;

/// Parameters of the #BASC_ENABLE_RSP message
typedef struct basc_enable_rsp
{
    /// Connection Index
    uint8_t conidx;
    /// Status
    uint16_t status;
    /// Number of BAS that have been found
    uint8_t bas_nb;
    /// Existing handle values bas
    bas_content_t bas[BASC_NB_BAS_INSTANCES_MAX];
} basc_enable_rsp_t;

/// Parameters of the #BASC_READ_INFO_REQ message
typedef struct basc_read_info_req
{
    /// Connection Index
    uint8_t conidx;
    /// Characteristic info (see #basc_info enumeration)
    uint8_t info;
    /// Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t bas_nb;
} basc_read_info_req_t;

///Parameters of the #BASC_READ_INFO_RSP message
typedef struct basc_read_info_rsp
{
    /// Connection Index
    uint8_t conidx;
    /// Status of the request
    uint16_t status;
    /// Characteristic info (see #basc_info enumeration)
    uint8_t info;
    /// Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t bas_nb;
    /// Information data
    union
    {
        /// Battery Level - if info = #BASC_BATT_LVL_VAL
        uint8_t batt_level;
        /// Notification Configuration Value - if info = #BASC_NTF_CFG
        uint16_t ntf_cfg;
        /// Characteristic Presentation Format - if info = #BASC_BATT_LVL_PRES_FORMAT
        prf_char_pres_fmt_t char_pres_format;
    } data;
} basc_read_info_rsp_t;

///Parameters of the #BASC_BATT_LEVEL_NTF_CFG_REQ message
typedef struct basc_batt_level_ntf_cfg_req
{
    /// Connection Index
    uint8_t  conidx;
    /// Notification Configuration
    uint16_t ntf_cfg;
    /// Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t bas_nb;
} basc_batt_level_ntf_cfg_req_t;

///Parameters of the #BASC_BATT_LEVEL_NTF_CFG_RSP message
typedef struct basc_batt_level_ntf_cfg_rsp
{
    /// Connection Index
    uint8_t conidx;
    /// Status
    uint16_t status;
    /// Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t bas_nb;
} basc_batt_level_ntf_cfg_rsp_t;

///Parameters of the #BASC_BATT_LEVEL_IND message
typedef struct basc_batt_level_ind
{
    /// Connection Index
    uint8_t conidx;
    /// Battery Level
    uint8_t batt_level;
    /// Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t bas_nb;
} basc_batt_level_ind_t;

/// @} BASC_API_MSG

#endif // BASC_MSG_H_
