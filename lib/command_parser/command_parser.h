#pragma once

#include "main.h"
#include <array>
#include <optional>
#include "uart.h"

class StringParser {
public:
  bool tick(char c);

  void reset();
  char get_prefix() const {
    return prefix_.value_or('\0');
  }
  uint16_t get_code() const {
    return code_.value_or(UINT16_MAX);
  }
  /**
   * @brief Get the value of a parameter or default
   * @details Returns true if the parameter is found, even without value
   * @param param Parameter to search
   * @param dest where to copy the value
   * @param def default value
   * @return true if parameter was found
   */
  bool get_parameter(char param, int16_t& dest, int16_t def = 0) const;

  bool is_valid() const {
    return prefix_ && code_;
  }

  bool check_and_clear_overflow() {
    bool retval = overflow_flag_;
    overflow_flag_ = false;
    return retval;
  }

  const char* get_str() const {
    return command_.data();
  }

  static bool is_end_char(char);

private:
  void process_command();


  enum state_t { WAITING_START, READING_COMMAND, COMMAND_OVERFLOW };
  state_t state_{ WAITING_START };

  std::array<char, 64> command_;
  size_t write_index_;

  bool overflow_flag_{ false };
  std::optional<char> prefix_;
  std::optional<uint16_t> code_;
};



class CommandDispatcher {
public:
  /// @name commands
  /// @{
  static void T100();  ///< test command, does nothing
  static void A0();    ///< request time from RTC
  static void A1();    ///< set RTC time
  /// @}

  void input_char(char c);

  void inject_uart_dependency(UART_DMA* uart) {
    uart_ = uart;
  }

private:
  void send_ack(int);
  void send_err(int);

  UART_DMA* uart_{ nullptr };

  using cmd_fcn_ptr = void (*)();
  cmd_fcn_ptr get_fcn_from_cmd() const;
  cmd_fcn_ptr search_T_code() const;
  cmd_fcn_ptr search_A_code() const;
  StringParser parser_;
};
