#include "LCD.h"

typedef uint8_t lcd_size_t;

typedef uint32_t init_screen_duration_ms_t;

const init_screen_duration_ms_t init_screen_duration_ms = 1000;

//size of screen
lcd_size_t lcd_columns = 20;
lcd_size_t lcd_rows = 4;


LiquidCrystal_I2C lcd(0x27, lcd_columns, lcd_rows);

//
//init display and turn on backlight
void LCDInit(void)
{
    // initialize LCD
    lcd.init();
    // turn on LCD backlight                      
    lcd.backlight();
}

//
// function useing to clear one line on the screen
// function get number pf line to clear
void ClearOneLine(uint8_t number_of_line)
{
    lcd.setCursor(0, number_of_line);
    lcd.print("                    ");
}

//
// print init screen after turn on
void LCDPrintInitScreen(void)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Praca inzynierska");
    lcd.setCursor(0, 1);
    lcd.print("Lukasz Urbanski");
    lcd.setCursor(0, 2);
    lcd.print("Mikroproc. sterownik");
    lcd.setCursor(0, 3);
    lcd.print("praca pompy wodnej");
    delay(init_screen_duration_ms);
}

//
// print pressure
void LCDPrintPressureScreen(bool pump_state, float set_pressure,  float measure_pressure)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Status Pompy: ");
    lcd.setCursor(14, 0);
    if(pump_state) lcd.print("ON ");
    else lcd.print("OFF");

    lcd.setCursor(0, 1);
    lcd.print("Cisnienie w ukladzie: ");
    

    lcd.setCursor(0, 2);
    lcd.print("Zadane: ");
    lcd.setCursor(8, 2);
    lcd.print(set_pressure, 1);
    lcd.setCursor(17, 2);
    lcd.print("psi");

    lcd.setCursor(0, 3);
    lcd.print("Zmierzone: ");
    lcd.setCursor(11, 3);
    lcd.print(measure_pressure, 1);
    lcd.setCursor(17, 3);
    lcd.print("psi");
}

//
// print pressure
void LCDUpdatePressureScreen(bool pump_state, float set_pressure,  float measure_pressure)
{
    lcd.setCursor(14, 0);
    if(pump_state) lcd.print("ON ");
    else lcd.print("OFF");
   
    lcd.setCursor(8, 2);
    lcd.print("     ");
    lcd.setCursor(8, 2);
    lcd.print(set_pressure, 1);

    lcd.setCursor(11, 3);
    lcd.print("     ");
    lcd.setCursor(11, 3);
    lcd.print(measure_pressure, 1);
}

//
// print pump
void LCDPrintPumpScreen(bool pump_state, float pump_flow, uint8_t pump_power)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Status Pompy: ");
    lcd.setCursor(14, 0);
    if(pump_state) lcd.print("ON ");
    else lcd.print("OFF");

    lcd.setCursor(0, 1);
    lcd.print("Przeplyw: ");
    lcd.setCursor(10, 1);
    lcd.print(pump_flow, 0);
    lcd.setCursor(14, 1);
    lcd.print("l/h");

    lcd.setCursor(0, 2);
    lcd.print("Pompa: ");
    lcd.setCursor(7, 2);
    lcd.print(pump_power);
    lcd.setCursor(10, 2);
    lcd.print("%");

    lcd.setCursor(0, 3);
    lcd.print("");
}

//
// print pump
void LCDUpdatePumpScreen(bool pump_state, float pump_flow, uint8_t pump_power)
{
    lcd.setCursor(14, 0);
    if(pump_state) lcd.print("ON ");
    else lcd.print("OFF");

    lcd.setCursor(10, 1);
    lcd.print("   ");
    lcd.setCursor(10, 1);
    lcd.print(pump_flow, 0);

    lcd.setCursor(7, 2);
    lcd.print("   ");
    lcd.setCursor(7, 2);
    lcd.print(pump_power);
}

void LCDPrintErrorScreen(uint8_t number_of_error)
{
    // TODO!
}

