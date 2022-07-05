/**
 * @file main_menu_screen.cpp
 */

#include "screens.h"
#include "display_objects.h"
#include "globals.h"
#include "nanoprintf.h"


void MainMenuScreen::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 0 });
  for (int i = 0; i < num_items; ++i) {
    gfx.move_cursor({ 0, i });
    if (i == current_item_) {
      gfx.printf("*");
    } else {
      gfx.printf(" ");
    }
    gfx.printf(menu_items[i]);
  }
  gfx.draw();
}

void MainMenuScreen::onEncoder(int32_t increment) {
  if (increment > 0) {
    ++current_item_;
  } else if (increment < 0 && current_item_) {  // do not decrement below zero
    --current_item_;
  }
  current_item_ = utils::constrain(current_item_, 0, num_items - 1);
}

bool MainMenuScreen::onClickUp() {
  switch (current_item_) {
    case 0:
      menu.goto_screen<MainScreen>();
      break;
    case 1:
      menu.goto_screen<SetAlarmScreen>(0);
      break;
    case 2:
      menu.goto_screen<SetAlarmScreen>(1);
      break;
    case 3:
      menu.goto_screen<AlarmScreen>(0);
      break;
    default:
      break;
  }
  return true;
}
