/**
 ****************************************************************************************
 *
 * @file gaf_adv_msg.h
 *
 * @brief Generic Audio Framework - Advertiser - Definition of Message API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef GAF_ADV_MSG_H_
#define GAF_ADV_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup GAF_ADV_MSG Message API
 * @ingroup GAF_ADV
 * @brief Description of Message API for Generic Audio Framework Advertiser module
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup GAF_ADV_MSG_ENUM Enumerations
 * @ingroup GAF_ADV_MSG
 * @brief Message API Enumerations for Generic Audio Framework Advertiser module
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup GAF_ADV_MSG_STRUCT Messages
 * @ingroup GAF_ADV_MSG
 * @brief Message structures for Generic Audio Framework Advertiser module
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gaf_msg.h"            // GAF Message API Definitions
#include "gaf_adv.h"            // GAF Advertiser Definitions

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// @addtogroup GAF_ADV_MSG_ENUM
/// @{

/// List of GAF_CMD command code values for Generic Audio Framework Advertiser module
enum gaf_adv_msg_cmd_code
{
    /// Start advertising (see #gaf_adv_start_cmd_t)
    GAF_ADV_START = GAF_CODE(GAF, ADV, GAF_ADV_CMD_TYPE_START),
    /// Stop advertising (see #gaf_adv_stop_cmd_t)
    GAF_ADV_STOP = GAF_CODE(GAF, ADV, GAF_ADV_CMD_TYPE_STOP),
    /// Start directed advertising (see #gaf_adv_start_directed_cmd_t)
    GAF_ADV_START_DIRECTED = GAF_CODE(GAF, ADV, GAF_ADV_CMD_TYPE_START_DIRECTED),
    /// Start high-duty cycle directed advertising (see #gaf_adv_start_directed_fast_cmd_t)
    GAF_ADV_START_DIRECTED_FAST = GAF_CODE(GAF, ADV, GAF_ADV_CMD_TYPE_START_DIRECTED_FAST),
};

/// List of GAF_REQ request code values for Generic Audio Framework Advertiser module
enum gaf_adv_msg_req_code
{
    /// Set advertising parameters (see #gaf_adv_set_params_req_t)
    GAF_ADV_SET_PARAMS = GAF_CODE(GAF, ADV, 0),
};

/// List of GAF_IND indication code values for Generic Audio Framework Advertiser module
enum gaf_adv_msg_ind_code
{
    /// Advertising stopped (see #gaf_adv_stopped_ind_t)
    GAF_ADV_STOPPED = GAF_CODE(GAF, ADV, 0),
};

/// @} GAF_ADV_MSG_ENUM

/*
 * API MESSAGES
 ****************************************************************************************
 */

/// @addtogroup GAF_ADV_MSG_STRUCT
/// @{

/// Structure for #GAF_ADV_SET_PARAMS request message
typedef struct gaf_adv_set_params_req
{
    /// Request code (shall be set to #GAF_ADV_SET_PARAMS)
    uint16_t req_code;
    /// Advertising Set local index
    uint8_t set_lid;
    /// Advertising interval in milliseconds for quicker connection setup\n
    /// Set to 30ms by default
    uint8_t quick_adv_intv_ms;
    /// Advertising interval in milliseconds for reduced power advertising\n
    /// Set to 150ms by default
    uint8_t adv_intv_ms;
    /// Indicate on which PHY primary advertising has to be performed\n
    /// LE 1M PHY is used by default\n
    /// LE 2M PHY is not allowed
    uint8_t phy_prim;
    /// Indicate on which PHY secondary advertising has to be performed\n
    /// LE 1M PHY is used by default
    uint8_t phy_second;
    /// Channel Map
    uint8_t ch_map;
    /// Maximum number of advertising events the controller can skip before sending the AUX_ADV_IND packets
    uint8_t max_skip;
    /// Maximum power level at which the advertising packets have to be transmitted (between -127 and 126 dBm)
    int8_t max_tx_pwr;
} gaf_adv_set_params_req_t;

