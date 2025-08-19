/**
 ****************************************************************************************
 *
 * @file iass_msg.h
 *
 * @brief Header file - Immediate Alert Service Server - Message API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef IASS_MSG_H_
#define IASS_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup IASS_API_MSG Message API
 * @ingroup IASS_API
 * @brief Description of Message API for Immediate Alert Service Server
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "iass.h"
#include "findt_msg.h"

/// @addtogroup IASS_API_MSG
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Message IDs for Immediate Alert Service Server
enum iass_msg_id
{
    /// Alert level indication\n
    /// See #iass_alert_level_ind_t
    IASS_ALERT_LEVEL_IND = MSG_ID(IASS, 0x00),
};

/// @} IASS_MSG_MSG

#endif // IASS_MSG_H_
