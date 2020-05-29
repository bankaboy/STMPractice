/* Code to interface with SD Card using SPI */

#include<stm32f30x.h>
#include<stm32f30x_spi.h>
#include<stm32f30x_rcc.h>
#include<stm32f30x_gpio.h>
#include "spi.h"

#define GPIO_Pin_CS GPIO_Pin_6
#define GPIO_CS GPIOC
#define RCC_APB2Periph_GPIO_CS RCC_AHBPeriph_GPIOC
#define SD_SPI SPI2

enum spiSpeed Speed = SPI_SLOW;

void Delay(uint32_t);

// Transmit bytes to the card
static void xmit_mmc (const BYTE* buff, UINT bc) {
    // BYTE* buff - data to be sent
    // bc - number of bytes to send
    spiReadWrite(SD_SPI, 0, buff, bc, Speed);
} 

// Receive bytes from the card
static void rcvr_mmc (BYTE* buff, UINT bc) {
    // BYTE* buff - Pointer to read data
    // bc - Number fo bytes to receive
    spiReadWrite(SD_SPI, buff, 0, bc, Speed);
}

// Deselect the card
static void deselect (void) {
    BYTE d;
    GPIO_SetBits(GPIO_CS, GPIO_Pin_CS);
    rcvr_mmc(&d, 1); 
    // Dummy Clock (force D0 hi-z for thhe multiple slave SPI)
}

// Select the card
// 1:OK , 0:Timeout
static int select (void) {
    BYTE d;
    GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS);
    rcvr_mmc(&d, 1); // Dummy clock (force D0 enabled)

    if (wait_ready()) 
        return 1; // OK
    deselect();
    return 0; // Failed
}

INIT_PORT() {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CS, ENABLE);
    // Configure I/O for Flash Chip select
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_CS, &GPIO_InitStructure);
    deselect();
}