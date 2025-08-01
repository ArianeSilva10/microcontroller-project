#include "uart.h"

void USART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |=  (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0) | GPIO_CRH_CNF9_1;

    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |=  GPIO_CRH_CNF10_0;

    USART1->BRR = 72000000UL / 9600UL;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}   

char USART1_ReadChar(void)
{
    while (!(USART1->SR & USART_SR_RXNE));
    return (char)USART1->DR;
}

void USART1_SendChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = (uint16_t)c;
}

int __io_putchar(int ch)
{
    USART1_SendChar((char)ch);
    return ch;
}