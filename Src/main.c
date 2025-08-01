#include "nrfl2401.h"
#include "uart.h"
#include "stm32f1xx.h"
#include <string.h>
#include <stdio.h>

void SystemClock_Config(void) {
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));
    
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));
    
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1;
}

//  (PC13)
void led_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH &= ~(0xF << 20);  // PC13
    GPIOC->CRH |= (0x3 << 20);   // Output 50MHz
}

void led_on(void) {
    GPIOC->BRR = GPIO_BRR_BR13;
}

void led_off(void) {
    GPIOC->BSRR = GPIO_BSRR_BS13;
}

void led_toggle(void) {
    GPIOC->ODR ^= GPIO_ODR_ODR13;
}


// basic test  NRF24L01
uint8_t nrf_test_connection(void) {
    // tests writing and reading a register
    uint8_t test_value = 0x55;
    nrf_write_reg(RF_CH, test_value);
    delay_ms(1);
    uint8_t read_value = nrf_read_reg(RF_CH);
    
    return (read_value == test_value);
}

// ==== TRANSMITTER MODE ====
void test_transmitter(void) {
    printf("Iniciando modo TRANSMISSOR...\n");
    
    // init NRF with transmitter
    nrf_init(NRF_MODE_TX);
    
    // setup address
    uint8_t tx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t rx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7}; // Mesmo endereço para ACK
    
    nrf_set_tx_addr(tx_addr, 5);
    nrf_set_rx_addr(0, rx_addr, 5); // to receive ACK
    
    printf("Transmissor configurado!\n");
    
    uint32_t counter = 0;
    while(1) {
        char message[32];
        sprintf(message, "Hello %lu", counter++);
        
        printf("Enviando: ");
        printf(message);
        printf("\n");
        
        // transmit data
        if(nrf_transmit((uint8_t*)message, strlen(message))) {
            printf("Transmissao OK!\n");
            led_on();
        } else {
            printf("Transmissao FALHOU!\n");
            led_off();
        }
        
        delay_ms(1000);
        led_toggle();
    }
}

// ==== RECEIVER MODE ====
void test_receiver(void) {
    printf("Iniciando modo RECEPTOR...\n");
    
    // init NRF an receiver
    nrf_init(NRF_MODE_RX);
    
    // setup reception adress
    uint8_t rx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    nrf_set_rx_addr(0, rx_addr, 5);
    
    printf("Receptor configurado!\n");
    printf("Aguardando dados...\n");
    
    while(1) {
        uint8_t buffer[32];
        
        if(nrf_data_available()) {
            if(nrf_receive(buffer, 32)) {
                buffer[31] = '\0';
                
                printf("Recebido: ");
                printf((char*)buffer);
                printf("\n");
                
                led_toggle();
            }
        }
        
        delay_ms(10);
    }
}

// ==== PING-PONG TEST ====
void test_ping_pong(void) {
    printf("Iniciando teste PING-PONG...\n");
    
    // setup address
    uint8_t addr1[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t addr2[5] = {0xD7, 0xD7, 0xD7, 0xD7, 0xD7};
    
    // init an trasmiter
    nrf_init(NRF_MODE_TX);
    nrf_set_tx_addr(addr2, 5);      // send to addr2
    nrf_set_rx_addr(0, addr1, 5);   // receive in addr1
    
    uint32_t ping_count = 0;
    uint8_t is_tx_mode = 1;
    
    while(1) {
        if(is_tx_mode) {
            // transmitt mode
            char ping_msg[32];
            sprintf(ping_msg, "PING %lu", ping_count++);
            
            printf("Enviando: ");
            printf(ping_msg);
            printf("\n");
            
            if(nrf_transmit((uint8_t*)ping_msg, strlen(ping_msg))) {
                printf("PING enviado!\r\n");
                
                // Switch to receiver mode to wait for PONG
                uint8_t config = nrf_read_reg(CONFIG);
                nrf_write_reg(CONFIG, config | PRIM_RX);
                NRF_CE_PORT->BSRR = NRF_CE_PIN;
                is_tx_mode = 0;
                
                led_on();
            } else {
                printf("PING falhou!\n");
                led_off();
            }
        } else {
            // mode receiver
            uint8_t buffer[32];
            
            if(nrf_data_available()) {
                if(nrf_receive(buffer, 32)) {
                    buffer[31] = '\0';
                    
                    printf("Recebido: ");
                    printf((char*)buffer);
                    printf("\n");
                    
                    // return to transmitter mode
                    NRF_CE_PORT->BRR = NRF_CE_PIN;
                    uint8_t config = nrf_read_reg(CONFIG);
                    nrf_write_reg(CONFIG, config & ~PRIM_RX);
                    is_tx_mode = 1;
                    
                    led_toggle();
                }
            }
        }
        
        delay_ms(100);
    }
}

// ==== DIAGNOSIS ====
void nrf_diagnostic(void) {
    printf("\n=== DIAGNÓSTICO NRF24L01 ===\n");
    
    // connection test
    if(nrf_test_connection()) {
        printf("✓ Conexão SPI OK\n");
    } else {
        printf("✗ ERRO na conexão SPI!\n");
        return;
    }
    
    // read important registers
    uint8_t config = nrf_read_reg(CONFIG);
    uint8_t status = nrf_read_reg(STATUS);
    uint8_t rf_ch = nrf_read_reg(RF_CH);
    uint8_t rf_setup = nrf_read_reg(RF_SETUP);
    
    char debug_msg[64];
    sprintf(debug_msg, "CONFIG: 0x%02X\n", config);
    printf(debug_msg);
    
    sprintf(debug_msg, "STATUS: 0x%02X\n", status);
    printf(debug_msg);
    
    sprintf(debug_msg, "RF_CH: %d\n", rf_ch);
    printf(debug_msg);
    
    sprintf(debug_msg, "RF_SETUP: 0x%02X\n", rf_setup);
    printf(debug_msg);
    
    printf("=========================\n\n");
}

// ==== MAIN ====
int main(void) {
    SystemClock_Config();
    led_init();
    USART1_Init();
    
    delay_ms(1000);
    printf("\n*** TESTE NRF24L01 ***\n");
    
    // diagnóstico inicial
    nrf_diagnostic();
    
    // ESCOLHA O MODO DE TESTE:
    
    // opcao 1: teste como transmissor
    test_transmitter();
    
    // opcao 2: Teste como receptor (descomente esta linha e comente a anterior)
    // test_receiver();
    
    // opcao 3: Teste ping-pong (descomente esta linha)
    // test_ping_pong();
    
    while(1) {
        led_toggle();
        delay_ms(500);
    }
}
