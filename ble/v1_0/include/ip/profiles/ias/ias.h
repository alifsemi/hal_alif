/**
 ****************************************************************************************
 *
 * @file ias.h
 *
 * @brief Header file - Immediate Alert Service - Definitions
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef IAS_H_
#define IAS_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup IAS_API Immediate Alert Service (IAS)
 * @ingroup PROFILE_API
 * @brief Description of Immediate Alert Service API
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup IAS_API_COMMON Common
 * @ingroup IAS_API
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h"
#include "prf_types.h"

/// @addtogroup IAS_API_COMMON
/// @{

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Alert level values
enum ias_alert_level
{
    /// No alert - 0
    IAS_ALERT_LEVEL_NONE = 0,
    /// Mild alert - 1
    IAS_ALERT_LEVEL_MILD,
    /// High alert - 2
    IAS_ALERT_LEVEL_HIGH,

    /// Maximal value - Alert level cannot be equal or larger
    IAS_ALERT_LEVEL_MAX,
};

/// @} IAS_API

#endif // IAS_H_
