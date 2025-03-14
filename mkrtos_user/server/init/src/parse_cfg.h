
/**
 * @file parse_cfg.h
 * @author ATShining (1358745329@qq.com)
 * @brief
 * @version 0.1
 * @date 2023-11-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <u_env.h>
#define CMD_LEN 64                         //!< 命令行最大长度
#define CMD_PARAMS_CN CONFIG_APP_PARAMS_NR //!< 参数个数
#define CMD_PARAMS_ITEM_LEN 16             //!< 每个参数的最大长度
void parse_cfg_init(void);
int parse_cfg(const char *parse_cfg_file_name, uenv_t *env);
