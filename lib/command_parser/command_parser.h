#pragma once

#include "main.h"
#include <array>
#include <optional>

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

private:
  void process_command();

  bool is_end_char(char) const;

  enum { WAITING_START, READING_COMMAND, OVERFLOW } state_{ WAITING_START };

  std::array<char, 64> command_;
  size_t write_index_;

  bool overflow_flag_{ false };
  std::optional<char> prefix_;
  std::optional<uint16_t> code_;
};
