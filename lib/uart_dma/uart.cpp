/**
 * @file uart.cpp
 * @brief Common functions for UART_DMA
 */

#include "uart.h"
#include "pin_api.h"
#include <algorithm>
#include "utils.h"
#include "nanoprintf.h"

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
  xTaskCreate(generic_tx_task, "tx task", 60, this, 20, &tx_task_);


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
  int fail_cnt = 0;
  // 1. check if we have data in buffer, and get a continuous segment of it
  while (uint16_t n = transmit_buff_.get_num_occupied_continuous()) {
    // 2. set transmit complete flag to false, and try to transmit
    tx_complete_flag_ = false;
    if (HAL_OK == HAL_UART_Transmit_DMA(&huart_, const_cast<uint8_t*>(&transmit_buff_.peek()), n)) {
      // 3a. If started transmission, wait for it to complete. Flag will be set in callback
      while (not tx_complete_flag_) {
        vTaskDelay(pdMS_TO_TICKS(5));
      }
      // 4a. Finally, pop the transmitted data from the buffer
      transmit_buff_.pop(n);
      break;
    } else {
      // 3b. If couldn't start transmission, wait or return after N tries
      if (++fail_cnt < 10) {
        vTaskDelay(pdMS_TO_TICKS(10));
      } else {
        return;
      }
    }
  }
}

void UART_DMA::tick() {
  xTaskNotifyWait(0, UINT32_MAX, nullptr, portMAX_DELAY);
  utils::Lock lck(tx_buff_mtx_);
  flush();
}


uint16_t UART_DMA::vprintf(const char* fmt, va_list args) {
  auto msglen = npf_vsnprintf(nullptr, 0, fmt, args);

  utils::Lock lck(tx_buff_mtx_);

  uint8_t* ptr = transmit_buff_.reserve(msglen + 1);
  if (!ptr) {
    flush();
    transmit_buff_.reset();
    ptr = transmit_buff_.reserve(msglen + 1);
  }

  if (!ptr) {
    return 0;
  }

  int written = npf_vsnprintf(reinterpret_cast<char*>(ptr), msglen + 1, fmt, args);
  transmit_buff_.head_ = transmit_buff_.decrement_idx(transmit_buff_.head_);  // final \0 is ignored/not sent

  return written;
}


uint16_t UART_DMA::vprintf_ISR(const char* fmt, va_list args) {
  auto msglen = npf_vsnprintf(nullptr, 0, fmt, args);

  utils::LockISR lck(tx_buff_mtx_);

  uint8_t* ptr = transmit_buff_.reserve(msglen + 1);

  if (!ptr) {
    return 0;
  }

  int written = npf_vsnprintf(reinterpret_cast<char*>(ptr), msglen + 1, fmt, args);
  transmit_buff_.head_ = transmit_buff_.decrement_idx(transmit_buff_.head_);  // final \0 is ignored/not sent

  return written;
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
