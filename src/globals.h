#pragma once
#include "uart.h"
#include "rtos_i2c.h"
#include "DS3231.h"
#include "SSD1306.h"
#include "GFX.h"
#include "command_parser.h"

extern UART_DMA uart2;
extern RTOS_I2C i2c;
extern DS3231 rtc;
extern SSD1306 display;
extern GFX gfx;
extern CommandDispatcher cmd;
