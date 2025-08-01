#ifndef LDR_H
#define LDR_H

#include "stm32f1xx.h"
#include <stdint.h>


void ADC_init(void);

uint16_t ADC_read(void);
void LED_init(void); 

#endif // LDR_H