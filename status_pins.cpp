#include "status_pins.h"
#include <Arduino.h>

// Estructura para almacenar la relación entre códigos de estado y números de pin
struct StatusPin {
  String codigo;
  int pin;
};

// Define de códigos de estado
const String GUINO_IZQ = "GUINO_IZQ";
const String GUINO_DER = "GUINO_DER";
const String PUERTA_AB = "PUERTA_AB";
const String CINTURON  = "CINTURON";
const String CAMBIO_D	 = "CAMBIO_D";
const String CAMBIO_R	 = "CAMBIO_R";

// Array de estructuras StatusPin
const StatusPin statusPines[] = {
  {GUINO_IZQ, 19},
  {GUINO_DER, 18},
  {PUERTA_AB, 17},
  {CINTURON,  16},
  {CAMBIO_R,  15},
  {CAMBIO_D,  14}
};

// Determina el número de elementos en el array
const int numStatusPines = sizeof(statusPines) / sizeof(statusPines[0]);
bool previousState[numStatusPines];

void statusPinsSetup() {
  // Configura los pines de estado como entrada
  for (int i = 0; i < numStatusPines; ++i) {
    pinMode(statusPines[i].pin, INPUT);
    previousState[i] = LOW;
  }
}

void statusPinsLoop() {

  // Lee el estado actual de cada pin de estado
  for (int i = 0; i < numStatusPines; ++i) {
    bool currentState = digitalRead(statusPines[i].pin) == HIGH;

    // Comprueba si el estado ha cambiado desde la última lectura
    if (currentState != previousState[i]) {
      // Actualiza el estado anterior almacenado
      previousState[i] = currentState;
      
      // Construye el mensaje de estado
      String mensaje = statusPines[i].codigo + "_" + (currentState ? "1" : "0");
      
      // Envía el mensaje por el puerto serial
      Serial.println(mensaje);
    }
  }
  
}

