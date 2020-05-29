#include<stm32f30x.h>
#include<stm32f30x_gpio.h>
#include<stm32f30x_rcc.h>
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

uint8_t txbuf[4], rxbuf[4];
uint16_t txbuf16[4], rxbuf16[4];

void main() {

    int i, j;
    csInit(); // Initialize chip select PC03
    spiInit(SPI2);

    for (i=0;i<8;i++) {
        for (j=0;j<4;j++) 
            txbuf16[j] = i*4 +j + (i<<8);
        GPIO_WriteBit(GPIOC, GPIO_Pin_3, 0);
        spiReadWrite16(SPI2, rxbuf16, txbuf16, 4, SPI_SLOW);
        GPIO_WriteBit(GPIOC, GPIO_Pin_3, 1);
        for (j=0; j<4; j++) 
            if (rxbuf16[j] != txbuf16[j])
                assert_failed(__FILE__, __LINE__);
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed (uint8_t* file, uint32_t line)
{
    // Infinite loop
    // Use GDB to find out why we're here
    while (1);
}
#endif