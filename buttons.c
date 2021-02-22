#include "F28x_Project.h"
#include "buttons.h"

#define BUTTONS_UPPIN       25
#define BUTTONS_DOWNPIN     26

int buttons_init()
{
    // init interrupts
    // UP button
    GPIO_SetupPinMux(BUTTONS_UPPIN, GPIO_MUX_CPU1, 0x00);
    GPIO_SetupPinOptions(BUTTONS_UPPIN, GPIO_INPUT, GPIO_PULLUP);
    GPIO_SetupXINT1Gpio(BUTTONS_UPPIN); // Make GPIO the input source for XINT1
    // Down button
    GPIO_SetupPinMux(BUTTONS_DOWNPIN, GPIO_MUX_CPU1, 0x00);
    GPIO_SetupPinOptions(BUTTONS_DOWNPIN, GPIO_INPUT, GPIO_PULLUP);
    GPIO_SetupXINT2Gpio(BUTTONS_DOWNPIN); // Make GPIO the input source for XINT2
    return 0;
}


int buttons_checkPress()
{
    return 0;
}
