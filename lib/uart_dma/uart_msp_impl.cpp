/**
 * @file uart_msp_impl.cpp
 * @brief UART init and other callbacks, that differ by board/UART number
 */

#include "main.h"
#include "pin_api.h"
#include "uart.h"
#include "utils.h"

/// @details Placed in MSP file, so compiler could inline it for specific cases
void UART_DMA::rx_event_cb(UART_HandleTypeDef* huart, uint16_t pos) {
  assert_param(&huart_ == huart);

  utils::LockISR lck(rx_buff_mtx_);

  for (; last_rxdma_pos_ < pos; ++last_rxdma_pos_) {
    receive_buff_.push(dma_buff_[last_rxdma_pos_]);
  }
  last_rxdma_pos_ = last_rxdma_pos_ % dma_buff_.size();

  assert_param(rx_notify_task_ != nullptr);
  if (rx_notify_task_ != nullptr) {
    xTaskNotifyFromISR(rx_notify_task_, 0, eNoAction, nullptr);
  }
}


//////
////// UART2
//////

void UART_DMA::uart2_enable_isrs(UART_DMA& uart) {
  // TX DMA
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

  // RX DMA
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

  HAL_NVIC_SetPriority(USART2_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

static void uart2_msp_init(UART_HandleTypeDef* uart) {
  assert_param(uart->Instance == USART2);
  if (uart->Instance != USART2) {
    return;
  }


  __HAL_RCC_USART2_CLK_ENABLE();

  pin_mode(pins::rx, pin_mode_t::ALTERNATE_PP, GPIO_AF7_USART2);
  pin_mode(pins::tx, pin_mode_t::ALTERNATE_PP, GPIO_AF7_USART2);

  __HAL_RCC_DMA1_CLK_ENABLE();

  uart2.hdmatx_.Instance = DMA1_Channel7;
  uart2.hdmatx_.Init.Direction = DMA_MEMORY_TO_PERIPH;
  uart2.hdmatx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  uart2.hdmatx_.Init.MemInc = DMA_MINC_ENABLE;
  uart2.hdmatx_.Init.Mode = DMA_NORMAL;
  uart2.hdmatx_.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
  uart2.hdmatx_.Init.PeriphInc = DMA_PINC_DISABLE;
  uart2.hdmatx_.Init.Priority = DMA_PRIORITY_LOW;

  if (HAL_DMA_Init(&uart2.hdmatx_) != HAL_OK) {
    // uart2.transmit("HAL DMA Init failed for tx");
  }

  uart2.hdmarx_.Instance = DMA1_Channel6;
  uart2.hdmarx_.Init.Direction = DMA_PERIPH_TO_MEMORY;
  uart2.hdmarx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  uart2.hdmarx_.Init.MemInc = DMA_MINC_ENABLE;
  uart2.hdmarx_.Init.Mode = DMA_CIRCULAR;
  uart2.hdmarx_.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
  uart2.hdmarx_.Init.PeriphInc = DMA_PINC_DISABLE;
  uart2.hdmarx_.Init.Priority = DMA_PRIORITY_LOW;

  if (HAL_DMA_Init(&uart2.hdmarx_) != HAL_OK) {
    // uart2.transmit("HAL DMA Init failed for rx");
  }

  __HAL_LINKDMA(uart, hdmatx, uart2.hdmatx_);
  __HAL_LINKDMA(uart, hdmarx, uart2.hdmarx_);
}

void UART_DMA::uart2_hw_init(UART_DMA& uart) {
  uart.huart_.Instance = USART2;
  uart.huart_.Init.BaudRate = uart.baudrate_;
  uart.huart_.Init.WordLength = UART_WORDLENGTH_8B;
  uart.huart_.Init.StopBits = UART_STOPBITS_1;
  uart.huart_.Init.Parity = UART_PARITY_NONE;
  uart.huart_.Init.Mode = UART_MODE_TX_RX;
  uart.huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart.huart_.Init.OverSampling = UART_OVERSAMPLING_16;
  uart.huart_.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;

  uart.huart_.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;


  HAL_UART_RegisterCallback(&uart.huart_, HAL_UART_MSPINIT_CB_ID, uart2_msp_init);
  if (HAL_UART_Init(&uart.huart_) != HAL_OK) {
    while (1) {
    }
  }

  HAL_UART_RegisterRxEventCallback(&uart.huart_, UART_DMA::uart2_rx_event_cb);
}

void UART_DMA::uart2_rx_event_cb(UART_HandleTypeDef* huart, uint16_t pos) {
  assert_param(huart->Instance == USART2);
  uart2.rx_event_cb(huart, pos);
}

//////
////// UART1
//////


void UART_DMA::uart1_enable_isrs(UART_DMA& uart) {
  // TX DMA
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

  // RX DMA
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

  HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

static void uart1_msp_init(UART_HandleTypeDef* uart) {
  assert_param(uart->Instance == USART1);
  if (uart->Instance != USART1) {
    return;
  }


  __HAL_RCC_USART1_CLK_ENABLE();

  pin_mode(pins::rx1, pin_mode_t::ALTERNATE_PP, GPIO_AF7_USART1);
  pin_mode(pins::tx1, pin_mode_t::ALTERNATE_PP, GPIO_AF7_USART1);

  __HAL_RCC_DMA1_CLK_ENABLE();

  uart1.hdmatx_.Instance = DMA1_Channel4;
  uart1.hdmatx_.Init.Direction = DMA_MEMORY_TO_PERIPH;
  uart1.hdmatx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  uart1.hdmatx_.Init.MemInc = DMA_MINC_ENABLE;
  uart1.hdmatx_.Init.Mode = DMA_NORMAL;
  uart1.hdmatx_.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
  uart1.hdmatx_.Init.PeriphInc = DMA_PINC_DISABLE;
  uart1.hdmatx_.Init.Priority = DMA_PRIORITY_LOW;

  if (HAL_DMA_Init(&uart1.hdmatx_) != HAL_OK) {
    // uart2.transmit("HAL DMA Init failed for tx");
  }

  uart1.hdmarx_.Instance = DMA1_Channel5;
  uart1.hdmarx_.Init.Direction = DMA_PERIPH_TO_MEMORY;
  uart1.hdmarx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  uart1.hdmarx_.Init.MemInc = DMA_MINC_ENABLE;
  uart1.hdmarx_.Init.Mode = DMA_CIRCULAR;
  uart1.hdmarx_.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
  uart1.hdmarx_.Init.PeriphInc = DMA_PINC_DISABLE;
  uart1.hdmarx_.Init.Priority = DMA_PRIORITY_LOW;

  if (HAL_DMA_Init(&uart1.hdmarx_) != HAL_OK) {
    // uart2.transmit("HAL DMA Init failed for rx");
  }

  __HAL_LINKDMA(uart, hdmatx, uart1.hdmatx_);
  __HAL_LINKDMA(uart, hdmarx, uart1.hdmarx_);
}

void UART_DMA::uart1_hw_init(UART_DMA& uart) {
  uart.huart_.Instance = USART1;
  uart.huart_.Init.BaudRate = uart.baudrate_;
  uart.huart_.Init.WordLength = UART_WORDLENGTH_8B;
  uart.huart_.Init.StopBits = UART_STOPBITS_1;
  uart.huart_.Init.Parity = UART_PARITY_NONE;
  uart.huart_.Init.Mode = UART_MODE_TX_RX;
  uart.huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart.huart_.Init.OverSampling = UART_OVERSAMPLING_16;
  uart.huart_.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;

  uart.huart_.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;


  HAL_UART_RegisterCallback(&uart.huart_, HAL_UART_MSPINIT_CB_ID, uart1_msp_init);
  if (HAL_UART_Init(&uart.huart_) != HAL_OK) {
    while (1) {
    }
  }

  HAL_UART_RegisterRxEventCallback(&uart.huart_, UART_DMA::uart1_rx_event_cb);
}

void UART_DMA::uart1_rx_event_cb(UART_HandleTypeDef* huart, uint16_t pos) {
  assert_param(huart->Instance == USART1);
  uart1.rx_event_cb(huart, pos);
}
