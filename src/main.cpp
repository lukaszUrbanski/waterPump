/**
  ******************************************************************************
  * @file       : main.cpp
  * @project		: Mikroprocesorowy sterownik pracą pompy wodnej
  * @author			: Łukasz Urbański
  * @purpose		: sterownik do silnika pompy wodnej
  * @todo			
  ******************************************************************************
  **/

#include "main.h"

void setup() 
{
  // Serial init
  DebugSerialInit();

  // Init LCD screen
  LCDInit();

  // Button using GPIO init
  GPIOInterruptInit();

  // Init Pressure Sensor
  PressureSensorInit();

  // Init Flow Sensor
  FlowSensorInit();

  // WDG init
  MyHWTimersInit();

  // Pump init
  PumpInit();

}

void loop() 
{
  MainProgramLoop();
}