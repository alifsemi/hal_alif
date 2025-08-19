/**
 ****************************************************************************************
 *
 * @file gapm_msg.h
 *
 * @brief Generic Access Profile Manager Message API.
 *
 * Copyright (C) RivieraWaves 2009-2023
 * Release Identifier: c9ec3d22
 *
 ****************************************************************************************
 */

#ifndef _GAPM_MSG_H_
#define _GAPM_MSG_H_

#include "rom_build_cfg.h"

/**
 ****************************************************************************************
 * @addtogroup GAPM_MSG_API Message API
 * @ingroup GAPM_API
 * @brief Message API for GAP Manager module
 * @details It handles messages from lower and higher layers not related to an ongoing connection.
 * @{
 * @}
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h"
#include "gap.h"
#include "gapm.h"

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// @addtogroup GAPM_MSG_ID_API Message Identifiers
/// @ingroup GAPM_MSG_API
/// @{

/// GAP Manager Message Interface
/*@TRACE*/
enum gapm_msg_ids
{
    /* Default event */
    /// Command Complete event
    GAPM_CMP_EVT = MSG_ID(GAPM, 0x00),
    /// Indicate that a message has been received on an unknown task
    GAPM_UNKNOWN_TASK_IND = MSG_ID(GAPM, 0x01),
    /// Indication to the task that sends the unknown message
    /// see #gapm_unknown_msg_ind_t
    GAPM_UNKNOWN_MSG_IND = MSG_ID(GAPM, 0x02),

    /* Device Configuration */
    /// Reset link layer and the host command
    GAPM_RESET_CMD = MSG_ID(GAPM, 0x03),
    /// Set device configuration command
    GAPM_CONFIGURE_CMD = MSG_ID(GAPM, 0x04),
    /// Set device le channel map
    GAPM_LE_SET_CHANNEL_MAP_CMD = MSG_ID(GAPM, 0x05),
    /// Modify current IRK
    GAPM_LE_SET_IRK_CMD = MSG_ID(GAPM, 0x06),
    /// Set Device Name
    GAPM_SET_NAME_CMD = MSG_ID(GAPM, 0x07),
    /// Insert into SDP a device identification record command.
    GAPM_BT_SET_SDP_IDENTIFICATION_RECORD_CMD = MSG_ID(GAPM, 0x08),
    /// Set acceptable parameters for subrating requests command
    GAPM_LE_SET_DEFAULT_SUBRATE_CMD = MSG_ID(GAPM, 0x09),
    /// Set Rx Max Slot command
    GAPM_BT_SET_RX_MAX_SLOT_CMD = MSG_ID(GAPM, 0x0A),
    #if (BLE_GAPC)
    /// Configure security level required for LE connections command
    /// (see #gapm_le_configure_security_level)
    GAPM_LE_CONFIGURE_SECURITY_LEVEL_CMD = MSG_ID(GAPM, 0x0B),
    #endif // (BLE_GAPC)
    /// Disable a supported feature\n
    /// (see #gapm_disable_feature)
    GAPM_DISABLE_FEATURE_CMD = MSG_ID(GAPM, 0x0C),
    /// Set Appearance
    GAPM_LE_SET_APPEARANCE_CMD = MSG_ID(GAPM, 0x0D),

    /* Local device information */
    /// Get local device info command
    GAPM_GET_INFO_CMD = MSG_ID(GAPM, 0x10),
    /// Local device version indication event
    GAPM_VERSION_IND = MSG_ID(GAPM, 0x11),
    /// BD Address indication event
    /// Could be either controller address (if retrieved using #GAPM_GET_INFO_CMD: #GAPM_GET_CONTROLLER_BDADDR operation).
    /// Device identity (if retrieved using #GAPM_GET_INFO_CMD: #GAPM_GET_IDENTITY operation).
    /// Message can also be triggered by an activity when the resolvable or non-resolvable private address is renew.
    GAPM_BDADDR_IND = MSG_ID(GAPM, 0x12),
    /// Advertising channel Tx power level
    GAPM_LE_ADV_TX_POWER_IND = MSG_ID(GAPM, 0x13),
    /// Debug Indication containing information about memory usage.
    GAPM_DBG_MEMORY_INFO_IND = MSG_ID(GAPM, 0x14),
    /// Indication containing controller antenna information
    GAPM_LE_ANTENNA_INFO_IND = MSG_ID(GAPM, 0x15),
    /// Suggested Default Data Length indication
    GAPM_LE_SUGGESTED_DEFAULT_DATA_LEN_IND = MSG_ID(GAPM, 0x16),
    /// Maximum Data Length indication
    GAPM_LE_MAX_DATA_LEN_IND = MSG_ID(GAPM, 0x17),
    /// Indicate maximum advertising data length supported by controller
    /// see #gapm_le_max_adv_data_len_ind_t
    GAPM_LE_MAX_ADV_DATA_LEN_IND = MSG_ID(GAPM, 0x18),
    /// Indicate number of available advertising sets
    /// see #gapm_le_nb_adv_sets_ind_t
    GAPM_LE_NB_ADV_SETS_IND = MSG_ID(GAPM, 0x19),
    /// Indicate the transmit powers supported by the controller
    /// see #gapm_le_tx_power_ind_t
    GAPM_LE_TX_POWER_IND = MSG_ID(GAPM, 0x1A),
    /// Indicate the RF path compensation values
    /// see #gapm_le_rf_path_compensation_ind_t
    GAPM_LE_RF_PATH_COMPENSATION_IND = MSG_ID(GAPM, 0x1B),
    /// Debug Indication containing statistics of the system.
    GAPM_DBG_STATISTICS_IND = MSG_ID(GAPM, 0x1C),

