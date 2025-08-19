/**
 ****************************************************************************************
 *
 * @file cgmc_msg.h
 *
 * @brief Header file - Continuous Glucose Monitoring Profile Collector/Client Role - Message API.
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef _CGMC_MSG_H_
#define _CGMC_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @addtogroup CGMC
 * @ingroup Profile
 * @brief  Continuous Glucose Monitoring Profile Collector/Client Role - Message API.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h"
#include "cgmp_common.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Message IDs
enum cgmc_msg_ids
{
    /// Enable the Profile Collector task - at connection
    CGMC_ENABLE_REQ                = MSG_ID(CGMC, 0x00),
    /// Response to Enable the Profile Collector task - at connection
    CGMC_ENABLE_RSP                = MSG_ID(CGMC, 0x01),
    ///*** CGMC CHARACTERISTIC REQUESTS
    /// Read Characteristic
    CGMC_READ_CMD                  = MSG_ID(CGMC, 0x02),
    /// Read Characteristic's CCC descriptor
    CGMC_READ_CCC_CMD              = MSG_ID(CGMC, 0x03),

    /// Write Session Start Time Characteristic
    CGMC_WRITE_SESS_START_TIME_CMD = MSG_ID(CGMC, 0x04),
    /// Configure Characteristic's CCC descriptor
    CGMC_CFG_CCC_CMD               = MSG_ID(CGMC, 0x05),
    /// Write Command to the Control Point
    CGMC_WRITE_RACP_CMD            = MSG_ID(CGMC, 0x06),
    /// Write Command to the Specific OPS Control Point
    CGMC_WRITE_OPS_CTRL_PT_CMD     = MSG_ID(CGMC, 0x07),
    /// Receive
    /// *   Continuous Measurement Notification
    /// *   Control Point Response Indication RACP_RESP
    /// *   Specific OPS Control Point Response Indication
    ///     Measurement
    ///     Record Access Control Point response
    ///     CGM Feature
    ///     CGM Status
    ///     CGM Session Start Time
    ///     CGM Session Run Time
    ///     CGM Specific OPS Control Point
    CGMC_VALUE_IND                 = MSG_ID(CGMC, 0x08),
    /// Read CCC value of specific characteristic
    CGMC_RD_CHAR_CCC_IND           = MSG_ID(CGMC, 0x09),

    /// Complete event for the Application commands
    CGMC_CMP_EVT                   = MSG_ID(CGMC, 0x0A),
};

/// Continuous Glucose Monitoring Service Characteristics
enum cgmc_char_ids
{
    /// CGM Measurement Characteristic
    CGMC_CHAR_MEAS = 0,
    /// CGM Feature Characteristic
    CGMC_CHAR_FEAT,
    /// CGM Status Characteristic
    CGMC_CHAR_STATUS,
    /// CGM Session Start Time Characteristic
    CGMC_CHAR_SESS_ST_TIME,
    /// CGM Session Run Time Characteristic
    CGMC_CHAR_SESS_RUN_TIME,
    /// CGM Record Access Control Point Characteristic
    CGMC_CHAR_RACP,
    /// CGM Specific Ops Control Point Characteristic
    CGMC_CHAR_SPEC_OPS_CTRL_PT,

    CGMC_CHAR_MAX,
};

/// Continuous Glucose Monitoring Service Characteristic Descriptors
enum cgmc_desc_ids
{
    /// CGM Measurement Characteristic Client Configuration
    CGMC_DESC_MEAS_CCC = 0,
    /// CGM Record Access Control Point Characteristic Client Configuration
    CGMC_DESC_RACP_CCC,
    /// CGM Specific Ops Control Point Characteristic Client Configuration
    CGMC_DESC_SPEC_OPS_CTRL_PT_CCC,

    CGMC_DESC_MAX,
};


/// Define command operation
enum cgmc_op_codes
{
    /// No Operations
    CGMC_NO_OP                            = 0,

    /// Read value
    CGMC_OP_CODE_READ_VALUE               = 1,
    /// Read CCC
    CGMC_OP_CODE_READ_CCC                 = 2,

    /// Write Session Start Time Characteristic
    CGMC_OP_CODE_WRITE_SESSION_START_TIME = 3,
    /// Write CCC descriptor
    CGMC_OP_CODE_WRITE_CCC                = 4,

    /// RACP request
    CGMC_OP_CODE_RACP                     = 5,
    /// Specific Ops Control Point request
    CGMC_OP_CODE_SPECIFIC_OPS_CTRL_PT     = 6,
};



/// Value identifier read or write
enum cgmc_value_id
{
    /// Read CGM Feature Characteristic
    CGMC_VAL_FEATURES                  = 0,
    /// Read CGM Status Characteristic
    CGMC_VAL_STATUS                    = 1,
    /// Session Start Time Characteristic
    CGMC_VAL_SESSION_START_TIME        = 2,
    /// Session Run Time Characteristic
    CGMC_VAL_SESSION_RUN_TIME          = 3,
    /// Measurement value Characteristic
    CGMC_VAL_MEASUREMENT               = 4,
    /// RACP response Characteristic
    CGMC_VAL_RACP_RSP                  = 5,
    /// Specific Ops Control Point response Characteristic
    CGMC_VAL_SPECIFIC_OPS_CTRL_PT_RSP  = 6,

    /// CCC Measurement Descriptor
    CGMC_VAL_CCC_MEASUREMENT           = 7,
    /// CCC RACP Descriptor
    CGMC_VAL_CCC_RACP                  = 8,
    /// CCC Specific Ops Control Point Descriptor
    CGMC_VAL_CCC_SPECIFIC_OPS_CTRL_PT  = 9,
};


/*
 * API MESSAGE STRUCTURES
 ****************************************************************************************
 */

