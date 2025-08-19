/**
 ****************************************************************************************
 *
 * @file iass_msg.h
 *
 * @brief Header file - Find Me Target - Message API.
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef FINDT_MSG_H_
#define FINDT_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup FINDT_API_MSG Message API
 * @ingroup FINDT_API
 * @brief Description of Message API for Find Me Profile Target
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "findt.h"

/// @addtogroup FINDT_API_MSG
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Messages IDs for Find Me Profile Target
/*@TRACE*/
enum findt_msg_id
{
    /// Alert indication\n
    /// See #findt_alert_ind_t
    FINDT_ALERT_IND = MSG_ID(FINDT, 0x00),
};

/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

/// Parameters of the #FINDT_ALERT_IND message
typedef struct findt_alert_ind
{
    /// Connection index
    uint8_t conidx;
    /// Alert level (see #findt_alert_lvl enumeration)
    uint8_t alert_lvl;
} findt_alert_ind_t;

/// @} FINDT_API_MSG

#endif // FINDT_MSG_H_