    /* Security / Encryption Toolbox */
    /// (AES command) Resolve address command
    /// see #gapm_le_resolve_address
    GAPM_LE_RESOLVE_ADDR_CMD = MSG_ID(GAPM, 0x30),
    /// Indicate that resolvable random address has been solved
    GAPM_LE_ADDR_RESOLVED_IND = MSG_ID(GAPM, 0x31),
    /// (AES command) Generate a random address.
    /// see #gapm_le_generate_random_addr
    /// @note Running in parallel different AES command from different task is dangerous,
    ///       result could be provided to last task which request AES function.
    GAPM_LE_GENERATE_RANDOM_ADDR_CMD = MSG_ID(GAPM, 0x32),
    /// (AES command) Use the AES-128 block in the controller
    /// see #gapm_le_aes_cipher
    GAPM_LE_USE_ENCRYPTION_BLOCK_CMD = MSG_ID(GAPM, 0x33),
    ///  AES-128 block result indication
    GAPM_LE_ENCRYPTION_RESULT_IND = MSG_ID(GAPM, 0x34),
    /// (AES command) Generate a 8-byte random number
    /// see gapm_le_generate_random_nb
    GAPM_LE_GENERATE_RANDOM_NB_CMD = MSG_ID(GAPM, 0x35),
    /// Random Number Indication
    GAPM_LE_RANDOM_NB_IND = MSG_ID(GAPM, 0x36),
    /// Request to provide DH Key
    GAPM_LE_COMPUTE_DH_KEY_CMD = MSG_ID(GAPM, 0x39),
    /// Indicates the DH Key computation is complete and available
    GAPM_LE_DH_KEY_IND = MSG_ID(GAPM, 0x3A),
    /// Retrieve Public Key
    GAPM_LE_GET_PUBLIC_KEY_CMD = MSG_ID(GAPM, 0x3B),
    /// Indicates the Public Key Pair value
    GAPM_LE_PUBLIC_KEY_IND = MSG_ID(GAPM, 0x3C),
    /// Generate some OOB Data before a secure connection paring
    GAPM_GENERATE_OOB_DATA_CMD = MSG_ID(GAPM, 0x3D),
    /// Generated LE OOB Data for a following secure connection paring
    GAPM_LE_OOB_DATA_IND = MSG_ID(GAPM, 0x3E),
    /// Generated BT Classic OOB Data for a following secure connection paring
    GAPM_BT_OOB_DATA_IND = MSG_ID(GAPM, 0x3F),

    /* List Management Operations */
    /// Get local or peer address
    /// see #gapm_le_get_ral_addr_cmd_t
    GAPM_LE_GET_RAL_ADDR_CMD = MSG_ID(GAPM, 0x50),
    /// Resolving address list address indication
    GAPM_LE_RAL_ADDR_IND = MSG_ID(GAPM, 0x51),
    /// Set content of either filter accept list or resolving list or periodic advertiser list
    /// see #gapm_le_fill_fal_cmd_t
    /// see #gapm_le_fill_ral_cmd_t
    /// see #gapm_le_fill_pal_cmd_t
    GAPM_LE_SET_LIST_CMD = MSG_ID(GAPM, 0x52),
    /// Indicate size of list indicated in GAPM_GET_DEV_CONFIG_CMD message
    /// see #gapm_le_list_size_ind_t
    GAPM_LE_LIST_SIZE_IND = MSG_ID(GAPM, 0x53),

