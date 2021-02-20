#include "F28x_Project.h"
#include "7segment.h"

#define SEVENSEG_CLK_GPIO1      4
#define SEVENSEG_DIO_GPIO1      3
#define SEVENSEG_CLK_GPIO2      6
#define SEVENSEG_DIO_GPIO2      5
#define SEVENSEG_CLK_PERIOD     20 // µs
#define SEVENSEG_NEG_IDX        16 // in sevenSeg_numberArray
#define SEVENSEG_BLANK_IDX      17 // in sevenSeg_numberArray

int sevenSeg_writeByte(char bData, bool biSendStop, int iDispNumber);
int sevenSeg_writeStart(int iDispNumber);
int sevenSeg_writeStop(int iDispNumber);
int sevenSeg_readAck(int iDispNumber);

int sevenSeg_numberArray[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0..7
                               0x7F, 0x6f, 0x77, 0x7c, 0x58, 0x5e, 0x79, 0x71, // 8..F
                               0x40, 0x00 }; // -, blank

//Public functions

int sevenSeg_init()
{
    EALLOW;

    // Pin-setup for the first 7-segment display

    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO4 = 0;

    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = 0;

    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;  //disable pull-up
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;  //disable pull-up

    GpioCtrlRegs.GPAODR.bit.GPIO4 = 1;  //enable open-drain logic
    GpioCtrlRegs.GPAODR.bit.GPIO3 = 1;  //enable open-drain logic

    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  //set GPIO as output
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  //set GPIO as output

    GpioCtrlRegs.GPAQSEL1.bit.GPIO4 = 3;  //Asynch input GPIO36
    GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;  //Asynch input GPIO37

    // Pin-setup for the second 7-segment display

    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO6 = 0;

    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO5 = 0;

    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;  //disable pull-up
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;  //disable pull-up

    GpioCtrlRegs.GPAODR.bit.GPIO6 = 1;  //enable open-drain logic
    GpioCtrlRegs.GPAODR.bit.GPIO5 = 1;  //enable open-drain logic

    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;  //set GPIO as output
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;  //set GPIO as output

    GpioCtrlRegs.GPAQSEL1.bit.GPIO6 = 3;  //Asynch input GPIOxx
    GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 3;  //Asynch input GPIOxx

    EDIS;

    GPIO_WritePin(SEVENSEG_CLK_GPIO1, 1);
    GPIO_WritePin(SEVENSEG_DIO_GPIO1, 1);
    GPIO_WritePin(SEVENSEG_CLK_GPIO2, 1);
    GPIO_WritePin(SEVENSEG_DIO_GPIO2, 1);

    sevenSeg_clear(1);
    sevenSeg_clear(2);

    return 0;
}

int sevenSeg_clear(int iDispNumber)
{
    int i;
    sevenSeg_writeByte(0x40, true, iDispNumber);
    sevenSeg_writeByte(0xC0, false, iDispNumber);
    for (i = 0; i < 6; i++)
    {
        sevenSeg_writeByte(0x00, false, iDispNumber);
    }
    sevenSeg_writeByte(0x00, true, iDispNumber);
    sevenSeg_writeByte(0x8B, true, iDispNumber);
    return 0;
}

int sevenSeg_writeTemp(int iTemp, int iDispNumber)
{
    char baSegments[4] = { 0 };
    char bTemp = 0;

    if (iTemp < 0)
    {
        baSegments[0] = sevenSeg_numberArray[SEVENSEG_NEG_IDX];
    }
    else
    {
        baSegments[0] = sevenSeg_numberArray[SEVENSEG_BLANK_IDX];
    }

    int thents = iTemp % 10;
    int ones = (iTemp % 100) / 10;
    int tens = (iTemp % 1000) / 100;

    baSegments[1] = sevenSeg_numberArray[tens];
    baSegments[2] = sevenSeg_numberArray[ones];
    baSegments[3] = sevenSeg_numberArray[thents];

    sevenSeg_writeByte(0x40, true, iDispNumber);
    sevenSeg_writeByte(0xC0, false, iDispNumber);
    sevenSeg_writeByte(baSegments[0], false, iDispNumber);
    sevenSeg_writeByte(baSegments[1], false, iDispNumber);
    sevenSeg_writeByte(baSegments[2], false, iDispNumber);
    sevenSeg_writeByte(baSegments[3], true, iDispNumber);
    sevenSeg_writeByte(0x8B, true, iDispNumber);

    return 0;
}

int sevenSeg_writeDisco(int iDispNumber)
{
    int i, j;
    for (i = 0; i <= 0x10; i++)
    {

        sevenSeg_writeByte(0x40, true, iDispNumber);
        sevenSeg_writeByte(0xC0, false, iDispNumber);
        for (j = 0; j <= 5; j++)
        {
            sevenSeg_writeByte(sevenSeg_numberArray[i], false, iDispNumber);
        }
        sevenSeg_writeByte(sevenSeg_numberArray[i], true, iDispNumber);
        sevenSeg_writeByte(0x8B, true, iDispNumber);
        DELAY_US(1E5);
    }
    return 0;
}

