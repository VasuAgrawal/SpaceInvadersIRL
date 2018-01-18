#include <Servo.h>

Servo motorLeftBack;
Servo motorRight;

void setup() {
  // put your setup code here, to run once:
  motorLeftBack.attach(6);
  motorRight.attach(9);

  Serial.begin(115200);
}

void loop() {

  int i = Serial.parseInt();
  motorLeftBack.write(i);
  motorRight.write(i);
}
