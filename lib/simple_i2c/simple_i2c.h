#pragma once

#include "main.h"
#include "pin_api.h"

class Simple_I2C {
public:
  I2C_HandleTypeDef hi2c_;
  void init_i2c1();

  [[nodiscard]] bool write(uint8_t address, uint8_t* data, size_t len);
  [[nodiscard]] bool read(uint8_t address, uint8_t* data, size_t len);
  [[nodiscard]] bool write_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len);
  [[nodiscard]] bool read_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len);

private:
  static inline constexpr uint32_t timeout_{ 100 };

  static void i2c1_msp_init(I2C_HandleTypeDef*);
};


inline void Simple_I2C::init_i2c1() {
  constexpr I2C_InitTypeDef init{ .Timing = 0x0000020B,
                                  .OwnAddress1 = 0,
                                  .AddressingMode = I2C_ADDRESSINGMODE_7BIT,
                                  .DualAddressMode = I2C_DUALADDRESS_DISABLE,
                                  .OwnAddress2 = 0,
                                  .OwnAddress2Masks = I2C_OA2_NOMASK,
                                  .GeneralCallMode = I2C_GENERALCALL_DISABLE,
                                  .NoStretchMode = I2C_NOSTRETCH_DISABLE };

  hi2c_.Instance = I2C1;
  hi2c_.Init = init;

  HAL_I2C_RegisterCallback(&hi2c_, HAL_I2C_MSPINIT_CB_ID, Simple_I2C::i2c1_msp_init);
  HAL_I2C_Init(&hi2c_);
  HAL_I2CEx_ConfigAnalogFilter(&hi2c_, I2C_ANALOGFILTER_ENABLE);
  HAL_I2CEx_ConfigDigitalFilter(&hi2c_, 0x00);
}

inline void Simple_I2C::i2c1_msp_init(I2C_HandleTypeDef* hi2c) {
  assert_param(hi2c->Instance == I2C1);
  if (hi2c->Instance == I2C1) {
    __HAL_RCC_I2C1_CLK_ENABLE();
    pin_mode(pins::sda1, pin_mode_t::ALTERNATE_OD_PU, GPIO_AF4_I2C1);
    pin_mode(pins::scl1, pin_mode_t::ALTERNATE_OD_PU, GPIO_AF4_I2C1);
  }
}

inline bool Simple_I2C::write(uint8_t address, uint8_t* data, size_t len) {
  return HAL_I2C_Master_Transmit(&hi2c_, address, data, len, timeout_) == HAL_OK;
}

inline bool Simple_I2C::read(uint8_t address, uint8_t* data, size_t len) {
  return HAL_I2C_Master_Receive(&hi2c_, address, data, len, timeout_) == HAL_OK;
}

inline bool Simple_I2C::write_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len) {
  return HAL_I2C_Mem_Write(&hi2c_, address, reg_addr, 1, data, len, timeout_) == HAL_OK;
}

inline bool Simple_I2C::read_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len) {
  return HAL_I2C_Mem_Read(&hi2c_, address, reg_addr, 1, data, len, timeout_) == HAL_OK;
}
