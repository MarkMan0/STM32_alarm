/**
 * @file unity_config.cpp
 * @brief unity test framework conector with board and with PC via UART2
 */
#include "unity_config.h"
#include "../src/main.h"
#include <cstdio>
#include <unity.h>

static UART_HandleTypeDef UartHandle;


#ifdef __cplusplus
extern "C" {
#endif

static void UART2_MspInit(UART_HandleTypeDef* uart) {
  if (uart->Instance == USART2) {
    GPIO_InitTypeDef gpio = { 0 };

    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    gpio.Alternate = GPIO_AF7_USART2;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pin = GPIO_PIN_15 | GPIO_PIN_2;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &gpio);
  }
}

void unityOutputStart() {
  UartHandle.Instance = USART2;
  UartHandle.Init.BaudRate = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.Mode = UART_MODE_TX_RX;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  UartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;

  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_RegisterCallback(&UartHandle, HAL_UART_MSPINIT_CB_ID, UART2_MspInit);

  if (HAL_UART_Init(&UartHandle) != HAL_OK) {
    while (1) {
    }
  }
}



void unityOutputChar(char c) {
  HAL_UART_Transmit(&UartHandle, (uint8_t*)(&c), 1, 1000);
}

void unityOutputFlush() {
}

void unityOutputComplete() {
  HAL_Delay(500);
  __HAL_RCC_USART2_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
}



void __weak SysTick_Handler(void) {
  HAL_IncTick();
}

void Error_Handler() {
  while (1)
    ;
}

void assert_failed(uint8_t* file, uint32_t line) {
  char buff[40];
  snprintf(buff, 40, "ASSERT: %s  :: %d", file, static_cast<int>(line));
  TEST_MESSAGE(buff);
  return;
}



void TIM7_DAC2_IRQHandler(void) {
  extern TIM_HandleTypeDef htim7;
  HAL_TIM_IRQHandler(&htim7);
}

TIM_HandleTypeDef htim7;

static void TIM7_period_elapsed_cb(TIM_HandleTypeDef* htim) {
  assert_param(&htim7 == htim);

  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
}

static HAL_StatusTypeDef init_TIM7(uint32_t TickPriority) {
  RCC_ClkInitTypeDef clkconfig;
  uint32_t uwTimclock = 0;
  uint32_t uwPrescalerValue = 0;
  uint32_t pFLatency;

  HAL_NVIC_SetPriority(TIM7_DAC2_IRQn, TickPriority, 0);
  HAL_NVIC_EnableIRQ(TIM7_DAC2_IRQn);

  __HAL_RCC_TIM7_CLK_ENABLE();

  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  uwTimclock = HAL_RCC_GetPCLK1Freq();

  uwPrescalerValue = (uint32_t)((uwTimclock / 1000000) - 1);

  htim7.Instance = TIM7;

  htim7.Init.Period = (1000000 / 1000) - 1;
  htim7.Init.Prescaler = uwPrescalerValue;
  htim7.Init.ClockDivision = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  if (HAL_TIM_Base_Init(&htim7) == HAL_OK) {
    HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM7_period_elapsed_cb);
    return HAL_TIM_Base_Start_IT(&htim7);
  }

  /* Return function status */
  return HAL_ERROR;
}



HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
  uwTickPrio = TickPriority;
  return init_TIM7(TickPriority);
}


void HAL_SuspendTick(void) {
  __HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);
}


void HAL_ResumeTick(void) {
  __HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
}


#ifdef __cplusplus
}
#endif
