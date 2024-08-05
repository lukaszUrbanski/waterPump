#pragma once 

#include "main.h"

#define ON_OF_BUTTON_SIGNAL 1 
#define CHANGE_VALUE_BUTTON_SIGNAL 2 
#define CHANGE_MENU_BUTTON_SIGNAL 3

void DebugSerialInit(void);
uint8_t WasPressed(void);
void GPIOInterruptInit(void);

