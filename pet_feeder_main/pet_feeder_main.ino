#include <Stepper.h>
const int motor1 = 11;
const int motor2 = 10;
const int motor3 = 9;
const int motor4 = 8;

Stepper motor = Stepper(4, motor1, motor2, motor3, motor4);


void setup() {

  motor.setSpeed(5000);
}

void loop() {
  motor.step(1);
  //delay(1000);
}
