#include <Servo.h>


#define FRONT_RIGHT 11
#define FRONT_LEFT 10
#define BACK_LEFT 9
#define BACK_RIGHT 6

Servo front_right;
Servo front_left;
Servo back_left;
Servo back_right;

int servoize(float in) {
  return int(in * 90 + 90);
}

void setup() {

  Serial.begin(115200);

  front_right.attach(FRONT_RIGHT);
  front_left.attach(FRONT_LEFT);
  back_left.attach(BACK_LEFT);
  back_right.attach(BACK_RIGHT);
}

void loop() {

  if (Serial.available() > 0) {
    // Read the @ sign
    while (true) {
      int incoming = Serial.read();
      if (incoming == '@') {
        break;
      }
    }

    // @[Left back] [Left front] [Right back] [Right front]
    float lb = Serial.parseFloat();
    float lf = Serial.parseFloat();
    float rb = Serial.parseFloat();
    float rf = Serial.parseFloat();

    // The floats will be from -1 to 1. Convert from 0 to 180
    front_right.write(servoize(rf));
    front_left.write(servoize(lf));
    back_left.write(servoize(lb));
    back_right.write(servoize(rb));
  }
}
