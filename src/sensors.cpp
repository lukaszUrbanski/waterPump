#include "sensors.h"


/* *********************************************************************************************************************
*
*           PRESSURE SENSOR
*
* **********************************************************************************************************************
*/
// 12 bit ADC - max value 4095
#define ADC_RESOLUTIONS_MAX_VALUE 4095

typedef uint32_t ADC_measured_value_t;
typedef float measured_voltage_on_voltage_divider_t;
typedef float measured_voltage_from_sensor_t;
typedef float measured_pressurre_t;
typedef float adc_correction_value_t;
typedef float resistors_value_k_t;

resistors_value_k_t R1_value_k = 10.0;
resistors_value_k_t R2_value_k = 20.0;

adc_correction_value_t adc_correction_value = 0.26;




// Pressure sensor init function
// return state in accordance with own_status_t
int PressureSensorInit(void)
{
    //nothing todo
    return OWN_STATUS_OK;
}

// Get pressure value
// Function get pointer on float variable and fill it actual pressure value
// return state in accordance with own_status_t
int PressureSensorGetValue(float * pressure_value)
{
    ADC_measured_value_t ADC_measured_value = 0; //12 bit resolution - it can be 0 to 4095 value
    measured_voltage_on_voltage_divider_t measured_voltage_on_voltage_divider = 0.0;
    measured_voltage_from_sensor_t measured_voltage_from_sensor = 0.0;
    measured_voltage_from_sensor_t voltage_from_sensor_after_normalized = 0.0;
    const float normalization_value = 0.87;
    measured_pressurre_t measured_pressurre = 0.0;

    ADC_measured_value = analogRead(PRESSURE_SENSOR_PIN);

    measured_voltage_on_voltage_divider = (float)((3.3 * (float)ADC_measured_value) /  (float)ADC_RESOLUTIONS_MAX_VALUE);

    measured_voltage_from_sensor = ((measured_voltage_on_voltage_divider * (R1_value_k + R2_value_k)) / R2_value_k) + adc_correction_value;

    if(measured_voltage_from_sensor < normalization_value) measured_pressurre = 0.0;
    else if(measured_voltage_from_sensor > 4.5) measured_pressurre = 29;
    else
    {
        voltage_from_sensor_after_normalized = measured_voltage_from_sensor - normalization_value; // 0 < x < 4
        measured_pressurre = (8.0 * voltage_from_sensor_after_normalized);
    }
    #if(SERIAL_DEBUG_ADC)
        Serial.print("Zmierzona rozdzielczosc:  ");
        Serial.print(ADC_measured_value);
        Serial.print(". Naięcie na dzielniku: ");
        Serial.print(measured_voltage_on_voltage_divider, 3);
        Serial.print("[V]. Napiecie z czujnika: ");
        Serial.print(measured_voltage_from_sensor, 3);
        Serial.print("[V] => ");
        Serial.print(measured_pressurre, 0);
        Serial.println(" [psi]");
    #endif

    *pressure_value = measured_pressurre;
    return OWN_STATUS_OK;
}

/* **********************************************************************************************************************
*
*           FLOW SENSOR
*
* **********************************************************************************************************************
*/

volatile uint32_t flow_sensor_counter = 0;
void IRAM_ATTR IncrementFlowSensorVariable(void);

// Flow sensor init function
// return state in accordance with own_status_t
int FlowSensorInit(void)
{
    // init second core using to avoid delay
    initDualCoreMode();

    // Flow sensor pin init
    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);

    // Set BUTTON_CHANGE_MENU_PIN as interrupt, assign interrupt function and set FALLING mode
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), IncrementFlowSensorVariable, FALLING);

    flow_sensor_counter = 0;

    return OWN_STATUS_OK;
}

// Get pressure value
// Function get pointer on float variable and fill it actual flow value
// return state in accordance with own_status_t
int FlowSensorGetValue(float * flow_value)
{
    float flow_per_hour = GetFlowValue_LitersPerHour();
    *flow_value = flow_per_hour;

        // #if(SERIAL_DEBUG_FLOW)
        //     Serial.print("Policzony przeplyw: ");
        //     Serial.print(flow_per_hour);
        //     Serial.println(" [l/h]");
        // #endif
    return OWN_STATUS_OK;
}

// Each sensor pulse will increment this variable
void IRAM_ATTR IncrementFlowSensorVariable(void)
{
    flow_sensor_counter++;
}

