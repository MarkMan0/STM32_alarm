/**
 * @file stm32f3xx_it.cpp
 * @brief Interrupt handlers
 */

#include "main.h"
#include "stm32f3xx_it.h"
#include "globals.h"
#include "tasks.h"

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
  while (1) {
  }
}


/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {
}

void DMA1_Channel6_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart2.hdmarx_);
}

void DMA1_Channel7_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart2.hdmatx_);
}

void USART2_IRQHandler(void) {
  HAL_UART_IRQHandler(&uart2.huart_);
}

void TIM7_DAC2_IRQHandler(void) {
  extern TIM_HandleTypeDef htim7;
  HAL_TIM_IRQHandler(&htim7);
}

/// reads pins A and B for encoder, and sends to handler task
void EXTI4_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
  GPIOStateContainer gpio{ 0 };
  gpio.pin_A = 2 | read_pin(pins::enc_A);
  gpio.pin_B = 2 | read_pin(pins::enc_B);
  xQueueSendFromISR(rtos_obj::gpio_queue, &gpio, NULL);
}

/// Reads encoder button, and sends to handler task
void EXTI1_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  GPIOStateContainer gpio{ 0 };
  gpio.pin_SW = 2 | read_pin(pins::enc_SW);
  xQueueSendFromISR(rtos_obj::gpio_queue, &gpio, NULL);
}

/// Alarm interrupt
void EXTI9_5_IRQHandler(void) {
  GPIOStateContainer gpio{ 0 };
  if (read_pin(pins::alarm_it) == 0) {
    HAL_GPIO_EXTI_IRQHandler(pin_api::pin_name_to_num(pins::alarm_it));
    gpio.pin_alarm = 2 | 0;  // is 0 because of the if()
    xQueueSendFromISR(rtos_obj::gpio_queue, &gpio, NULL);
  }
}
