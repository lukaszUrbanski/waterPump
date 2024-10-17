#include "main_loop.h"

main_program_loop_state_t main_program_loop_state = MPL_INIT;
lcd_state_t lcd_state = LCD_INIT;

extern volatile int watchdog_reset_timer_counter;

typedef bool is_pump_on_t;
is_pump_on_t is_pump_on = false;
is_pump_on_t last_is_pump_on = false;

pressure_value_t pressure_possible_tab[NUMBER_OF_POSSIBLE_PRESSURE_TO_SET] = {3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
uint8_t pressure_possible_tab_cnt = 0;

pressure_value_t set_pressure_value = pressure_possible_tab[pressure_possible_tab_cnt];
pressure_value_t last_set_pressure_value = pressure_possible_tab[pressure_possible_tab_cnt];

pressure_value_t current_pressure_value = 0.0;
pressure_value_t last_current_pressure_value = 0.0;

pump_fill_t set_pump_fill = 0;
pump_fill_t last_set_pump_fill = 0;

flow_value_t current_flow_value = 0.0;
flow_value_t last_current_flow_value = 0.0;

type_of_error_t type_of_error = ERROR_UNKNOWN;

typedef bool was_screen_changed_t;
was_screen_changed_t was_screen_changed = true;

typedef unsigned long time_from_millis_t;
time_from_millis_t last_screen_update_time = 0UL;
const time_from_millis_t screen_refresh_time = 1000UL;

const time_from_millis_t pid_count_interval = 1000UL;
time_from_millis_t pid_last_count = 0UL;

typedef uint8_t init_refresh_counter_t;
init_refresh_counter_t init_refresh_counter = 5;


void Mpl_IdleActivity(void);
void Mpl_InitActivity(void);
void Mpl_SensorMeasureActivity(void);
void Mpl_LcdUpdateActivity(void);
void Mpl_ButtonActivity(void);
void Mpl_PumpControlActivity(void);
void Mpl_ErrorHandlerActivity(void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MAIN FUNCTION TO HANDLING STAND MACHINE
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainProgramLoop(void)
{
    
    #if(WATCHDOG_IMPLEMENTATION_PUMP)
        if(watchdog_reset_timer_counter > 0)
        {
            esp_task_wdt_reset();
            watchdog_reset_timer_counter--;
        }
    #endif

        switch(main_program_loop_state)
        {
            case MPL_IDLE:
                Mpl_IdleActivity();
                break;

            case MPL_INIT:
                Mpl_InitActivity();
                break;

            case MPL_SENSOR_MEASURE:
                Mpl_SensorMeasureActivity();
                break;

            case MPL_PUMP_CONTROL:
                Mpl_PumpControlActivity();
                break;

            case MPL_LCD_UPDATE:
                Mpl_LcdUpdateActivity();
                break;

            case MPL_BUTTON:
                Mpl_ButtonActivity();
                break;

            case MPL_ERROR_HANDLER:
                Mpl_ErrorHandlerActivity();
                break;
        }
}

///////////////////////////////////////////////////
//
// IDLE
//
void Mpl_IdleActivity(void)
{
    // Nothing to do...
}

///////////////////////////////////////////////////
//
// INIT
//
void Mpl_InitActivity(void)
{
    LCDPrintInitScreen();
    init_refresh_counter = 5;
    lcd_state = LCD_PRESSURE;
    main_program_loop_state = MPL_SENSOR_MEASURE;
}

///////////////////////////////////////////////////
//
// SENSORS
//
void Mpl_SensorMeasureActivity(void)
{
    last_current_pressure_value = current_pressure_value;
    last_current_flow_value = current_flow_value;

    if(OWN_STATUS_OK != PressureSensorGetValue(&current_pressure_value))
    {
        main_program_loop_state = MPL_ERROR_HANDLER;
        return;
    }
    
    if(OWN_STATUS_OK != FlowSensorGetValue(&current_flow_value))
    {
        main_program_loop_state = MPL_ERROR_HANDLER;
        return;
    }
    main_program_loop_state = MPL_PUMP_CONTROL;
}

///////////////////////////////////////////////////
//
// PUMP
//
void Mpl_PumpControlActivity(void)
{
    if(is_pump_on)
    {
        if((millis() - pid_last_count) > pid_count_interval)
        {
            pid_last_count = millis();
            MakePumpControl(set_pressure_value, current_pressure_value, &set_pump_fill);
        }
        
    }
    main_program_loop_state = MPL_LCD_UPDATE;
}

///////////////////////////////////////////////////
//
// LCD
//
void Mpl_LcdUpdateActivity(void)
{
    if(LCD_INIT == lcd_state)
    {
        LCDPrintInitScreen();
    }

    else if(LCD_PRESSURE == lcd_state)
    {
        if(was_screen_changed) // if screen was changed
        {
            was_screen_changed = false;
            LCDPrintPressureScreen(is_pump_on, set_pressure_value, current_pressure_value);
            last_screen_update_time = millis();
        }
        else if(millis() - last_screen_update_time > screen_refresh_time) // if refresh time passed
        {
            if((last_current_pressure_value != current_pressure_value) || (last_set_pressure_value != set_pressure_value) || (last_is_pump_on != is_pump_on)) // if sth was changed
            {
                last_is_pump_on = is_pump_on;
                last_current_pressure_value = current_pressure_value;
                last_set_pressure_value = set_pressure_value;
                last_screen_update_time = millis();
                LCDUpdatePressureScreen(is_pump_on, set_pressure_value, current_pressure_value); 
            }
            if(init_refresh_counter > 0)
            {
                init_refresh_counter--;
                last_screen_update_time = millis();
                LCDUpdatePressureScreen(is_pump_on, set_pressure_value, current_pressure_value);
            }
        }
    }

    else if(LCD_PUMP == lcd_state)
    {
        if(was_screen_changed)
        {
            was_screen_changed = false;
            LCDPrintPumpScreen(is_pump_on, current_flow_value, set_pump_fill);
            last_screen_update_time = millis();
        }
        else if(millis() - last_screen_update_time > screen_refresh_time) // if refresh time passed
        {
            
            if((last_current_flow_value != current_flow_value) || (last_set_pump_fill != set_pump_fill) || (last_is_pump_on != is_pump_on))
            {
                last_is_pump_on = is_pump_on;
                last_current_flow_value = current_flow_value;
                last_set_pump_fill = set_pump_fill;
                last_screen_update_time = millis();
                LCDPrintPumpScreen(is_pump_on, current_flow_value, set_pump_fill); 
            }   
        }
    }

    else if(LCD_ERROR == lcd_state)
    {
        LCDPrintErrorScreen(type_of_error);
        main_program_loop_state = MPL_ERROR_HANDLER;
        last_screen_update_time = millis();
    }
    main_program_loop_state = MPL_BUTTON;
}

///////////////////////////////////////////////////
//
// BUTTON
//
void Mpl_ButtonActivity(void)
{
    uint8_t button_state = 0;
    button_state = WasPressed();

    if(button_state > 0)
    {
        if(ON_OFF_BUTTON_SIGNAL == button_state)
        {
            last_is_pump_on = is_pump_on;
            is_pump_on = !is_pump_on;
            if(!is_pump_on) PumpOff();
            else PumpOn();
        }

        else if(CHANGE_VALUE_BUTTON_SIGNAL == button_state)
        {
            if(pressure_possible_tab_cnt < (NUMBER_OF_POSSIBLE_PRESSURE_TO_SET-1)) pressure_possible_tab_cnt++;
            else pressure_possible_tab_cnt = 0;

            last_set_pressure_value = set_pressure_value;
            set_pressure_value = pressure_possible_tab[pressure_possible_tab_cnt];
        }

        else if(CHANGE_MENU_BUTTON_SIGNAL == button_state)
        {
            if(LCD_PRESSURE == lcd_state) lcd_state = LCD_PUMP;
            else if(LCD_PUMP == lcd_state) lcd_state = LCD_PRESSURE;
            was_screen_changed = true;
        } 
    }
    main_program_loop_state = MPL_SENSOR_MEASURE;
}

///////////////////////////////////////////////////
//
// ERROR
//
void Mpl_ErrorHandlerActivity(void)
{
    #if(SERIAL_DEBUG_PUMP)
        Serial.println("!!! ERROR HANDLER !!!");
    #endif  
    lcd_state = LCD_ERROR;
}