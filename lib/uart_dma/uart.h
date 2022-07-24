#pragma once

#include "main.h"
#include "ring_buffer.h"
#include <array>
#include <cstring>
#include <cstdarg>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "utils.h"




/**
 * @brief Uart wrapper with DMA for rx and tx
 * @details It can be used for multiple uarts if set up properly in the constructor
 * User would need to implement the 2 functions for the constructor
 * Implementation is provided for UART1 and UART2
 *
 */
class UART_DMA {
public:
  using hw_init_fcn_t = void(UART_DMA&);     ///< Function called in begin(), inits the hardware
  using isr_enable_fcn_t = void(UART_DMA&);  ///< Function called in begin(), enables the interrupts

  UART_DMA(hw_init_fcn_t*, isr_enable_fcn_t*);  ///< Provide the necessary callbacks

  void hw_init(uint32_t baud);  ///< Call once only, to init the hardware and start tasks
  void begin(TaskHandle_t* tx_task);

  /// @brief Return the number of bytes available for reading
  [[nodiscard]] uint16_t available() const {
    return dma_buff_.get_num_occupied();
  }

  /// @name getters from buffer
  /// @{

  /// @brief Return and pop one byte from the RX buffer
  [[nodiscard]] uint8_t get_one();

  /// @brief Reads at max @p n bytes into @p dest. @return The number of bytes read
  [[nodiscard]] uint16_t get_n(uint8_t* dest, uint16_t n);

  /// @}

  /** @name Immediate transmission
   *  @details Messages are sent out immediately
   */
  ///@{
  void transmit(uint8_t c) {
    HAL_UART_Transmit(&huart_, &c, 1, HAL_MAX_DELAY);
  }
  void transmit(const void* data, size_t n) {
    HAL_UART_Transmit(&huart_, reinterpret_cast<uint8_t*>(const_cast<void*>(data)), n, HAL_MAX_DELAY);
  }
  void transmit(const char* str) {
    transmit(str, strlen(str));
  }
  ///@}

  /** @name transmit messages using buffer in tick()
   *  @details Messages are placed into transmission_buffer and sent out later, asynchronously
   */
  ///@{
  void send(const void*, size_t);
  void send(const char* str) {
    send(str, strlen(str));
  }

  /// @brief Fully functional printf-style messages
  /// @details Messages are placed directly into the TX buffer
  /// If not enough place in buffer, transmits nothing
  uint16_t printf(const char* fmt, ...) {
    std::va_list args;
    va_start(args, fmt);
    const auto res = this->vprintf(fmt, args);
    va_end(args);
    if (res) {
      xTaskNotify(tx_task_, 0, eNoAction);
    }
    return res;
  }

  uint16_t printf_ISR(const char* fmt, ...) {
    std::va_list args;
    va_start(args, fmt);
    const auto res = this->vprintf_ISR(fmt, args);
    va_end(args);
    if (res) {
      xTaskNotifyFromISR(tx_task_, 0, eNoAction, NULL);
    }
    return res;
  }

  uint16_t println(const char* fmt, ...) {
    std::va_list args;
    va_start(args, fmt);
    const auto res = this->vprintf(fmt, args);
    va_end(args);

    if (res) {
      {
        utils::Lock lck(tx_buff_mtx_);
        transmit_buff_.push('\n');
      }
      xTaskNotify(tx_task_, 0, eNoAction);
    }
    return res;
  }
  /// @brief Flush the transmission buffer
  void flush();
  ///@}

  void tick();  ///< Called periodically to empty the transmit buffer

  void reset_buffers();  ///< Resets both buffers so head=tail=0

  /// @brief This task will be notified if anything is received
  void register_task_to_notify_on_rx(TaskHandle_t t) {
    rx_notify_task_ = t;
  }

  const RingBuffer<uint8_t, 128, true>& get_dma_buff() const {
    return dma_buff_;
  }

  UART_HandleTypeDef huart_;
  DMA_HandleTypeDef hdmarx_, hdmatx_;

private:
  uint16_t vprintf(const char* fmt, va_list args);
  uint16_t vprintf_ISR(const char* fmt, va_list args);

  SemaphoreHandle_t tx_buff_mtx_;  ///< Mutex to lock the receive buffer

  TaskHandle_t tx_task_{ nullptr };  ///< Handle to task that calls tick()

  TaskHandle_t rx_notify_task_{ nullptr };  ///< Task that will be notified on RX

  volatile bool tx_complete_flag_{ true };

  uint32_t baudrate_{ 115200 };
  uint16_t last_rxdma_pos_{ 0 };  ///< Used in rx event callback to track DMA


  RingBuffer<uint8_t, 128, true> dma_buff_;
  RingBuffer<uint8_t, 64> transmit_buff_;  ///< Buffer for non-immediate transmission

  const hw_init_fcn_t* hw_init_cb;
  const isr_enable_fcn_t* isr_enable_cb;


public:
  /** @brief Generic RX event callback */
  void rx_event_cb(UART_HandleTypeDef*, uint16_t);

  /// @brief calls tick on UART_DMA passed as argument
  /// @param ptr pointer to UART_DMA instance
  static void generic_tx_task(void* ptr);

  static void generic_tx_cplt_cb(UART_DMA& uart, UART_HandleTypeDef* huart);

  /** @name UART2 init and callback functions */
  ///@{

  /** @brief Initialization of the UART peripheral and handle */
  static void uart2_hw_init(UART_DMA&);
  /** @brief Enables interrupts for UART and DMAs */
  static void uart2_enable_isrs(UART_DMA&);
  /** @brief Called by HAL on DMA or IDLE interrupt */
  static void uart2_rx_event_cb(UART_HandleTypeDef* huart, uint16_t Pos);
  static void uart2_tx_cplt_cb(UART_HandleTypeDef* huart);

  ///@}


  /** @name UART1 init and callback functions */
  ///@{

  /** @brief Initialization of the UART peripheral and handle */
  static void uart1_hw_init(UART_DMA&);
  /** @brief Enables interrupts for UART and DMAs */
  static void uart1_enable_isrs(UART_DMA&);
  /** @brief Called by HAL on DMA or IDLE interrupt */
  static void uart1_rx_event_cb(UART_HandleTypeDef* huart, uint16_t Pos);
  static void uart1_tx_cplt_cb(UART_HandleTypeDef* huart);

  ///@}
};
