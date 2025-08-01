#include "aht20.h"

// Buffer para dados do sensor
uint8_t data[7];
float temperature, humidity;

// Funções utilitárias para delay simples (não precisa de timer para este exemplo)
void delay_ms_ht20(volatile uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 8000; i++) __NOP();
}

// --- Inicializa GPIO para I2C e USART ---
void GPIO_Init_ht20(void) {
    // Habilita clocks GPIOB e GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN;

    // PB6 (I2C1_SCL) e PB7 (I2C1_SDA) como AF Open Drain, 50MHz
    GPIOB->CRL &= ~((0xF << (6*4)) | (0xF << (7*4)));
    GPIOB->CRL |=  ((0xB << (6*4)) | (0xB << (7*4)));
}

// --- Inicializa I2C1 ---
void I2C1_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    // Frequência APB1 36MHz
    I2C1->CR2 = 36;
    // Configura clock I2C para 100kHz
    I2C1->CCR = 180;
    // Configura TRISE
    I2C1->TRISE = 37;

    // Habilita I2C
    I2C1->CR1 |= I2C_CR1_PE;
}

// --- Gerar condição START ---
void I2C1_Start(void) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

// --- Gerar condição STOP ---
void I2C1_Stop(void) {
    I2C1->CR1 |= I2C_CR1_STOP;
}

// --- Envia endereço (7 bits) + rw (0=write,1=read) ---
void I2C1_SendAddress(uint8_t addr, uint8_t rw) {
    I2C1->DR = (addr << 1) | (rw & 0x1);
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR2; // limpa flag ADDR
}

// --- Envia byte ---
void I2C1_WriteByte(uint8_t byte) {
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = byte;
}

// --- Recebe byte ---
uint8_t I2C1_ReadByte(int ack) {
    if (ack)
        I2C1->CR1 |= I2C_CR1_ACK;
    else
        I2C1->CR1 &= ~I2C_CR1_ACK;

    while (!(I2C1->SR1 & I2C_SR1_RXNE));
    return I2C1->DR;
}

// --- Escreve comando para o sensor ---
void AHT20_WriteCmd(uint8_t *cmd, uint8_t len) {
    I2C1_Start();
    I2C1_SendAddress(0x38, 0); // write
    for (uint8_t i=0; i<len; i++) {
        I2C1_WriteByte(cmd[i]);
    }
    I2C1_Stop();
}

// --- Lê bytes do sensor ---
void AHT20_ReadData(uint8_t *buf, uint8_t len) {
    I2C1_Start();
    I2C1_SendAddress(0x38, 1); // read
    for (uint8_t i=0; i<len; i++) {
        buf[i] = I2C1_ReadByte(i < (len-1)); // ACK para todos exceto último
    }
    I2C1_Stop();
}


// --- Conversão e cálculo temperatura/umidade ---
void AHT20_ConvertData(uint8_t *data, float *temp, float *hum) {
    uint32_t raw_humidity = ((data[1] << 12) | (data[2] << 4) | (data[3] >> 4));
    uint32_t raw_temperature = (((data[3] & 0x0F) << 16) | (data[4] << 8) | data[5]);

    *hum = (raw_humidity * 100.0f) / 1048576.0f;
    *temp = (raw_temperature * 200.0f / 1048576.0f) - 50.0f;
}
