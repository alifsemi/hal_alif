/**
 ****************************************************************************************
 *
 * @file bass_msg.h
 *
 * @brief Header file - Battery Service Server - Message API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */


#ifndef BASS_MSG_H_
#define BASS_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup BASS_API_MSG Message API
 * @ingroup BASS_API
 * @brief Description of Message API for Battery Service Server
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "bass.h"

/// @addtogroup BASS_API_MSG
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/*@TRACE*/
/// Messages IDs for Battery Service Server
enum bass_msg_id
{
    /// Start the Battery Server - at connection used to restore bond data\n
    /// See #bass_enable_req_t
    BASS_ENABLE_REQ             = MSG_ID(BASS, 0x00),
    /// Confirmation of the Battery Server start\n
    /// See #bass_enable_rsp_t
    BASS_ENABLE_RSP             = MSG_ID(BASS, 0x01),
    /// Battery Level Value Update Request\n
    /// See #bass_batt_level_upd_req_t
    BASS_BATT_LEVEL_UPD_REQ     = MSG_ID(BASS, 0x02),
    /// Inform APP if Battery Level value has been notified or not\n
    /// See #bass_batt_level_upd_rsp_t
    BASS_BATT_LEVEL_UPD_RSP     = MSG_ID(BASS, 0x03),
    /// Inform APP that Battery Level Notification Configuration has been changed - use to update bond data\n
    /// See #bass_batt_level_ntf_cfg_ind_t
    BASS_BATT_LEVEL_NTF_CFG_IND = MSG_ID(BASS, 0x04),
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the #BASS_ENABLE_REQ message
typedef struct bass_enable_req
{
    /// Connection index
    uint8_t conidx;
    /// Notification Configuration
    uint8_t ntf_cfg;
    /// Old Battery Level used to decide if notification should be triggered
    uint8_t old_batt_lvl[BASS_NB_BAS_INSTANCES_MAX];
} bass_enable_req_t;

/// Parameters of the #BASS_ENABLE_RSP message
typedef struct bass_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status
    uint16_t status;
} bass_enable_rsp_t;

/// Parameters of the #BASS_BATT_LEVEL_UPD_REQ message
typedef struct bass_batt_level_upd_req
{
    /// BAS instance
    uint8_t bas_instance;
    /// Battery Level
    uint8_t batt_level;
} bass_batt_level_upd_req_t;

/// Parameters of the #BASS_BATT_LEVEL_UPD_RSP message
typedef struct bass_batt_level_upd_rsp
{
    /// Status
    uint16_t status;
} bass_batt_level_upd_rsp_t;

/// Parameters of the #BASS_BATT_LEVEL_NTF_CFG_IND message
typedef struct bass_batt_level_ntf_cfg_ind
{
    /// Connection index
    uint8_t conidx;
    /// Notification Configuration
    uint8_t ntf_cfg;
} bass_batt_level_ntf_cfg_ind_t;

/// @} BASS_API_MSG

#endif // BASS_MSG_H_
