#include "nrfl2401.h"

// delays
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 8000; j++);
    }
}

void delay_us(uint32_t us) {
    for (uint32_t i = 0; i < us; i++) {
        for (volatile uint32_t j = 0; j < 8; j++);
    }
}

// FUNÇÃO ÚNICA PARA GPIO - REMOVIDA A DUPLICAÇÃO
void nrf_gpio_init(void){
    // Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    
    // SPI1 pins configuration
    // PA5 SCK - Alternate Function Output Push-Pull
    GPIOA->CRL |= GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1; 
    GPIOA->CRL |= GPIO_CRL_CNF5_1;                     
    GPIOA->CRL &= ~(GPIO_CRL_CNF5_0);
    
    // PA7 MOSI - Alternate Function Output Push-Pull
    GPIOA->CRL |= GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1; 
    GPIOA->CRL |= GPIO_CRL_CNF7_1;                     
    GPIOA->CRL &= ~(GPIO_CRL_CNF7_0);
    
    // PA6 MISO - Input Floating
    GPIOA->CRL &= ~(GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1); 
    GPIOA->CRL &= ~GPIO_CRL_CNF6_1;                       
    GPIOA->CRL |= (GPIO_CRL_CNF6_0);
    
    // NRF24L01 control pins
    // CE pin (PA0) - Output Push-Pull
    GPIOA->CRL &= ~(0xF << (4 * 0));
    GPIOA->CRL |= (0x3 << (4 * 0));
    
    // CSN pin (PA1) - Output Push-Pull  
    GPIOA->CRL &= ~(0xF << (4 * 1));
    GPIOA->CRL |= (0x3 << (4 * 1));
    
    // IRQ pin (PA2) - Input Pull-up
    GPIOA->CRL &= ~(0xF << (4 * 2));
    GPIOA->CRL |= (0x8 << (4 * 2));
    GPIOA->ODR |= NRF_IRQ_PIN;
    
    // Initial states
    NRF_CSN_PORT->BSRR = NRF_CSN_PIN; // CSN high
    NRF_CE_PORT->BRR = NRF_CE_PIN;    // CE low
}

// init spi
void spi_init(void){
    SPI1->CR1 &= ~((1 << 0) | (1 << 1)); // CPOL=0, CPHA=0
    SPI1->CR1 |= (1 << 2);             // Master Mode
    SPI1->CR1 |= (2 << 3);             // BR[2:0] = 010: fPCLK/8
    SPI1->CR1 &= ~(1 << 7);            // MSB first
    SPI1->CR1 |= (1 << 8) | (1 << 9);  // Software Slave Management
    SPI1->CR1 &= ~(1 << 10);           // Full-duplex
    SPI1->CR1 &= ~(1 << 11);           // 8 bit data
    SPI1->CR1 |= (1 << 6);             // Enable SPI
    delay_ms(10);
}

// basic spi transfer 
uint8_t spi_transfer(uint8_t data){
    while (!(SPI1->SR & SPI_SR_TXE));   
    SPI1->DR = data;
    while (!(SPI1->SR & SPI_SR_RXNE));  
    return SPI1->DR;
}

// write in registers
void nrf_write_reg(uint8_t reg, uint8_t value){
    NRF_CSN_PORT->BRR = NRF_CSN_PIN; // CSN low
    spi_transfer(W_REGISTER | (REGISTER_MASK & reg));
    spi_transfer(value);
    NRF_CSN_PORT->BSRR = NRF_CSN_PIN; // CSN high 
}

// read registers
uint8_t nrf_read_reg(uint8_t reg){
    NRF_CSN_PORT->BRR = NRF_CSN_PIN; // CSN low
    spi_transfer(R_REGISTER | (REGISTER_MASK & reg));
    uint8_t value = spi_transfer(0xFF);
    NRF_CSN_PORT->BSRR = NRF_CSN_PIN; // CSN high
    return value;
}

// buffer write
void nrf_write_buf(uint8_t reg, uint8_t *buf, uint8_t len){
    NRF_CSN_PORT->BRR = NRF_CSN_PIN; // CSN low
    spi_transfer(W_REGISTER | (REGISTER_MASK & reg));
    for (uint8_t i = 0; i < len; i++) {
        spi_transfer(buf[i]);
    }
    NRF_CSN_PORT->BSRR = NRF_CSN_PIN; // CSN high
}

// buffer read
void nrf_read_buf(uint8_t reg, uint8_t *buf, uint8_t len){
    NRF_CSN_PORT->BRR = NRF_CSN_PIN; // CSN low
    spi_transfer(R_REGISTER | (REGISTER_MASK & reg));
    for (uint8_t i = 0; i < len; i++) {
        buf[i] = spi_transfer(0xFF);
    }
    NRF_CSN_PORT->BSRR = NRF_CSN_PIN; // CSN high
}

// FUNÇÃO CORRIGIDA PARA FLUSH
void nrf_flush_tx(void){
    NRF_CSN_PORT->BRR = NRF_CSN_PIN;
    spi_transfer(FLUSH_TX);
    NRF_CSN_PORT->BSRR = NRF_CSN_PIN;
}

void nrf_flush_rx(void){
    NRF_CSN_PORT->BRR = NRF_CSN_PIN;
    spi_transfer(FLUSH_RX);
    NRF_CSN_PORT->BSRR = NRF_CSN_PIN;
}

