/*
Exercise 2 of chapter 4
Write Code of blinking LED using push button
push button - PA0, LED - PC8
*/

#include<stm32f30x.h>
#include<stm32f30x_rcc.h>
#include<stm32f30x_gpio.h>

int main(void) {

    //Structure to configure ports and pins
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable the clocks for the two ports on AHB bus
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_GPIOC, ENABLE);

    // Setting configuration for PA0
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Setting configuration for PC8
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    while (1) {

        // Read button value
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
            GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
            // Write 1 to the pin 8 of port C
            // Arguments - GPIO_TypedDef GPIOx, uint16_t GPIO_Pin, BitAction BitVal
        else 
            GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
    }
    return 0;
}

#ifdef USE_FULL_ASSERT
void assert_failed (uint8_t* file, uint32_t line) {
    while(1);
}
#endif
