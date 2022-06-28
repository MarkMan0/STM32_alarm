#pragma once
#include "abstract_screen.h"
#include <variant>
#include <optional>
#include "screens.h"



class Menu {
public:
  void init();
  void tick();
  void sleep() {
    was_sleeping_ = true;
  }

  void goto_screen(AbstractScreen* scr) {
    next_screen_ = scr;
  }


private:
  AbstractScreen* curr_screen_;
  AbstractScreen* next_screen_;
  int32_t last_encoder_{ 0 };
  bool held_handled_ = false;
  bool was_sleeping_ = false;
};