/// Structure for response message
typedef struct gaf_adv_rsp
{
    /// Request code (see #gaf_adv_msg_req_code enumeration)
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// Advertising Set local index
    uint8_t set_lid;
} gaf_adv_rsp_t;

/// Structure for #GAF_ADV_START command message
typedef struct gaf_adv_start_cmd
{
    /// Command code (shall be set to #GAF_ADV_START)
    uint16_t cmd_code;
    /// Advertising Set local index
    uint8_t set_lid;
    /// Configuration bit field (see #gaf_adv_cfg_bf enumeration)
    uint8_t cfg_bf;
    /// Timeout duration in seconds\n
    /// Meaningless if Limited Discoverable mode is used (timeout duration is 30s in that case)\n
    /// 0 means that the advertising will last until #GAF_ADV_STOP command is used
    uint8_t timeout_s;
    /// Advertising SID
    uint8_t adv_sid;
    /// Length of BAP Announcement Metadata\n
    /// Meaningless if BAP Unicast Server is not configured
    uint8_t metadata_len;
    /// Length of additional advertising data
    uint16_t adv_data_len;
    /// Additional advertising data followed by BAP Announcement Metadata in LTV format\n
    uint8_t data[__ARRAY_EMPTY];
} gaf_adv_start_cmd_t;

/// Structure for #GAF_ADV_START_DIRECTED command message
typedef struct gaf_adv_start_directed_cmd
{
    /// Command code (shall be set to #GAF_ADV_START_DIRECTED)
    uint16_t cmd_code;
    /// Advertising Set local index
    uint8_t set_lid;
    /// Configuration bit field (see #gaf_adv_cfg_bf enumeration)
    uint8_t cfg_bf;
    /// Timeout duration in seconds\n
    /// 0 means that the advertising will last until #GAF_ADV_STOP command is used
    uint8_t timeout_s;
    /// Target address
    gap_bdaddr_t target_addr;
    /// Advertising SID
    uint8_t adv_sid;
    /// Length of BAP Announcement Metadata\n
    /// Meaningless if BAP Unicast Server is not configured
    uint8_t metadata_len;
    /// Length of advertising data
    uint16_t adv_data_len;
    /// Additional advertising data followed by BAP Announcement Metadata in LTV format\n
    uint8_t data[__ARRAY_EMPTY];
} gaf_adv_start_directed_cmd_t;

/// Structure for #GAF_ADV_START_DIRECTED_FAST command message
typedef struct gaf_adv_start_directed_fast_cmd
{
    /// Command code (shall be set to #GAF_ADV_START_DIRECTED_FAST)
    uint16_t cmd_code;
    /// Advertising Set local index
    uint8_t set_lid;
    /// Configuration bit field (see #gaf_adv_cfg_bf enumeration)
    uint8_t cfg_bf;
    /// Target address
    gap_bdaddr_t target_addr;
} gaf_adv_start_directed_fast_cmd_t;

/// Structure for #GAF_ADV_STOP command message
typedef struct gaf_adv_stop_cmd
{
    /// Command code (shall be set to #GAF_ADV_STOP)
    uint16_t cmd_code;
    /// Advertising Set local index
    uint8_t set_lid;
} gaf_adv_stop_cmd_t;

/// Structure for command complete event message
typedef struct gaf_adv_cmp_evt
{
    /// Command code (see #gaf_adv_msg_cmd_code enumeration)
    uint16_t cmd_code;
    /// Status
    uint16_t status;
    /// Advertising Set local index
    uint8_t set_lid;
} gaf_adv_cmp_evt_t;

/// Structure for #GAF_ADV_STOPPED indication message
typedef struct gaf_adv_stopped_ind
{
    /// Indication code (see #gaf_adv_msg_ind_code enumeration)
    uint16_t ind_code;
    /// Advertising Set local index
    uint8_t set_lid;
    /// Reason
    uint8_t reason;
} gaf_adv_stopped_ind_t;

/// @} GAF_ADV_MSG

#endif // GAF_ADV_MSG_H_
