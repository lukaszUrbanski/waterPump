#pragma once 

#include "main.h"

#define NUMBER_OF_POSSIBLE_PRESSURE_TO_SET 8

void MainProgtramLoop(void);

typedef enum 
{
    MPL_IDLE,
    MPL_INIT,
    MPL_SENSOR_MEASURE,
    MPL_LCD_UPDATE,
    MPL_BUTTON_CONTROL,
    MPL_PUMP_CONTROL,
    ERROR_HANDLER
} main_program_loop_state_t;

typedef enum
{
    LCD_INIT,
    LCD_PRESURE,
    LCD_PUMP,
    LCD_ERROR
} lcd_state_t;

typedef enum 
{
    ERROR_PRESURE_SENSOR,
    ERROR_FLOW_SENSOR,
    ERROR_PUMP,
    ERROR_UNKNOWN
} type_of_error_t;

typedef enum
{
    OWN_STATUS_OK,       
    OWN_STATUS_ERROR,
    OWN_STATUS_BUSY,
    OWN_STATUS_TIMEOUT
} 