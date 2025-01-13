#include <SD.h>

const int chipSelect = 10; // Ajusta según tu conexión

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Inicializando tarjeta SD...");
  pinMode(chipSelect, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("Error al inicializar la tarjeta SD.");
    return;
  }

  Serial.println("Tarjeta SD inicializada correctamente.");

  File testFile = SD.open("test.txt", FILE_WRITE);
  if (testFile) {
    testFile.println("Prueba de escritura exitosa.");
    testFile.close();
    Serial.println("Archivo creado y datos escritos correctamente.");
  } else {
    Serial.println("Error al crear el archivo.");
  }
}

void loop() {
  // Nada aquí
}
