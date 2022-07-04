/**
 * @file globals.h
 * @brief Global objects that are used across the whole project
 */
#pragma once
#include "uart.h"
#include "rtos_i2c.h"
#include "DS3231.h"
#include "SSD1306.h"
#include "GFX.h"
#include "command_parser.h"
#include "encoder.h"
#include "queue.h"
#include "display/menu.h"

extern UART_DMA uart2;           ///< Uart for communication with PC
extern RTOS_I2C i2c;             ///< I2C bus with the RTC and OLED
extern DS3231 rtc;               ///< RTC
extern TIM_HandleTypeDef htim7;  ///< HAL tick timer
extern RotaryEncoder encoder;    ///< Rotary encoder


/// @brief Track GPIO states from interrupt.
/// @details If bit 1 of filed is set, bit 0 constains the value that was read in the ISR
struct GPIOStateContainer {
  uint8_t pin_A : 2;
  uint8_t pin_B : 2;
  uint8_t pin_SW : 2;
  uint8_t pin_alarm : 2;
};
