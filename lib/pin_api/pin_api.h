/**
 * @file pin_api.h
 * @brief Code to simplify working with pins. Wraps HAL with low to none overhead
 */

#pragma once

#include <cassert>
#include <array>

/// @brief All existing pins
enum pin_name : uint8_t {
  // clang-format off
  PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
  PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
  PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
  PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
  PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
  PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15,
  PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG7, PG8, PG9, PG10, PG11, PG12, PG13, PG14, PG15,
  PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10, PH11, PH12, PH13, PH14, PH15,
  // clang-format on
};

/// @brief The mode for pins
enum class pin_mode_t {
  INPUT,
  INPUT_PU,
  INPUT_PD,
  OUT_PP,
  OUT_OD,
  OUT_OD_PU,
  ANALOG,
  IT_RISING,
  IT_RISING_PU,
  IT_RISING_PD,
  IT_FALLING,
  IT_FALLING_PU,
  IT_FALLING_PD,
  IT_RISE_FALL,
  IT_RISE_FALL_PU,
  IT_RISE_FALL_PD,
  ANALOG_PU,
  ANALOG_PD,
  ALTERNATE_PP,
  ALTERNATE_OD,
  ALTERNATE_OD_PU,
};


/// @brief Backend for pin_api, not needed by the user
namespace pin_api {

  /// @brief The available pins on the current board/MCU
  inline constexpr pin_name usable_pins[] = { PA0,  PA1,  PA2,  PA3,  PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11,
                                              PA12, PA13, PA14, PA15, PB0, PB1, PB2, PB3, PB4, PB5, PB6,  PB7 };

  /// @brief Returns the pointer to the PORT for the current pin or nullptr
  [[nodiscard]] inline GPIO_TypeDef* pin_name_to_port(const pin_name pin) {
    constexpr std::array<GPIO_TypeDef*, 8> table = {
#ifdef GPIOA
      GPIOA,
#else
      nullptr,
#endif
#ifdef GPIOB
      GPIOB,
#else
      nullptr,
#endif
#ifdef GPIOC
      GPIOC,
#else
      nullptr,
#endif
#ifdef GPIOD
      GPIOD,
#else
      nullptr,
#endif
#ifdef GPIOE
      GPIOE,
#else
      nullptr,
#endif
#ifdef GPIOF
      GPIOF,
#else
      nullptr,
#endif
#ifdef GPIOG
      GPIOG,
#else
      nullptr,
#endif
#ifdef GPIOH
      GPIOH,
#else
      nullptr,
#endif
    };

    assert_param(pin / 16 < table.size());
    GPIO_TypeDef* ret = table[pin / 16];
    assert_param(ret != nullptr);

    return ret;
  }

  /// @brief Returns the HAL pin number for the current pin
  [[nodiscard]] inline constexpr uint16_t pin_name_to_num(const pin_name pin) {
    return 0x1 << (pin % 16);
  }

  /// @brief In DEBUG build, checks if the given pin is available on the board. In RELEASE, return true
  [[nodiscard]] inline constexpr bool pin_is_available(const pin_name pin) {
#ifdef NDEBUG
    return true;
#endif

    for (auto p : usable_pins) {
      if (pin == p) {
        return true;
      }
    }
    return false;
  }

  /// @brief Extracts the pull mode for the given pin_mode
  [[nodiscard]] constexpr uint16_t get_pull_type(const pin_mode_t mode) {
    switch (mode) {
      default:
        return GPIO_NOPULL;
      case pin_mode_t::ANALOG_PD:
      case pin_mode_t::INPUT_PD:
      case pin_mode_t::IT_RISING_PD:
      case pin_mode_t::IT_FALLING_PD:
      case pin_mode_t::IT_RISE_FALL_PD:
        return GPIO_PULLDOWN;
      case pin_mode_t::ALTERNATE_OD_PU:
      case pin_mode_t::ANALOG_PU:
      case pin_mode_t::OUT_OD_PU:
      case pin_mode_t::INPUT_PU:
      case pin_mode_t::IT_RISING_PU:
      case pin_mode_t::IT_FALLING_PU:
      case pin_mode_t::IT_RISE_FALL_PU:
        return GPIO_PULLUP;
    }
  }

