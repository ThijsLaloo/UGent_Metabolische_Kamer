#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include <stdint.h>

int TempSensor_CalculateTempCx10(uint16_t uiTcValueRaw, uint16_t uiChipTempValueRaw);

#endif