    /* Air Operations */
    /// Create an advertising, a scanning, an initiating or a periodic synchronization activity
    /// see #gapm_activity_create_cmd_t
    /// see #gapm_le_activity_create_adv_cmd_t
    ///
    /// If #gapm_activity_create_cmd_t.own_addr_type equals either #GAPM_GEN_RSLV_ADDR or #GAPM_GEN_NON_RSLV_ADDR,
    /// #GAPM_BDADDR_IND is triggered to indicate resolvable or non-resolvable address generated for the activity.
    /// This message is also triggered when address is renewed.
    GAPM_ACTIVITY_CREATE_CMD = MSG_ID(GAPM, 0x60),
    /// Start a previously created activity
    /// see #gapm_activity_start_cmd_t
    GAPM_ACTIVITY_START_CMD = MSG_ID(GAPM, 0x61),
    /// Stop either a given activity or all existing activities
    /// see #gapm_activity_stop_cmd_t
    GAPM_ACTIVITY_STOP_CMD = MSG_ID(GAPM, 0x62),
    /// Delete either a given activity or all existing activities
    /// see #gapm_activity_delete_cmd_t
    GAPM_ACTIVITY_DELETE_CMD = MSG_ID(GAPM, 0x63),
    /// Indicate that an activity has well been created
    /// see #gapm_activity_created_ind_t
    GAPM_ACTIVITY_CREATED_IND = MSG_ID(GAPM, 0x64),
    /// Indicate that an activity has been stopped and can be restarted
    /// see #gapm_activity_stopped_ind_t
    GAPM_ACTIVITY_STOPPED_IND = MSG_ID(GAPM, 0x65),
    /// Set either advertising data or scan response data or periodic advertising data
    /// see #gapm_le_set_adv_data_cmd_t
    GAPM_LE_SET_ADV_DATA_CMD = MSG_ID(GAPM, 0x66),
    /// Indicate reception of an advertising report (periodic or not), a scan response report
    /// see #gapm_le_adv_report_ind_t
    GAPM_LE_ADV_REPORT_IND = MSG_ID(GAPM, 0x67),
    /// Indicate reception of a scan request
    /// see #gapm_le_scan_request_ind_t
    GAPM_LE_SCAN_REQUEST_IND = MSG_ID(GAPM, 0x68),
    #if (BLE_PER_ADV)
    /// Indicate that synchronization has been successfully established with a periodic advertiser
    /// see #gapm_le_per_sync_established_ind_t
    GAPM_LE_PER_SYNC_ESTABLISHED_IND = MSG_ID(GAPM, 0x69),
    /// Control reception or not of Periodic Advertising Report in a Periodic Advertising Sync activity
    /// see #gapm_le_control_per_sync_report_cmd_t
    GAPM_LE_CONTROL_PER_ADV_REPORT_CMD = MSG_ID(GAPM, 0x6A),
    /// Control capturing IQ samples from the Constant Tone Extension of periodic advertising packets
    GAPM_LE_CONTROL_PER_SYNC_IQ_SAMPLING_CMD = MSG_ID(GAPM, 0x6B),
    /// Indicate reception of a IQ Report event over a periodic advertising sync activity
    GAPM_LE_PER_ADV_IQ_REPORT_IND = MSG_ID(GAPM, 0x6C),
    /// Control CTE transmission in a periodic advertising activity
    GAPM_LE_CONTROL_PER_ADV_CTE_TX_CMD = MSG_ID(GAPM, 0x6D),
    #endif // (BLE_PER_ADV)
    /// Name of peer device retrieve from device name query procedure
    GAPM_PEER_NAME_IND = MSG_ID(GAPM, 0x6E),
    #if (BLE_BIS)
    /// Indicate reception of periodic advertising report that contains BIGInfo data
    /// see #gapm_le_big_info_ind_t
    GAPM_LE_BIG_INFO_IND = MSG_ID(GAPM, 0x6F),
    #endif // (BLE_BIS)
    /// Reception of an Inquiry report
    GAPM_BT_INQUIRY_REPORT_IND = MSG_ID(GAPM, 0x70),
    /// Set policy for resolvable private address update when advertising data is updated
    GAPM_LE_SET_ADV_ADDR_CHANGE_REASONS_CMD = MSG_ID(GAPM, 0x71),
    /// Peer features discovered during remote name discovery
    GAPM_BT_PEER_FEATURES_IND = MSG_ID(GAPM, 0x72),



    /* LE Test Mode */
    /// Control of the test mode command
    GAPM_LE_CONTROL_TEST_MODE_CMD = MSG_ID(GAPM, 0x90),
    /// Indicate end of test mode
    GAPM_LE_TEST_END_IND = MSG_ID(GAPM, 0x91),
    /// Indicate reception of a IQ report in LE test mode
    GAPM_LE_TEST_IQ_REPORT_IND = MSG_ID(GAPM, 0x92),

    /* BT Test Mode */
    /// Allows the local BR/EDR controller to enter test mode via LMP test commands.
    /// \if btdm see #gapm_bt_write_loopback_mode \endif
    GAPM_BT_WRITE_LOOPBACK_MODE_CMD = MSG_ID(GAPM, 0x93),
    /// Allows the local BR/EDR controller to enter test mode via LMP test commands.
    /// \if btdm see #gapm_bt_write_simple_pairing_debug_mode \endif
    GAPM_BT_ENABLE_DEVICE_UNDER_TEST_MODE_CMD = MSG_ID(GAPM, 0x94),
    /// Configures the BR/EDR controller to use a predefined Diffie Hellman private key for simple pairing
    /// \if btdm see #gapm_bt_write_simple_pairing_debug_mode \endif
    GAPM_BT_WRITE_SIMPLE_PAIRING_DEBUG_MODE_CMD = MSG_ID(GAPM, 0x95),
    /// Configures the BR/EDR controller to enable and disable the two test modes
    /// \if btdm see #gapm_bt_write_secure_connections_test_mode \endif
    GAPM_BT_WRITE_SECURE_CONNECTIONS_TEST_MODE_CMD = MSG_ID(GAPM, 0x96),
    /// Event triggered when executed when reads controller's loopback mode procedure is completed
    /// \if btdm see #GAPM_BT_READ_LOOPBACK_MODE \endif
    GAPM_BT_LOOPBACK_IND = MSG_ID(GAPM, 0x97),

    /* Channel Scan Activity */
    /// Set Channel Scan Activity
    GAPM_LE_CONTROL_CH_SCAN_CMD = MSG_ID(GAPM, 0x98),

    /* Profile Management */
    /// Create new task for specific profile
    GAPM_ADD_PROFILE_CMD = MSG_ID(GAPM, 0xA0),
    /// Inform that profile task has been added.
    GAPM_PROFILE_ADDED_IND = MSG_ID(GAPM, 0xA1),

    /// Controller hardware error indication
    GAPM_CTRL_HW_ERR_IND = MSG_ID(GAPM, 0xC0),

    /* ************************************************ */
    /* ------------ Vendor specific commands ---------- */
    /* ************************************************ */
    /// Provide all needed priority/increment pairs as a buffer that is copied directly to the priority table.\n
    /// This message should be sent before any activity is started in order to avoid unexpected scheduling issues.\n
    /// Activities whose priority increment is 0 no longer benefit from the random increment to ensure that their
    /// priority stays the same.\n
    /// See #gapm_vs_set_sched_prio_cmd_t
    GAPM_VS_SET_SCHED_PRIO_CMD = MSG_ID(GAPM, 0xD0),

