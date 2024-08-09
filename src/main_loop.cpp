#include "pump_control.h"

#define PWM_PUMP_CHANNEL 0
#define PWM_PUMP_RESOLUTION 8
#define PWM_PUMP_MAX_VALUE 256
#define PWM_PUMP_FREQ_HZ 2000

#define PUMP_IN_1_INCREASE_PRESSURE_DIRECTION LOW
#define PUMP_IN_2_INCREASE_PRESSURE_DIRECTION HIGH

#define PUMP_IN_1_LOWER_PRESSURE_DIRECTION HIGH
#define PUMP_IN_2_LOWER_PRESSURE_DIRECTION LOW

#define PUMP_IN_1_FAST_STOP HIGH
#define PUMP_IN_2_FAST_STOP HIGH


/*
 * PID
 */
typedef float value_of_predefined_pressure_to_be_maintained_t;
typedef float regulator_settings_t;
typedef float regulator_calculations_t;
typedef uint8_t pwm_pump_fill_t;
typedef float error_value_t;
typedef float temperature_value_t;
typedef float pressure_hysteresis_value_t;
typedef float pressure_emergency_off_value_t;
typedef bool was_hysteresis_on_t;

pressure_hysteresis_value_t pressure_hysteresis_value = 0.5;
was_hysteresis_on_t was_hysteresis_on = false;

pressure_emergency_off_value_t pressure_emergency_off_value = 3.0;

regulator_settings_t Kp = 1.4;
regulator_settings_t Ki = 0.5;
regulator_settings_t Kd = 0.2;

regulator_calculations_t Pout = 0.0;
regulator_calculations_t Iout = 0.0;
regulator_calculations_t Dout = 0.0;

pwm_pump_fill_t actual_pump_fill = 0;

temperature_value_t input_actural_pressure = 0.0;

error_value_t error_value = 0.0;
error_value_t last_error_value = 0.0;
error_value_t error_sum = 0.0;
error_value_t Derror = 0.0;

int PumpSet(uint8_t pump_in_fill);

int PumpInit(void)
{
    // init pin
    pinMode(PUMP_IN_1_PIN, OUTPUT);
    pinMode(PUMP_IN_2_PIN, OUTPUT);

    digitalWrite(PUMP_IN_1_PIN, LOW);
    digitalWrite(PUMP_IN_2_PIN, LOW);

    // Configure PWM functionalities
    ledcSetup(PWM_PUMP_CHANNEL, PWM_PUMP_FREQ_HZ, PWM_PUMP_RESOLUTION);
    // attach the channel to the GPIO to be controlled
    ledcAttachPin(PUMP_ENA_PWM_PIN, PWM_PUMP_CHANNEL);
    // set init value
    ledcWrite(PWM_PUMP_CHANNEL, 0);

    return OWN_STATUS_OK;
}

