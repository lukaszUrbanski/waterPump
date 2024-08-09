#pragma once

#include "main.h"

int PumpInit(void);
int MakePumpControl(float set_pressure_value, float measured_pressure_value, uint8_t * set_pump_fill);
int PumpOff(void);
int PumpOn(void);

typedef uint8_t pump_fill_t;