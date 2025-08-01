#include "ldr.h"

void ADC_init(void) {
    // Habilita clock GPIOA e ADC1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;

    // PA0 como entrada analogica (ADC_IN0)
    GPIOA->CRL &= ~GPIO_CRL_MODE0;
    GPIOA->CRL &= ~GPIO_CRL_CNF0;

    // Ativa o ADC e calibra
    ADC1->CR2 |= ADC_CR2_ADON;      // Liga ADC
    for (volatile int i = 0; i < 1000; i++); // delay

    ADC1->CR2 |= ADC_CR2_CAL;       // Inicia calibracao
    while (ADC1->CR2 & ADC_CR2_CAL); // espera calibrar
}

uint16_t ADC_read(void) {
    ADC1->SQR3 = 0; // Canal 0 (PA0)
    ADC1->CR2 |= ADC_CR2_ADON; // Inicia conversao
    while (!(ADC1->SR & ADC_SR_EOC)); // Espera terminar
    return ADC1->DR; // Retorna valor (0 a 4095)
}

void LED_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // PB9 como saida push-pull
    GPIOB->CRH &= ~GPIO_CRH_MODE9;
    GPIOB->CRH &= ~GPIO_CRH_CNF9;
    GPIOB->CRH |= GPIO_CRH_MODE9_1; // saida 2MHz
}