#pragma once

#include "main.h"

int PressureSensorInit(void);
int PressureSensorGetValue(float * pressure_value);
int FlowSensorInit(void);
int FlowSensorGetValue(float * flow_value);

typedef float pressure_value_t;
typedef float flow_value_t;