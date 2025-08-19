/**
 ****************************************************************************************
 *
 * @file cgms.h
 *
 * @brief Header file - Continuous Glucose Monitoring Profile - Native API.
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef _CGMS_H_
#define _CGMS_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @addtogroup CGMS
 * @ingroup Profile
 * @brief Continuous Glucose Monitoring Profile - Native API.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "cgms_msg.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/*
 * NATIVE API CALLBACKS
 ****************************************************************************************
 */

/// Continuous glucose monitoring service server callback set
typedef struct cgms_cb
{
    /**
     ****************************************************************************************
     * @brief Completion of continuous glucose measurement transmission
     *
     * @param[in] conidx        Connection index
     * @param[in] status Status of the procedure execution (see enum #hl_err)
     ****************************************************************************************
     */
    void (*cb_meas_send_cmp)(uint8_t conidx, uint16_t status);

    /**
     ****************************************************************************************
     * @brief Inform that bond data updated for the connection.
     *
     * @param[in] conidx        Connection index
     * @param[in] char_type     Characteristic type see enum #cgms_char_type
     * @param[in] ntf_ind_cfg   Char. Client Characteristic Configuration
     ****************************************************************************************
     */
    void (*cb_bond_data_upd)(uint8_t conidx, uint8_t char_type, uint16_t ind_cfg);

    /**
     ****************************************************************************************
     * @brief Read CGM Status
     *
     * CGM Status must be returned by application using #cgms_rd_status_cfm
     *
     * @param[in] conidx        Connection index
     * @param[in] token         Token information that must be returned in confirmation
     ****************************************************************************************
     */
    void (*cb_rd_status_req)(uint8_t conidx, uint32_t token);

    /**
     ****************************************************************************************
     * @brief Read CGM Session Start Time
     *
     * CGM Status must be returned by application using #cgms_rd_sess_start_time_cfm
     *
     * @param[in] conidx        Connection index
     * @param[in] token         Token information that must be returned in confirmation
     ****************************************************************************************
     */
    void (*cb_rd_sess_start_time_req)(uint8_t conidx, uint32_t token);

    /**
     ****************************************************************************************
     * @brief Read CGM Session Run Time
     *
     * CGM Status must be returned by application using #cgms_rd_sess_run_time_cfm
     *
     * @param[in] conidx        Connection index
     * @param[in] token         Token information that must be returned in confirmation
     ****************************************************************************************
     */
    void (*cb_rd_sess_run_time_req)(uint8_t conidx, uint32_t token);

    /**
     ****************************************************************************************
     * @brief Receive update of CGM Session Start Time
     *
     * @param[in] conidx             Connection index
     * @param[in] p_sess_start_time  Pointer to CGM Session Start Time structure
     ****************************************************************************************
     */
    void (*cb_sess_start_time_upd)(uint8_t conidx, const cgm_sess_start_time_t* p_sess_start_time);

    /**
     ****************************************************************************************
     * @brief Command received on the RACP control point
     *
     * @param[in] conidx          Connection index
     * @param[in] op_code         Control Point OpCode see enum #cgmp_racp_opcodes_id
     * @param[in] func_operator   Operator see enum #cgmp_racp_operator_id
     * @param[in] filter_type     Filter Type
     * @param[in] min_time_offset Min time filter parameter
     * @param[in] max_time_offset Max time filter parameter
     ****************************************************************************************
     */
    void (*cb_racp_req)(uint8_t conidx, uint8_t op_code, uint8_t func_operator, uint8_t filter_type,
                        uint16_t min_time_offset, uint16_t max_time_offset);

    /**
     ****************************************************************************************
     * @brief Completion of RACP command response transmission
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     ****************************************************************************************
     */
    void (*cb_racp_rsp_send_cmp)(uint8_t conidx, uint16_t status);

    /**
     ****************************************************************************************
     * @brief Command received on  the RACP control point
     *
     * @param[in] conidx          Connection index
     * @param[in] opcode          Control Point OpCode see enum #cgmp_spec_ops_opcodes_id
     * @param[in] p_operand       Pointer to operand enum specific to opcode
     ****************************************************************************************
     */
    void (*cb_ops_ctrl_pt_req)(uint8_t conidx, uint8_t op_code, const union cgm_ops_operand* p_operand);

    /**
     ****************************************************************************************
     * @brief Completion of Special OPS Control Point command response transmission
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     ****************************************************************************************
     */
    void (*cb_ops_ctrl_pt_rsp_send_cmp)(uint8_t conidx, uint16_t status);
} cgms_cb_t;

