#include "F28x_Project.h"
#include "7segment.h"

#define SEVENSEG_CLK_GPIO       4
#define SEVENSEG_DIO_GPIO       3
#define SEVENSEG_CLK_PERIOD     600
#define SEVENSEG_NEG_IDX        16 // in sevenSeg_numberArray
#define SEVENSEG_BLANK_IDX      17 // in sevenSeg_numberArray

int sevenSeg_writeByte(char bData, bool biSendStop);
int sevenSeg_writeStart();
int sevenSeg_writeStop();
int sevenSeg_readAck();

int sevenSeg_numberArray[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0..7
                               0x7F, 0x6f, 0x77, 0x7c, 0x58, 0x5e, 0x79, 0x71, // 8..F
                               0x40, 0x00 }; // -, blank

//Public functions

int sevenSeg_init()
{
    EALLOW;

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

    EDIS;

    GPIO_WritePin(SEVENSEG_CLK_GPIO, 1);
    GPIO_WritePin(SEVENSEG_DIO_GPIO, 1);

    sevenSeg_clear();

    return 0;
}

int sevenSeg_clear()
{
    int i;
    sevenSeg_writeByte(0x40, true);
    sevenSeg_writeByte(0xC0, false);
    for (i = 0; i < 6; i++)
    {
        sevenSeg_writeByte(0x00, false);
    }
    sevenSeg_writeByte(0x00, true);
    sevenSeg_writeByte(0x8B, true);
    return 0;
}

int sevenSeg_writeTemp(int iTemp)
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

    sevenSeg_writeByte(0x40, true);
    sevenSeg_writeByte(0xC0, false);
    sevenSeg_writeByte(baSegments[0], false);
    sevenSeg_writeByte(baSegments[1], false);
    sevenSeg_writeByte(baSegments[2], false);
    sevenSeg_writeByte(baSegments[3], true);
    sevenSeg_writeByte(0x8B, true);

    return 0;
}

int sevenSeg_writeDisco()
{
    int i, j;
    for (i = 0; i <= 0x10; i++)
    {

        sevenSeg_writeByte(0x40, true);
        sevenSeg_writeByte(0xC0, false);
        for (j = 0; j <= 5; j++)
        {
            sevenSeg_writeByte(sevenSeg_numberArray[i], false);
        }
        sevenSeg_writeByte(sevenSeg_numberArray[i], true);
        sevenSeg_writeByte(0x8B, true);
        DELAY_US(1E5);
    }
    return 0;
}

// private functions

int sevenSeg_writeStart()
{

    GPIO_WritePin(SEVENSEG_DIO_GPIO, 0);
    DELAY_US(SEVENSEG_CLK_PERIOD/4);

    return 0;
}

int sevenSeg_writeStop()
{
    GPIO_WritePin(SEVENSEG_DIO_GPIO, 0);
    GPIO_WritePin(SEVENSEG_CLK_GPIO, 0);
    DELAY_US(SEVENSEG_CLK_PERIOD/2);

    GPIO_WritePin(SEVENSEG_CLK_GPIO, 1);
    DELAY_US(SEVENSEG_CLK_PERIOD/2);

    GPIO_WritePin(SEVENSEG_DIO_GPIO, 1);
    DELAY_US(SEVENSEG_CLK_PERIOD/2);

    return 0;
}

int sevenSeg_readAck()
{
    int iRetValue = 0;
    GPIO_WritePin(SEVENSEG_CLK_GPIO, 0);
    DELAY_US(SEVENSEG_CLK_PERIOD/4);

    iRetValue = GPIO_ReadPin(SEVENSEG_DIO_GPIO);

    GPIO_WritePin(SEVENSEG_CLK_GPIO, 1);
    DELAY_US(SEVENSEG_CLK_PERIOD/2);

    return iRetValue;
}

int sevenSeg_writeByte(char bData, bool biSendStop)
{

    sevenSeg_writeStart();

    int iBitCnt = 0;

    for (iBitCnt = 0; iBitCnt < 8; ++iBitCnt)
    {
        GPIO_WritePin(SEVENSEG_CLK_GPIO, 0);
        DELAY_US(SEVENSEG_CLK_PERIOD/4);

        // (bData >> iBitCnt) & 0x01 =  telkens eerste bit afzonderen

        GPIO_WritePin(SEVENSEG_DIO_GPIO, (bData >> iBitCnt) & 0x01);
        DELAY_US(SEVENSEG_CLK_PERIOD/4);

        GPIO_WritePin(SEVENSEG_CLK_GPIO, 1);
        DELAY_US(SEVENSEG_CLK_PERIOD/2);
    }

    sevenSeg_readAck();

    if (biSendStop)
    {
        sevenSeg_writeStop();
    }
}