    /* ************************************************ */
    /* ---------------- Debug commands ---------------- */
    /* ************************************************ */
    /// Configure the Debug Platform I&Q Sampling generator
    GAPM_LE_DBG_CONFIGURE_IQGEN_CMD = MSG_ID(GAPM, 0xE0),
    /// Undocumented security test command - Debug purpose only
    GAPM_LE_DBG_SECURITY_TEST_CMD = MSG_ID(GAPM, 0xE1),
    /// Undocumented security test result - Debug purpose only
    GAPM_LE_DBG_SECURITY_TEST_IND = MSG_ID(GAPM, 0xE2),

    /* ************************************************ */
    /* -------------- Internal usage only ------------- */
    /* ************************************************ */
    /// Message received to unknown task received
    GAPM_UNKNOWN_TASK_MSG = MSG_ID(GAPM, 0xF0),
};

/// Meaning of bit field indicating features supported by Host
enum gapm_features_bf
{
    /// Bluetooth Low Energy Observer role
    GAPM_FEAT_BYTE0_LE_ROLE_OBSERVER_BIT = CO_BIT(0),
    /// Bluetooth Low Energy Broadcaster role
    GAPM_FEAT_BYTE0_LE_ROLE_BROADCASTER_BIT = CO_BIT(1),
    /// Bluetooth Low Energy Central role
    GAPM_FEAT_BYTE0_LE_ROLE_CENTRAL_BIT = CO_BIT(2),
    /// Bluetooth Low Energy Peripheral role
    GAPM_FEAT_BYTE0_LE_ROLE_PERIPHERAL_BIT = CO_BIT(3),
    /// Bluetooth Classic
    GAPM_FEAT_BYTE0_BT_BIT = CO_BIT(4),
    /// Broadcast Isochronous Stream
    GAPM_FEAT_BYTE0_BIS_BIT = CO_BIT(5),
    /// Connected Isochronous Stream
    GAPM_FEAT_BYTE0_CIS_BIT = CO_BIT(6),
    /// GATT Client
    GAPM_FEAT_BYTE0_GATT_CLI_BIT = CO_BIT(7),

    /// GATT Caching
    GAPM_FEAT_BYTE1_GATT_CACHING_BIT = CO_BIT(0),
    /// Enhanced ATT
    GAPM_FEAT_BYTE1_EATT_BIT = CO_BIT(1),
    /// Periodic Advertising
    GAPM_FEAT_BYTE1_PER_ADV_BIT = CO_BIT(2),
    /// Periodic Advertising Sync Transfer
    GAPM_FEAT_BYTE1_PAST_BIT = CO_BIT(3),
    /// Low Energy Power Control
    GAPM_FEAT_BYTE1_LE_PWR_CTRL_BIT = CO_BIT(4),
    /// Angle of Arrival
    GAPM_FEAT_BYTE1_AOA_BIT = CO_BIT(5),
    /// Angle of Departure
    GAPM_FEAT_BYTE1_AOD_BIT = CO_BIT(6),
    /// Subrating
    GAPM_FEAT_BYTE1_SUBRATING_BIT = CO_BIT(7),

    /// TWS LE Forwarding
    GAPM_FEAT_BYTE2_TWS_LE_FWD_BIT = CO_BIT(0),
    /// Encrypted Advertising Data
    GAPM_FEAT_BYTE2_EAD_BIT = CO_BIT(1),
    /// Security Levels characteristic
    GAPM_FEAT_BYTE2_SEC_LEVELS_CHAR_BIT = CO_BIT(2),
    /// Periodic Advertising With Response
    GAPM_FEAT_BYTE2_PAWR_BIT = CO_BIT(3),
    /// Coding Scheme Selection on Advertising
    GAPM_FEAT_BYTE2_CSSA_BIT = CO_BIT(4),
    /// GAF
    GAPM_FEAT_BYTE2_GAF_BIT = CO_BIT(5),
};

/// List of LTV type values for #GAPM_VERSION_IND message
enum gapm_version_type
{
    /// Supported Profiles\n
    /// Length shall be equal to number of 1-byte Profile Identifiers\n
    /// If present, unique in the LTV string
    GAPM_VERSION_TYPE_SUPPORTED_PROFILES = 0,
    /// Supported Features bit field\n
    /// Length set to #GAPM_VERSION_LENGTH_SUPPORTED_FEATURES\n
    /// See #gapm_features_bf enumeration for bit field meaning\n
    GAPM_VERSION_TYPE_SUPPORTED_FEATURES,
    /// Profile SIG Version\n
    /// Length set to #GAPM_VERSION_LENGTH_PROFILE_SIG_VERSION\n
    ///     - Byte 0: Profile Task Identifier
    ///     - Byte 1: SIG Version X value (X.Y.Z)
    ///     - Byte 2: SIG Version Y value (X.Y.Z)
    ///     - Byte 3: SIG Version Z value (X.Y.Z)
    /// Can be present several times in the LTV string
    GAPM_VERSION_TYPE_PROFILE_SIG_VERSION,
};

/// Length of LTV values for #GAPM_VERSION_IND message
enum gapm_version_length
{
    /// Profile SIG Version (#GAPM_VERSION_TYPE_PROFILE_SIG_VERSION)
    GAPM_VERSION_LENGTH_PROFILE_SIG_VERSION = 4,
    /// Length of Supported Features bit field value (#GAPM_VERSION_TYPE_SUPPORTED_FEATURES)
    GAPM_VERSION_LENGTH_SUPPORTED_FEATURES = 3,
};
/// @} GAPM_MSG_ID_API

