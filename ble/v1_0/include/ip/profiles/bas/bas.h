/**
 ****************************************************************************************
 *
 * @file bas.h
 *
 * @brief Header file - Battery Service - API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef BAS_H_
#define BAS_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup BAS_API Battery Service (BAS)
 * @ingroup PROFILE_API
 * @brief Description of API for Battery Service
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup BAS_API_COMMON Common
 * @ingroup BAS_API
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h"
#include "prf_types.h"

/// @addtogroup BAS_API_COMMON
/// @{

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximal battery level value
#define BAS_BATTERY_LVL_MAX               (100)

/// @} BAS_API_COMMON

#endif // BAS_H_
