#include <LCD.h>

typedef uint8_t lcd_size_t; 

typedef uint32_t init_screen_duration_ms_t;

const init_screen_duration_ms_t init_screen_duration_ms = 1000U;

lcd_size_t lcd_columns = 20;
lcd_size_t lcd_rows = 4; 

LiquidCrystal_I2C lcd(0x27, lcd_columns, lcd_rows);

void LCDInit(void){
    lcd.init();
    lcd.backlight();
}

void LCDPrintInitScreen(void){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Praca inzynierska");
    lcd.setCursor(0, 1);
    lcd.print("Lukasz Urbanski");
    lcd.setCursor(0, 2);
    lcd.print("Mikroproc. sterownik");
    lcd.setCursor(0, 3);
    lcd.print("praca pompy wodnej"); 
    delay(init_screen_duration_ms);
}

void LCDClearOneLine(uint8_t line_number){
    lcd.setCursor(0, line_number);
    lcd.print("                    ");
}

