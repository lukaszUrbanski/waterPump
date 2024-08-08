#include "PWM_on_c0.h"

void Task1code(void *pvParameters);
TaskHandle_t Task1;

typedef unsigned long time_from_millis_t;
typedef float flow_sensor_value_t;

extern volatile uint32_t flow_sensor_counter;

flow_sensor_value_t flow_liters_per_hour = 0.0;

time_from_millis_t curent_time = 0UL;
time_from_millis_t last_time = 0UL;
time_from_millis_t measure_interval = 1000UL;

void initDualCoreMode(void)
{
    xTaskCreatePinnedToCore(
        Task1code, /* Task function. */
        "Task1",   /* name of task. */
        10000,     /* Stack size of task */
        NULL,      /* parameter of the task */
        1,         /* priority of the task */
        &Task1,    /* Task handle to keep track of created task */
        0);        /* pin task to core 0 */
    last_time = millis();
}

/*
 *   YF-S201: 450 pulse for 1 liter, so each pulse means 1/450 liter water flowing through
 *   V_total(L) = N* 1/450(L)
 *   V_total(L) = Q(L/s)*t(s)
 *   N* 1/450 = Q(L/s)*t(s)
 *   N/t = 450 * Q(L/s)
 *   f = 450*Q(L/s);
 *   Q(L/s) = f/450;
 *   Q(L/hour) = f*60*60/450 = f*60 /7.5
 */
void Task1code(void *pvParameters)
{
    while (1)
    {
        curent_time = millis();
        if ((curent_time - last_time) >= measure_interval)
        {
            flow_liters_per_hour = (flow_sensor_counter * 60.0) / 7.5;
            #if(SERIAL_DEBUG_FLOW)
                Serial.print(">> Flow sensor >> ilość impulsow: ");
                Serial.print(flow_sensor_counter);
                Serial.print(" => przeplyw = ");
                Serial.print(flow_liters_per_hour);
                Serial.println(" [l/h]");
            #endif
            flow_sensor_counter = 0;
            last_time = curent_time;
        }
        delay(1);
    }
}

float GetFlowValue_LitersPerHour(void)
{
    return flow_liters_per_hour;
}

float GetFlowValue_LitersPerMinute(void)
{
    return flow_liters_per_hour / 60.0;
}
