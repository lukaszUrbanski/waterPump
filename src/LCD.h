#pragma once

#include "main.h"

void LCDInit(void);
void LCDPrintInitScreen(void);
void LCDPrintPressureScreen(bool pump_state, float set_pressure,  float measure_pressure);
void LCDUpdatePressureScreen(bool pump_state, float set_pressure,  float measure_pressure);
void LCDPrintPumpScreen(bool pump_state, float pump_flow, uint8_t pump_power);
void LCDUpdatePumpScreen(bool pump_state, float pump_flow, uint8_t pump_power);
void LCDPrintErrorScreen(uint8_t number_of_error);