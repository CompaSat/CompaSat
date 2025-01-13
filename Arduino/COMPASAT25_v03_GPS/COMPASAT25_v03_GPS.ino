#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SdFat.h>
#include <TinyGPS++.h>
#include <AltSoftSerial.h>

// Configuración para BMP280
Adafruit_BMP280 bmp;

// Configuración para SD utilizando SdFat
SdFat sd;
SdFile logFile;
const int chipSelect = 10; // Pin CS de la tarjeta SD

// Configuración de LED
int pinLed = 7; // Pin del LED indicador
int idPaquete = 1; // Contador de paquetes

// Configuración para GPS
AltSoftSerial altSerial;
TinyGPSPlus gps;

void setup() {
  // Inicialización del puerto serie
  Serial.begin(9600);
  while (!Serial);

  // Inicialización del BMP280
  if (!bmp.begin()) {
    Serial.println("Error: No se encontró el BMP280. Revisa el cableado.");
    while (1);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  // Inicialización de la tarjeta SD utilizando SdFat
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

  // Inicialización del GPS
  altSerial.begin(9600); // Iniciar AltSoftSerial a 9600 baudios

  // Configuración del LED
  pinMode(pinLed, OUTPUT);

  Serial.println("Sistema inicializado correctamente.");
}

void loop() {
  // Leer datos del BMP280
  float temperatura = bmp.readTemperature();
  float presion = bmp.readPressure();
  float altitud = bmp.readAltitude(1013.25); // Ajusta según la presión a nivel del mar

  // Leer datos del GPS
  while (altSerial.available() > 0) {
    char c = altSerial.read();
    gps.encode(c);
  }

  float latitud = gps.location.isValid() ? gps.location.lat() : NAN;
  float longitud = gps.location.isValid() ? gps.location.lng() : NAN;
  int satelites = gps.satellites.isValid() ? gps.satellites.value() : 0;

  // Mostrar datos en el puerto serie
  Serial.print(idPaquete); Serial.print(",");
  Serial.print(temperatura); Serial.print(",");
  Serial.print(presion); Serial.print(",");
  Serial.print(altitud); Serial.print(",");
  Serial.print(latitud, 6); Serial.print(",");
  Serial.print(longitud, 6); Serial.print(",");
  Serial.println(satelites);

  // Escribir datos en la SD utilizando SdFat
  if (logFile.open("COMPASAT.txt", O_WRITE | O_APPEND)) {
    logFile.print(idPaquete); logFile.print(",");
    logFile.print(temperatura); logFile.print(",");
    logFile.print(presion); logFile.print(",");
    logFile.print(altitud); logFile.print(",");
    logFile.print(latitud, 6); logFile.print(",");
    logFile.print(longitud, 6); logFile.print(",");
    logFile.println(satelites);
    logFile.close();
  } else {
    Serial.println("Error al escribir en la SD.");
  }

  // Encender y apagar el LED
  digitalWrite(pinLed, HIGH);
  delay(100);
  digitalWrite(pinLed, LOW);

  // Incrementar ID del paquete y esperar
  idPaquete++;
  delay(1000);
}
