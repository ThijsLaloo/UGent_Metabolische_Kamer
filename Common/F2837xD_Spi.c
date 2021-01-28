//###########################################################################
//
// FILE:   F2837xD_Spi.c
//
// TITLE:  F2837xD SPI Initialization & Support Functions.
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
#include "F2837xD_device.h"
#include "F2837xD_Examples.h"

//
// Calculate BRR: 7-bit baud rate register value
// SPI CLK freq = 500 kHz
// LSPCLK freq  = CPU freq / 4  (by default)
// BRR          = (LSPCLK freq / SPI CLK freq) - 1
//
#if CPU_FRQ_200MHZ
// #define SPI_BRR        ((200E6 / 4) / 500E3) - 1
#define SPI_BRR        ((200E6 / 4) / 100E3) - 1   //adjusted for 100kHz
#endif

#if CPU_FRQ_150MHZ
#define SPI_BRR        ((150E6 / 4) / 100E3) - 1   //adjusted for 100kHz
#endif

#if CPU_FRQ_120MHZ
#define SPI_BRR        ((120E6 / 4) / 100E3) - 1   //adjusted for 100kHz
#endif

//
// InitSPI - This function initializes the SPI to a known state
//
void InitSpi(void)
{
    // Initialize SPI-A

    // Set reset low before configuration changes
    // Clock polarity (0 == rising, 1 == falling)
    // 16-bit character
    // Enable loop-back
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpiaRegs.SPICCR.bit.SPICHAR = (8 - 1);
    SpiaRegs.SPICCR.bit.SPILBK = 0;

    // Enable master (0 == slave, 1 == master)
    // Enable transmission (Talk)
    // Clock phase (0 == normal, 1 == delayed)
    // SPI interrupts are disabled
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPICTL.bit.CLK_PHASE = 1;
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;

    // Set the baud rate
    SpiaRegs.SPIBRR.bit.SPI_BIT_RATE = SPI_BRR;

    // Set FREE bit
    // Halting on a breakpoint will not halt the SPI
    SpiaRegs.SPIPRI.bit.FREE = 1;

    // Some information gathered from https://e2e.ti.com/support/microcontrollers/c2000/f/171/t/887718
    //SpiaRegs.SPIFFTX.bit.SPIFFENA = 1;
    //SpiaRegs.SPICTL.bit.SPIINTENA = 1;

    // Release the SPI from reset
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;

}

//
// InitSpiGpio - This function initializes GPIO pins to function as SPI pins.
//               Each GPIO pin can be configured as a GPIO pin or up to 3
//               different peripheral functional pins. By default all pins come
//               up as GPIO inputs after reset.
//
//               Caution:
//               For each SPI peripheral
//               Only one GPIO pin should be enabled for SPISOMO operation.
//               Only one GPIO pin should be enabled for SPISOMI operation.
//               Only one GPIO pin should be enabled for SPICLK  operation.
//               Only one GPIO pin should be enabled for SPISTE  operation.
//               Comment out other unwanted lines.
//
void InitSpiGpio()
{
    InitSpiaGpio();
}

//
// InitSpiaGpio - Initialize SPIA GPIOs
//
void InitSpiaGpio()
{
    EALLOW;

    //
    // Enable internal pull-up for the selected pins
    //
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;  // Enable pull-up on GPIO16 (SPISIMOA)
//  GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;   // Enable pull-up on GPIO5 (SPISIMOA)
    //GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;  // Enable pull-up on GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPBPUD.bit.GPIO59 = 0;  // Enable pull-up on GPIO17 (SPISOMIA)
//  GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;   // Enable pull-up on GPIO3 (SPISOMIA)
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;  // Enable pull-up on GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;  // Enable pull-up on GPIO19 (SPISTEA)

    //
    // Set qualification for selected pins to asynch only
    //
    // This will select asynch (no qualification) for the selected pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 3;  // Asynch input GPIO5 (SPISIMOA)
    //GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3; // Asynch input GPIO17 (SPISOMIA)
//  GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 3;  // Asynch input GPIO3 (SPISOMIA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

    //
    //Configure SPI-A pins using GPIO regs
    //
    // This specifies which of the possible GPIO pins will be SPI functional
    // pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1; // Configure GPIO16 as SPISIMOA
//  GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 2;  // Configure GPIO5 as SPISIMOA
    //GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1; // Configure GPIO17 as SPISOMIA
//  GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 2;  // Configure GPIO3 as SPISOMIA
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1; // Configure GPIO18 as SPICLKA
    //GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1; // Configure GPIO19 as SPISTEA


    //Register settings for use with pins 58,59,60,61

    /*GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0; // Enable pull-up on GPIO16 (SPISIMOA)
   GpioCtrlRegs.GPBPUD.bit.GPIO59 = 0; // Enable pull-up on GPIO17 (SPISOMIA)
   GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0; // Enable pull-up on GPIO18 (SPICLKA)
   GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0; // Enable pull-up on GPIO19 (SPISTEA)

   GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 3; // Asynch input GPIO16 (SPISIMOA)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3; // Asynch input GPIO17 (SPISOMIA)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3; // Asynch input GPIO18 (SPICLKA)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 3; // Asynch input GPIO19 (SPISTEA)*/

   //GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 3; // Configure GPIO16 as SPISIMOA
   GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 3; // Configure GPIO17 as SPISOMIA
   //GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 3; // Configure GPIO18 as SPICLKA
   //GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 3; // Configure GPIO19 as SPISTEA

   //GpioCtrlRegs.GPBGMUX2.bit.GPIO58 = 3; // Configure GPIO16 as SPISIMOA
   GpioCtrlRegs.GPBGMUX2.bit.GPIO59 = 3; // Configure GPIO17 as SPISOMIA
   //GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 3; // Configure GPIO18 as SPICLKA
   //GpioCtrlRegs.GPBGMUX2.bit.GPIO61 = 3; // Configure GPIO19 as SPISTEA





    EDIS;
}

//
// End of file
//
