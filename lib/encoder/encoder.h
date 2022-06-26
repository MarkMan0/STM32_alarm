#pragma once
#include "pin_api.h"

class Encoder {
public:
  void init(bool a_state) {
    prev_a_state_ = a_state;
    counter_ = 0;
  }

  bool update(bool a_now, bool b_now) {
    bool retval = false;
    if (a_now != prev_a_state_) {
      if (a_now != b_now) {
        ++counter_;
      } else {
        --counter_;
      }
      retval = true;
    }
    prev_a_state_ = a_now;
    return retval;
  }

  void reset() {
    counter_ = 0;
  }

  int32_t get() const {
    return counter_;
  }

private:
  int32_t counter_{ 0 };
  bool prev_a_state_;
};
