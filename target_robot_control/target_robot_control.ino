#include <Servo.h>
#include <PololuLedStrip.h>

#define FRONT_RIGHT 11
#define FRONT_LEFT 10
#define BACK_LEFT 9
#define BACK_RIGHT 6

#define T1_PORT 1
#define T2_PORT 2
#define T3_PORT 3
#define T4_PORT 4
#define T5_PORT 15
#define HIT_THRESHOLD 150
int targets[] = {T1_PORT, T2_PORT, T3_PORT, T4_PORT, T5_PORT};
const int NUM_TARGETS = sizeof(targets) / sizeof(targets[0]);
int hit_count[] = {-1, -1, -1, -1, -1};

PololuLedStrip<4> led_strip;
#define NUM_LEDS 47
rgb_color colors[NUM_LEDS];

const rgb_color hit_colors[] = {rgb_color(255, 0, 0), rgb_color(255, 255, 0), rgb_color(0, 0, 255)};
const int NUM_HIT_COLORS = sizeof(hit_colors) / sizeof(hit_colors[0]);

const size_t NUM_LED_SPACES = 4;
const int led_map[][NUM_LED_SPACES] = {
  {34, 35,  8,  9},
  {37, 38, 11, 12},
  {40, 40, 14, 14},
  {42, 43, 16, 17},
  {45, 46, 19, 20}
};

#define INTER_HIT_TIME 1000 /* Millis */
#define DISPLAY_TIME 2000 /* Millis */

Servo front_right;
Servo front_left;
Servo back_left;
Servo back_right;

long int last_hit = millis();

inline const int servoize(float in) {
  return int(in * 90 + 90);
}

void reset_leds() {
  for (int i = 0; i < NUM_LEDS; ++i) {
    colors[i] = rgb_color(0, 0, 0);
  }
  
  led_strip.write(colors, NUM_LEDS); 
}

void setup() {

  Serial.begin(115200);

  front_right.attach(FRONT_RIGHT);
  front_left.attach(FRONT_LEFT);
  back_left.attach(BACK_LEFT);
  back_right.attach(BACK_RIGHT);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  reset_leds();
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
    int reset = Serial.parseInt();

    // The floats will be from -1 to 1. Convert from 0 to 180
    front_right.write(servoize(rf));
    front_left.write(servoize(lf));
    back_left.write(servoize(lb));
    back_right.write(servoize(rb));

  }

  if (reset) {
    reset_leds();
  }

  // Go through all of the targets and check to see if they've been hit
  for (int i = 0; i < NUM_TARGETS; ++i) {
    int val = analogRead(targets[i]);
    if (val > HIT_THRESHOLD) {

      // If we detected a hit, only reset the hit timer if enough time has elapsed
      if ((millis() - last_hit) > INTER_HIT_TIME) {
        last_hit = millis();
        Serial.print("Detected a hit on target ");
        Serial.println(i);
        hit_count[i] += 1;

        for (int j = 0; j < NUM_LED_SPACES; ++j) {
          colors[led_map[i][j]] = hit_colors[min(hit_count[i], NUM_HIT_COLORS - 1)];
        }
        
        led_strip.write(colors, NUM_LEDS);
        
      }
    }
  }  
}
