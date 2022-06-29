/**
 * @file SSD1306.h
 * @brief SSD1306 OLED driver
 *
 */
#pragma once

#include <cstdint>
#include "rtos_i2c.h"
#include "GFX.h"

/// SSD1306 driver
class SSD1306 {
public:
  SSD1306(RTOS_I2C& i2c) : i2c_{ i2c } {
  }

  /// Initialize the display
  bool begin();

  /// Transfer the canvas to the display
  bool draw_canvas(GFX::canvas_t&);

  /// set whole ram to value
  void set_ram_val(uint8_t val);

  /// Turn of the display for low power
  bool sleep();

private:
  /// Reset the ram address in the display
  bool reset_ram_address();
  RTOS_I2C& i2c_;
  inline static constexpr uint8_t addr_{ 0x3C << 1 };  ///< I2C address already shifted
};
