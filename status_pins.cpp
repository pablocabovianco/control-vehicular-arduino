#include "status_pins.h"
#include <Arduino.h>

// Estructura para almacenar la relación entre códigos de estado y números de pin
struct StatusPin {
  char codigo;
  int pin;
};

// Define de códigos de estado
const char GUINO_IZQ = 0b1010000;
const char GUINO_DER = 0b1010001;
const char PUERTA_AB = 0b1010010;
const char CINTURON  = 0b1010011;
const char CAMBIO_D	 = 0b1010100;
const char CAMBIO_R	 = 0b1010101;

// Array de estructuras StatusPin
const StatusPin statusPines[] = {
  {GUINO_IZQ, 14},
  {GUINO_DER, 15},
  {PUERTA_AB, 16},
  {CINTURON,  17},
  {CAMBIO_D,  18},
  {CAMBIO_R,  19}
};

// Determina el número de elementos en el array
const int numSatusPines = sizeof(statusPines) / sizeof(statusPines[0]);
bool previousState[numSatusPines];

void statusPinsSetup() {
  // Configura los pines de estado como entrada e inicializa los estados anteriores
  for (int i = 0; i < numSatusPines; ++i) {
    pinMode(statusPines[i].pin, INPUT);
    // Almacena el estado inicial del pin
    previousState[i] = digitalRead(statusPines[i].pin);
  }
}

void statusPinsLoop() {

  for (int i = 0; i < numSatusPines; ++i) {
    // Para cada pin de estado lee el estado actual del pin
    bool currentState = digitalRead(statusPines[i].pin);

    // Si el estado ha cambiado...
    if (currentState != previousState[i]) {

      // Imprime información de depuración
      //Serial.print("Pin ");
      //Serial.print(statusPines[i].pin);
      //Serial.print(" cambio de ");
      //Serial.print(previousState[i]);
      //Serial.print(" a ");
      //Serial.println(currentState);

      // Construye el código de estado, añadiendo un bit al final para indicar el estado actual
      char codigo = (statusPines[i].codigo << 1) | currentState;

      // Envía el código de estado por el puerto serial
      Serial.write(codigo);

      // Almacena el estado actual como el estado anterior para la próxima iteración
      previousState[i] = currentState;
    }
  }
  
}

