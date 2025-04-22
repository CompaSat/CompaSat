// Librerías
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <SD.h>

// Definición de pines y constantes
Adafruit_BMP280 bmp;
const int chipSelect = 10; // Pin CS de la tarjeta SD
int pinLed = 9;            // Pin del LED indicador
int idPaquete = 1;         // Contador de paquetes
File logFile;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Esperar conexión serie (para USB nativo)

  // Inicialización del sensor BMP280
  Serial.println("1.1 TEST-BMP280-INICIO");
  if (!bmp.begin()) {
    Serial.println("Error: No se encontró el BMP280. Revisa el cableado.");
    while (1); // Detener ejecución si falla
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  Serial.println("1.2 TEST-BMP280-FIN");

  // Inicialización de la tarjeta SD
  Serial.println("2.1 TEST-SD-INICIO");
  pinMode(chipSelect, OUTPUT); // Asegurarse de que CS es salida
  if (!SD.begin(chipSelect)) {
    Serial.println("PIN 10: Inicialización FALLIDA!");
    while (1); // Detener ejecución si falla
  }
  Serial.println("Inicialización OK!");
  Serial.println("2.2 TEST-SD-FIN");

  // Crear archivo en la SD
  logFile = SD.open("COMPASAT.txt", FILE_WRITE);
  if (logFile) {
    Serial.println("Escribiendo encabezado en archivo SD...");
    logFile.println("PAQ,TEMP.,PRES.,ALT.,SAT.");
    logFile.close();
    Serial.println("Encabezado escrito correctamente.");
  } else {
    Serial.println("Error al abrir el archivo SD.");
  }

  // Configuración del LED indicador
  pinMode(pinLed, OUTPUT);
}

void loop() {
  // Encender LED indicador
  digitalWrite(pinLed, HIGH);

  // Leer datos del BMP280
  float temperatura = bmp.readTemperature();
  float presion = bmp.readPressure();
  float altitud = bmp.readAltitude(1013.25); // Ajusta el valor según la presión a nivel del mar

  // Mostrar datos por puerto serie
  Serial.print(idPaquete); Serial.print(",");
  Serial.print(temperatura); Serial.print(",");
  Serial.print(presion); Serial.print(",");
  Serial.print(altitud); Serial.print(",");
  Serial.println("COMPASAT25");

  // Escribir datos en la tarjeta SD
  logFile = SD.open("COMPASAT.txt", FILE_WRITE);
  if (logFile) {
    logFile.print(idPaquete); logFile.print(",");
    logFile.print(temperatura); logFile.print(",");
    logFile.print(presion); logFile.print(",");
    logFile.print(altitud); logFile.print(",");
    logFile.println("COMPASAT25");
    logFile.close();
  } else {
    Serial.println("Error al abrir el archivo SD.");
  }

  // Incrementar ID del paquete
  idPaquete++;

  // Apagar LED indicador
  digitalWrite(pinLed, LOW);

  // Esperar 1 segundo
  delay(1000);
}
