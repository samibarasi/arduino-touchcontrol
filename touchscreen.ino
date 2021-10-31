#include <HID-Project.h>

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#define USE_TIMER_1     true

#include "TimerInterrupt.h"

#define TIMER1_INTERVAL_MS    1000

#define LED_BUILTIN   17

#define DEBUG true

int last_update[10]= {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int threshold = 10000;

void TimerHandler1(void)
{
  /* timeout resetten */
  int timeouts = 0;
  for (int i = 0; i < 10; i++) {
    if (last_update[i] > 0)
      Serial.println(String(i) + " : " + String(last_update[i]));
    if ((last_update[i] > 0) && (millis() - last_update[i] > threshold)) {
      Touchscreen.releaseFinger(i);
      last_update[i] = -1;
      timeouts = timeouts + 1;
    }
  }
  if (timeouts) {
    Serial.println("had to reset fingers: " + String(timeouts));
  }
  static bool toggle1 = false;

  //timer interrupt toggles pin LED_BUILTIN
  digitalWrite(LED_BUILTIN, toggle1);
  toggle1 = !toggle1;
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  ITimer1.init();

  if (ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS, TimerHandler1))
  {
    Serial.print(F("Starting  ITimer1 OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
  
  Serial.begin(115200);
  Serial1.begin(115200);

  Serial.println("init");
}

void loop() {
  if (Serial1.available() > 0) {
    int down = Serial1.parseInt();
    int finger = Serial1.parseInt();
    int x = Serial1.parseInt();
    int y = Serial1.parseInt();
    if (Serial1.read() == '\n') {
      if (down > 0) {
        Touchscreen.setFinger(finger, x, y);
        last_update[finger] = millis();
        Serial.println("touch: " + String(millis()));
      } else {
        Touchscreen.releaseFinger(finger);
        last_update[finger] = -1;
        Serial.println("release: " + String(millis()));
      }
      Touchscreen.send();
    }  
  }
}
