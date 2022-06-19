#pragma once

#include <cstdint>
#include "rtos_i2c.h"
#include "GFX.h"


class SSD1306 {
public:
  SSD1306(RTOS_I2C& i2c) : i2c_{ i2c } {
  }
  bool begin();

  bool draw_canvas(GFX::canvas_t&);

  void set_ram_val(uint8_t val);

private:
  bool reset_ram_address();
  RTOS_I2C& i2c_;
  inline static constexpr uint8_t addr_{ 0x3C << 1 }; /*!< I2C address already shifted */
};
