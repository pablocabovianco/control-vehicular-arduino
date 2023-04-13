
// Calibración de RPM para el sensor Hall:
const byte PulsesPerRevolution = 1;       
const unsigned long ZeroTimeout = 300000; // Timeout para mostrar 0 si no registra pulsos
const byte numReadings = 2;               // Smoothing RPM: Cantidad de muestras para suavizar el resultado


// Variables:
volatile unsigned long LastTimeWeMeasured;  
volatile unsigned long PeriodBetweenPulses = ZeroTimeout+1000;  // Stores the period between pulses in microseconds.
volatile unsigned long PeriodAverage = ZeroTimeout+1000;  // Stores the period between pulses in microseconds in total, if we are taking multiple pulses.

unsigned long FrequencyRaw;  // Calculated frequency, based on the period. This has a lot of extra decimals without the decimal point.
unsigned long FrequencyReal;  // Frequency without decimals.
unsigned long RPM;  // Raw RPM without any processing.
unsigned int PulseCounter = 1;  // Counts the amount of pulse readings we took so we can average multiple pulses before calculating the period.
unsigned long PeriodSum; // Stores the summation of all the periods to do the average.
unsigned long LastTimeCycleMeasure = LastTimeWeMeasured;  // Stores the last time we measure a pulse in that cycle.
unsigned long CurrentMicros = micros();  // Stores the micros in that cycle.
unsigned int AmountOfReadings = 1; //cantidad de lecturas para calcular RPM (va a variar dependiendo de la velocidad para ganar precisión)
unsigned int ZeroDebouncingExtra;  // Stores the extra value added to the ZeroTimeout to debounce it.

// Variables for smoothing tachometer:
unsigned long readings[numReadings];  // The input.
unsigned long readIndex;  // The index of the current reading.
unsigned long total;  // The running total.
unsigned long average;  // The RPM value after applying the smoothing.

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
  {LUZ_POS, 45},
  {LUZ_BAJ, 47},
  {LUZ_ALT, 49},
  {LUZ_REF, 51},
  {LUZ_INT, 53},
  {BLK_PUE, 41},
  {BOCINA,  43}
};

// Determina el número de elementos en el array
const int numControlPines = sizeof(controlPines) / sizeof(controlPines[0]);

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), Pulse_Event, RISING);  // Enable interruption pin 2 when going from LOW to HIGH.

  // Configura los pines de control como salida
  for (int i = 0; i < numControlPines; ++i) {
    pinMode(controlPines[i].pin, OUTPUT);
  }

  delay(1000);  
} 

void loop() {
  LastTimeCycleMeasure = LastTimeWeMeasured; 
  CurrentMicros = micros(); 

  if(CurrentMicros < LastTimeCycleMeasure){
    LastTimeCycleMeasure = CurrentMicros;
  }

  // Calculate the frequency:
  FrequencyRaw = 10000000000 / PeriodAverage;  // Calculate the frequency using the period between pulses.
  
  // Detect if pulses stopped or frequency is too low, so we can show 0 Frequency:
  if(PeriodBetweenPulses > ZeroTimeout - ZeroDebouncingExtra || CurrentMicros - LastTimeCycleMeasure > ZeroTimeout - ZeroDebouncingExtra) {  // If the pulses are too far apart that we reached the timeout for zero:
    FrequencyRaw = 0; 
    ZeroDebouncingExtra = 2000;
  } else {
    ZeroDebouncingExtra = 0;  // Reset the threshold to the normal value so it doesn't bounce.
  }

  FrequencyReal = FrequencyRaw / 10000;  

  // Calculate the RPM:
  RPM = FrequencyRaw / PulsesPerRevolution * 60; 
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

  //Envia el promedio de RPM
  Serial.println(average);
  delay(200);
} 

void Pulse_Event() {

  PeriodBetweenPulses = micros() - LastTimeWeMeasured; 
  LastTimeWeMeasured = micros(); 

  if(PulseCounter >= AmountOfReadings) {
    PeriodAverage = PeriodSum / AmountOfReadings;  // Calculate the final period dividing the sum of all readings by the amount of readings to get the average.
    PulseCounter = 1;  // Reset the counter to start over. The reset value is 1 because its the minimum setting allowed (1 reading).
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
