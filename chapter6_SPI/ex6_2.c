#include<stm32f30x.h>
#include<stm32f30x_rcc.h>
#include<stm32f30x_gpio.h>
#include<stm32f30x_usart.h>
#include<stm32f30x_spi.h>


enum spiSpeed {SPI_SLOW, SPI_MEDIUM, SPI_FAST};

void spiInit (SPI_TypeDef* SPIx);

int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf, 
const uint8_t* tbuf, int cnt, enum spiSpeed speed);

int spiReadWrite16(SPI_TypeDef* SPIx, uint16_t *rbuf, 
const uint16_t* tbuf, int cnt, enum spiSpeed speed);

static const uint16_t speeds[] = {
    [SPI_SLOW] = SPI_BaudRatePrescaler_64,
    [SPI_MEDIUM] = SPI_BaudRatePrescaler_8,
    [SPI_FAST] = SPI_BaudRatePrescaler_2
};

void spiInit (SPI_TypeDef *SPIx) {

    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(& GPIO_InitStructure);
    SPI_StructInit(& SPI_InitStructure);

    if (SPIx == SPI2) {
        //Enable Clocks , configure pins
    } else {
        return;
    }

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA  = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = speeds[SPI_SLOW];
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPIx, &SPI_InitStructure);

    SPI_Cmd(SPIx, ENABLE);
}

int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf, 
const uint8_t* tbuf, int cnt, enum spiSpeed speed) {

    int i;

    SPIx->CR1 = (SPIx->CR1 & ~SPI_BaudRatePrescaler_256) | speeds[speed];

    for (i=0;i<cnt;i++) {

        if (tbuf) {
            SPI_I2S_SendData(SPIx, *tbuf++);
        } else {
            SPI_I2S_SendData(SPIx, 0xff) ;
        }
        while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
        if (rbuf) {
            *rbuf++ = SPI_I2S_ReceiveData(SPIx);
        } else {
            SPI_I2S_ReceiveData(SPIx);
        }       
    }
    return i;
}

/*
EEPROM_PORT - PORT OF GPIO CONNECTED TO EEPROM
EEPROM_CS - CHIP SELECT PIN OF THE GPIO 
EEPROMSPEED - SPEED OF EEPROM TRANSMISSION
EEPORM_SPI - SPIx SELECTED FOR SPI PROTOCOL EXECUTION
*/

EEPROM_PORT = GPIOB; // Pin 13,14,15
EEPROM_CS = GPIO_Pin_10; // Port C for this 
EEPROM_SPEED = SPI_SLOW; 
EEPROM_SPI = SPI2;

enum eepromCMD { cmdREAD = 0x03 , cmdWRITE = 0x02,
                 cmdWREN = 0x06 , cmdWRDI = 0x04,
                 cmdRDSR = 0x05 , cmdWRSR = 0x01};


void eepromInit();
void eepromWriteEnable() {
    uint8_t cmd = cmdWREN;

    while (WIP(eepromReadStatus()));

    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
    spiReadWrite(EEPROM_SPI, 0, &cmd, 1, EEPROM_SPEED);
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
    
}
void eepromWriteDisable();
uint8_t eepromReadStatus() {
    uint8_t cmd[] = {cmdRDSR, 0xff};
    uint8_t res[2];
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 0);
    spiReadWrite(EEPROM_SPI, res, cmd, 2, EEPROM_SPEED);
    GPIO_WriteBit(EEPROM_PORT, EEPROM_CS, 1);
    return res[1];

}
void eepromWriteStatus(uint8_t status);
int eepromWrite(uint8_t *buf, uint8_t cnt, uint16_t offset);
int eepromRead(uint8_t *buf, uint8_t cnt, uint16_t offset);

