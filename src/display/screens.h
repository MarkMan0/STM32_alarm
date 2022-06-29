/**
 * @file screens.h
 * @brief Declaration of Screens
 *
 */

#pragma once

#include "abstract_screen.h"
#include <variant>


/// The default screen
class MainScreen : public AbstractScreen {
public:
  void draw() override;
  bool onClickUp() override;
};

/// Main menu
class MainMenuScreen : public AbstractScreen {
public:
  void draw() override;
  void onEncoder(int32_t) override;
  bool onClickUp() override;

private:
  static constexpr const char* menu_items[] = { "Back", "Screen 2", "Screen 3", "Screen 4" };
  static constexpr unsigned num_items = sizeof(menu_items) / sizeof(menu_items[0]);

  unsigned current_item_ = 0;
};

/// Example screen
class Screen2 : public AbstractScreen {
public:
  void draw() override;
  bool onClickUp() override;
  bool onClickHeld() override;

private:
  unsigned counter_ = 0;
};

/// Example screen
class Screen3 : public AbstractScreen {
public:
  void draw() override;
  bool onClickUp() override;
};

/// Example screen
class Screen4 : public AbstractScreen {
public:
  void draw() override;
  bool onClickUp() override;
};




class ScreenAllocator {
public:
  using screen_collection_t = std::variant<MainScreen, MainMenuScreen, Screen2, Screen3, Screen4>;
  int i = sizeof(MainScreen);
  template <class T>
  static AbstractScreen* allocate(const T& t) {
    AbstractScreen* ptr;
    auto& coll = flag_ ? coll_1_ : coll_2_;
    coll = t;
    std::visit([&](auto& arg) { ptr = &arg; }, coll);
    return ptr;
  }

private:
  static bool flag_;
  static screen_collection_t coll_1_, coll_2_;
};
