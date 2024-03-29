/**
 * @file main.cpp
 * @brief Program entry point and global objects declaration
 *
 */



#include "main.h"
#include "pin_api.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "GFX.h"
#include "rtos_i2c.h"
#include "command_parser.h"
#include "globals.h"
#include "encoder.h"
#include "queue.h"
#include "stm_config.h"
#include "tasks.h"

/// @name Global objects declaration
/// @{

UART_DMA uart2(UART_DMA::uart2_hw_init, UART_DMA::uart2_enable_isrs);
RTOS_I2C i2c;
DS3231 rtc(i2c);
RotaryEncoder encoder;

/// @}

int main(void) {
  HAL_Init();
  SystemClock_Config();
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /// initialize global hardware objects
  uart2.hw_init(115200);
  RTOS_I2C::init_i2c1(&i2c);

  TIM2_Init_1kHz();

  pin_mode(pins::enc_A, pin_mode_t::IT_RISE_FALL_PU);
  pin_mode(pins::enc_B, pin_mode_t::INPUT_PU);
  pin_mode(pins::enc_SW, pin_mode_t::IT_RISE_FALL_PU);
  pin_mode(pins::alarm_it, pin_mode_t::IT_FALLING_PU);
  pin_mode(pins::led, pin_mode_t::ALTERNATE_PP, GPIO_AF1_TIM2);


  encoder.enc.init(read_pin(pins::enc_A));


  HAL_NVIC_SetPriority(EXTI1_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /// Initialize RTOS objects and tasks

  rtos_obj::gpio_queue = xQueueCreate(10, sizeof(GPIOStateContainer));

  uart2.begin(&rtos_obj::uart2_tx_handle);
  xTaskCreate(rtos_tasks::gpio_task, "GPIO task", 128, nullptr, 20, &rtos_obj::gpio_handle);
  xTaskCreate(rtos_tasks::command_task, "Command task", 128, nullptr, 20, &rtos_obj::command_handle);
  xTaskCreate(rtos_tasks::ui_task, "UI task", 150, nullptr, 20, &rtos_obj::display_handle);
#ifdef MONITOR_TASK
  xTaskCreate(rtos_tasks::monitor_task, "monitor task", 110, nullptr, 20, &rtos_obj::monitor_handle);
#endif

  uart2.register_task_to_notify_on_rx(rtos_obj::command_handle);


  /// Start the scheduler
  vTaskStartScheduler();
  while (1) {
  }
}
