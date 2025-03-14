/**
 * @file GUI_interface_extern.h
 * @author ATShining (1358745329@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once
#include "x_types.h"

uint32_t GUIGetTick(void);
void GUIDrawPixel(int16_t x, int16_t y, uintColor color);
uintColor GUIGetPixel(int16_t x, int16_t y);
void GUIDrawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uintColor color);
void GUIDrawBitmap(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t *bitmap);
