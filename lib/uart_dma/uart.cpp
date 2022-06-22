/**
 * @file uart.cpp
 * @brief Common functions for UART_DMA
 */

#include "uart.h"
#include "pin_api.h"
#include <algorithm>
#include "utils.h"


void UART_DMA::generic_tx_task(void* ptr) {
  UART_DMA& uart = *reinterpret_cast<UART_DMA*>(ptr);
  while (1) {
    uart.tick();
  }
}


UART_DMA::UART_DMA(hw_init_fcn_t* a, isr_enable_fcn_t* b) : hw_init_cb(a), isr_enable_cb(b) {
}

void UART_DMA::begin(uint32_t baud) {
  baudrate_ = baud;
  hw_init_cb(*this);
  isr_enable_cb(*this);

  tx_buff_mtx_ = xSemaphoreCreateBinary();
  xSemaphoreGive(tx_buff_mtx_);
  xTaskCreate(generic_tx_task, "tx task", 128, this, osPriorityAboveNormal1, &tx_task_);


  HAL_UART_Receive_DMA(&huart_, dma_buff_.buff_.data(), dma_buff_.buff_.size());
  huart_.ReceptionType = HAL_UART_RECEPTION_TOIDLE;  // also reacts to IDLE interrupt, so only one callback is needed

  // enable IDLE interrupt
  SET_BIT(huart_.Instance->CR1, USART_CR1_IDLEIE);
}

void UART_DMA::send(const void* buff, size_t sz) {
  size_t sent = 0;
  while (sent < sz) {
    sent += transmit_buff_.push(reinterpret_cast<const uint8_t*>(buff) + sent, sz - sent);
  }
  xTaskNotify(tx_task_, 0, eNoAction);
}

void UART_DMA::flush() {
  while (uint16_t n = transmit_buff_.get_num_occupied_continuous()) {
    for (int i = 0; i < 20; ++i) {
      if (HAL_OK == HAL_UART_Transmit_DMA(&huart_, const_cast<uint8_t*>(&transmit_buff_.peek()), n)) {
        transmit_buff_.pop(n);
        break;
      }
      osDelay(pdMS_TO_TICKS(10));
    }
  }
}

void UART_DMA::tick() {
  xTaskNotifyWait(0, UINT32_MAX, nullptr, portMAX_DELAY);
  utils::Lock lck(tx_buff_mtx_);
  flush();
}


uint16_t UART_DMA::vprintf(const char* fmt, va_list args) {
  auto msglen = vsnprintf(nullptr, 0, fmt, args);

  utils::Lock lck(tx_buff_mtx_);

  auto ptr = transmit_buff_.reserve(msglen + 1);
  if (!ptr) {
    flush();
    transmit_buff_.reset();
    ptr = transmit_buff_.reserve(msglen + 1);
  }

  if (!ptr) {
    return 0;
  }

  return vsnprintf(reinterpret_cast<char*>(ptr), msglen + 1, fmt, args);
}

void UART_DMA::reset_buffers() {
  {
    utils::Lock lck(tx_buff_mtx_);
    transmit_buff_.reset();
  }
  {
    while (dma_buff_.get_num_occupied()) {
      UNUSED(dma_buff_.pop());
    }
  }
}

uint8_t UART_DMA::get_one() {
  return dma_buff_.pop();
}

uint16_t UART_DMA::get_n(uint8_t* dst, uint16_t n) {
  n = std::min(n, dma_buff_.get_num_occupied());

  for (int i = 0; i < n; ++i) {
    dst[i] = dma_buff_.pop();
  }
  return n;
}
