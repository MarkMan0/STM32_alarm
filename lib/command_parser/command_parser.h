/**
 * @file command_parser.h
 * @brief Command parser class and utilities
 *
 */

#pragma once

#include "main.h"
#include <array>
#include <optional>
#include "uart.h"

/**
 * @brief Parses a stream of characters into commands
 * @details Can be used continuously. Upon detection of end_char (\n, \r, \0), parses the preceding characters into
 * prefix and command number. The command parameters can be queried, while processing the command. The command is stored
 * in an internal buffer.
 * An example command is: B1 C18 K0, where:
 * + prefix is B
 * + command code is 1
 * + parameter C is 18
 * + parameter K is 0
 *
 */
class StringParser {
public:
  /// @brief Puts the character into the buffer. If c is end_char, processes the command
  /// @return true if c was end_char, and command is processed
  bool tick(char c);

  /// @brief Resets the state of the class
  void reset();

  /// @brief Returns the prefix of the command, or '\0'
  char get_prefix() const {
    return prefix_.value_or('\0');
  }

  /// @brief Returns the code of the command, or UIN16_MAX
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

  /// @brief Command is valid, if it has a code and prefix
  bool is_valid() const {
    return prefix_ && code_;
  }

  /// @brief pointer to the command string
  const char* get_str() const {
    return command_.data();
  }

  /// @returns true if @p c marks the end of a command
  static bool is_end_char(char c);

private:
  /// @brief Parses the command into prefix and code
  void process_command();


  enum state_t { WAITING_START, READING_COMMAND, COMMAND_OVERFLOW };
  state_t state_{ WAITING_START };  ///< The current state of the command reading

  std::array<char, 64> command_;  ///< Command buffer
  size_t write_index_;            ///< Write index in command buffer

  std::optional<char> prefix_;    ///< Prefix of the command, if any
  std::optional<uint16_t> code_;  ///< Code of the command, if any
};



/**
 * @brief Handles input from UART, and calls the correct function
 *
 */
class CommandDispatcher {
public:
  /// @name commands
  /// @brief Commands are implemented as methods.
  /// @{
  void T100();  ///< test command, does nothing
  void A0();    ///< request time from RTC
  void A1();    ///< set RTC time
  void A2();    ///< set alarm
  void A3();    ///< get alarm
  /// @}

  CommandDispatcher(UART_DMA* uart) : uart_(uart) {
  }

  /// @brief Process one character
  /// @details The actual command function is called from inside this function
  void input_char(char c);

private:
  void send_ack(int);  ///< Called, if the command is valid
  void send_err(int);  ///< Called on invalid command

  UART_DMA* const uart_{ nullptr };  ///< UART dependency

  using cmd_fcn_ptr = void (CommandDispatcher::*)();  ///< Pointer type to own method
  cmd_fcn_ptr get_fcn_from_cmd() const;               ///< Returns the function for the current command, or nullptr
  cmd_fcn_ptr search_T_code() const;                  ///< Searches T commands only
  cmd_fcn_ptr search_A_code() const;                  ///< Searches A commands only
  StringParser parser_;                               ///< The string parser
};
