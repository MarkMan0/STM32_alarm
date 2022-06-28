/**
 * @file main.cpp
 * @brief Program entry point
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



UART_DMA uart2(UART_DMA::uart2_hw_init, UART_DMA::uart2_enable_isrs);
RTOS_I2C i2c;
DS3231 rtc(i2c);
CommandDispatcher cmd(&uart2);
RotaryEncoder encoder;


int main(void) {
  HAL_Init();
  SystemClock_Config();
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /// initialize global hardware objects
  uart2.hw_init(115200);
  i2c.init_i2c1();

  pin_mode(pins::enc_A, pin_mode_t::IT_RISE_FALL_PU);
  pin_mode(pins::enc_B, pin_mode_t::INPUT_PU);
  pin_mode(pins::enc_SW, pin_mode_t::IT_RISE_FALL_PU);

  encoder.enc.init(read_pin(pins::enc_A));


  HAL_NVIC_SetPriority(EXTI1_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  /// Initialize RTOS objects and tasks

  rtos_obj::gpio_queue = xQueueCreate(10, sizeof(GPIOStateContainer));
  rtos_obj::btn_event_queue = xQueueCreate(3, sizeof(btn_event_t));

  uart2.begin(&rtos_obj::uart2_tx_handle);
  xTaskCreate(rtos_tasks::gpio_task, "GPIO task", 128, nullptr, 20, &rtos_obj::gpio_handle);
  xTaskCreate(rtos_tasks::command_task, "Command task", 128, nullptr, 20, &rtos_obj::command_handle);
  xTaskCreate(rtos_tasks::display_task, "display task", 150, nullptr, 20, &rtos_obj::display_handle);
  xTaskCreate(rtos_tasks::monitor_task, "monitor task", 110, nullptr, 20, &rtos_obj::monitor_handle);

  uart2.register_task_to_notify_on_rx(rtos_obj::command_handle);


  /// Start the scheduler
  vTaskStartScheduler();
  while (1) {
  }
}