/// @addtogroup GAPM_MSG_OPID_API Operation Identifiers
/// @ingroup GAPM_MSG_API
/// @{

/// GAP Manager operation type - application interface
/*@TRACE*/
enum gapm_operation
{
    /* No Operation (if nothing has been requested)     */
    /* ************************************************ */
    /// No operation.
    GAPM_NO_OP = 0x00,

    /* Configuration operations                         */
    /* ************************************************ */
    /// Reset BLE subsystem: LL and HL.
    GAPM_RESET = 0x01,
    /// Perform a platform reset - Debug only
    GAPM_PLATFORM_RESET = 0x02,
    /// Set device configuration
    GAPM_CONFIGURE = 0x03,
    /// Set LE device channel map
    GAPM_LE_SET_CHANNEL_MAP = 0x04,
    /// Set IRK
    GAPM_LE_SET_IRK = 0x05,
    /// Set Device Name
    GAPM_SET_NAME = 0x06,
    /// Insert into SDP a device identification record
    /// \if btdm see #gapm_bt_set_sdp_identification_record \endif
    GAPM_BT_SET_SDP_IDENTIFICATION_RECORD = 0x07,
    /// Set acceptable parameters for subrating requests
    GAPM_LE_SET_DEFAULT_SUBRATE = 0x08,
    /// Set Rx Max Slot
    GAPM_BT_SET_RX_MAX_SLOT = 0x09,
    #if (BLE_GAPC)
    /// Configure security level required for LE connections
    GAPM_LE_CONFIGURE_SECURITY_LEVEL = 0x0A,
    #endif // (BLE_GAPC)
    /// Disable a supported feature
    GAPM_DISABLE_FEATURE = 0x0B,
    /// Set Appearance
    GAPM_LE_SET_APPEARANCE = 0x0C,


    /* Retrieve device information                      */
    /* ************************************************ */
    /// Get Local device version
    /// (see #gapm_get_version)
    GAPM_GET_VERSION = 0x10,
    /// Get Local device BD Address
    /// (see #gapm_get_controller_bdaddr)
    GAPM_GET_CONTROLLER_BDADDR = 0x11,
    /// Get device advertising power level
    GAPM_LE_GET_ADV_TX_POWER = 0x12,
    /// Get Filter Accept List Size.
    GAPM_LE_GET_FAL_SIZE = 0x13,
    /// Retrieve Antenna information
    GAPM_LE_GET_ANTENNA_INFO = 0x14,
    /// Get memory usage - Debug only
    GAPM_DBG_GET_MEMORY_INFO = 0x15,
    /// Get Suggested Default LE Data Length
    GAPM_LE_GET_SUGGESTED_DEFAULT_DATA_LEN = 0x16,
    /// Get Maximum LE Data Length
    GAPM_LE_GET_MAX_DATA_LEN = 0x17,
    /// Get number of available advertising sets
    GAPM_LE_GET_NB_ADV_SETS = 0x18,
    /// Get maximum advertising data length supported by the controller
    GAPM_LE_GET_MAX_ADV_DATA_LEN = 0x19,
    /// Get minimum and maximum transmit powers supported by the controller
    GAPM_LE_GET_TX_POWER = 0x1A,
    /// Get the RF Path Compensation values used in the TX Power Level and RSSI calculation
    GAPM_LE_GET_RF_PATH_COMPENSATION = 0x1B,
    /// Get statistics - Debug only
    GAPM_DBG_GET_STATISTICS = 0x1C,
    /// Get host identity BD Address
    /// (see #gapm_get_identity)
    GAPM_GET_IDENTITY = 0x1D,

    /* Security / Encryption Toolbox                    */
    /* ************************************************ */
    /// Resolve device address
    GAPM_LE_RESOLVE_ADDR = 0x30,
    /// Generate a random address
    GAPM_LE_GENERATE_RANDOM_ADDR = 0x31,
    /// Use the controller's AES-128 block
    GAPM_LE_USE_ENCRYPTION_BLOCK = 0x32,
    /// Generate a 8-byte random number
    GAPM_LE_GENERATE_RANDOM_NB = 0x33,
    /// Generate DH_Key
    GAPM_LE_COMPUTE_DH_KEY = 0x34,
    /// Retrieve Public Key
    GAPM_LE_GET_PUBLIC_KEY = 0x35,
    /// Generate LE OOB Data
    GAPM_LE_GENERATE_OOB_DATA = 0x36,
    /// Generate BT Classic OOB Data
    GAPM_BT_GENERATE_OOB_DATA = 0x37,

    /* List Management for air operations               */
    /* ************************************************ */
    /// Get resolving address list size
    GAPM_LE_GET_RAL_SIZE = 0x50,
    /// Get resolving local address
    GAPM_LE_GET_RAL_LOCAL_RPA = 0x51,
    /// Get resolving peer address
    GAPM_LE_GET_RAL_PEER_RPA = 0x52,
    /// Set content of filter accept list
    GAPM_LE_FILL_FAL = 0x53,
    /// Set content of resolving list
    GAPM_LE_FILL_RAL = 0x54,
    #if (BLE_PER_ADV)
    /// Set content of periodic advertiser list
    GAPM_LE_FILL_PAL = 0x55,
    /// Get periodic advertiser list size
    GAPM_LE_GET_PAL_SIZE = 0x56,
    #endif // (BLE_PER_ADV)

