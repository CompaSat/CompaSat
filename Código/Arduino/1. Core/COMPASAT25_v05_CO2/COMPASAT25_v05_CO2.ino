#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SdFat.h>
#include <TinyGPS++.h>
#include <AltSoftSerial.h>

// BMP280
Adafruit_BMP280 bmp;

// SD
SdFat sd;
SdFile logFile;
const int chipSelect = 10; // Pin CS de la tarjeta SD

// LED
int pinLed = 5; // Pin del LED indicador
int idPaquete = 1; // Contador de paquetes

// GPS
AltSoftSerial altSerial; // Crear objeto AltSoftSerial
TinyGPSPlus gps; // Objeto de la librería TinyGPS++

// MOTOR
const int motorPin1 = 3;
const int motorPin2 = 4;
const int motorPin3 = 6;
const int motorPin4 = 7;
int motorSpeed = 1000;
int stepCounter = 0;
int stepsPerRev = 4076;
int stepsPer90 = stepsPerRev / 4;
const int numSteps = 8;
const int stepsLookup[8] = { B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001 };

// CO2
#define MG_PIN               A0
#define DC_GAIN              8.5
#define ZERO_POINT_VOLTAGE   (0.246) // 2.09 / 8.5
#define REACTION_VOLTGAE     (0.030)
float CO2Curve[3] = {2.602, ZERO_POINT_VOLTAGE, (REACTION_VOLTGAE / (2.602 - 3))};

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // BMP280
  if (!bmp.begin()) {
    Serial.println("Error: No se encontró el BMP280. Revisa el cableado.");
    while (1);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  // SD
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    Serial.println("Error: Inicialización de la tarjeta SD fallida.");
    while (1);
  }
  if (!logFile.open("COMPASAT.txt", O_CREAT | O_WRITE | O_APPEND)) {
    Serial.println("Error al crear o abrir el archivo en la SD.");
    while (1);
  }
  logFile.println("PAQ,TEMP.,PRES.,ALT.,LAT.,LON.,SAT.");
  logFile.close();

  // GPS
  altSerial.begin(9600);

  // MOTOR
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  // LED
  pinMode(pinLed, OUTPUT);

  Serial.println(F("Sistema inicializado correctamente."));
}

void loop() {
  // BMP280
  float temperatura = bmp.readTemperature();
  float presion = bmp.readPressure();
  float altitud = bmp.readAltitude(1013.25); // Ajustar según la presión a nivel del mar

  // GPS
  unsigned long start = millis();
  while (altSerial.available() > 0 && millis() - start < 100) { // Timeout after 100ms
      gps.encode(altSerial.read());
  }

  // Recibir datos

  float latitud = gps.location.isValid() ? gps.location.lat() : NAN;
  float longitud = gps.location.isValid() ? gps.location.lng() : NAN;
  int satelites = gps.satellites.isValid() ? gps.satellites.value() : 0;

  float voltajeCO2 = MGRead(MG_PIN);
  int ppmCO2 = MGGetPercentage(voltajeCO2, CO2Curve);

  // Mostrar datos en el puerto serie
  Serial.print(idPaquete); Serial.print(",");
  Serial.print(temperatura); Serial.print(",");
  Serial.print(presion); Serial.print(",");
  Serial.print(altitud); Serial.print(",");
  Serial.print(latitud, 6); Serial.print(",");
  Serial.print(longitud, 6); Serial.print(",");
  Serial.print(satelites); Serial.print(",");
  Serial.print(ppmCO2 == -1 ? "-1" : String(ppmCO2)); Serial.print(",");
  Serial.println("COMPASAT25");

  // Escribir datos en la SD
  if (logFile.open("COMPASAT.txt", O_WRITE | O_APPEND)) {
    logFile.print(idPaquete); logFile.print(",");
    logFile.print(temperatura); logFile.print(",");
    logFile.print(presion); logFile.print(",");
    logFile.print(altitud); logFile.print(",");
    logFile.print(latitud, 6); logFile.print(",");
    logFile.print(longitud, 6); logFile.print(",");
    logFile.print(satelites); logFile.print(",");
    logFile.print(ppmCO2 == -1 ? -1 : String(ppmCO2)); logFile.print(",");
    logFile.println("COMPASAT25");
    logFile.close();
  } else {
    Serial.println("Error al escribir en la SD.");
  }

  // LED
  digitalWrite(pinLed, HIGH);
  delay(100);
  digitalWrite(pinLed, LOW);

  if (idPaquete == 5 || idPaquete == 15) {
      moveStepper(stepsPer90, true);
  }

  // Incrementar ID del paquete y esperar
  idPaquete++;
  delay(1000);
}

// CO2

float MGRead(int mg_pin) {
  float v = 0;
  for (int i = 0; i < 5; i++) {
    v += analogRead(mg_pin);
    delay(50);
  }
  v = (v / 5) * 5.0 / 1024;
  return v;
}

int MGGetPercentage(float volts, float *pcurve) {
  if ((volts / DC_GAIN) >= ZERO_POINT_VOLTAGE) {
    return -1;
  } else {
    return pow(10, ((volts / DC_GAIN) - pcurve[1]) / pcurve[2] + pcurve[0]);
  }
}

// MOTOR

void moveStepper(int steps, bool clockwise) {
    for (int i = 0; i < steps; i++) {
        if (clockwise) {
            stepCounter++;
            if (stepCounter >= numSteps) stepCounter = 0;
        } else {
            stepCounter--;
            if (stepCounter < 0) stepCounter = numSteps - 1;
        }
        setOutput(stepCounter);
        delayMicroseconds(motorSpeed);
    }
}

void setOutput(int step) {
    digitalWrite(motorPin1, bitRead(stepsLookup[step], 0));
    digitalWrite(motorPin2, bitRead(stepsLookup[step], 1));
    digitalWrite(motorPin3, bitRead(stepsLookup[step], 2));
    digitalWrite(motorPin4, bitRead(stepsLookup[step], 3));
}