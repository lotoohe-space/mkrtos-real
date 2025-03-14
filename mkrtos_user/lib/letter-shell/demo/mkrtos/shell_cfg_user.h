/**
 * @file shell_cfg_user.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell config
 * @version 3.0.0
 * @date 2019-12-31
 *
 * @copyright (c) 2019 Letter
 *
 */

#ifndef __SHELL_CFG_USER_H__
#define __SHELL_CFG_USER_H__

#include <u_sys.h>
#include <u_malloc.h>
/**
 * @brief 获取系统时间(ms)
 *        定义此宏为获取系统Tick，如`HAL_GetTick()`
 * @note 此宏不定义时无法使用双击tab补全命令help，无法使用shell超时锁定
 */
#define SHELL_GET_TICK() sys_read_tick()
/**
 * @brief shell内存分配
 *        shell本身不需要此接口，若使用shell伴生对象，需要进行定义
 */
#define SHELL_MALLOC(size) u_malloc(size)

/**
 * @brief shell内存释放
 *        shell本身不需要此接口，若使用shell伴生对象，需要进行定义
 */
#define SHELL_FREE(obj) u_free(obj)
/**
 * @brief 使用函数签名
 *        使能后，可以在声明命令时，指定函数的签名，shell 会根据函数签名进行参数转换，
 *        而不是自动判断参数的类型，如果参数和函数签名不匹配，会停止执行命令
 */
#define SHELL_USING_FUNC_SIGNATURE 1

/**
 * @brief 支持数组参数
 *        使能后，可以在命令中使用数组参数，如`cmd [1,2,3]`
 *        需要使能 `SHELL_USING_FUNC_SIGNATURE` 宏，并且配置 `SHELL_MALLOC`, `SHELL_FREE`
 */
#define SHELL_SUPPORT_ARRAY_PARAM 0
#endif