  /// @brief Starts the clock for the given pin
  inline void init_clock_for_gpio(const pin_name pin) {
    switch (pin / 16) {
#ifdef GPIOA_BASE
      case 0:
        __HAL_RCC_GPIOA_CLK_ENABLE();
        break;
#endif
#ifdef GPIOB_BASE
      case 1:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
#endif
#ifdef GPIOC_BASE
      case 2:
        __HAL_RCC_GPIOC_CLK_ENABLE();
        break;
#endif
#ifdef GPIOD_BASE
      case 3:
        __HAL_RCC_GPIOD_CLK_ENABLE();
        break;
#endif
#ifdef GPIOE_BASE
      case 4:
        __HAL_RCC_GPIOE_CLK_ENABLE();
        break;
#endif
#ifdef GPIOF_BASE
      case 5:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
#endif
#ifdef GPIOG_BASE
      case 6:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
#endif
#ifdef GPIOH_BASE
      case 7:
        __HAL_RCC_GPIOB_CLK_ENABLE();
        break;
#endif
    }
  }
  /// @name init functions for all modes
  /// @{
  inline void init_pin_input(const pin_name pin, const pin_mode_t mode) {
    GPIO_InitTypeDef gpio{ .Pin = pin_name_to_num(pin),
                           .Mode = GPIO_MODE_INPUT,
                           .Pull = get_pull_type(mode),
                           .Speed = GPIO_SPEED_FREQ_MEDIUM,
                           .Alternate = 0 };

    init_clock_for_gpio(pin);
    HAL_GPIO_Init(pin_name_to_port(pin), &gpio);
  }

  inline void init_pin_output_pp(const pin_name pin, const pin_mode_t mode) {
    const auto port = pin_name_to_port(pin);
    const auto num = pin_name_to_num(pin);

    GPIO_InitTypeDef gpio{
      .Pin = num, .Mode = GPIO_MODE_OUTPUT_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Alternate = 0
    };

    init_clock_for_gpio(pin);
    HAL_GPIO_WritePin(port, num, GPIO_PIN_RESET);
    HAL_GPIO_Init(port, &gpio);
  }

  inline void init_pin_output_od(const pin_name pin, const pin_mode_t mode) {
    GPIO_InitTypeDef gpio{ .Pin = pin_name_to_num(pin),
                           .Mode = GPIO_MODE_OUTPUT_OD,
                           .Pull = get_pull_type(mode),
                           .Speed = GPIO_SPEED_FREQ_MEDIUM,
                           .Alternate = 0 };

    init_clock_for_gpio(pin);
    HAL_GPIO_Init(pin_name_to_port(pin), &gpio);
  }

  inline void init_pin_analog(const pin_name pin, const pin_mode_t mode) {
    GPIO_InitTypeDef gpio{ .Pin = pin_name_to_num(pin),
                           .Mode = GPIO_MODE_ANALOG,
                           .Pull = get_pull_type(mode),
                           .Speed = GPIO_SPEED_FREQ_MEDIUM,
                           .Alternate = 0 };

    init_clock_for_gpio(pin);
    HAL_GPIO_Init(pin_name_to_port(pin), &gpio);
  }

  inline void init_pin_alternate(const pin_name pin, const pin_mode_t mode, const uint16_t alternate) {
    GPIO_InitTypeDef gpio{ .Pin = pin_name_to_num(pin),
                           .Mode = mode == pin_mode_t::ALTERNATE_PP ? GPIO_MODE_AF_PP : GPIO_MODE_AF_OD,
                           .Pull = get_pull_type(mode),
                           .Speed = GPIO_SPEED_FREQ_MEDIUM,
                           .Alternate = alternate };

    init_clock_for_gpio(pin);
    HAL_GPIO_Init(pin_name_to_port(pin), &gpio);
  }

  inline void init_pin_IT(const pin_name pin, const pin_mode_t mode) {
    GPIO_InitTypeDef gpio{ .Pin = pin_name_to_num(pin),
                           .Mode = GPIO_MODE_INPUT,
                           .Pull = get_pull_type(mode),
                           .Speed = GPIO_SPEED_FREQ_MEDIUM,
                           .Alternate = 0 };

    switch (mode) {
      case pin_mode_t::IT_FALLING:
      case pin_mode_t::IT_FALLING_PU:
      case pin_mode_t::IT_FALLING_PD:
        gpio.Mode = GPIO_MODE_IT_FALLING;
        break;
      case pin_mode_t::IT_RISING:
      case pin_mode_t::IT_RISING_PU:
      case pin_mode_t::IT_RISING_PD:
        gpio.Mode = GPIO_MODE_IT_RISING;
        break;
      case pin_mode_t::IT_RISE_FALL:
      case pin_mode_t::IT_RISE_FALL_PU:
      case pin_mode_t::IT_RISE_FALL_PD:
        gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
      default:
        break;
    }
    HAL_GPIO_Init(pin_name_to_port(pin), &gpio);
  }

