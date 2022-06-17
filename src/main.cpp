#include <FreeRTOS.h>
#include <cmsis_os2.h>

int main() {
  osKernelInitialize();

  osKernelStart();

  while (1) {
  }
}