/*
 * NATIVE API FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Restore bond data of a known peer device (at connection establishment)
 *
 * @param[in] conidx       Connection index
 * @param[in] ntf_ind_cfg  Profile characteristic configuration:
 *                           - Bit 0: CGM Measurement Characteristic notification config
 *                           - Bit 1: Record Access Control Point indication config
 *                           - Bit 2: CGM Specific Ops Control Point indication config
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgms_enable(uint8_t conidx, uint16_t ntf_ind_cfg);

/**
 ****************************************************************************************
 * @brief Send continuous glucose measurement information
 *
 * @param[in] conidx    Connection index
 * @param[in] p_meas    Pointer to measurement information
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgms_meas_send(uint8_t conidx, const cgm_meas_value_t* p_meas);

/**
 ****************************************************************************************
 * @brief Send back CGM status to peer device.
 *
 * @param[in] conidx        Connection index
 * @param[in] token         Token information received in request
 * @param[in] status        Accept or reject status code of application (see enum #hl_err)
 * @param[in] p_status_val  Pointer to CGM Status value
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgms_rd_status_cfm(uint8_t conidx, uint32_t token, uint16_t status, const cgm_status_t* p_status_val);


/**
 ****************************************************************************************
 * @brief Send back CGM Session Start Time to peer device.
 *
 * @param[in] conidx             Connection index
 * @param[in] token              Token information received in request
 * @param[in] status             Accept or reject status code of application (see enum #hl_err)
 * @param[in] p_sess_start_time  Pointer to CGM Session Start Time value
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgms_rd_sess_start_time_cfm(uint8_t conidx, uint32_t token, uint16_t status,
                                     const cgm_sess_start_time_t* p_sess_start_time);

/**
 ****************************************************************************************
 * @brief Send back CGM Session Run Time to peer device.
 *
 * @param[in] conidx             Connection index
 * @param[in] token              Token information received in request
 * @param[in] status             Accept or reject status code of application (see enum #hl_err)
 * @param[in] sess_run_time      CGM session run time value
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgms_rd_sess_run_time_cfm(uint8_t conidx, uint32_t token, uint16_t status, uint16_t sess_run_time);


/**
 ****************************************************************************************
 * @brief Send back RACP command response
 *
 * @param[in] conidx             Connection index
 * @param[in] opcode          Control Point OpCode see enum #cgmp_racp_opcodes_id
 * @param[in] racp_status        Record access control point execution status (see enum #cgmp_racp_rsp_id)
 * @param[in] num_of_record      Number of record (meaningful for CGMP_OPCODE_REP_NUM_OF_STRD_RECS operation)
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgms_racp_rsp_send(uint8_t conidx, uint8_t op_code, uint8_t racp_status, uint16_t num_of_record);


/**
 ****************************************************************************************
 * @brief Send back Ops Control Point command response
 *
 * @param[in] conidx             Connection index
 * @param[in] op_code            Ops Control Point OpCode see enum #cgmp_spec_ops_opcodes_id
 * @param[in] p_operand          Pointer to response operand value enum specific to opcode
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgms_ops_ctrl_pt_rsp_send(uint8_t conidx, uint8_t op_code, const union cgm_ops_operand* p_operand);


/// @} CGMS

#endif //(_CGMS_H_)
