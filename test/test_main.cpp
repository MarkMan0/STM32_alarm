#include "main.h"

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "unity.h"

extern void test_task(void*);
extern void pre_test();



int main() {
  HAL_Init();
  HAL_Delay(500);

  osKernelInitialize();
  pre_test();

  TaskHandle_t handle;
  xTaskCreate(test_task, "test task", 128, nullptr, osPriorityNormal, &handle);

  osKernelStart();
}


__attribute__((weak)) void pre_test() {
}
