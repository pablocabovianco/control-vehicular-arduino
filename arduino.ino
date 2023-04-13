#include "rpm_sensor.h"
#include "control_pins.h"

void setup() {
  Serial.begin(9600);
  rpmSensorLoop();
  controlPinsLoop();
} 

void loop() {
  attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);
  rpmSensorLoop();
  controlPinsLoop();
  delay(200);
} 

