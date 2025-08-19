/**
 ****************************************************************************************
 *
 * @file cgms_msg.h
 *
 * @brief Header file - Continuous Glucose Monitoring Profile - Message API.
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef _CGMS_MSG_H_
#define _CGMS_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @addtogroup CGMS
 * @ingroup Profile
 * @brief Continuous Glucose Monitoring Profile - Message API.
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

/// Messages for Continuous Glucose Monitoring Profile Service
enum cgms_msg_id
{
    /// Enable the CGMP Sensor task at connection
    CGMS_ENABLE_REQ              = MSG_ID(CGMS, 0x00),
    /// Enable the CGMP Sensor task at connection
    CGMS_ENABLE_RSP              = MSG_ID(CGMS, 0x01),
    /// Send the Measurement or Measurement Record
    CGMS_MEAS_VALUE_CMD          = MSG_ID(CGMS, 0x02),

    /// Write to the Session Start Time Indication
    CGMS_WR_SESS_START_TIME_IND  = MSG_ID(CGMS, 0x03),

    /// Read Request for the CGM Status, CGM Session Start Time, CGM Session Run Time
    CGMS_RD_CHAR_REQ_IND         = MSG_ID(CGMS, 0x04),
    /// Read confirmation from the app with data
    CGMS_RD_CHAR_CFM             = MSG_ID(CGMS, 0x05),

    /// Inform Application on the Characteristics CCC descriptor changes
    CGMS_WR_CHAR_CCC_IND         = MSG_ID(CGMS, 0x06),

    /// Write to the Control Point forward to Application
    CGMS_RACP_REQ_RECV_IND       = MSG_ID(CGMS, 0x07),
    /// Send Control Point Response
    CGMS_RACP_RSP_SEND_CMD       = MSG_ID(CGMS, 0x08),

    /// Write to the OPS Control Point
    CGMS_OPS_CP_REQ_RECV_IND     = MSG_ID(CGMS, 0x09),
    /// Confirmation from OPS Control Point
    CGMS_OPS_CP_RSP_SEND_CMD     = MSG_ID(CGMS, 0x0A),

    /// Complete event for the Application commands
    CGMS_CMP_EVT                 = MSG_ID(CGMS, 0x0B),
};

/// Define Complete Event command (CGMS_CMP_EVT) operation op codes
enum cgms_op_codes
{
    /// CGMS Measurement operation
    CGMS_MEAS_CMD_OP_CODE            = 1,
    /// Special OPS Control Point operation
    CGMS_OPS_CTRL_PT_OP_CODE         = 2,
    /// RACP operation
    CGMS_RACP_CMD_OP_CODE            = 3,
};

/// Define characteristic type
enum cgms_char_type
{
    /// CGM Measurement CCC
    CGMS_CHAR_ID_MEAS_CCC               = 1,
    /// CGM Specific Ops Control Point CCC
    CGMS_CHAR_ID_OPS_CTRL_PT_CCC        = 2,
    /// Record Access Control Point CCC
    CGMS_CHAR_ID_RACP_CCC               = 3,
    /// CGM Status value
    CGMS_CHAR_ID_STATUS_VAL             = 4,
    /// CGM Session Start Time value
    CGMS_CHAR_ID_SESSION_START_TIME_VAL = 5,
    /// CGM Session Run Time value
    CGMS_CHAR_ID_SESSION_RUN_TIME_VAL   = 6,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the initialization function
struct cgms_db_cfg
{
    /// If enabled in CGM Features see enum #cgm_feat_bf
    uint32_t cgm_feature;
    /// Define the Type of Sample see enum #cgmp_type_sample_id
    uint8_t  type_sample;
    /// Define the Location of Sample see enum #cgmp_sample_location_id
    uint8_t  sample_location;
};

/// Parameters of the #CGMS_ENABLE_REQ message
struct cgms_enable_req
{
    /// Connection index
    uint8_t  conidx;
    /**
     * Profile characteristic configuration:
     *   CGM Measurement Characteristic notification config Bit 0
     *   Record Access Control Point indication config      Bit 1
     *   CGM Specific Ops Control Point indication config   Bit 2
     */
    uint16_t prfl_ntf_ind_cfg;
};

/// Parameters of the #CGMS_ENABLE_RSP message
struct cgms_enable_rsp
{
    /// Connection index
    uint8_t  conidx;
    /// Status
    uint16_t status;
};