    /* Air Operations                                   */
    /* ************************************************ */
    /// Create advertising activity
    GAPM_LE_CREATE_ADV = 0x60,
    /// Create scanning activity
    GAPM_LE_CREATE_SCAN = 0x61,
    /// Create initiating activity
    GAPM_LE_CREATE_INIT = 0x62,
    #if (BLE_PER_ADV)
    /// Create periodic synchronization activity
    GAPM_LE_CREATE_PER_SYNC = 0x63,
    #endif // (BLE_PER_ADV)
    /// Start an activity
    GAPM_START_ACTIVITY = 0x64,
    /// Stop an activity
    GAPM_STOP_ACTIVITY = 0x65,
    /// Delete an activity
    GAPM_DELETE_ACTIVITY = 0x67,
    /// Set advertising data
    GAPM_LE_SET_ADV_DATA = 0x69,
    /// Set scan response data
    GAPM_LE_SET_SCAN_RESPONSE_DATA = 0x6A,
    #if (BLE_PER_ADV)
    /// Set periodic advertising data
    GAPM_LE_SET_PER_ADV_DATA = 0x6B,
    /// Enable/Disable reception of periodic advertising report
    GAPM_LE_CONTROL_PER_SYNC_REPORT = 0x6C,
    /// Enable / Disable IQ sampling
    GAPM_LE_CONTROL_PER_SYNC_IQ_REPORT = 0x6D,
    /// Enable / Disable CTE transmission
    GAPM_LE_CONTROL_PER_ADV_CTE_TX = 0x6E,
    /// Fake update of periodic advertising data in order to update the Data ID (DID)
    GAPM_LE_SET_PER_ADV_DATA_FAKE = 0x6F,
    #endif // (BLE_PER_ADV)
    /// Create inquiry activity
    GAPM_BT_CREATE_INQUIRY = 0x70,
    /// Create inquiry scan activity
    GAPM_BT_CREATE_INQUIRY_SCAN = 0x71,
    /// Create page activity
    GAPM_BT_CREATE_PAGE = 0x72,
    /// Create page scan activity
    GAPM_BT_CREATE_PAGE_SCAN = 0x73,
    /// Set policy for resolvable private address update when advertising data is updated
    GAPM_LE_SET_ADV_ADDR_CHANGE_REASONS = 0x74,
    /// Force advertising data. Host doen't check content of the data.
    /// AD Type Flag is not automatically added and configured according to advertising mode.
    /// (see #gapm_le_force_adv_data)
    GAPM_LE_FORCE_ADV_DATA = 0x75,



    /* LE Direct Test Mode                              */
    /* ************************************************ */
    /// Stop the test mode
    GAPM_LE_STOP_TEST = 0x90,
    /// Start RX Test Mode
    GAPM_LE_START_TEST_RX = 0x91,
    /// Start TX Test Mode
    GAPM_LE_START_TEST_TX = 0x92,

    /* BT Test Mode                                     */
    /* ************************************************ */
    /// Allows the local BR/EDR controller to enter test mode via LMP test commands.
    GAPM_BT_WRITE_LOOPBACK_MODE = 0x93,
    /// Configures the BR/EDR controller to use a predefined Diffie Hellman private key
    GAPM_BT_ENABLE_DEVICE_UNDER_TEST_MODE = 0x94,
    /// Configures the BR/EDR controller to use a predefined Diffie Hellman private key for simple pairing
    GAPM_BT_WRITE_SIMPLE_PAIRING_DEBUG_MODE = 0x95,
    /// Configures the BR/EDR controller to enable and disable the two test modes
    GAPM_BT_WRITE_SECURE_CONNECTIONS_TEST_MODE = 0x96,
    /// Read BT loopback mode using #GAPM_GET_INFO_CMD
    /// \if btdm see #gapm_bt_read_loopback_mode \endif
    GAPM_BT_READ_LOOPBACK_MODE = 0x97,

    /* Channel Scan Activity                            */
    /* ************************************************ */
    /// Start Channel Scan Activity
    GAPM_LE_START_CH_SCAN = 0x98,
    /// Stop Channel Scan Activity
    GAPM_LE_STOP_CH_SCAN = 0x99,

    /* Profile Management                               */
    /* ************************************************ */
    /// Create new task for specific profile
    GAPM_ADD_PROFILE = 0xA0,

    /* Vendor Specific Commands                         */
    /* ************************************************ */
    /// Provide all needed priority/increment pairs as a buffer that is copied directly to the priority table
    GAPM_VS_SET_SCHED_PRIO = 0xD0,

    /* Debug Commands                                   */
    /* ************************************************ */
    /// Configure the Debug Platform I&Q Sampling generator
    GAPM_LE_DBG_CONFIGURE_IQGEN = 0xE0,
    /// Undocumented security test command - Debug purpose only
    GAPM_LE_DBG_SECURITY_TEST = 0xE1,
};
/// @} GAPM_MSG_OPID_API

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// @addtogroup GAPM_MSG_STRUCT_API Message Structures
/// @ingroup GAPM_MSG_API
/// @{

/// Operation command structure in order to keep requested operation.
typedef struct gapm_operation_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    uint8_t operation;
} gapm_operation_cmd_t;

/// Command complete event data structure
/*@TRACE*/
typedef struct gapm_cmp_evt
{
    /// Requested operation type (see #gapm_operation enumeration)
    uint8_t operation;
    /// Status of the request
    uint16_t status;
    /// Activity index (valid only for air operation, else discard)
    uint8_t actv_idx;
} gapm_cmp_evt_t;

