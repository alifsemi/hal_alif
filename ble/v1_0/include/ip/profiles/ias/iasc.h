/**
 ****************************************************************************************
 *
 * @file iasc.h
 *
 * @brief Header file - Immediate Alert Service Client - Native API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef IASC_H_
#define IASC_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup IASC_API Immediate Alert Service Client
 * @ingroup IAS_API
 * @brief Description of API for Immediate Alert Service Client
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup IASC_API_NATIVE Native API
 * @ingroup IASC_API
 * @brief Description of Native API for Immediate Alert Service Client
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "ias.h"
#include "findl.h"

/// @addtogroup IASC_API_NATIVE
/// @{

/*
 * TYPES DEFINITION
 ****************************************************************************************
 */

/// Set of callback functions for backward communication with an upper layer
typedef struct findl_cb iasc_cb_t;

/*
 * API FUNCTIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable by doing a discovery or restoring bond data
 *
 * @param[in] conidx        Connection index
 * @param[in] con_type      Connection type (see enum #prf_con_type)
 * @param[in] p_ias         Pointer to bond data information (valid if con_type == PRF_CON_NORMAL)
 *
 * @return Function execution status (see enum #hl_err)
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t iasc_enable(uint8_t conidx, uint8_t con_type, const ias_content_t* p_ias)
{
    return (findl_enable(conidx, con_type, p_ias));
}

/**
 ****************************************************************************************
 * @brief Update peer device alert level
 *
 * @param[in] conidx        Connection index
 * @param[in] alert_lvl     Alert level (see enum findl_alert_lvl)
 *
 * @return Function execution status (see enum #hl_err)
 ****************************************************************************************
 */
__STATIC_FORCEINLINE uint16_t iasc_alert_upd(uint8_t conidx, uint8_t alert_lvl)
{
    return (findl_alert_upd(conidx, alert_lvl));
}

/// @} IASC_API_NATIVE

#endif // IASC_H_
