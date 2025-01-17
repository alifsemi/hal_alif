/**
 ****************************************************************************************
 *
 * @file iass.h
 *
 * @brief Header file - Immediate Alert Service Server - Native API
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef IASS_H_
#define IASS_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup IASS_API Immediate Alert Service Server
 * @ingroup IAS_API
 * @brief Description of API for Immediate Alert Service Server
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup IASS_API_NATIVE Native API
 * @ingroup IASS_API
 * @brief Description of Native API for Immediate Alert Service Server
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "ias.h"
#include "findt.h"

/// @addtogroup IASS_API_NATIVE
/// @{

/*
 * TYPES DEFINITION
 ****************************************************************************************
 */

/// Set of callback functions for backward communication with an upper layer
typedef findt_cb_t iass_cb_t;

/// @} IASS_MSG_NATIVE

#endif // IASS_H_