/**
 * Structure containing the characteristics handles, value handles and descriptors for
 * the Continuous Glucose Monitoring Service
 */
typedef struct cgmc_cgms_content
{
    /// service info
    prf_svc_t           svc;

    /// Characteristic info:
    ///  - Feature
    ///  - Measurement
    prf_char_t      chars[CGMC_CHAR_MAX];

    /// Descriptor handles:
    ///  - Client cfg
    prf_desc_t descs[CGMC_DESC_MAX];
} cgmc_cgms_content_t;

/// Parameters of the #CGMC_ENABLE_REQ message
struct cgmc_enable_req
{
    /// Connection index
    uint8_t             conidx;
    /// Connection type
    uint8_t             con_type;
    /// Sensor features see enum #cgm_feat_bf
    uint32_t            cgm_feat;
    /// Existing handle values CGMC
    cgmc_cgms_content_t cgms;
};

/// Parameters of the #CGMC_ENABLE_RSP message
struct cgmc_enable_rsp
{
    /// Connection index
    uint8_t             conidx;
    /// status
    uint16_t            status;
    /// Read Sensor features see enum #cgm_feat_bf
    uint32_t            cgm_feat;
    /// Existing handle values CGMC
    cgmc_cgms_content_t cgms;
};


///*** CGMC CHARACTERISTIC READ REQUESTS
/// Parameters of the #CGMC_READ_CMD message
/// value:      CGMC_VAL_FEATURES
///             CGMC_VAL_STATUS
///             CGMC_VAL_SESSION_START_TIME
///             CGMC_VAL_SESSION_RUN_TIME

struct cgmc_read_cmd
{
    /// Connection index
    uint8_t  conidx;
    /// Value Identifier see enum #cgmc_value_id
    uint8_t  val_id;
};

/// Parameters of the #CGMC_READ_CCC_CMD message
/// Read CCC values of a CGM characteristic
/// value:      CGMC_VAL_CCC_MEASUREMENT
///             CGMC_VAL_CCC_RACP
///             CGMC_VAL_CCC_SPECIFIC_OPS_CTRL_PT
struct cgmc_read_ccc_cmd
{
    /// Connection index
    uint8_t  conidx;
    /// Value Identifier see enum #cgmc_value_id
    uint8_t  val_id;
};

/// Inform Application about the Characteristic's CCC descriptor
/// Parameters of the #CGMC_RD_CHAR_CCC_IND,
struct cgmc_rd_char_ccc_ind
{
    /// Connection index
    uint8_t  conidx;
    /// Value Identifier see enum #cgmc_value_id
    uint8_t  val_id;
    /// Char. Client Characteristic Configuration
    uint16_t ind_cfg;
};


