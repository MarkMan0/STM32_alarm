#include "command_parser.h"
#include <array>
#include <cstring>
#include <algorithm>


bool StringParser::is_end_char(char c) const {
  return c == '\n' || c == '\r' || c == '\0';
}

bool StringParser::tick(char c) {
  bool command_ready = false;
  switch (state_) {
    case WAITING_START:
      if (not std::isblank(c)) {
        reset();
        write_index_ = 0;
        command_[write_index_++] = c;
        state_ = READING_COMMAND;
      }
      break;

    case READING_COMMAND:
      if (write_index_ == command_.size()) {
        state_ = OVERFLOW;
      } else if (is_end_char(c)) {
        state_ = WAITING_START;
        command_ready = true;
      } else {
        command_[write_index_++] = c;
      }
      break;

    case OVERFLOW:
      if (is_end_char(c)) {
        state_ = WAITING_START;
      }
      break;
  }

  if (command_ready) {
    process_command();
    return true;
  } else {
    return false;
  }
}

void StringParser::process_command() {
  const size_t cmd_len = strlen(command_.data());

  if (cmd_len < 2) {
    return;
  }

  if (std::isalpha(command_[0])) {
    prefix_ = std::toupper(command_[0]);
  }
  if (std::isdigit(command_[1])) {
    code_ = atoi(command_.data() + 1);
  }
}

void StringParser::reset() {
  prefix_ = std::nullopt;
  code_ = std::nullopt;
  write_index_ = 0;
  std::fill(command_.begin(), command_.end(), 0);
}

bool StringParser::get_parameter(char param, int16_t& dest, int16_t def) const {
  dest = def;
  if (!is_valid()) return false;
  const auto end = command_.data() + strlen(command_.data());     // create crude end iterator
  const auto place = std::find(command_.data() + 1, end, param);  // skip first char / prefix when searching
  if (place == end) return false;

  if (*(place + 1) == '\0' || std::isblank(*(place + 1))) {
    // parameter has no value
    return true;
  }

  dest = atoi(place + 1);  // convert to int
  return true;
}
