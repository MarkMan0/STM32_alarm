#include "menu.h"
#include "globals.h"


void Menu::init() {
  curr_screen_ = ScreenAllocator::allocate(MainScreen());
  curr_screen_->onEntry();
}

void Menu::tick() {
  if (next_screen_) {
    curr_screen_->onExit();
    curr_screen_ = next_screen_;
    next_screen_ = nullptr;
    curr_screen_->onEntry();
    return;
  }

  int32_t enc = encoder.enc.get();
  if (enc != last_encoder_) {
    curr_screen_->onEncoder(enc - last_encoder_);
    last_encoder_ = enc;
  }

  using e = btn_event_t;
  switch (encoder.btn.get_state()) {
    case e::NONE:
      break;
    case e::PRESSED:
      curr_screen_->onClickDown();
      break;
    case e::RELEASED:
      curr_screen_->onClickUp();
      break;
    case e::HELD:
      curr_screen_->onClickHeld();
      break;
  }


  curr_screen_->draw();
}
