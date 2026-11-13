// Motor pins
#define PWM_L 9
#define PWM_R 10
#define DIR_L 15
#define DIR_R 16
#define ENC_L 2
#define ENC_R 3

// change if wheels are moving diff way
const int FORWARD_L = HIGH;
const int FORWARD_R = HIGH;

volatile long leftTicks = 0;
volatile long rightTicks = 0;

volatile int8_t leftDirection = 1;
volatile int8_t rightDirection = 1;

int step = 0;
unsigned long stepStart = 0;
unsigned long lastPrint = 0;

void setup() {
  // Set up motor and encoder pins
  pinMode(PWM_L, OUTPUT);
  pinMode(PWM_R, OUTPUT);
  pinMode(DIR_L, OUTPUT);
  pinMode(DIR_R, OUTPUT);
  pinMode(ENC_L, INPUT);
  pinMode(ENC_R, INPUT);

  analogWrite(PWM_L, 0);
  analogWrite(PWM_R, 0);
  digitalWrite(DIR_L, FORWARD_L);
  digitalWrite(DIR_R, FORWARD_R);

  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(ENC_L),
                  leftEncoderCallback, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_R),
                  rightEncoderCallback, CHANGE);

  applyStep();
  stepStart = millis();
}

void loop() {
  unsigned long currentTime = millis();

  // Move to next step after 5 seconds bc, "Each step should run for 5 seconds."
  if (step < 12 && currentTime - stepStart >= 5000) {
    step++;
    applyStep();
    stepStart = millis();
  }

  // prints totals every 100 ms
  if (currentTime - lastPrint >= 100) {
    lastPrint = currentTime;

    // copys counts make sure no intrsupts to not stop (Had Help from copilot because it would not work without this)
    noInterrupts();
    long leftCount = leftTicks;
    long rightCount = rightTicks;
    interrupts();

    Serial.print("Left: ");
    Serial.print(leftCount);
    Serial.print(" Right: ");
    Serial.println(rightCount);
  }
}

void applyStep() {
  // Stop both motors before setting the next step
  analogWrite(PWM_L, 0);
  analogWrite(PWM_R, 0);

  // Left wheel
  if (step == 1) {
    leftDirection = 1;
    digitalWrite(DIR_L, FORWARD_L);
    analogWrite(PWM_L, 128);
  }
  else if (step == 2) {
    analogWrite(PWM_L, 255);
  }
  else if (step == 4) {
    leftDirection = -1;
    digitalWrite(DIR_L, !FORWARD_L);
    analogWrite(PWM_L, 128);
  }
  else if (step == 5) {
    analogWrite(PWM_L, 255);
  }

  // Right wheel
  else if (step == 7) {
    rightDirection = 1;
    digitalWrite(DIR_R, FORWARD_R);
    analogWrite(PWM_R, 128);
  }
  else if (step == 8) {
    analogWrite(PWM_R, 255);
  }
  else if (step == 10) {
    rightDirection = -1;
    digitalWrite(DIR_R, !FORWARD_R);
    analogWrite(PWM_R, 128);
  }
  else if (step == 11) {
    analogWrite(PWM_R, 255);
  }
  // other steps none bc it is stopped
}

void leftEncoderCallback() {
  leftTicks += leftDirection;
}

void rightEncoderCallback() {
  rightTicks += rightDirection;
}