/**
 ****************************************************************************************
 *
 * @file cgmc_msg.h
 *
 * @brief Header file - Continuous Glucose Monitoring Profile Collector/Client Role - Native API.
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef _CGMC_H_
#define _CGMC_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @addtogroup CGMC
 * @ingroup Profile
 * @brief  Continuous Glucose Monitoring Profile Collector/Client Role - Native API.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "cgmc_msg.h"

/// Continuous Glucose Monitoring Profile client callback set
typedef struct cgmc_cb
{
    /**
     ****************************************************************************************
     * @brief Completion of enable procedure
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] cgm_feat      Sensor features see enum #cgm_feat_bf
     * @param[in] p_cgms        Pointer to peer database description bond data
     ****************************************************************************************
     */
    void (*cb_enable_cmp)(uint8_t conidx, uint16_t status, uint32_t cgm_feat, const cgmc_cgms_content_t* p_cgms);

    /**
     ****************************************************************************************
     * @brief Completion of read feature procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] p_features    Pointer to CGM Features
     *
     ****************************************************************************************
     */
    void (*cb_read_features_cmp)(uint8_t conidx, uint16_t status, const cgm_feat_value_t* p_features);

    /**
     ****************************************************************************************
     * @brief Completion of read sensor status procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] p_status_val  Pointer to CGM Status value
     *
     ****************************************************************************************
     */
    void (*cb_read_status_cmp)(uint8_t conidx, uint16_t status, const cgm_status_t* p_status_val);

    /**
     ****************************************************************************************
     * @brief Completion of read session start time information procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] p_start_time  Pointer to CGM Session Start Time information
     *
     ****************************************************************************************
     */
    void (*cb_read_sess_start_time_cmp)(uint8_t conidx, uint16_t status, const cgm_sess_start_time_t* p_start_time);

    /**
     ****************************************************************************************
     * @brief Completion of read session run time information procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] sess_run_time Run time in minutes
     *
     ****************************************************************************************
     */
    void (*cb_read_sess_run_time_cmp)(uint8_t conidx, uint16_t status, uint16_t sess_run_time);

    /**
     ****************************************************************************************
     * @brief Completion of read Characteristic Configuration procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] val_id        Value Identifier see enum #cgmc_value_id
     *                              - CGMC_VAL_CCC_MEASUREMENT
     *                              - CGMC_VAL_CCC_RACP
     *                              - CGMC_VAL_CCC_SPECIFIC_OPS_CTRL_PT
     * @param[in] cfg_val       Configuration value
     *
     ****************************************************************************************
     */
    void (*cb_read_cfg_cmp)(uint8_t conidx, uint16_t status, uint8_t val_id, uint16_t cfg_val);

    /**
     ****************************************************************************************
     * @brief Completion of Session start time update procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     *
     ****************************************************************************************
     */
    void (*cb_write_sess_start_time_cmp)(uint8_t conidx, uint16_t status);

    /**
     ****************************************************************************************
     * @brief Completion of write Characteristic Configuration procedure.
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the procedure execution (see enum #hl_err)
     * @param[in] val_id        Value Identifier see enum #cgmc_value_id
     *                              - CGMC_VAL_CCC_MEASUREMENT
     *                              - CGMC_VAL_CCC_RACP
     *                              - CGMC_VAL_CCC_SPECIFIC_OPS_CTRL_PT
     *
     ****************************************************************************************
     */
    void (*cb_write_cfg_cmp)(uint8_t conidx, uint16_t status, uint8_t val_id);

    /**
     ****************************************************************************************
     * @brief Function called when glucose measurement information is received
     *
     * @param[in] conidx         Connection index
     * @param[in] seq_num        Measurement sequence number
     * @param[in] p_meas         Pointer to glucose measurement information
     ****************************************************************************************
     */
    void (*cb_meas)(uint8_t conidx, const cgm_meas_value_t* p_meas);

    /**
     ****************************************************************************************
     * @brief Completion of record access control point request send
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the Request Send (see enum #hl_err)
     * @param[in] req_op_code   Requested Operation Code (see enum #cgmp_racp_opcodes_id)
     ****************************************************************************************
     */
    void (*cb_racp_req_cmp)(uint8_t conidx, uint16_t status, uint8_t req_op_code);


    /**
     ****************************************************************************************
     * @brief Reception of record access point response.
     *
     * @param[in] conidx        Connection index
     * @param[in] req_op_code   Requested Operation Code (see enum #cgmp_racp_opcodes_id)
     * @param[in] racp_status   Record access control point execution status (see enum #cgmp_racp_rsp_id)
     * @param[in] num_of_record Number of record (meaningful for GLP_REQ_REP_NUM_OF_STRD_RECS operation)
     ****************************************************************************************
     */
    void (*cb_racp_rsp_recv)(uint8_t conidx, uint8_t req_op_code, uint8_t racp_status, uint16_t num_of_record);


    /**
     ****************************************************************************************
     * @brief Completion of Special Ops control point request send
     *
     * @param[in] conidx        Connection index
     * @param[in] status        Status of the Request Send (see enum #hl_err)
     * @param[in] req_op_code   Ops Control Point OpCode see enum #cgmp_spec_ops_opcodes_id
     ****************************************************************************************
     */
    void (*cb_ops_ctrl_pt_req_cmp)(uint8_t conidx, uint16_t status, uint8_t req_op_code);

    /**
     ****************************************************************************************
     * @brief Reception of Special Ops control point response.
     *
     * @param[in] conidx        Connection index
     * @param[in] req_op_code   Ops Control Point OpCode see enum #cgmp_spec_ops_opcodes_id
     * @param[in] p_operand     Pointer to response operand value enum specific to opcode
     ****************************************************************************************
     */
    void (*cb_ops_ctrl_pt_rsp_recv)(uint8_t conidx, uint8_t req_op_code,
                                    const union cgm_ops_operand* p_operand);
} cgmc_cb_t;

