
#include "../main.h"
#include "cmsis_os.h"
#include "uart_tests.h"
#include <unity.h>



static void test_task(void*) {
  UNITY_BEGIN();

  RUN_TEST(test_send_buffer);
  RUN_TEST(test_transmit_immediate);
  RUN_TEST(test_transmit_data);
  RUN_TEST(test_send_data);
  RUN_TEST(test_printf);


  UNITY_END();

  while (1) {
  }
}


int main() {
  HAL_Init();
  HAL_Delay(500);
  uart1.begin(115200);

  TaskHandle_t handle;
  xTaskCreate(test_task, "test task", 128, nullptr, osPriorityNormal, &handle);

  osKernelInitialize();
  osKernelStart();
}




#ifdef __cplusplus
extern "C" {
#endif

void DMA1_Channel4_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart1.hdmatx_);
}

void DMA1_Channel5_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart1.hdmarx_);
}
void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&uart1.huart_);
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