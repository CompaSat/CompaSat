// CANSAT 2024
//  V01: SENSOR BMP280
//  V02: SENSOR BMP280 + BALIZA

/* 1. LIBRERÍAS */
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>


/* 2. DEFINICIÓN CONSTANTES E VARIABLES */

// Sensor BMP280
Adafruit_BMP280 bmp; // I2C

int pinLed = 9;     // led func. global

int idPaquete = 1;  // Def. paquete envío


/* 3. SETUP */
void setup() {
  
  Serial.begin(9600);
  while ( !Serial ) delay(100);   // wait for native usb
  unsigned status;
  
  // Led
  pinMode(pinLed, OUTPUT);

  // Sensor BMP
  status = bmp.begin();
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  // Sensor BMP
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

/* 4. LOOP */
void loop() {
  digitalWrite(pinLed, HIGH);
  Serial.print(idPaquete);
  Serial.print(", ");
  Serial.print(bmp.readTemperature());
  Serial.print(", ");
  Serial.print(bmp.readPressure());
  Serial.print(", ");
  Serial.print(bmp.readAltitude(1053.25));
  Serial.println(); /* Adjusted to local forecast! */
  idPaquete = idPaquete + 1;
  digitalWrite(pinLed, LOW);
  delay(1000);
}
