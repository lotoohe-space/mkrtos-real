
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

void parse_cfg_init(void);
int parse_cfg(const char *parse_cfg_file_name, uenv_t *env);
