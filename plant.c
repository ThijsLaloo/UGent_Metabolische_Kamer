#include "F28x_Project.h"
#include "plant.h"

#define PLANT_REFRIGITATECTRLPIN    GPIO7
#define PLANT_HEATCTRLPIN           GPIO8

int plant_refrigirate(bool biActive)
{
    int iReturnValue = 0;

    if(biActive)
        GpioDataRegs.GPASET.bit.PLANT_REFRIGITATECTRLPIN = 1;
    else
        GpioDataRegs.GPACLEAR.bit.PLANT_REFRIGITATECTRLPIN = 1;

    return iReturnValue;
}