/// Parameters of the #CGMC_WRITE_SESS_START_TIME_CMD message
struct cgmc_write_sess_start_time_cmd
{
    /// Connection index
    uint8_t               conidx;
    /// CGM Session Start Time
    cgm_sess_start_time_t sess_start_time;
};

///*** CGMC CHARACTERISTIC/CCC DESCRIPTOR  WRITE REQUESTS
/// Parameters of the #CGMC_CFG_CCC_CMD message
/// operation
struct cgmc_cfg_ccc_cmd
{
    /// Connection index
    uint8_t  conidx;
    /// Value Identifier see enum #cgmc_value_id
    uint8_t  val_id;
    /// The Client Characteristic Configuration Value
    uint16_t ccc;
};

/// Write Operation Command to the Control Point
/// Parameters of the #CGMC_WRITE_RACP_CMD,
struct cgmc_write_racp_cmd
{
    /// Connection index
    uint8_t  conidx;
    /// Control Point OpCode see enum #cgmp_racp_opcodes_id
    uint8_t  cp_opcode;
    /// Operator  see enum #cgmp_racp_operator_id
    uint8_t  cp_operator;
    /// Filter type
    uint8_t  filter_type;
    /// Filter parameter
    uint16_t min_time_offset;
    /// Filter parameter
    uint16_t max_time_offset;
};

/// Receive Features
/// *   Receive Measurement Notification 
/// *   Receive Control Point Response Indication RACP_RESP
///    CGMC_VALUE_IND,

/// Indication Record Access Control Point Response
typedef struct cgmc_racp_rsp
{
    /// Control Point OpCode see enum #cgmp_racp_opcodes_id
    uint8_t  cp_opcode;
    /// Request Control Point OpCode see enum #cgmp_racp_opcodes_id
    uint8_t  req_cp_opcode;
    /// Response Code see enum #cgmp_racp_rsp_id
    uint8_t  rsp_code;
    /// Number of Records
    uint16_t records_num;
} cgmc_racp_rsp_t;


/// Write Operation Command to the Specific Ops Control Point
/// Parameters of the #CGMC_WRITE_OPS_CTRL_PT_CMD,
struct cgmc_write_ops_ctrl_pt_cmd
{
    /// Connection index
    uint8_t                conidx;
    /// Control Point OpCode see enum #cgmp_spec_ops_opcodes_id
    uint8_t                opcode;
    /// operand specific to opcode
    union cgm_ops_operand  operand;
};


/// Specific OPS Control Point Response
typedef struct cgmc_ops_ctrl_pt_rsp
{
    /// Op-code Response  see enum #cgmp_spec_ops_opcodes_id
    uint8_t                req_op_code;
    /// operand specific to opcode
    union cgm_ops_operand  operand;
} cgmc_ops_ctrl_pt_rsp_t;

/// Indication the Record Access Control Point response
/// Indication CGM Measurement
/// Parameters of the #CGMC_VALUE_IND,
struct cgmc_value_ind
{
    /// Connection index
    uint8_t  conidx;
    /// Value Identifier see enum #cgmc_value_id
    uint8_t  val_id;
    /// Value to indicate
    union cgmc_value
    {
        /// Measurement
        cgm_meas_value_t              meas;
        /// Record Access Control Point response
        cgmc_racp_rsp_t               racp_rsp;
        /// Specific Ops Control Point response
        cgmc_ops_ctrl_pt_rsp_t        ops_cp_rsp;
        /// CGM Feature
        cgm_feat_value_t              feat;
        /// CGM Status value
        cgm_status_t                  status_val;
        /// CGM Session Start Time
        cgm_sess_start_time_t         start_time;
        /// Run time in minutes
        uint16_t                      sess_run_time;
    } value;
};

/// Complete event for the Application commands
/// Parameters of the #CGMC_CMP_EVT message
struct cgmc_cmp_evt
{
    /// Connection index
    uint8_t  conidx;
    /// Operation  see enum #cgmc_op_codes
    uint8_t  operation;
    /// Status
    uint16_t status;
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/// @} CGMC

#endif //(_CGMC_MSG_H_)
