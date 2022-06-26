#pragma once
#include "uart.h"
#include "rtos_i2c.h"
#include "DS3231.h"
#include "SSD1306.h"
#include "GFX.h"
#include "command_parser.h"
#include "encoder.h"
#include "queue.h"

extern UART_DMA uart2;
extern RTOS_I2C i2c;
extern DS3231 rtc;
extern SSD1306 display;
extern GFX gfx;
extern CommandDispatcher cmd;
extern QueueHandle_t gpio_queue;

struct RotaryEncoder {
  Encoder enc;
  BtnTracker<false> btn;
};


// bit 0 is the value, bit 1 if it has value
struct GPIOStateContainer {
  uint8_t pin_A : 2;
  uint8_t pin_B : 2;
  uint8_t pin_SW : 2;
};

extern RotaryEncoder encoder;