/*
 * NATIVE API FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Restore bond data of a known peer device (at connection establishment)
 *
 * Wait for #cb_enable_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 * @param[in] con_type      Connection type
 * @param[in] cgm_feat      Sensor features see enum #cgm_feat_bf
 * @param[in] p_cgms        Pointer to peer database description bond data
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_enable(uint8_t conidx, uint8_t con_type, uint32_t cgm_feat, const cgmc_cgms_content_t* p_cgms);

/**
 ****************************************************************************************
 * @brief Perform a read feature procedure.
 *
 * Wait for #cb_read_features_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_read_features(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Perform a read sensor status procedure.
 *
 * Wait for #cb_read_status_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_read_status(uint8_t conidx);
/**
 ****************************************************************************************
 * @brief Perform a read session start time information procedure.
 *
 * Wait for #cb_read_sess_start_time_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_read_sess_start_time(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Perform a read session run time information procedure.
 *
 * Wait for #cb_read_sess_run_time_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_read_sess_run_time(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Perform a read Characteristic Configuration procedure.
 *
 * Wait for #cb_read_cfg_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 * @param[in] val_id        Value Identifier see enum #cgmc_value_id
 *                              - CGMC_VAL_CCC_MEASUREMENT
 *                              - CGMC_VAL_CCC_RACP
 *                              - CGMC_VAL_CCC_SPECIFIC_OPS_CTRL_PT
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_read_cfg(uint8_t conidx, uint8_t val_id);

/**
 ****************************************************************************************
 * @brief Register to Glucose sensor notifications.
 *
 * Wait for #cb_write_sess_start_time_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 * @param[in] p_start_time  Pointer to CGM Session Start Time information
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_write_sess_start_time(uint8_t conidx, const cgm_sess_start_time_t* p_start_time);

/**
 ****************************************************************************************
 * @brief Write Characteristic Configuration.
 *
 * Wait for #cb_write_cfg_cmp execution before starting a new procedure
 *
 * @param[in] conidx        Connection index
 * @param[in] val_id        Value Identifier see enum #cgmc_value_id
 *                              - CGMC_VAL_CCC_MEASUREMENT
 *                              - CGMC_VAL_CCC_RACP
 *                              - CGMC_VAL_CCC_SPECIFIC_OPS_CTRL_PT
 * @param[in] cfg_val       Configuration value
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_write_cfg(uint8_t conidx, uint8_t val_id, uint16_t cfg_val);

/**
 ****************************************************************************************
 * @brief Function called to send a record access control point request
 *
 * Wait for #cb_racp_req_cmp execution before starting a new procedure
 *
 * @param[in] conidx           Connection index
 * @param[in] req_op_code      Requested Operation Code (see #cgmp_racp_opcodes_id)
 * @param[in] func_operator    Function operator (see enum cgmp_racp_operator_id)
 * @param[in] filter_type      Filter type (see enum #cgmp_racp_filter)
 * @param[in] min_time_offset  Pointer to minimum filter range parameter
 * @param[in] max_time_offset  Pointer to maximum filter range parameter
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_racp_req(uint8_t conidx, uint8_t req_op_code, uint8_t func_operator,
                       uint8_t filter_type, uint16_t min_time_offset, uint16_t max_time_offset);

/**
 ****************************************************************************************
 * @brief Function called to send a record access control point request
 *
 * Wait for #cb_ops_ctrl_pt_req_cmp execution before starting a new procedure
 *
 * @param[in] conidx          Connection index
 * @param[in] opcode          Control Point OpCode see enum #cgmp_spec_ops_opcodes_id
 * @param[in] p_operand       Pointer to operand enum specific to opcode
 *
 * @return Status of the function execution (see enum #hl_err)
 ****************************************************************************************
 */
uint16_t cgmc_ops_ctrl_pt_req(uint8_t conidx, uint8_t op_code, const union cgm_ops_operand* p_operand);

/// @} CGMC

#endif //(_CGMC_H_)