/// CGM Measurement Value
/// Parameters of the #CGMS_MEAS_VALUE_CMD,
struct cgms_meas_value_cmd
{
    /// Connection index
    uint8_t          conidx;
    /// CGMS_MEAS_CMD_OP_CODE for this command see enum #cgms_op_codes
    uint8_t          operation;
    /// Measurement information
    cgm_meas_value_t meas;
};


/// CGM Status
/// Parameters of the #CGMS_RD_CHAR_REQ_IND
struct cgms_rd_char_req_ind
{
    /// Connection index
    uint8_t  conidx;
    /// characteristic id #cgms_char_type
    uint8_t  char_type;
    /// Token information that must be returned in confirmation message
    uint32_t token;
};

/// Parameters of the #CGMS_RD_CHAR_CFM message
struct cgms_rd_char_cfm
{
    /// Connection index
    uint8_t  conidx;
    /// characteristic id #cgms_char_type
    uint8_t  char_type;
    /// Operation Status
    uint16_t status;
    /// Token information received in request message
    uint32_t token;
    /// data for the characteristic
    union 
    {
        /// CGM Status value
        cgm_status_t          status_val;
        /// CGM Session Start Time
        cgm_sess_start_time_t sess_start_time;
        /// CGM Session Run Time
        uint16_t              sess_run_time;
    } value;
}; 

/// Parameters of the #CGMS_WR_SESS_START_TIME_IND message
struct cgms_wr_sess_start_time_ind
{
    /// Connection index
    uint8_t               conidx;
    /// CGM Session Start Time
    cgm_sess_start_time_t sess_start_time;
};

/// Inform Application on the Characteristic's CCC descriptor changes
/// Parameters of the #CGMS_WR_CHAR_CCC_IND,
struct cgms_wr_char_ccc_ind
{
    /// Connection index
    uint8_t  conidx;
    /// Characteristic type see enum #cgms_char_type
    uint8_t  char_type;
    /// Char. Client Characteristic Configuration
    uint16_t ind_cfg;
};

/// Write to the Control Point forward to Application
/// Parameters of the #CGMS_RACP_REQ_RECV_IND,
struct cgms_racp_req_recv_ind
{
    /// Connection index
    uint8_t  conidx;
    /// Control Point OpCode see enum #cgmp_racp_opcodes_id
    uint8_t  req_opcode;
    /// Operator see enum #cgmp_racp_operator_id
    uint8_t  operator;
    /// Filter Type
    uint8_t  filter_type;
    /// Filter parameter
    uint16_t min_time_offset;
    /// Filter parameter
    uint16_t max_time_offset;
};

/// Send Control Point Response
/// Parameters of the #    CGMS_RACP_RSP_SEND_CMD,
struct cgms_racp_rsp_send_cmd
{
    /// Connection index
    uint8_t  conidx;
    /// Operation = CGMS_RACP_CMD_OP_CODE see enum #cgms_op_codes
    uint8_t  operation;
    /// Control Point OpCode see enum #cgmp_racp_opcodes_id
    uint8_t  req_opcode;
    /// Response Code see enum #cgmp_racp_rsp_id
    uint8_t  rsp_code;
    /// Number of Records
    uint16_t records_num;
};


/// Write to the Control Point forward to Application
/// Parameters of the #CGMS_OPS_CP_REQ_RECV_IND,
struct cgms_ops_cp_req_recv_ind
{
    /// Connection index
    uint8_t               conidx;
    /// Control Point OpCode see enum #cgmp_spec_ops_opcodes_id
    uint8_t               req_opcode;
    /// operand specific to opcode
    union cgm_ops_operand operand;
};

/// OPS Control Point Response
/// Parameters of the #CGMS_OPS_CP_RSP_SEND_CMD,
struct cgms_ops_cp_rsp_send_cmd
{
    /// Connection index
    uint8_t               conidx;
    /// Operation = CGMS_OPS_CTRL_PT_OP_CODE see enum #cgms_op_codes
    uint8_t               operation;
    /// Control Point OpCode see enum #cgmp_spec_ops_opcodes_id
    uint8_t               req_opcode;
    /// operand specific to opcode
    union cgm_ops_operand operand;
};

/// Send App command complete  event
/// Parameters of the #CGMS_CMP_EVT message
struct cgms_cmp_evt
{
    /// Connection index
    uint8_t  conidx;
    /// Operation see enum #cgms_op_codes
    uint8_t  operation;
    /// Operation Status
    uint16_t status;
};

/// @} CGMS

#endif //(_CGMS_MSG_H_)
