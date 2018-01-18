#include <Servo.h>

#define TRIGGER 5
#define BARREL 6
#define SHOOT 11

Servo trigger;
Servo barrel;
Servo shoot;

int servoize(float in) {
  return int(in * 90 + 90);
}

void setup() {

  Serial.begin(115200);

  trigger.attach(TRIGGER);
  barrel.attach(BARREL);
  shoot.attach(SHOOT);
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

    // @[Barrel] [Trigger]
    float bar = Serial.parseFloat();
    float trig = Serial.parseFloat();
    float sht = Serial.parseFloat();

    // The floats will be from -1 to 1. Convert from 0 to 180
    barrel.write(servoize(bar));
    trigger.write(servoize(trig));
    shoot.write(servoize(sht));
  }
}
