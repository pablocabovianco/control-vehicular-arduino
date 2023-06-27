#include "rpm_sensor.h"
#include <Arduino.h>

const byte PulsesPerRevolution = 2;       
const unsigned long ZeroTimeout = 500000; // Timeout para mostrar 0 si no registra pulsos
const byte numReadings = 2;               // Smoothing RPM: Cantidad de muestras para suavizar el resultado
const float pi = 3.14159;
const float radio_rueda = 0.35;

volatile unsigned long LastTimeWeMeasured;  
volatile unsigned long PeriodBetweenPulses = ZeroTimeout+1000;  // Periodo entre pulsos (en microsegundos)
volatile unsigned long PeriodAverage = ZeroTimeout+1000;  // Periodo entre pulsos (en microsegundos) en total, cuando tomamos multiples pulsos

unsigned long Frecuency;
unsigned long RPM;
unsigned int PulseCounter = 1;
unsigned long PeriodSum;
unsigned long LastTimeCycleMeasure = LastTimeWeMeasured; 
unsigned long CurrentMicros = micros(); 
unsigned int AmountOfReadings = 1; //cantidad de lecturas para calcular RPM (va a variar dependiendo de la velocidad para ganar precisión)
unsigned int ZeroDebouncingExtra; 

// Variables para suavizar las lecturas:
unsigned long readings[numReadings];
unsigned long readIndex;
unsigned long total;
unsigned long average;

// Tiempo en milisegundos para enviar el dato
const unsigned long tiempoEnvio = 500;  // Por ejemplo, cada 0.5 segundos
unsigned long ultimoEnvio = 0;  // Cuando fue el ultimo envio

void rpmSensorSetup() {
  delay(1000);  
}

void rpmSensorLoop() {
  LastTimeCycleMeasure = LastTimeWeMeasured; 
  CurrentMicros = micros(); 

  if(CurrentMicros < LastTimeCycleMeasure){
    LastTimeCycleMeasure = CurrentMicros;
  }

  // Calculate the frequency:
  Frecuency = 10000000000 / PeriodAverage;  // Calculate the frequency using the period between pulses.
  
  // Detect if pulses stopped or frequency is too low, so we can show 0 Frequency:
  if(PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra) {  // If the pulses are too far apart that we reached the timeout for zero:
    Frecuency = 0; 
    ZeroDebouncingExtra = 2000;
  } else {
    ZeroDebouncingExtra = 0;  // Reset the threshold to the normal value so it doesn't bounce.
  }

  // Calculate the RPM:
  RPM = Frecuency / PulsesPerRevolution * 60; 
  RPM = RPM / 10000;  // Remove the decimals.

  // Smoothing RPM:
  total = total - readings[readIndex];  // Advance to the next position in the array.
  readings[readIndex] = RPM;  // Takes the value that we are going to smooth.
  total = total + readings[readIndex];  // Add the reading to the total.
  readIndex = readIndex + 1;  // Advance to the next position in the array.

  if (readIndex >= numReadings) {
    readIndex = 0;  // Reset array index.
  }
  
  // Calculate the average:
  average = total / numReadings;  // The average value it's the smoothed result.

  //Convierto las rpm promedio en km/h
  //Velocidad (km/h) = (RPM * 2 * pi * Radio de la rueda (m) * 60) / (1000 * Relación de transmisión)
  unsigned int kmh = (average * 2 * pi * radio_rueda * 60) / (1000 * 1);

  // Envía el promedio de RPM cada cierto tiempo
  if(millis() - ultimoEnvio > tiempoEnvio){
    ultimoEnvio = millis();
    Serial.println("KMH:" + String(kmh));  // Enviará "KMH:123\n"
  }  
  
}

void Pulse_Event() {

  PeriodBetweenPulses = micros() - LastTimeWeMeasured; 
  LastTimeWeMeasured = micros(); 

  if (PulseCounter >= AmountOfReadings) {
    PeriodAverage = PeriodSum / AmountOfReadings;
    PulseCounter = 1;  
    PeriodSum = PeriodBetweenPulses;  // Reset PeriodSum to start a new averaging operation.

    // We change the amount of readings depending on the period between pulses.
    int RemapedAmountOfReadings = map(PeriodBetweenPulses, 40000, 5000, 1, 10);  // Remap the period range to the reading range.
    RemapedAmountOfReadings = constrain(RemapedAmountOfReadings, 1, 10);  // Constrain the value so it doesn't go below or above the limits.
    AmountOfReadings = RemapedAmountOfReadings;  // Set amount of readings as the remaped value.
  } else {
    PulseCounter++;  // Increase the counter for amount of readings by 1.
    PeriodSum = PeriodSum + PeriodBetweenPulses;  // Add the periods so later we can average.
  }

}
