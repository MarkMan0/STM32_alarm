#include "screens.h"
#include "display_task.h"
#include "globals.h"


void MainScreen::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 2 });
  gfx.printf("Main screen");
  gfx.draw();
}


bool MainScreen::onClickUp() {
  menu.goto_screen(ScreenAllocator::allocate(MainMenuScreen()));
  return true;
}

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
  } else if (increment < 0) {
    --current_item_;
  }
  current_item_ = utils::constrain(current_item_, 0, num_items - 1);
}

bool MainMenuScreen::onClickUp() {
  switch (current_item_) {
    case 0:
      menu.goto_screen(ScreenAllocator::allocate(MainScreen()));
      break;
    case 1:
      menu.goto_screen(ScreenAllocator::allocate(Screen2()));
      break;
    case 2:
      menu.goto_screen(ScreenAllocator::allocate(Screen3()));
      break;
    case 3:
      menu.goto_screen(ScreenAllocator::allocate(Screen4()));
      break;
    default:
      break;
  }
  return true;
}

void Screen2::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 2 });
  gfx.printf("Screen2 screen\n%d", counter_);
  gfx.draw();
}
bool Screen2::onClickUp() {
  menu.goto_screen(ScreenAllocator::allocate(MainScreen()));
  return true;
}
bool Screen2::onClickHeld() {
  ++counter_;
  return true;
}

void Screen3::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 2 });
  gfx.printf("Screen3 screen");
  gfx.draw();
}
bool Screen3::onClickUp() {
  menu.goto_screen(ScreenAllocator::allocate(MainScreen()));
  return true;
}

void Screen4::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 2 });
  gfx.printf("Screen4 screen");
  gfx.draw();
}
bool Screen4::onClickUp() {
  menu.goto_screen(ScreenAllocator::allocate(MainScreen()));
  return true;
}
