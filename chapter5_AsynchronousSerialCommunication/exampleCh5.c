#include<stm32f30x.h>
#include<stm32f30x_usart.h>
#include<stm32f30x_gpio.h>
#include<stm32f30x_rcc.h>

int putchar (int c) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART1->TDR = (c & 0xff);
    return 0;
}

int getchar (void) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return USART1->RDR & 0xff;
}

int main(void) {

    // USART1 in APB2, USART2 and USART3 in APB1 Peripherals
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    
    // CONFIGURE GPIO
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);

    // Initialize USART1_Tx    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Initialize USART1_Rx    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // CONFIGURE USART
    USART_InitTypeDef USART_InitStructure;
    USART_StructInit(& USART_InitStructure);

    // Modify USART_InitStructure for non default values . eg
    // USART_InitStructure.USART_BaudRate = 38400;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);

    // Enable USART
    USART_Cmd(USART1, ENABLE);

    const char hello[] = "Hello World\0";
    while (1) {
        int i = 0;
        while (hello[i] != '\0') {
            putchar(hello[i]);
            i++;
        }
    }
    return 0;

}