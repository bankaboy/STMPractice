/*
Exercise 1 of chapter 4
Write Code of blinking LED
*/

#include<stm32f30x.h> // header file for stm32 standard
#include<stm32f30x_rcc.h> // header file for clock control - Reset and clock control 
#include<stm32f30x_gpio.h> // header file for GPIO operations

void Delay(uint32_t nTime); // Delay function to induce delay for nTime miliseconds

int main(void) {

    GPIO_InitTypeDef GPIO_InitStructure; // Structure to intitialize a port and set its pins and configurations
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE); // Enable the clock for the port C on the AHB bus
    
    GPIO_StructInit(&GPIO_InitStructure);  // Initialise the GPIO Structure to initial values
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // Configure the pin to 9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; // Select mode of the pin
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Select configuration of the selected mode
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // Select the speed of data transfer
    GPIO_Init(GPIOC, &GPIO_InitStructure); // Iniitialise the port and pin to configured settings

    if (SysTick_Config(SystemCoreClock/1000)) // Waits till the first second as warmup time 
        while(1);

    while(1) {
        static int ledval = 0; // select value to be sent to pin for LED state
        GPIO_WriteBit(GPIOC, GPIO_Pin_9, (ledval) ? Bit_SET : Bit_RESET);
        /*
        Write the value to led at pin9 of portC (PC9) based on condition
        if value of ledval is 1 , condition will be true and Bit_SET(1) will set the bit for pin 9
        if value of ledval is 0 , condition will be false and Bit_RESET(0) will reset the bit for pin 9
        */
        ledval = 1-ledval; // change the value of the setting of the pin to the other end

        Delay(250); // induce delay for 250ms
    }
}

static __IO uint32_t TimingDelay ; 
/* Variable to measure time delays
__IO is a volatile type declaration - means no optimization (no idea what it means)
probably - the decrement operation in the following code takes a certain time
and optimizing will have unwanted effect on it 
__IO is a volatile type declaration - means no optimization 
Sometimes compilers may change datatype or structure for optimiaztions, 
by setting __IO we force the compiler not to optimize it
*/

void Delay(uint32_t nTime) {
    TimingDelay = nTime;
    while(TimingDelay != 0);
}
/* This function acts like a buffer to the actual delay operation
Does nothing until global variable TimingDelay reaches 0,
by decrementation of SysTick_Handler 
*/

void SysTick_Handler (void) {
    if (TimingDelay != 0x00)
        TimingDelay--;
}
/* No idea how this function is called, called in no other function ,
This is an interrupt handler, it is called when an interrupt is triggered
also no input paramters
it doesnt need one, it uses shared variable timingDelay
Decrements timing variable till it reaches 0 from delay value 
*/


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    while (1);
}
#endif
/* no idea what this is or how it works ------
#ifdef is if defined , checks if the following label is defined
and which include file is this defined in? because its not defined in this script
and also this does nothing. if it did domething , what would it look like?
we can call if there is some error, think of this as an error handling interrupt. 
In the code we have never defined when to call this so its safe to delete this code.
If we set USE_FULL_ASSERT meaning we want to debug errors then it will simply call this function. 
Right now the function simply puts the program to sleep indefinitely
*/