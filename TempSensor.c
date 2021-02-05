#include "TempSensor.h"

#define ADC_CONVERSIONCONSTANT  1.95 // �V/LSB
#define ADC_INTTEMPCONVCONSTANT 0.03125 // �C/LSB
#define TC_LUTENTRIES           9


float TempSensor_CalculateChipTemp(uint16_t uiChipTempValueRaw);
int TempSensor_KTcVoltToTemp(long fVoltageMicroV);
long TempSensor_KTcTempToVolt(int iTempCx100);


long malKTcVoltLUT[] = {-1527, -1156, -778, -392,        // in �V, -40,-30,-20,-10�C
                      0, 397, 798, 1203, 1612,          // in �V, 0,10,20,30,40�C
                      };

int maiKTcTempLUT[] = {-4000, -3000, -2000, -1000,
                      0, 1000, 2000, 3000, 4000,
                      };

// All temperatueres in �C x 100 (eg: 1.23�C --> 123) (int format)
// All voltages in �V (long format)

int TempSensor_CalculateTempCx100(uint16_t uiTcValueRaw, uint16_t uiChipTempValueRaw)
{
    int iReturnValue = 0;

    int iChipTemp = TempSensor_CalculateChipTemp(uiChipTempValueRaw);

    return iReturnValue;
}

int TempSensor_CalculateChipTemp(uint16_t uiChipTempValueRaw)
{
    float fTemp = 0.0;

    // Raw value is 14 bit Left justified
    uiChipTempValueRaw = uiChipTempValueRaw >> 2;

    if((uiChipTempValueRaw & 0x2000) != 0)
    {
        // value is negative, stretch sign bits
        uiChipTempValueRaw |= 0xc000;
    }

    fTemp = (float)uiChipTempValueRaw * ADC_INTTEMPCONVCONSTANT;
    return (int)(fTemp * 100);
}

int TempSensor_KTcVoltToTemp(long lVoltageMicroV)
{
    int iLUTIdxCnt = 0;

    for(iLUTIdxCnt = 0; iLUTIdxCnt < TC_LUTENTRIES; iLUTIdxCnt++)
    {
        if(lVoltageMicroV > malKTcVoltLUT[iLUTIdxCnt])1
        {
            break;
        }
    }

}

long TempSensor_KTcTempToVolt(int iTempCx100)
{
    int iLUTIdxCnt = 0;

    for(iLUTIdxCnt = 0; iLUTIdxCnt < TC_LUTENTRIES; iLUTIdxCnt++)
    {
        if(iTempCx100 > maiKTcTempLUT[iLUTIdxCnt])
        {
            break;
        }
    }
}
