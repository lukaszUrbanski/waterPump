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
#include "main_loop.h"
#include "sensors.h"
#include "support_functions.h"
#include "pump_control.h"
#include "my_tim_set.h"
#include "PWM_on_c0.h"

#define WATCHDOG_IMPLEMENTATION_PUMP 1
#define WATCHDOG_TIMEOUT_IN_S 10

#define SERIAL_DEBUG_PUMP 1
#define SERIAL_DEBUG_GPIO_PUMP 0
#define SERIAL_DEBUG_PID 0
#define SERIAL_DEBUG_ADC 0
#define SERIAL_DEBUG_FLOW 0




