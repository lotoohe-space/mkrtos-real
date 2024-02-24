/**
 * @file x_tool.c
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "x_types.h"
#include "gui.h"
#include <string.h>
#include <assert.h>
/**
 * @brief 获取gbk字符串的第index个字符的地址
 *
 * @param str
 * @param index
 * @return const char*
 */
const char *GBK_CharGet(const char *str, uint16_t index)
{
	uint16_t i = 0;
	uint16_t len = 0;
	if (!str)
	{
		return NULL;
	}
	while (str[i])
	{
		if (len == index)
		{
			return &str[i];
		}
		len++;
		if (str[i] & 0x80)
		{
			i += 2;
		}
		else
		{
			i++;
		}
	}
	return NULL;
}
/**
 * @brief 获取UNICODE的长度
 *
 * @param str
 * @return uint32_t
 */
uint32_t UNI_Strlen(uint16_t *str)
{
	uint16_t i = 0;
	if (str == NULL)
	{
		return 0;
	}
	while (str[i])
	{
		i++;
	}
	return i;
}
/**
 * @brief 获取GBK字符串长度
 *
 * @param str
 * @return uint32_t
 */
uint32_t GBK_Strlen(const char *str)
{
	uint16_t i = 0;
	uint16_t len = 0;
	if (!str)
	{
		return 0;
	}
	while (str[i])
	{
		if (str[i] & 0x80)
		{
			i += 2;
		}
		else
		{
			i++;
		}
		len++;
	}

	return len;
}