// private functions

int sevenSeg_writeStart(int iDispNumber)
{
    if (iDispNumber == 1)
    {
        GPIO_WritePin(SEVENSEG_DIO_GPIO1, 0);
    }
    else if (iDispNumber == 2)
    {
        GPIO_WritePin(SEVENSEG_DIO_GPIO2, 0);
    }
    else
    {

    }
    DELAY_US(SEVENSEG_CLK_PERIOD/4);

    return 0;
}

int sevenSeg_writeStop(int iDispNumber)
{
    if (iDispNumber == 1)
    {
        GPIO_WritePin(SEVENSEG_DIO_GPIO1, 0);
        GPIO_WritePin(SEVENSEG_CLK_GPIO1, 0);
        DELAY_US(SEVENSEG_CLK_PERIOD/2);

        GPIO_WritePin(SEVENSEG_CLK_GPIO1, 1);
        DELAY_US(SEVENSEG_CLK_PERIOD/2);

        GPIO_WritePin(SEVENSEG_DIO_GPIO1, 1);
        DELAY_US(SEVENSEG_CLK_PERIOD/2);
    }
    else if (iDispNumber == 2)
    {
        GPIO_WritePin(SEVENSEG_DIO_GPIO2, 0);
        GPIO_WritePin(SEVENSEG_CLK_GPIO2, 0);
        DELAY_US(SEVENSEG_CLK_PERIOD/2);

        GPIO_WritePin(SEVENSEG_CLK_GPIO2, 1);
        DELAY_US(SEVENSEG_CLK_PERIOD/2);

        GPIO_WritePin(SEVENSEG_DIO_GPIO2, 1);
        DELAY_US(SEVENSEG_CLK_PERIOD/2);
    }
    else
    {

    }

    return 0;
}

int sevenSeg_readAck()
{
    if (iDispNumber == 1)
    {
        int iRetValue = 0;
        GPIO_WritePin(SEVENSEG_CLK_GPIO1, 0);
        DELAY_US(SEVENSEG_CLK_PERIOD/4);

        iRetValue = GPIO_ReadPin(SEVENSEG_DIO_GPIO1);

        GPIO_WritePin(SEVENSEG_CLK_GPIO1, 1);
        DELAY_US(SEVENSEG_CLK_PERIOD/2);
    }
    else if (iDispNumber == 2)
    {
        int iRetValue = 0;
        GPIO_WritePin(SEVENSEG_CLK_GPIO2, 0);
        DELAY_US(SEVENSEG_CLK_PERIOD/4);

        iRetValue = GPIO_ReadPin(SEVENSEG_DIO_GPIO2);

        GPIO_WritePin(SEVENSEG_CLK_GPIO2, 1);
        DELAY_US(SEVENSEG_CLK_PERIOD/2);
    }
    else
    {

    }

    return iRetValue;
}

int sevenSeg_writeByte(char bData, bool biSendStop, int iDispNumber)
{

    sevenSeg_writeStart(iDispNumber);

    int iBitCnt = 0;

    if (iDispNumber == 1)
    {
        for (iBitCnt = 0; iBitCnt < 8; ++iBitCnt)
        {
            GPIO_WritePin(SEVENSEG_CLK_GPIO1, 0);
            DELAY_US(SEVENSEG_CLK_PERIOD/4);

            // (bData >> iBitCnt) & 0x01 =  telkens eerste bit afzonderen

            GPIO_WritePin(SEVENSEG_DIO_GPIO1, (bData >> iBitCnt) & 0x01);
            DELAY_US(SEVENSEG_CLK_PERIOD/4);

            GPIO_WritePin(SEVENSEG_CLK_GPIO1, 1);
            DELAY_US(SEVENSEG_CLK_PERIOD/2);
        }
    }
    else if (iDispNumber == 2)
    {
        for (iBitCnt = 0; iBitCnt < 8; ++iBitCnt)
        {
            GPIO_WritePin(SEVENSEG_CLK_GPIO2, 0);
            DELAY_US(SEVENSEG_CLK_PERIOD/4);

            // (bData >> iBitCnt) & 0x01 =  telkens eerste bit afzonderen

            GPIO_WritePin(SEVENSEG_DIO_GPIO2, (bData >> iBitCnt) & 0x01);
            DELAY_US(SEVENSEG_CLK_PERIOD/4);

            GPIO_WritePin(SEVENSEG_CLK_GPIO2, 1);
            DELAY_US(SEVENSEG_CLK_PERIOD/2);
        }
    }
    else
    {

    }

    sevenSeg_readAck(iDispNumber);

    if (biSendStop)
    {
        sevenSeg_writeStop(iDispNumber);
    }
}