///  Reset link layer and the host command
/*@TRACE*/
typedef struct gapm_reset_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    /// - #GAPM_RESET: Reset BLE subsystem: LL and HL.
    uint8_t operation;
} gapm_reset_cmd_t;

/// Set device configuration command
/*@TRACE*/
typedef struct gapm_configure_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_CONFIGURE: Set device configuration
    uint8_t operation;
    /// Device configuration
    gapm_config_t cfg;
} gapm_configure_cmd_t;

/// Parameters of #GAPM_SET_NAME_CMD
/*@TRACE*/
typedef struct gapm_set_name_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_SET_NAME: Set device name
    uint8_t operation;
    /// Size of the device name
    uint8_t name_len;
    /// Device Name
    uint8_t name[__ARRAY_EMPTY];
} gapm_set_name_cmd_t;

/// Parameters of #GAPM_LE_SET_APPEARANCE_CMD
/*@TRACE*/
typedef struct gapm_le_set_appearance_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_LE_SET_APPEARANCE: Set Appearance
    uint8_t operation;
    /// Appearance
    uint16_t appearance;
} gapm_le_set_appearance_cmd_t;

/// Parameters of #GAPM_GET_INFO_CMD
/*@TRACE*/
typedef struct gapm_get_info_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_GET_VERSION: Get Local device version
    ///  - #GAPM_GET_CONTROLLER_BDADDR: Get controller BD Address
    ///  - #GAPM_GET_IDENTITY: Get host identity address
    ///  - #GAPM_LE_GET_ADV_TX_POWER: Get device advertising power level
    ///  - #GAPM_DBG_GET_MEMORY_INFO: Get memory usage (debug only)
    ///  - #GAPM_DBG_GET_STATISTICS: Get device statistics
    ///  - #GAPM_LE_GET_SUGGESTED_DEFAULT_DATA_LEN: Get Suggested Default LE Data Length
    ///  - #GAPM_LE_GET_MAX_DATA_LEN: Get Maximum LE Data Length
    ///  - #GAPM_LE_GET_FAL_SIZE: Get size of Filter Accept List
    ///  - #GAPM_LE_GET_PAL_SIZE: Get size of Periodic Address List
    ///  - #GAPM_LE_GET_RAL_SIZE: Get size of Resolving Address List
    ///  - #GAPM_LE_GET_NB_ADV_SETS: Read number of advertising sets currently supported by the controller
    ///  - #GAPM_LE_GET_MAX_ADV_DATA_LEN: Get maximum data length for advertising data
    ///  - #GAPM_LE_GET_ANTENNA_INFO: Retrieve Antenna information
    ///  - #GAPM_LE_GET_TX_POWER: Get minimum and maximum transmit power values supported by the controller
    ///  - #GAPM_LE_GET_RF_PATH_COMPENSATION: Get the RF Path Compensation values used in the TX Power Level and RSSI
    /// calculation
    ///  - #GAPM_BT_READ_LOOPBACK_MODE: Read BT loopback mode
    uint8_t operation;
} gapm_get_info_cmd_t;

/// Parameters of #GAPM_DISABLE_FEATURE_CMD message
/*@TRACE*/
typedef struct gapm_disable_feature_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_DISABLE_FEATURE: Disable GAPM feature.
    uint8_t operation;
    /// Feature to be disabled descriptor (see #gapm_optional_features)
    uint16_t feature_idx;
} gapm_disable_feature_cmd_t;

/// Local device version indication event
/*@TRACE*/
typedef struct gapm_version_ind
{
    /// HCI version
    uint8_t hci_ver;
    /// LMP version
    uint8_t lmp_ver;
    /// Host version
    uint8_t host_ver;
    /// HCI revision
    uint16_t hci_subver;
    /// LMP subversion
    uint16_t lmp_subver;
    /// Host revision
    uint16_t host_subver;
    /// Manufacturer name
    uint16_t manuf_name;
    /// Length of information field
    uint16_t length;
    /// Information (in LTV format)\n
    /// See #gapm_version_type enumeration for list of available type and their meaning
    uint8_t info[__ARRAY_EMPTY];
} gapm_version_ind_t;

/// Parameters of @ref GAPM_BDADDR_IND message
/*@TRACE*/
typedef struct gapm_bdaddr_ind
{
    /// Address information
    gap_bdaddr_t addr;
    /// Activity index
    uint8_t actv_idx;
} gapm_bdaddr_ind_t;

/// Parameters of the @ref GAPM_GENERATE_OOB_DATA_CMD message
/*@TRACE*/
typedef struct gapm_generate_oob_data_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    /// - #GAPM_LE_GENERATE_OOB_DATA: LE OOB data
    /// - #GAPM_BT_GENERATE_OOB_DATA: BT-Classic OOB data
    uint8_t operation;
} gapm_generate_oob_data_cmd_t;

/// Create new task for specific profile
/*@TRACE*/
typedef struct gapm_add_profile_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_ADD_PROFILE: Add new profile task
    uint8_t operation;
    /// Service Security level - Only for a GATT service (see #gatt_svc_info_bf enumeration)
    uint8_t sec_lvl;
    /// GATT user priority
    uint8_t user_prio;
    /// Profile Application identifier (use profile identifier)
    uint16_t prf_api_id;
    /// Application task number
    uint16_t app_task;
    /// Service start handle
    /// Only applies for services - Ignored by collectors
    /// 0: dynamically allocated in Attribute database
    uint16_t start_hdl;
    /// 32 bits value that contains value to initialize profile (database parameters, etc...)
    uint32_t param[__ARRAY_EMPTY];
} gapm_add_profile_cmd_t;

