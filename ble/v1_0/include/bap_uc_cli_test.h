/**
 ****************************************************************************************
 *
 * @file bap_uc_cli_test.h
 *
 * @brief Basic Audio Profile - Unicast Client (Test) - Definitions
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef BAP_UC_CLI_TEST_H_
#define BAP_UC_CLI_TEST_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup BAP_UC_CLI_FUNCTION_TEST Test
 * @ingroup BAP_UC_CLI_FUNCTION
 * @brief Description of test functions for Unicast Client module
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "bap_uc_cli.h"         // Basic Audio Profile - Unicast Client Definitions

/*
 * API FUNCTIONS DECLARATION
 ****************************************************************************************
 */

#if (GAF_BAP_UC_CLI)
#if (GAF_BAP_TEST_PARAM_SUPPORT)
/// @addtogroup BAP_UC_CLI_FUNCTION_TEST
/// @{

/**
 ****************************************************************************************
 * @brief Create a group (using test parameters)
 *
 * @param[in] cig_id            CIG ID
 * @param[in] p_param           Pointer to Group parameters structure
 * @param[out] p_grp_lid        Pointer at which allocated Group local index is returned
 *
 * @return An error status (see #gaf_err enumeration)
 ****************************************************************************************
 */
uint16_t bap_uc_cli_create_group_test(uint8_t cig_id, gapi_ug_test_param_t* p_param, uint8_t* p_grp_lid);

/**
 ****************************************************************************************
 * @brief Configure QoS for an ASE (using test parameters)
 *
 * @param[in] ase_lid           ASE local index
 * @param[in] grp_lid           Group local index
 * @param[in] cis_id            CIS ID
 * @param[in] p_qos_cfg         Pointer to QoS configuration structure
 * @param[in] p_us_param        Pointer to Stream parameters (memory must be allocated by upper layer)
 * @param[in] max_tlatency_ms   Maximum transport latency in milliseconds
 *
 * @return An error status (see #gaf_err enumeration)
 ****************************************************************************************
 */
uint16_t bap_uc_cli_configure_qos_test(uint8_t ase_lid, uint8_t grp_lid, uint8_t cis_id,
                                       const bap_uc_cli_qos_cfg_t* p_qos_cfg, gapi_us_test_param_t* p_us_param,
                                       uint16_t max_tlatency_ms);

/// @} BAP_UC_CLI_FUNCTION_TEST
#endif //(GAF_BAP_TEST_PARAM_SUPPORT)
#endif //(GAF_BAP_UC_CLI)

#endif // BAP_UC_CLI_H_
