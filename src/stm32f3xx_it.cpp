/**
 * @file stm32f3xx_it.cpp
 * @brief Interrupt handlers
 */

#include "main.h"
#include "stm32f3xx_it.h"
#include "uart.h"

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


void DMA1_Channel4_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart1.hdmatx_);
}

void DMA1_Channel5_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart1.hdmarx_);
}

void DMA1_Channel6_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart2.hdmarx_);
}

void DMA1_Channel7_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart2.hdmatx_);
}

void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&uart1.huart_);
}

void USART2_IRQHandler(void) {
  HAL_UART_IRQHandler(&uart2.huart_);
}

void TIM7_DAC2_IRQHandler(void) {
  extern TIM_HandleTypeDef htim7;
  HAL_TIM_IRQHandler(&htim7);
}