// init module
void nrf_init(uint8_t mode){
    nrf_gpio_init();
    spi_init();
    
    // power down and wait
    nrf_write_reg(CONFIG, 0);
    delay_ms(5);
    
    // basic setup
    nrf_write_reg(EN_AA, ENAA_P0);       
    nrf_write_reg(EN_RXADDR, ERX_P0);    
    nrf_write_reg(SETUP_AW, AW_5BYTES);  
    nrf_write_reg(SETUP_RETR, (ARD_250US<<4) | ARC_15RET); 
    nrf_write_reg(RF_CH, RF_CH_10);      
    nrf_write_reg(RF_SETUP, RF_DR_2MBPS | RF_PWR_0DBM); 
    
    // enable CRC (2 bytes)
    uint8_t config = EN_CRC | CRCO;
    nrf_write_reg(CONFIG, config);
    
    // default addresses
    uint8_t tx_addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t rx_addr[5] = {0xD7, 0xD7, 0xD7, 0xD7, 0xD7};
    
    nrf_set_tx_addr(tx_addr, 5);
    nrf_set_rx_addr(0, rx_addr, 5);
    
    // payload length
    nrf_write_reg(RX_PW_P0, 32);
    
    // enable features
    nrf_write_reg(FEATURE, EN_DPL | EN_ACK_PAY);
    nrf_write_reg(DYNPD, (1<<DPL_P0));
    
    // clear FIFOs and status - CORRIGIDO
    nrf_flush_tx();
    nrf_flush_rx();
    nrf_write_reg(STATUS, RX_DR | TX_DS | MAX_RT);
    
    // setup mode
    if (mode == NRF_MODE_TX) {
        nrf_write_reg(CONFIG, config | PWR_UP);
    } else {
        nrf_write_reg(CONFIG, config | PWR_UP | PRIM_RX);
        NRF_CE_PORT->BSRR = NRF_CE_PIN; // start receive
    }
    delay_ms(5);
}

// data transfer
uint8_t nrf_transmit(uint8_t *data, uint8_t len){
    // switch to TX mode
    uint8_t config = nrf_read_reg(CONFIG);
    if (config & PRIM_RX) {
        NRF_CE_PORT->BRR = NRF_CE_PIN;
        nrf_write_reg(CONFIG, config & ~PRIM_RX);
        delay_us(130);
    }
    
    // clear FIFO TX - CORRIGIDO
    nrf_flush_tx();
    
    // write payload
    NRF_CSN_PORT->BRR = NRF_CSN_PIN;
    spi_transfer(W_TX_PAYLOAD);
    for (uint8_t i = 0; i < len; i++) {
        spi_transfer(data[i]);
    }
    NRF_CSN_PORT->BSRR = NRF_CSN_PIN;
    
    // start transmission
    NRF_CE_PORT->BSRR = NRF_CE_PIN;
    delay_us(15);
    NRF_CE_PORT->BRR = NRF_CE_PIN;
    
    // wait for completion
    uint8_t status;
    uint32_t timeout = 100000;
    while (timeout--) {
        status = nrf_read_reg(STATUS);
        if (status & (TX_DS | MAX_RT)) break;
    }
    
    // clear interrupts
    nrf_write_reg(STATUS, TX_DS | MAX_RT);
    
    return (status & TX_DS) ? 1 : 0;
}

// data reception
uint8_t nrf_receive(uint8_t *data, uint8_t len){
    uint8_t status = nrf_read_reg(STATUS);
    
    if (status & RX_DR) {
        NRF_CSN_PORT->BRR = NRF_CSN_PIN;
        spi_transfer(R_RX_PAYLOAD);
        for (uint8_t i = 0; i < len; i++) {
            data[i] = spi_transfer(0xFF);
        }
        NRF_CSN_PORT->BSRR = NRF_CSN_PIN;
        
        nrf_write_reg(STATUS, RX_DR);
        return 1;
    }
    return 0;
}

// setup transmission address
void nrf_set_tx_addr(uint8_t *addr, uint8_t len){
    nrf_write_buf(TX_ADDR, addr, len);
}

// setup reception address - CORRIGIDO
void nrf_set_rx_addr(uint8_t pipe, uint8_t *addr, uint8_t len){
    if (pipe == 0) {
        nrf_write_buf(RX_ADDR_P0, addr, len);
    } else if (pipe == 1) {
        nrf_write_buf(RX_ADDR_P1, addr, len);
    } else if (pipe >= 2 && pipe <= 5) {
        // pipes 2-5 use only LSB
        nrf_write_reg(RX_ADDR_P2 + (pipe-2), addr[0]);
    }
}

// power control functions
void nrf_power_up(void){
    uint8_t config = nrf_read_reg(CONFIG);
    nrf_write_reg(CONFIG, config | PWR_UP);
    delay_ms(5);
}

void nrf_power_down(void){
    uint8_t config = nrf_read_reg(CONFIG);
    nrf_write_reg(CONFIG, config & ~PWR_UP);
}

// status functions
uint8_t nrf_data_available(void){
    uint8_t status = nrf_read_reg(STATUS);
    return (status & RX_DR);
}

uint8_t nrf_is_sending(void){
    uint8_t status = nrf_read_reg(STATUS);
    
    if (status & TX_DS) {
        return 0; // transmission finished
    }
    
    return !(status & (TX_DS | MAX_RT));
}

uint8_t nrf_get_status(void){
    return nrf_read_reg(STATUS);
}