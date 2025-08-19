/**
 ****************************************************************************************
 *
 * @file co_version.h
 *
 * @brief Version definitions for BT5.2
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 *
 ****************************************************************************************
 */

#ifndef _CO_VERSION_H_
#define _CO_VERSION_H_
#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @defgroup CO_VERSION Version Defines
 * @ingroup COMMON
 *
 * @brief Bluetooth Controller Version definitions.
 *
 * @{
 ****************************************************************************************
 */
/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_bt.h"           // BT standard definitions

/// RWBT SW Major Version
#define RWBT_SW_VERSION_MAJOR                   BT_VERSION(CFG_BT_VERSION)

/// RWBT SW Minor Version
#define RWBT_SW_VERSION_MINOR                   0

/// RWBT SW Build Version
#define RWBT_SW_VERSION_BUILD                   18
/// RWBT SW Sub-Build Version
#define RWBT_SW_VERSION_SUB_BUILD               0

/// @} CO_VERSION


#endif // _CO_VERSION_H_
