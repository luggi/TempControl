#include <stm32f4xx.h>
#include "gpio.h"
/* This funcion shows how to initialize
 * the GPIO pins on GPIOD and how to configure
 * them as inputs and outputs
 */
void init_GPIO(void)
{

    /* This TypeDef is a structure defined in the
     * ST's library and it contains all the properties
     * the corresponding peripheral has, such as output mode,
     * pullup / pulldown resistors etc.
     *
     * These structures are defined for every peripheral so
     * every peripheral has it's own TypeDef. The good news is
     * they always work the same so once you've got a hang
     * of it you can initialize any peripheral.
     *
     * The properties of the periperals can be found in the corresponding
     * header file e.g. stm32f4xx_gpio.h and the source file stm32f4xx_gpio.c
     */
    GPIO_InitTypeDef GPIO_InitStruct;

    /* This enables the peripheral clock to
     * the GPIOA IO module
     */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    /* Here the GPIOB module is initialized.
     * We want to use PB15 as an input because
     * the USER button on the board is connected
     * between this pin and VCC.
     */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;       // we want to configure PB13 SCK
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;  // we want it to be an input
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;      // this sets the GPIO modules clock speed
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // this sets the pin type to push / pull (as opposed to open drain)
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       // this enables the pulldown resistor --> we want to detect a high level
    GPIO_Init(GPIOB, &GPIO_InitStruct); // this passes the configuration to the Init function which takes care of the low level stuff


    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;       // we want to configure PE13 PE14
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;  // we want it to be an input
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;      // this sets the GPIO modules clock speed
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // this sets the pin type to push / pull (as opposed to open drain)
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStruct); // this passes the configuration to the Init function which takes care of the low level stuff

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;     // MISO-PIN
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;   // we want it to be an input
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;      // this sets the GPIO modules clock speed
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // this sets the pin type to push / pull (as opposed to open drain)
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       // 
    GPIO_Init(GPIOB, &GPIO_InitStruct); // this passes the configuration to the Init function which takes care of the low level stuff
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_1;     // PC1, PC3 fet-off and REM-SB
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;   // we want it to be an input
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;      // this sets the GPIO modules clock speed
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // this sets the pin type to push / pull (as opposed to open drain)
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;       // 
    GPIO_Init(GPIOC, &GPIO_InitStruct); // this passes the configuration to the Init function which takes care of the low level stuff

}
