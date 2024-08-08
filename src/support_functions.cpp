#include "support_functions.h"

typedef bool tact_switch_pressed_t;
typedef unsigned long time_from_millis_t;

volatile tact_switch_pressed_t tact_switch_pressed_func = false; 
volatile tact_switch_pressed_t on_off_tact_switch_pressed_func = false; 
volatile tact_switch_pressed_t change_value_tact_switch_pressed_func = false;
volatile tact_switch_pressed_t change_menu_tact_switch_pressed_func = false; 

time_from_millis_t now = millis();
time_from_millis_t last_trigger = 0;
const time_from_millis_t button_debouncing_time = 500UL;

void IRAM_ATTR OnOffSwitchPressedActivity(void);
void IRAM_ATTR ChangeValueSwitchPressedActivity(void);
void IRAM_ATTR ChangeMenuSwitchPressedActivity(void);

void DebugSerialInit(void)
{
    Serial.setTxBufferSize(1024);
    Serial.setRxBufferSize(1024);
    Serial.begin(115200);

     delay(1000);
        #if(SERIAL_DEBUG_PUMP)
            Serial.print("\r\n=====================================================\r\n");
            Serial.print("Praca inzynierska Lukasz Urbanski\r\n");
            Serial.print("Mikroproc. sterownik praca pompy wodnej\r\n");
            Serial.print("\r\n=====================================================\r\n");

            Serial.print("Main program work on core:");
            Serial.println(xPortGetCoreID());
            Serial.printf("Internal Total heap %d, internal Free Heap %d\n\r", ESP.getHeapSize(), ESP.getFreeHeap());
            Serial.printf("SPIRam Total heap %d, SPIRam Free Heap %d\n\r", ESP.getPsramSize(), ESP.getFreePsram());
            Serial.printf("ChipRevision %d, Cpu Freq %d, SDK Version %s\n\r", ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
            Serial.printf("Flash Size %d, Flash Speed %d\n\r", ESP.getFlashChipSize(), ESP.getFlashChipSpeed());
        #endif
}

void GPIOInterruptInit(void)
{
    pinMode(BUTTON_ON_OFF_PIN, INPUT_PULLUP);
    pinMode(BUTTON_CHANGE_VALUE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_CHANGE_MENU_PIN, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(BUTTON_ON_OFF_PIN), OnOffSwitchPressedActivity, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_CHANGE_VALUE_PIN), ChangeValueSwitchPressedActivity, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_CHANGE_MENU_PIN), ChangeMenuSwitchPressedActivity, FALLING);
}

void IRAM_ATTR OnOffSwitchPressedActivity(void)
{
    tact_switch_pressed_func = true;
    on_off_tact_switch_pressed_func = true;
}

void IRAM_ATTR ChangeValueSwitchPressedActivity(void)
{
    tact_switch_pressed_func = true; 
    change_value_tact_switch_pressed_func = true;
}

void IRAM_ATTR ChangeMenuSwitchPressedActivity(void)
{
    tact_switch_pressed_func = true; 
    change_menu_tact_switch_pressed_func = true;
}

uint8_t WasPressed(void)
{
    uint8_t tact_switch_state = 0;

    if (tact_switch_pressed_func)
    {
        now = millis();
        if (now - last_trigger > button_debouncing_time)
        {
            if (on_off_tact_switch_pressed_func)
            {
                tact_switch_state = ON_OF_BUTTON_SIGNAL;
                last_trigger = now;
                #if(SERIAL_DEBUG_GPIO_PUMP)
                    Serial.println("# ON / OFF button");
                #endif
            }

            else if (change_value_tact_switch_pressed_func)
            {
                tact_switch_state = CHANGE_VALUE_BUTTON_SIGNAL;
                last_trigger = now; // zero time from last press    
                    #if(SERIAL_DEBUG_GPIO_PUMP)
                        Serial.println("# change value button.");
                    #endif  
            }

             else if (change_menu_tact_switch_pressed_func)
            {
                tact_switch_state = CHANGE_MENU_BUTTON_SIGNAL;
                last_trigger = now; // zero time from last press   
                    #if(SERIAL_DEBUG_GPIO_PUMP)
                        Serial.println("# change menu button.");
                    #endif   
            }

        }
        else 
        {
            #if(SERIAL_DEBUG_GPIO_PUMP)
                Serial.println("Double click.");
            #endif 
        }

    }
    
    tact_switch_pressed_func = false;
    on_off_tact_switch_pressed_func = false;
    change_value_tact_switch_pressed_func = false;
    change_menu_tact_switch_pressed_func = false;

    return tact_switch_state;
}
