#include "rpm_sensor.h"
#include "control_pins.h"

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(7, INPUT);
  pinMode(31, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);
  rpmSensorSetup();
  controlPinsSetup();
} 

void loop() {
  rpmSensorLoop();
  controlPinsLoop();
  int estado = digitalRead(7);  
  if (estado == HIGH) {   
    digitalWrite(31, HIGH);  
  } else {   
    digitalWrite(31, LOW);
  }  

} 

