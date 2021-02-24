//###########################################################################
//
// FILE:   Example_2837xDSpi_FFDLB.c
//
// TITLE:  SPI Digital Loop Back program.
//
//! \addtogroup cpu01_example_list
//! <h1>SPI Digital Loop Back (spi_loopback)</h1>
//!
//!  This program uses the internal loop back test mode of the peripheral.
//!  Other then boot mode pin configuration, no other hardware configuration
//!  is required. Interrupts are not used.
//!
//!  A stream of data is sent and then compared to the received stream.
//!  The sent data looks like this: \n
//!  0000 0001 0002 0003 0004 0005 0006 0007 .... FFFE FFFF \n
//!  This pattern is repeated forever.
//!
//!  \b Watch \b Variables \n
//!  - \b sdata - sent data
//!  - \b rdata - received data
//!
//
//###########################################################################
// $TI Release: F2837xD Support Library v210 $
// $Release Date: Tue Nov  1 14:46:15 CDT 2016 $
// $Copyright: Copyright (C) 2013-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

//
// Included Files
//
#include "F28x_Project.h"
#include "stdlib.h"
#include "7segment.h"
#include "TempSensor.h"
#include "ads1120.h"
#include "buttons.h"

//
// Function Prototypes
//
//void delay_loop(void);
void spi_xmit(unsigned char a);
void spi_fifo_init(void);
void spi_init(void);
void error(void);
void scia_echoback_init(void);
void scia_fifo_init(void);
void scia_xmit(int a);
void scia_msg(char *msg);

//
// Globals
//
Uint16 LoopCount;
Uint16 mawAdcMeasurements[2] = { 0 }; // initialize global buffer for ADC measurements
int miSetValueDegCx10 = 15;

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

void main(void)
{
    char *msg;

    //
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the F2837xD_SysCtrl.c file.
    //
    InitSysCtrl();

    //
    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xD_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    //
    InitGpio();

    //
    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xD_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    // Setup only the GP I/O only for SPI-A functionality
    // This function is found in F2837xD_Spi.c
    //
    InitSpiaGpio();

    //
    // For this example, only init the pins for the SCI-A port.
    //  GPIO_SetupPinMux() - Sets the GPxMUX1/2 and GPyMUX1/2 register bits
    //  GPIO_SetupPinOptions() - Sets the direction and configuration of the GPIOS
    // These functions are found in the F2837xD_Gpio.c file.
    //
    //   GPIO_SetupPinMux(28, GPIO_MUX_CPU1, 1);
    //   GPIO_SetupPinOptions(28, GPIO_INPUT, GPIO_PUSHPULL);
    //   GPIO_SetupPinMux(29, GPIO_MUX_CPU1, 1);
    //   GPIO_SetupPinOptions(29, GPIO_OUTPUT, GPIO_ASYNC);

    GPIO_SetupPinMux(43, GPIO_MUX_CPU1, 0x0F);
    GPIO_SetupPinOptions(43, GPIO_INPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(42, GPIO_MUX_CPU1, 0x0F);
    GPIO_SetupPinOptions(42, GPIO_OUTPUT, GPIO_ASYNC);

    //
    // Step 3. Clear all interrupts:
    //
    DINT;

    //
    // Initialize PIE control registers to their default state.
    // The default state is all PIE __interrupts disabled and flags
    // are cleared.
    // This function is found in the F2837xD_PieCtrl.c file.
    //
    InitPieCtrl();

    //
    // Disable CPU __interrupts and clear all CPU __interrupt flags:
    //
    IER = 0x0000;
    IFR = 0x0000;

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the __interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2837xD_DefaultIsr.c.
    // This function is found in F2837xD_PieVect.c.
    //
    InitPieVectTable();

    //
    // Step 4. Initialize the Device Peripherals:
    //
    //spi_fifo_init();     // Initialize the SPI FIFO
    InitSpi();

    //
    // Step 5. User specific code:

    sevenSeg_init();
    sevenSeg_clear(1);
    DELAY_US(1E6);
    sevenSeg_writeDisco(1);
    sevenSeg_clear(2);
    sevenSeg_writeDisco(2);

    scia_fifo_init();       // Initialize the SCI FIFO
    scia_echoback_init();   // Initialize SCI for echoback

    ads1120_init();
    buttons_init();
    
    // Enables PIE to drive a pulse into the CPU
	PieCtrlRegs.PIEACK.all = 0xFFFF;
    
    // Enable global interrupts and realtime debug
    EINT;
    
    for (;;)
    {
        buttons_checkPress();
        DELAY_US(5E5);
        mawAdcMeasurements[0] = ads1120_readThermocouple();
        DELAY_US(1E5);
        mawAdcMeasurements[1] = ads1120_readInternalTempSensor();

        sevenSeg_writeTemp(TempSensor_CalculateTempCx10((int)mawAdcMeasurements[0], mawAdcMeasurements[1]), 1);
    }
}

//
// error - Error function that halts the debugger
//
void error(void)
{
    asm("     ESTOP0");
    // Test failed!! Stop!
    for (;;)
        ;
}

//
// spi_xmit - Transmit value via SPI
//
void spi_xmit(unsigned char a)
{
    SpiaRegs.SPITXBUF = (Uint16) a << 8;
    DELAY_US(1E2);
}

//
// spi_fifo_init - Initialize SPIA FIFO
//
void spi_fifo_init()
{
    //
    // Initialize SPI FIFO registers
    //
    /*
     * SPIFFTX
     * -------
     * SPIRST: 1 - fifo can resume tx/rx
     * SPIFFENA: 1 - fifo enhancements enabled
     * TXFIFO: 1 - release from reset
     *
     */
    SpiaRegs.SPIFFTX.all = 0xE040;
    SpiaRegs.SPIFFRX.all = 0x2044;
    SpiaRegs.SPIFFCT.all = 0x0;

    //
    // Initialize core SPI registers
    //
    InitSpi();
}

//
//  scia_echoback_init - Test 1,SCIA  DLB, 8-bit word, baud rate 0x000F,
//                       default, 1 STOP bit, no parity
//
void scia_echoback_init()
{
    //
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    //

    SciaRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
                                    // No parity,8 char bits,
                                    // async mode, idle-line protocol
    SciaRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
                                    // Disable RX ERR, SLEEP, TXWAKE
    SciaRegs.SCICTL2.all = 0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

    //
    // SCIA at 9600 baud
    // @LSPCLK = 50 MHz (200 MHz SYSCLK) HBAUD = 0x02 and LBAUD = 0x8B.
    // @LSPCLK = 30 MHz (120 MHz SYSCLK) HBAUD = 0x01 and LBAUD = 0x86.
    //
    SciaRegs.SCIHBAUD.all = 0x0002;
    SciaRegs.SCILBAUD.all = 0x008B;

    SciaRegs.SCICTL1.all = 0x0023;  // Relinquish SCI from Reset
}

//
// scia_xmit - Transmit a character from the SCI
//
void scia_xmit(int a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0)
    {
        // wait for empty fifo
    }
    SciaRegs.SCITXBUF.all = a;
}

//
// scia_msg - Transmit message via SCIA
//
void scia_msg(char *msg)
{
    int i;
    i = 0;
    while (msg[i] != '\0')
    {
        scia_xmit(msg[i]);
        i++;
    }
}

//
// scia_fifo_init - Initialize the SCI FIFO
//
void scia_fifo_init()
{
    SciaRegs.SCIFFTX.all = 0xE040;
    SciaRegs.SCIFFRX.all = 0x2044;
    SciaRegs.SCIFFCT.all = 0x0;
}

//
// End of file
//
