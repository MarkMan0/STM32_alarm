/**
 * @file uart_tests.h
 * @brief UART test declarations
 */
#pragma once

#include "uart.h"

void test_send_buffer();
void test_transmit_immediate();
void test_transmit_data();
void test_send_data();
void test_printf();
void test_printf_overflow();

extern UART_DMA uart1;
