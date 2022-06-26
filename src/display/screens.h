#pragma once

#include "abstract_screen.h"
#include <variant>


class MainScreen : public AbstractScreen {
  // void onEntry() override;
  void draw() override;
  void onClickUp() override;
  // void onEncoder(int32_t) override;
};

class Screen2 : public AbstractScreen {
  void draw() override;
  void onClickUp() override;
};




class ScreenAllocator {
public:
  using screen_collection_t = std::variant<MainScreen, Screen2>;

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
