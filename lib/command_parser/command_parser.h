#pragma once

#include "main.h"

class StringParser {
public:
  void set_string(const char* arr);

  void reset();
  char get_prefix() const;
  uint16_t get_number() const;
  /**
   * @brief Get the value of a parameter or default
   * @details Returns true if the parameter is found, even without value
   * @param param Parameter to search
   * @param dest where to copy the value
   * @param def default value
   * @return true if parameter was found
   */
  bool get_parameter(char param, int16_t& dest, int16_t def = 0) const;

  bool has_string() const;

private:
  /**
   * @brief Index of first non-whitespace char
   *
   * @return size_t
   */
  size_t get_first_not_whitespace() const;
  const char* str_{ nullptr };
  uint16_t str_sz_{ 0 };
};
