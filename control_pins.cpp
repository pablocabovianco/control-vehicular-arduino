#include "control_pins.h"
#include <Arduino.h>

// Estructura para almacenar la relación entre códigos de control y números de pin
struct ControlPin {
  char codigo;
  int pin;
};

// Define de códigos de control
const char LUZ_POS = 0b0000000;
const char LUZ_BAJ = 0b0000001;
const char LUZ_ALT = 0b0000010;
const char LUZ_REF = 0b0000011;
const char LUZ_INT = 0b0000100;
const char BLK_PUE = 0b0000101;
const char BOCINA  = 0b0000110;

// Array de estructuras ControlPin
const ControlPin controlPines[] = {
  {LUZ_POS, 33},
  {LUZ_BAJ, 35},
  {LUZ_ALT, 37},
  {LUZ_REF, 39},
  {LUZ_INT, 41},
  {BLK_PUE, 43},
  {BOCINA,  45}
};

// Determina el número de elementos en el array
const int numControlPines = sizeof(controlPines) / sizeof(controlPines[0]);

void controlPinsSetup() {
  // Configura los pines de control como salida
  for (int i = 0; i < numControlPines; ++i) {
    pinMode(controlPines[i].pin, OUTPUT);
  }
}

void controlPinsLoop() {

  if (Serial.available() > 0) {
    char received = Serial.read();
    char code = received >> 1;
    uint8_t state = (received & 0b1) == 0b0 ? LOW : HIGH;

    // Busca el código en el array controlPines y le asigna el estado correspondiente
    for (int i = 0; i < numControlPines; ++i) {
      if (controlPines[i].codigo == code) {
        digitalWrite(controlPines[i].pin, state);
      }
    }
  }
  
}