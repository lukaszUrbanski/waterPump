#pragma once

#include "main.h"

int PumpInit(void);
int MakePumpControl(float set_pressure_value, float measured_pressure_value, uint8_t * set_pump_fill, float flow_value, bool * is_pump_on );
bool IsFlow(float flow_value);
int PumpOff(void);
int PumpOn(void);

typedef uint8_t pump_fill_t;