#include <AccelStepper.h>

// Definir los pines de conexión para un motor paso a paso bipolar con 4 cables
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Crear un objeto AccelStepper en modo FULL4WIRE
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN2, IN3, IN4);

// Definir parámetros del motor
const float degrees = 90;
const int STEPS_PER_REV = 2048; // Pasos por revolución (ajustar según el motor)
const float degrees_to_steps = STEPS_PER_REV / (360 / degrees); // Grados a pasos
const int DURATION = 0; // Duración en milisegundos

void setup() {
  stepper.setMaxSpeed(1000); // Ajustar velocidad máxima
  stepper.setAcceleration(1000); // Ajustar aceleración para suavizar el movimiento
  stepper.setCurrentPosition(0); // Establecer posición inicial
  Serial.begin(115200);

}

void loop() {
  stepper.setCurrentPosition(0);

  while(Serial.available() == 0) { }
  Serial.read();
  delay(DURATION);

  // Mover x grados hacia adelante
  stepper.moveTo(degrees_to_steps);
  while (stepper.distanceToGo() != 0) {
    stepper.run();
    // Serial.println(stepper.speed());
  }
  
  while(Serial.available() == 0) { }
  Serial.read();
  delay(DURATION);
  
  // Mover 180 grados hacia atrás (volver a la posición inicial)
  stepper.moveTo(0);
  while (stepper.distanceToGo() != 0) {
    stepper.run();
    // Serial.println(stepper.speed());
  }
  
}
