#pragma once

//Standard libraries
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include <LiquidCrystal_I2C.h>
#include <esp_task_wdt.h>

//My files 
#include "pinout.h"
#include "LCD.h"
#include "support_functions.h"
#include "sensors.h"
#include "PWM_on_c0.h"


#define SERIAL_DEBUG_PUMP 1
#define SERIAL_DEBUG_GPIO_PUMP 1
#define SERIAL_DEBUG_PID 0
#define SERIAL_DEBUG_ADC 0
#define SERIAL_DEBUG_FLOW 0


