/**
 * @file main.cpp
 * @brief Program entry point
 *
 */



#include "main.h"
#include "pin_api.h"
#include "uart.h"
#include <array>
#include "FreeRTOS.h"
#include "GFX.h"
#include "rtos_i2c.h"
#include "display_task.h"
#include "uart_task.h"
#include "command_parser.h"
#include "globals.h"


void SystemClock_Config(void);


UART_DMA uart2(UART_DMA::uart2_hw_init, UART_DMA::uart2_enable_isrs);
RTOS_I2C i2c;
DS3231 rtc(i2c);
SSD1306 display(i2c);
GFX gfx;
CommandDispatcher cmd(&uart2);

static void led_task(void*) {
  pin_mode(pins::led, pin_mode_t::OUT_PP);
  while (1) {
    toggle_pin(pins::led);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
  assert_param(0);
}

int main(void) {
  HAL_Init();
  SystemClock_Config();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  uart2.begin(115200);
  i2c.init_i2c1();
  display.begin();
  gfx.insert_ssd1306_dependency(&display);

  TaskHandle_t led_handle, uart2_handle, display_handle;
  xTaskCreate(led_task, "blink task", 128, nullptr, 20, &led_handle);
  xTaskCreate(uart_task, "uart2 RX task", 128, nullptr, 20, &uart2_handle);
  xTaskCreate(display_task, "display task", 128, nullptr, 20, &display_handle);

  // uart2.register_task_to_notify_on_rx(uart2_handle);

  vTaskStartScheduler();
  while (1) {
  }
}

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
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
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
