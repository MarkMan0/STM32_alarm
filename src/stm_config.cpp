/**
 * @file stm_config.cpp
 * @brief Board and MCU configuration
 */

#include "main.h"
#include "stm_config.h"
#include "FreeRTOS.h"
#include "globals.h"

TIM_HandleTypeDef htim7;  ///< Timer used for HAL tick


void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
  RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
}


/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
  volatile bool b = true;
  portDISABLE_INTERRUPTS();
  while (b) {
    HAL_Delay(1000);
  }
  portENABLE_INTERRUPTS();
}
#endif /* USE_FULL_ASSERT */


void HAL_MspInit(void) {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}


/// Called by HAL when timer period elapses
static void TIM7_period_elapsed_cb(TIM_HandleTypeDef* htim) {
  assert_param(&htim7 == htim);

  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
}

/// TIM init for 1kHz / 1ms period
static HAL_StatusTypeDef init_TIM7(uint32_t TickPriority) {
  RCC_ClkInitTypeDef clkconfig;
  uint32_t uwTimclock = 0;
  uint32_t uwPrescalerValue = 0;
  uint32_t pFLatency;

  HAL_NVIC_SetPriority(TIM7_DAC2_IRQn, TickPriority, 0);
  HAL_NVIC_EnableIRQ(TIM7_DAC2_IRQn);

  __HAL_RCC_TIM7_CLK_ENABLE();

  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

  uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();

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


/// Called by HAL to init the timer
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
  uwTickPrio = TickPriority;
  return init_TIM7(TickPriority);
}


/// Called by HAL to pause the timer
void HAL_SuspendTick(void) {
  __HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);
}


/// Called by HAL to resume the timer
void HAL_ResumeTick(void) {
  __HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
}


static void TIM2_Msp_Init(TIM_HandleTypeDef* htim) {
  assert_param(htim == &htim2);
  assert_param(htim->Instance == TIM2);
  __HAL_RCC_TIM2_CLK_ENABLE();
}

// TIM 2
TIM_HandleTypeDef htim2;

void TIM2_Init_1kHz() {
  TIM_ClockConfigTypeDef clk_src_config = { 0 };
  TIM_MasterConfigTypeDef master_config = { 0 };
  TIM_OC_InitTypeDef oc_config = { 0 };

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 35;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

  HAL_TIM_RegisterCallback(&htim2, HAL_TIM_BASE_MSPINIT_CB_ID, TIM2_Msp_Init);

  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
    Error_Handler();
  }
  clk_src_config.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &clk_src_config) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK) {
    Error_Handler();
  }
  master_config.MasterOutputTrigger = TIM_TRGO_RESET;
  master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &master_config) != HAL_OK) {
    Error_Handler();
  }
  oc_config.OCMode = TIM_OCMODE_ACTIVE;
  oc_config.Pulse = 500;
  oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
  oc_config.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &oc_config, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
}
