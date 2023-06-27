#include "rpm_sensor.h"
#include "control_pins.h"
#include "status_pins.h"

void setup() {
  Serial.begin(9600);
  pinMode(20, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(20), Pulse_Event, RISING);
  rpmSensorSetup();
  controlPinsSetup();
  statusPinsSetup();
} 

void loop() {
  rpmSensorLoop();
  controlPinsLoop();
  statusPinsLoop();
}