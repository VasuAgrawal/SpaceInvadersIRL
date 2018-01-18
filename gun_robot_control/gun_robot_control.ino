#include <Servo.h>

#define TRIGGER_SIDE_MOTOR_PORT 5
#define BARREL_SIDE_MOTOR_PORT 6
#define SHOOTING_SERVO_PORT 11

#define TRIGGER_PULLED 102
#define TRIGGER_RELEASED 90

Servo trigger_side_motor;
Servo barrel_side_motor;
Servo shooting_servo;

int servoize(float in) {
  return int(in * 90 + 90);
}

void setup() {

  Serial.begin(115200);

  trigger_side_motor.attach(TRIGGER_SIDE_MOTOR_PORT);
  barrel_side_motor.attach(BARREL_SIDE_MOTOR_PORT);
  
  shooting_servo.attach(SHOOTING_SERVO_PORT);
  shooting_servo.write(TRIGGER_RELEASED);
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
    float barrel_side_vel = Serial.parseFloat();
    float trigger_side_vel = Serial.parseFloat();
    int pull_trigger = Serial.parseInt();

    // The floats will be from -1 to 1. Convert from 0 to 180
    barrel_side_motor.write(servoize(barrel_side_vel));
    trigger_side_motor.write(servoize(trigger_side_vel));

    // The integer will be either 0 or 1.
    // Use the experimentally determined shooting values.
    if (pull_trigger) {
      shooting_servo.write(TRIGGER_PULLED);
    } else {
      shooting_servo.write(TRIGGER_RELEASED);
    }
  }
}
