#include "main_loop.h"

main_program_loop_state_t main_program_loop_state = MPL_INIT; 
lcd_state_t lcd_state = LCD_INIT; 

// TO DO watch dog 

typedef bool is_pump_on_t;
is_pump_on_t is_pump_on = false; 
is_pump_on_t last_is_pump_on = false;


