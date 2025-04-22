#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SdFat.h>
#include <TinyGPS++.h>
#include <AltSoftSerial.h>

// Configuración para BMP280
Adafruit_BMP280 bmp;
float presionAlNivelDelMar = 1004.8;

// Configuración para SD utilizando SdFat
SdFat sd;
SdFile logFile;
const int chipSelect = 10; // Pin CS de la tarjeta SD

// Configuración de LED
int pinLed = 5; // Pin del LED indicador
int idPaquete = 1; // Contador de paquetes

// Configuración para GPS
AltSoftSerial altSerial;
TinyGPSPlus gps;

// Motor paso a paso
const int motorPin1 = 3;
const int motorPin2 = 4;
const int motorPin3 = 6;
const int motorPin4 = 7;
int motorSpeed = 1000;
int stepCounter = 0;
const int stepsPerRev = 4076;
const int stepsPer90 = stepsPerRev / 4;
const int numSteps = 8;
const int stepsLookup[8] = { B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001 };

// Control de altura
float alturaMaxima = -10000;
bool motorAbierto = false;

// Motor no bloqueante
bool moviendoMotor = false;
int pasosRestantes = 0;
unsigned long tiempoUltimoPaso = 0;
const int intervaloPaso = 1000; // microsegundos
bool direccionMotor = true; // true = horario

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!bmp.begin()) {
    Serial.println("Error: No se encontró el BMP280.");
    while (1);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    Serial.println("Error al inicializar la SD.");
    while (1);
  }
  if (!logFile.open("COMPASAT.txt", O_CREAT | O_WRITE | O_APPEND)) {
    Serial.println("Error al abrir archivo en SD.");
    while (1);
  }
  logFile.println("PAQ,TEMP.,PRES.,ALT.,LAT.,LON.,SAT.");
  logFile.close();

  altSerial.begin(9600);

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(pinLed, OUTPUT);

  Serial.println(F("Sistema inicializado correctamente."));
}

void loop() {
  float temperatura = bmp.readTemperature();
  float presion = bmp.readPressure();
  float altitud = bmp.readAltitude(presionAlNivelDelMar);

  // Registrar altura
  if (altitud > alturaMaxima) alturaMaxima = altitud;

    // Motor: abrir si está bajando 5 metros desde el máximo
  if (!motorAbierto && altitud < (alturaMaxima - 5)) {
    moveStepper(stepsPer90, true);
    motorAbierto = true;
    Serial.println("Motor abierto");
  }

  // Leer datos del GPS
  unsigned long start = millis();
  while (altSerial.available() > 0 && millis() - start < 100) {
    gps.encode(altSerial.read());
  }

  float latitud = gps.location.isValid() ? gps.location.lat() : NAN;
  float longitud = gps.location.isValid() ? gps.location.lng() : NAN;
  int satelites = gps.satellites.isValid() ? gps.satellites.value() : 0;

  // Mostrar datos por puerto serie
  Serial.print(idPaquete); Serial.print(",");
  Serial.print(temperatura); Serial.print(",");
  Serial.print(presion); Serial.print(",");
  Serial.print(altitud); Serial.print(",");
  Serial.print(latitud, 6); Serial.print(",");
  Serial.print(longitud, 6); Serial.print(",");
  Serial.print(satelites); Serial.print(",");
  Serial.println("COMPASAT25");

  // Guardar datos en la SD
  if (logFile.open("COMPASAT.txt", O_WRITE | O_APPEND)) {
    logFile.print(idPaquete); logFile.print(",");
    logFile.print(temperatura); logFile.print(",");
    logFile.print(presion); logFile.print(",");
    logFile.print(altitud); logFile.print(",");
    logFile.print(latitud, 6); logFile.print(",");
    logFile.print(longitud, 6); logFile.print(",");
    logFile.print(satelites); logFile.print(",");
    logFile.println("COMPASAT25");
    logFile.close();
  } else {
    Serial.println("Error al escribir en la SD.");
  }

  // Encender LED
  digitalWrite(pinLed, HIGH);
  delay(100);
  digitalWrite(pinLed, LOW);

  idPaquete++;
  delay(1000);
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