/// Inform that profile task has been added.
/*@TRACE*/
typedef struct gapm_profile_added_ind
{
    /// Profile task identifier
    uint16_t prf_task_id;
    /// Profile task number allocated
    uint16_t prf_task_nb;
    /// Service start handle
    /// Only applies for services - Ignored by collectors
    uint16_t start_hdl;
} gapm_profile_added_ind_t;

/// Indicate that a message has been received on an unknown task
/*@TRACE*/
typedef struct gapm_unknown_task_ind
{
    /// Message identifier
    uint16_t msg_id;
    /// Task identifier
    uint16_t task_id;
} gapm_unknown_task_ind_t;

/// Create an advertising, a scanning, an initiating, a periodic synchronization activity command (common)
/*@TRACE*/
typedef struct gapm_activity_create_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_LE_CREATE_ADV: Create advertising activity
    ///  - #GAPM_LE_CREATE_SCAN: Create scanning activity
    ///  - #GAPM_LE_CREATE_INIT: Create initiating activity
    ///  - #GAPM_LE_CREATE_PER_SYNC: Create periodic synchronization activity
    ///  - #GAPM_BT_CREATE_INQUIRY: Create inquiry activity
    ///  - #GAPM_BT_CREATE_INQUIRY_SCAN: Create inquiry scan activity
    ///  - #GAPM_BT_CREATE_PAGE: Create page activity
    ///  - #GAPM_BT_CREATE_PAGE_SCAN: Create page scan activity
    ///  - #GAPM_LE_PAWR_CREATE: Create Periodic Advertising with Response activity as Broadcaster
    ///  - #GAPM_LE_PAWR_SYNC_CREATE: Create Periodic Advertising with Response activity as Observer
    uint8_t operation;
    /// Own address type (see #gapm_le_own_addr enumeration)
    uint8_t own_addr_type;
} gapm_activity_create_cmd_t;

/// Start a given activity command
/*@TRACE*/
typedef struct gapm_activity_start_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_START_ACTIVITY: Start a given activity
    uint8_t operation;
    /// Activity identifier
    uint8_t actv_idx;
    /// Activity parameters
    uint16_t u_param[__ARRAY_EMPTY]; // 16-bit aligned parameters
} gapm_activity_start_cmd_t;

/// Stop one or all activity(ies) command
/*@TRACE*/
typedef struct gapm_activity_stop_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_STOP_ACTIVITY: Stop a given activity
    uint8_t operation;
    /// Activity identifier - used only if operation is GAPM_STOP_ACTIVITY
    uint8_t actv_idx;
} gapm_activity_stop_cmd_t;

/// Delete one or all activity(ies) command
/*@TRACE*/
typedef struct gapm_activity_delete_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_DELETE_ACTIVITY: Delete a given activity
    uint8_t operation;
    /// Activity identifier - used only if operation is GAPM_DELETE_ACTIVITY
    uint8_t actv_idx;
} gapm_activity_delete_cmd_t;

/// Indicate creation of an activity
/*@TRACE
 @trc_exec activity_map[$actv_idx] = $actv_type
 activity_map = {}*/
typedef struct gapm_activity_created_ind
{
    /// Activity identifier
    uint8_t actv_idx;
    /// Activity type (see #gapm_actv_type enumeration)
    uint8_t actv_type;
    /// Selected TX power for advertising activity
    int8_t tx_pwr;
} gapm_activity_created_ind_t;

/// Indicate that an activity has been stopped
/*@TRACE*/
typedef struct gapm_activity_stopped_ind
{
    /// Activity identifier
    uint8_t actv_idx;
    /// Activity type (see #gapm_actv_type enumeration)
    uint8_t actv_type;
    /// Activity stop reason (see #hl_err enumeration)
    uint16_t reason;
    /// In case of periodic advertising, indicate if periodic advertising has been stopped
    uint8_t per_adv_stop;
} gapm_activity_stopped_ind_t;

/// Indicate that an unknown message has been received
/*@TRACE*/
typedef struct gapm_unknown_msg_ind
{
    /// Unknown message id
    uint16_t unknown_msg_id;
} gapm_unknown_msg_ind_t;

/// Name of peer device indication
/*@TRACE*/
typedef struct gapm_peer_name_ind
{
    /// peer device bd address
    gap_bdaddr_t addr;
    /// peer device name length
    uint8_t name_len;
    /// peer device name
    uint8_t name[__ARRAY_EMPTY];
} gapm_peer_name_ind_t;


/// Parameters of #GAPM_VS_SET_SCHED_PRIO_CMD message
/*@TRACE*/
typedef struct gapm_vs_set_sched_prio_cmd
{
    /// Requested operation type (see #gapm_operation enumeration)
    ///  - #GAPM_VS_SET_SCHED_PRIO: Set scheduling priorities
    uint8_t operation;
    /// Number of priority/increment pairs\n
    /// Must be set to RWIP_PRIO_IDX_MAX
    uint8_t pair_count;
    /// Priority/increment pairs'n
    /// Length must be 2 * pair_count
    uint8_t prio_data[__ARRAY_EMPTY];
} gapm_vs_set_sched_prio_cmd_t;

/// Controller Hardware error indication
/*@TRACE*/
typedef struct gapm_ctrl_hw_err_ind
{
    /// See #enum co_error "HW ERROR CODES"
    uint8_t hw_err_code;
} gapm_ctrl_hw_err_ind_t;

/// @} GAPM_MSG_STRUCT_API

#endif /* _GAPM_MSG_H_ */
