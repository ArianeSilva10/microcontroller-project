#ifndef  UART_H
#define UART_H
#include <stdint.h>
#include "stm32f1xx.h"

void USART1_Init(void);
void USART1_SendChar(char c);
char USART1_ReadChar(void);
int  __io_putchar(int ch);
#endif // UART_H