  /// @}
}  // namespace pin_api


/// @name public API of pin_api
/// @{

/// @brief pin initialization in mode @p mode. If mode is alternate, specify AF in @p alternate
inline void pin_mode(const pin_name pin, const pin_mode_t mode, const uint16_t alternate = 0) {
  assert_param(pin_api::pin_is_available(pin));

  switch (mode) {
    case pin_mode_t::INPUT:
    case pin_mode_t::INPUT_PD:
    case pin_mode_t::INPUT_PU:
      pin_api::init_pin_input(pin, mode);
      break;

    case pin_mode_t::OUT_PP:
      pin_api::init_pin_output_pp(pin, mode);
      break;

    case pin_mode_t::OUT_OD:
    case pin_mode_t::OUT_OD_PU:
      pin_api::init_pin_output_od(pin, mode);
      break;

    case pin_mode_t::ANALOG:
    case pin_mode_t::ANALOG_PD:
    case pin_mode_t::ANALOG_PU:
      pin_api::init_pin_analog(pin, mode);
      break;

    case pin_mode_t::ALTERNATE_OD:
    case pin_mode_t::ALTERNATE_OD_PU:
    case pin_mode_t::ALTERNATE_PP:
      pin_api::init_pin_alternate(pin, mode, alternate);
      break;

    case pin_mode_t::IT_FALLING:
    case pin_mode_t::IT_FALLING_PU:
    case pin_mode_t::IT_FALLING_PD:
    case pin_mode_t::IT_RISING:
    case pin_mode_t::IT_RISING_PU:
    case pin_mode_t::IT_RISING_PD:
    case pin_mode_t::IT_RISE_FALL:
    case pin_mode_t::IT_RISE_FALL_PU:
    case pin_mode_t::IT_RISE_FALL_PD:
      pin_api::init_pin_IT(pin, mode);
      break;
  }
}

inline void write_pin(const pin_name pin, uint16_t val) {
  assert_param(pin_api::pin_is_available(pin));
  HAL_GPIO_WritePin(pin_api::pin_name_to_port(pin), pin_api::pin_name_to_num(pin), static_cast<GPIO_PinState>(val));
}

inline void set_pin(const pin_name pin) {
  write_pin(pin, 1);
}

inline void reset_pin(const pin_name pin) {
  write_pin(pin, 0);
}

inline void toggle_pin(const pin_name pin) {
  assert_param(pin_api::pin_is_available(pin));
  HAL_GPIO_TogglePin(pin_api::pin_name_to_port(pin), pin_api::pin_name_to_num(pin));
}

[[nodiscard]] inline bool read_pin(const pin_name pin) {
  assert_param(pin_api::pin_is_available(pin));
  return HAL_GPIO_ReadPin(pin_api::pin_name_to_port(pin), pin_api::pin_name_to_num(pin));
}

inline void deinit_pin(const pin_name pin) {
  assert_param(pin_api::pin_is_available(pin));
  HAL_GPIO_DeInit(pin_api::pin_name_to_port(pin), pin_api::pin_name_to_num(pin));
}


/// @}

/// @brief pin labels on the board
namespace pin_labels {
  inline constexpr auto A0 = PA0, A1 = PA1, A2 = PA3, A3 = PA4, A4 = PA5, A5 = PA6, A6 = PA7, A7 = PA2, D0 = PA10,
                        D1 = PA9, D2 = PA12, D3 = PB0, D4 = PB7, D5 = PB6, D6 = PB1, D7 = PF0, D8 = PF1, D9 = PA8,
                        D10 = PA11, D11 = PB5, D12 = PB4, D13 = PB3;
}  // namespace pin_labels


/// @brief pin aliases
namespace pins {
  using namespace pin_labels;
  inline constexpr auto rx = PA15, tx = PA2, led = D13, rx1 = D0, tx1 = D1, sda1 = PB7, scl1 = PB6, enc_A = A3,
                        enc_B = A4, enc_SW = A1, alarm_it = A5;
}  // namespace pins
