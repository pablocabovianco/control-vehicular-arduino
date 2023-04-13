#include "rpm_sensor.h"
#include "control_pins.h"

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);
  rpmSensorSetup();
  controlPinsSetup();
} 

void loop() {
  rpmSensorLoop();
  controlPinsLoop();
  delay(200);
} 

