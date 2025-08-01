#ifndef AHT20_H
#define AHT20_H

#include "stm32f1xx.h"
#include <string.h>
#include <stdint.h>

// Variáveis globais
extern uint8_t data[7];
extern float temperature, humidity;

// Protótipos das funções
void delay_ms_ht20(volatile uint32_t ms);

void GPIO_Init_ht20(void);

void I2C1_Init(void);
void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_SendAddress(uint8_t addr, uint8_t rw);
void I2C1_WriteByte(uint8_t byte);
uint8_t I2C1_ReadByte(int ack);

void AHT20_WriteCmd(uint8_t *cmd, uint8_t len);
void AHT20_ReadData(uint8_t *buf, uint8_t len);
void AHT20_ConvertData(uint8_t *data, float *temp, float *hum);


#endif // AHT20_H
