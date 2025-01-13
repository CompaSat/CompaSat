#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

// Configuración de pines para el módulo GPS
static const int RXPin = 4, TXPin = 3; // RX y TX del GPS
static const uint32_t GPSBaud = 9600;  // Velocidad de comunicación con el GPS

TinyGPSPlus gps;                  // Objeto de la librería TinyGPS++
SoftwareSerial ss(RXPin, TXPin);  // Comunicación serial con el GPS
const int chipSelect = 10;        // Pin CS de la tarjeta SD
File logFile;                     // Archivo para la SD

void setup() {
  Serial.begin(9600);  // Comunicación con el monitor serie
  ss.begin(GPSBaud);   // Comunicación con el módulo GPS

  // Inicialización de la tarjeta SD
  Serial.println("Inicializando tarjeta SD...");
  pinMode(chipSelect, OUTPUT); // Asegurarse de que CS es salida
  if (!SD.begin(chipSelect)) {
    Serial.println("Error: No se pudo inicializar la tarjeta SD.");
    while (1);
  }
  Serial.println("Tarjeta SD inicializada correctamente.");
}

void loop() {
  // Leer datos del GPS
  while (ss.available() > 0) {
    char c = ss.read();
    gps.encode(c);
  }

  // Escribir datos ficticios en la tarjeta SD
  logFile = SD.open("PRUEBA.txt", FILE_WRITE);
  if (logFile) {
    logFile.println("Prueba de escritura en SD.");
    logFile.close();
    Serial.println("Datos escritos en SD.");
  } else {
    Serial.println("Error al escribir en la SD.");
  }

  delay(1000); // Esperar un segundo
}
