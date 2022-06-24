#include "command_parser.h"
#include <array>
#include <cstring>
#include <algorithm>

bool StringParser::is_end_char(char c) {
  return c == '\n' || c == '\r' || c == '\0';
}

bool StringParser::tick(char c) {
  bool command_ready = false;
  switch (state_) {
    case WAITING_START:
      if (not std::isspace(c)) {
        reset();
        write_index_ = 0;
        command_[write_index_++] = c;
        state_ = READING_COMMAND;
      }
      break;

    case READING_COMMAND:
      if (write_index_ == command_.size()) {
        state_ = COMMAND_OVERFLOW;
      } else if (is_end_char(c)) {
        state_ = WAITING_START;
        command_ready = true;
      } else {
        command_[write_index_++] = c;
      }
      break;

    case COMMAND_OVERFLOW:
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

CommandDispatcher::cmd_fcn_ptr CommandDispatcher::search_T_code() const {
  int code = parser_.get_code();

  switch (code) {
    case 100:
      return &CommandDispatcher::T100;

    default:
      return nullptr;
  }
}

CommandDispatcher::cmd_fcn_ptr CommandDispatcher::search_A_code() const {
  int code = parser_.get_code();

  switch (code) {
    case 0:
      return &CommandDispatcher::A0;
    case 1:
      return &CommandDispatcher::A1;
    case 2:
      return &CommandDispatcher::A2;
    case 3:
      return &CommandDispatcher::A3;

    default:
      return nullptr;
  }
}



CommandDispatcher::cmd_fcn_ptr CommandDispatcher::get_fcn_from_cmd() const {
  if (not parser_.is_valid()) {
    return nullptr;
  }

  char prefix = parser_.get_prefix();

  switch (prefix) {
    case 'A':
      return search_A_code();
    case 'T':
      return search_T_code();

    default:
      return nullptr;
  }
}

void CommandDispatcher::send_ack(int free) {
  uart_->println("ACK %d", free);
}

void CommandDispatcher::send_err(int free) {
  uart_->println("Err %d: Unknown command %s", free, parser_.get_str());
}

void CommandDispatcher::input_char(char c) {
  if (parser_.tick(c)) {
    auto cmd = get_fcn_from_cmd();
    auto free = uart_->get_dma_buff().get_num_free();
    if (cmd) {
      send_ack(free);
      (this->*cmd)();
    } else {
      send_err(free);
    }
  }
}
