#pragma once

#include "uart.h"

void test_send_buffer();
void test_transmit_immediate();
void test_transmit_data();
void test_send_data();
void test_printf();

extern UART_DMA uart1;