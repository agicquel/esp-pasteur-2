#include <Arduino.h>
#include "pasteur_esp_sys.h"

PasteurESPSys *pasteur_sys;

void setup()
{
  pinMode(GPIO_SW1, INPUT);
  pinMode(GPIO_SW2, INPUT);
  pinMode(GPIO_SW3, INPUT);

  pasteur_sys = new PasteurESPSys();
  pasteur_sys->print_bootscreen();
}

void loop()
{
  pasteur_sys->run();
  delay(200);
}