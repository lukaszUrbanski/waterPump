#include "my_tim_set.h"

#include "ESP32TimerInterrupt.h"
#include "ESP32_ISR_Timer.h"

volatile int watchdog_reset_timer_counter = 0;

bool timers_initialization_correctly_variable = false;

#define HW_TIMER_INTERVAL_US 1000UL // 1000000L

// Init ESP32 timer 1
ESP32Timer ITimer(1);

// Init ESP32_ISR_Timer
ESP32_ISR_Timer ISR_Timer;

// With core v2.0.0+, you can't use Serial.print/println in ISR or crash.
// and you can't use float calculation inside ISR
// Only OK in core v1.0.6-
bool IRAM_ATTR TimerHandler(void *timerNo)
{
  ISR_Timer.run();

  return true;
}

void WatchdogReset(void)
{
  watchdog_reset_timer_counter++;
}

void MyHWTimersInit(void)
{
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_US, TimerHandler))
  {
    Serial.print("Starting Timers OK. ");
    timers_initialization_correctly_variable = true;
  }
  else
  {
    Serial.println("Can't set Timer. Select another freq. or timer");
    timers_initialization_correctly_variable = false;
  }

#if (WATCHDOG_IMPLEMENTATION_PUMP)
  if (ESP_OK == esp_task_wdt_init(WATCHDOG_TIMEOUT_IN_S, true))
  {
    Serial.print("Watchdog ON and set on ");
    Serial.print(WATCHDOG_TIMEOUT_IN_S);
    Serial.println(" [s]. ");
    esp_task_wdt_add(NULL);
  }
  else
    Serial.println("Watchdog initialization error.");
#else
  Serial.println("Watchdog OFF.");
#endif

  //
  // Set periodic functions frequency
  //

  ISR_Timer.setInterval(((WATCHDOG_TIMEOUT_IN_S * 1000) / 4), WatchdogReset); // to secure work watchdog timer should be four times reset before it will reset device
                                                                              // it should avoid not needed or random device reset
}
