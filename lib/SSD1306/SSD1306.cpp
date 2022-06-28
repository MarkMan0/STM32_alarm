
#include "SSD1306.h"
#include "SSD1306/commands.h"

#include "rtos_i2c.h"


bool SSD1306::begin() {
  // set mux
  namespace reg = SSD_1306_reg;

  static constexpr uint8_t config[] = { // turn off display
                                        reg::SET_DISPLAY_OFF,
                                        // Display height - 1
                                        reg::SET_MUX_RATIO, 63,
                                        // display vertical shift
                                        reg::SET_DISPLAY_OFFSET, 0,
                                        // display start line is 0
                                        reg::SET_DISPLAY_START_LINE,
                                        // segment remap 1 - flip in X
                                        reg::SET_SEGMENT_REMAP | 0x1,
                                        // normal COM scan
                                        reg::SET_COM_OUTPUT_SCAN_DIR,
                                        // COM pins hardware layout
                                        reg::SET_COM_HW_CONFIG, 0x2 | (0x1 << 4) | (0x0 << 5),
                                        // set display contrast/brightness
                                        reg::SET_CONTRAST_CONTROL, 100,
                                        // use ram to display
                                        reg::ENTIRE_DISPLAY_FROM_RAM,
                                        // 1 in RAM means OLED on
                                        reg::SET_NORMAL_DISPLAY,
                                        // set oscillator from datasheet
                                        reg::SET_CLOCK_DIVIDE_RATIO, (0b1000 << 4),
                                        // enable charge pump
                                        reg::CHARGE_PUMP_SETTINGS, 0x10 | (0x1 << 2),
                                        // set addressing mode
                                        SSD_1306_reg::SET_MEMORY_ADDRESSING_MODE, 0x01,
                                        // turn on display
                                        reg::SET_DISPLAY_ON
  };

  return i2c_.write_register(addr_, 0, const_cast<uint8_t*>(config), sizeof(config));
}

bool SSD1306::draw_canvas(GFX::canvas_t& canvas) {
  if (!reset_ram_address()) return false;
  // data is stored in a 2D std::array, which is contiguous, so we can transfer in one go
  return i2c_.write_register(addr_, 0x40, reinterpret_cast<uint8_t*>(canvas.data()), 128 * 8);
}

void SSD1306::set_ram_val(uint8_t val) {
  if (!reset_ram_address()) return;
  uint8_t buff[2] = { 0x40, (val >= 0) ? 0xFF : 0 };
  auto lck(i2c_.get_lock());


  for (int curr_col = 0; curr_col < 128; ++curr_col) {
    for (int curr_page = 0; curr_page < 8; ++curr_page) {
      if (HAL_OK != HAL_I2C_Master_Transmit(&i2c_.hi2c_, addr_, buff, 2, 100)) {
        return;
      }
    }
  }
}


bool SSD1306::reset_ram_address() {
  static constexpr uint8_t buff[]{ SSD_1306_reg::SET_PAGE_ADDRESS, 0, 0x7, SSD_1306_reg::SET_COLUMN_ADDRESS, 0, 127 };
  return i2c_.write_register(addr_, 0x00, const_cast<uint8_t*>(buff), sizeof(buff));
}


bool SSD1306::sleep() {
  uint8_t conf = SSD_1306_reg::SET_DISPLAY_OFF;
  return i2c_.write_register(addr_, 0, &conf, sizeof(1));
}
