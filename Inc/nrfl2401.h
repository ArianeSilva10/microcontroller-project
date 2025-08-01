#ifndef NRF24L01_H
#define NRF24L01_H

#include <stdint.h>
#include "stm32f1xx.h"

#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */

#define REGISTER_MASK  0x1F  // Máscara de 5 bits para endereços de registradores
#define DPL_P0        0x01   // Habilitar Dynamic Payload Length no Pipe 0


// pins defines
#define NRF_CE_PIN   GPIO_PIN_0
#define NRF_CE_PORT  GPIOB
#define NRF_CSN_PIN  GPIO_PIN_1
#define NRF_CSN_PORT GPIOA
#define NRF_IRQ_PIN  GPIO_PIN_2
#define NRF_IRQ_PORT GPIOA

// operations mode
#define NRF_MODE_TX 0
#define NRF_MODE_RX 1

// spi comands
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define W_ACK_PAYLOAD 0xA8
#define W_TX_PAYLOAD_NOACK 0xB0
#define NOP           0xFF

// NRF24L01 registers
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD       0x1C
#define FEATURE     0x1D

// bits mask
#define MASK_RX_DR  (1<<6)
#define MASK_TX_DS  (1<<5)
#define MASK_MAX_RT (1<<4)
#define EN_CRC      (1<<3)
#define CRCO        (1<<2)
#define PWR_UP      (1<<1)
#define PRIM_RX     (1<<0)

#define ENAA_P5     (1<<5)
#define ENAA_P4     (1<<4)
#define ENAA_P3     (1<<3)
#define ENAA_P2     (1<<2)
#define ENAA_P1     (1<<1)
#define ENAA_P0     (1<<0)

#define ERX_P5      (1<<5)
#define ERX_P4      (1<<4)
#define ERX_P3      (1<<3)
#define ERX_P2      (1<<2)
#define ERX_P1      (1<<1)
#define ERX_P0      (1<<0)

#define AW_5BYTES   0x03
#define ARD_250US   0x00
#define ARC_15RET   0x0F
#define RF_CH_10    0x0A
#define RF_DR_2MBPS (1<<3)
#define RF_PWR_0DBM (0x03<<1)
#define LNA_HCURR   (1<<0)

#define RX_DR       (1<<6)
#define TX_DS       (1<<5)
#define MAX_RT      (1<<4)
#define RX_P_NO_MASK 0x0E
#define TX_FULL     (1<<0)

#define EN_DPL      (1<<2)
#define EN_ACK_PAY  (1<<1)
#define EN_DYN_ACK  (1<<0)

void GPIO_setup();
void nrf_gpio_init(void);
void spi_init(void);
uint8_t spi_transfer(uint8_t data);
void nrf_write_reg(uint8_t reg, uint8_t value);
uint8_t nrf_read_reg(uint8_t reg);
void nrf_write_buf(uint8_t reg, uint8_t *buf, uint8_t len);
void nrf_read_buf(uint8_t reg, uint8_t *buf, uint8_t len);
void nrf_init(uint8_t mode);
uint8_t nrf_transmit(uint8_t *data, uint8_t len);
uint8_t nrf_receive(uint8_t *data, uint8_t len);
//void nrf_debug_status(void);
void nrf_set_tx_addr(uint8_t *addr, uint8_t len);
void nrf_set_rx_addr(uint8_t pipe, uint8_t *addr, uint8_t len);
void nrf_power_up(void);
void nrf_power_down(void);
uint8_t nrf_data_available(void);
uint8_t nrf_is_sending(void);
uint8_t nrf_get_status(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
#endif // NRF24L01_H
