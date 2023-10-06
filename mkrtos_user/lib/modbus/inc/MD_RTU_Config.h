/**
 *@brief Modbus configuration file.
 *@file MD_RTU_Config.h
 *@author zspace
 */
#ifndef _MD_RTU_CONFIG_H__
#define _MD_RTU_CONFIG_H__

#define MD_RTU_USED_OS 0 ///< Whether modbus RTU USES an operating system

#define MD_STRICT_MODE 0         ///< Strict mode
#define MD_FRAME_INTERVAL_TIME 5 ///< Ms

#define MD_RTU_CRC16_FAST_MODE 1 ///< CRC check mode configuration

///< Configuration related to the slave
#define MDS_REG_COIL_ITEM_NUM 20    ///< Maximum number of discrete maps
#define MDS_RTU_CMD_SIZE 256        ///< Single instruction length
#define MDS_RTU_SEND_CACHE_SIZE 256 ///< Send buffer length
#define MDS_USE_SEND_CACHE 1        ///< Whether to enable sending cache
#define MDS_RTU_QUEUE_SIZE 256      ///< Buffer queue size

///< Host related configuration
#define MDM_REG_COIL_ITEM_NUM 20    ///< Maximum number of discrete maps
#define MDM_RTU_SEND_CACHE_SIZE 256 ///< Send buffer length
#define MDM_USE_SEND_CACHE 1        ///< Whether to enable sending cache
#define MDM_RTU_QUEUE_SIZE 256      ///< Buffer queue size

#define MDM_MIX_CALL_DELAY 500 ///< The delay time before the mix call

///< Configuration related to blocking read-write controller
#define MDM_RW_CTRL_LIST_SIZE 20 ///< Maximum number of controllers

#define MDS_USE_IDENTICAL_MAPPING 1 ///< Different slaves use the same mapping source

#ifdef MDS_USE_IDENTICAL_MAPPING
#define STATIC_T static
#else
#define STATIC_T
#endif

#endif