int MakePumpControl(float set_pressure_value, float measured_pressure_value, uint8_t * set_pump_fill)
{
    if (measured_pressure_value > set_pressure_value)
    {
        if(measured_pressure_value > (set_pressure_value + pressure_emergency_off_value))
        {
            error_value = 0.0;
            error_sum = 0.0;
            Derror = 0.0; 
            was_hysteresis_on = true;

            #if(SERIAL_DEBUG_PID)
                Serial.println("$PID: measured_pressure_value > (set_pressure_value + pressure_emergency_off_value, emergency pump off");
            #endif 
        }
        else
        {
            if(!was_hysteresis_on)
            {
                was_hysteresis_on = true;
                actual_pump_fill = (actual_pump_fill*0.9);
                #if(SERIAL_DEBUG_PID)
                    Serial.println("$PID: measured_pressure_value > set_pressure_value - pressure_emergency_off_value, hysteresis ON, pump set 90%");
                #endif
            }
                    #if(SERIAL_DEBUG_PID)
                        Serial.print("$PID: Actual pressure: ");
                        Serial.print(measured_pressure_value);
                        Serial.print(". Set presure: ");
                        Serial.print(set_pressure_value);
                        Serial.print(". Pout: ");
                        Serial.print(Pout);
                        Serial.print(". Iout: ");
                        Serial.print(Iout);
                        Serial.print(". Dout: ");
                        Serial.println(Dout);
                        Serial.print("Last_error_value: ");
                        Serial.print(last_error_value);
                        Serial.print(". error_value: ");
                        Serial.print(error_value);
                        Serial.print(". actual_pump_fill: ");
                        Serial.println(actual_pump_fill);
                    #endif

            *set_pump_fill = actual_pump_fill;

            // Control Motor
            PumpSet(actual_pump_fill);
            // return flow value
            return actual_pump_fill;
        }  
    }
    else
    {
        if(was_hysteresis_on)
        {
            if(measured_pressure_value < (set_pressure_value - pressure_hysteresis_value)) /* has reached the cutoff value of the hysteresis loop */
            {
                was_hysteresis_on = false; 
                    #if(SERIAL_DEBUG_PID)
                        Serial.println("$PID: measured_pressure_value < (set_pressure_value - pressure_hysteresis_value, hysteresis off, pump on");
                    #endif
            }
        }
        else if(measured_pressure_value < set_pressure_value )                                              /* Count PID */
            {
                error_value =  set_pressure_value - measured_pressure_value; 		                        /* P */
                error_sum = error_sum + ((error_value + last_error_value)*0.5); 							/* I */
                Derror = (error_value - last_error_value);						                            /* D */					
            } 
    }
    
    Pout = Kp * error_value;		/* P */
	Iout = Ki * error_sum;			/* I */
	Dout = Kd * Derror;				/* D */

    if(Iout > 100) Iout = 100;
	else if(Iout < 0) Iout = 0;

    actual_pump_fill = Pout + Iout + Dout;
    if(actual_pump_fill > 100) actual_pump_fill = 100;
    else if(actual_pump_fill < 0) actual_pump_fill = 0;

        #if(SERIAL_DEBUG_PID)
            Serial.print("$PID: Actual pressure: ");
            Serial.print(measured_pressure_value);
            Serial.print(". Set presure: ");
            Serial.print(set_pressure_value);
            Serial.print(". Pout: ");
            Serial.print(Pout);
            Serial.print(". Iout: ");
            Serial.print(Iout);
            Serial.print(". Dout: ");
            Serial.println(Dout);
            Serial.print("Last_error_value: ");
            Serial.print(last_error_value);
            Serial.print(". error_value: ");
            Serial.print(error_value);
            Serial.print(". actual_pump_fill: ");
            Serial.println(actual_pump_fill);
        #endif

    last_error_value = error_value;

    *set_pump_fill = actual_pump_fill;

    // Control Motor
    PumpSet(actual_pump_fill);
    // return flow value
    return actual_pump_fill;
}

int PumpSet(uint8_t pump_in_fill)
{   
    uint32_t pump_max_value = PWM_PUMP_MAX_VALUE;
    float duty_to_count = (((float)(pump_in_fill))/100.0)*((float)(pump_max_value));
    uint32_t duty_to_set = (uint32_t)duty_to_count;

    Serial.print("Pump resolution: ");
    Serial.print(duty_to_set);
    Serial.print("/256\r\n\r\n ");

    digitalWrite(PUMP_IN_1_PIN, PUMP_IN_1_INCREASE_PRESSURE_DIRECTION);
    digitalWrite(PUMP_IN_2_PIN, PUMP_IN_2_INCREASE_PRESSURE_DIRECTION);

    ledcWrite(PWM_PUMP_CHANNEL, duty_to_set);

    return OWN_STATUS_OK;
}

int PumpOff(void)
{
    digitalWrite(PUMP_IN_1_PIN, PUMP_IN_1_FAST_STOP);
    digitalWrite(PUMP_IN_2_PIN, PUMP_IN_1_FAST_STOP);

    ledcWrite(PWM_PUMP_CHANNEL, PWM_PUMP_MAX_VALUE);

    error_value = 0.0;
    error_sum = 0.0;
    Derror = 0.0;

    last_error_value = 0.0;
    error_value = 0.0;

    #if(SERIAL_DEBUG_PUMP)
        Serial.println("Pump OFF!");
    #endif

    return OWN_STATUS_OK;
}

int PumpOn(void)
{
    digitalWrite(PUMP_IN_1_PIN, PUMP_IN_1_INCREASE_PRESSURE_DIRECTION);
    digitalWrite(PUMP_IN_2_PIN, PUMP_IN_2_INCREASE_PRESSURE_DIRECTION);

    ledcWrite(PWM_PUMP_CHANNEL, PWM_PUMP_MAX_VALUE);

    error_value = 0.0;
    error_sum = 0.0;
    Derror = 0.0;

    last_error_value = 0.0;
    error_value = 0.0;

    #if(SERIAL_DEBUG_PUMP)
        Serial.println("Pump ON!");
    #endif

    return OWN_STATUS_OK;
}