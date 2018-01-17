int targetStates[10];
int prevTargetStates[10];
int targetHealth[10];
int targetSettle[10];

int pinOffset = 2;
int startingHealth = 3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(1200);
  for(int i = 0; i < 1; i++) {
    pinMode(i + pinOffset, INPUT);
    targetHealth[i] = startingHealth;
    prevTargetStates[i] = 1;
    targetSettle[i] = 1;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < 1; i++) {
    targetStates[i] = digitalRead(i + pinOffset);
    if(targetStates[i] == HIGH) {
      if(prevTargetStates[i] == 1 && targetSettle[i] >= 10000) {
        targetHealth[i]--;
        Serial.print("Hit!");
      }
      prevTargetStates[i] = 0;
      targetSettle[i] = 0;
    }
    else {
      prevTargetStates[i] = 1;
      if(targetSettle[i] < 10000) {
        targetSettle[i]++;
      }
    }
    //Serial.print(targetStates[i]);
  }
  
}
