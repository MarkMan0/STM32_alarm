/**
 * @file rtos_i2c.h
 * @brief RTOS I2C driver
 *
 */

#pragma once

#include "FreeRTOS.h"
#include "semphr.h"
#include "utils.h"
#include "main.h"
#include "pin_api.h"
#include <algorithm>

/**
 * @brief I2C hardware wrapper to be used inside and RTOS
 *
 */
class RTOS_I2C {
public:
  I2C_HandleTypeDef hi2c_;  ///< Handle to the i2c

  /// Initialize the i2c1 hardware for @p i2c
  static void init_i2c1(RTOS_I2C* i2c) {
    constexpr I2C_InitTypeDef init{ .Timing = 0x0000020B,
                                    .OwnAddress1 = 0,
                                    .AddressingMode = I2C_ADDRESSINGMODE_7BIT,
                                    .DualAddressMode = I2C_DUALADDRESS_DISABLE,
                                    .OwnAddress2 = 0,
                                    .OwnAddress2Masks = I2C_OA2_NOMASK,
                                    .GeneralCallMode = I2C_GENERALCALL_DISABLE,
                                    .NoStretchMode = I2C_NOSTRETCH_DISABLE };

    i2c->hi2c_.Instance = I2C1;
    i2c->hi2c_.Init = init;

    HAL_I2C_RegisterCallback(&i2c->hi2c_, HAL_I2C_MSPINIT_CB_ID, RTOS_I2C::i2c1_msp_init);
    HAL_I2C_Init(&i2c->hi2c_);
    HAL_I2CEx_ConfigAnalogFilter(&i2c->hi2c_, I2C_ANALOGFILTER_ENABLE);
    HAL_I2CEx_ConfigDigitalFilter(&i2c->hi2c_, 0x00);


    i2c->mtx_ = xSemaphoreCreateMutex();
  }

  [[nodiscard]] bool write(uint8_t address, uint8_t* data, size_t len) {
    utils::Lock lck(mtx_);
    portENTER_CRITICAL();
    auto ret = HAL_I2C_Master_Transmit(&hi2c_, address, data, len, timeout_) == HAL_OK;
    portEXIT_CRITICAL();
    return ret;
  }
  [[nodiscard]] bool read(uint8_t address, uint8_t* data, size_t len) {
    utils::Lock lck(mtx_);
    portENTER_CRITICAL();
    auto ret = HAL_I2C_Master_Receive(&hi2c_, address, data, len, timeout_) == HAL_OK;
    portEXIT_CRITICAL();
    return ret;
  }
  [[nodiscard]] bool write_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len) {
    utils::Lock lck(mtx_);
    portENTER_CRITICAL();
    auto ret = HAL_I2C_Mem_Write(&hi2c_, address, reg_addr, 1, data, len, timeout_) == HAL_OK;
    portEXIT_CRITICAL();
    return ret;
  }
  [[nodiscard]] bool read_register(uint8_t address, uint8_t reg_addr, uint8_t* data, size_t len) {
    utils::Lock lck(mtx_);
    portENTER_CRITICAL();
    auto ret = HAL_I2C_Mem_Read(&hi2c_, address, reg_addr, 1, data, len, timeout_) == HAL_OK;
    portEXIT_CRITICAL();
    return ret;
  }

  /// If i2c bus is manipulated externally from the class, it should be locked using this object
  [[nodiscard]] utils::Lock&& get_lock() {
    return std::move(utils::Lock(mtx_));
  }

private:
  static inline constexpr uint32_t timeout_{ 100 };  ///< default timeout for I2C communication
  SemaphoreHandle_t mtx_;                            ///< I2C mutex

  /// MSP init for i2c1
  static void i2c1_msp_init(I2C_HandleTypeDef* hi2c) {
    assert_param(hi2c->Instance == I2C1);
    if (hi2c->Instance == I2C1) {
      __HAL_RCC_I2C1_CLK_ENABLE();
      pin_mode(pins::sda1, pin_mode_t::ALTERNATE_OD_PU, GPIO_AF4_I2C1);
      pin_mode(pins::scl1, pin_mode_t::ALTERNATE_OD_PU, GPIO_AF4_I2C1);
    }
  };
};
