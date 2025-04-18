int idPaquete = 1; // Contador de paquetes
// Configuración del motor paso a paso
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

void setup() {
  Serial.begin(9600);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

}

void loop() {
  if (idPaquete == 5) {
    moveStepper(stepsPer90, true);
    delay(1000);
    moveStepper(stepsPer90, false);
    idPaquete = 1;
  }

  // Incrementar ID del paquete y esperar
  Serial.println(idPaquete);
  idPaquete++;
  delay(1000);

}

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
