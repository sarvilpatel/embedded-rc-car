// ---- Motor pins ----
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

// ---- Enable pins (PWM) ----
#define ENA 9
#define ENB 10

// ---- Ultrasonic Sensor pins ----
#define TRIG 2
#define ECHO 3

// ---- Speed state ----
int speedPWM = 255; // updated by slider
int lastPWM = 0;

const int MIN_PWM = 100;
const int MAX_PWM = 255;

// ---- Drive state machine ----
enum Mode { MODE_STOP, MODE_FWD, MODE_BACK, MODE_LEFT, MODE_RIGHT };
Mode mode = MODE_STOP;

// ---- Helpers ----
int stepToPwm(int step) {
  step = constrain(step, 1, 10);
  return map(step, 1, 10, MIN_PWM, MAX_PWM);
}

void writeSpeed(int pwm) {
  speedPWM = constrain(pwm, 0, 255);
  analogWrite(ENA, speedPWM);
  analogWrite(ENB, speedPWM);
}

// NEW: stop motors but keep current mode (used for obstacle stop/resume)
void motorStopKeepMode() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// Original stopAll for explicit 'S' command
void stopAll() {
  motorStopKeepMode();
  mode = MODE_STOP; // remember we are stopped
}

void forward() {
  writeSpeed(speedPWM);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void backward() {
  writeSpeed(speedPWM);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

// Your original L/R mapping
void leftTurn() {
  writeSpeed(speedPWM);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void rightTurn(){
  writeSpeed(speedPWM);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

// ---- Ultrasonic Distance ----
int getDistanceCM() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long duration = pulseIn(ECHO, HIGH, 20000); // 20ms timeout
  if (duration <= 0) return 999;
  int cm = (int)(duration * 0.034 / 2);
  if (cm < 2) cm = 2;
  if (cm > 400) cm = 400;
  return cm;
}

// tiny smoother: median of 3
int distanceMedian3() {
  int a = getDistanceCM(), b = getDistanceCM(), c = getDistanceCM();
  int lo = min(a,b), hi = max(a,b);
  return max(lo, min(hi, c));
}

// ---- Adaptive stop threshold (fast=30 as you asked) ----
int stopThresholdForSpeed(int pwm) {
  if (pwm > 200) return 30; // fast
  if (pwm >= 150) return 22; // medium
  return 15; // slow
}

// ---- Resume hysteresis (prevents jitter) ----
int resumeThresholdForSpeed(int pwm) {
  return stopThresholdForSpeed(pwm) + 6; // ~6cm above stop
}

void setup() {
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  stopAll();
  Serial.begin(9600);
}

void handleSpeedFromChar(char firstDigit) {
  String buf = "";
  buf += firstDigit;
  delay(10);
  while (Serial.available()) {
    char n = Serial.peek();
    if (n >= '0' && n <= '9') buf += (char)Serial.read();
    else break;
  }
  int step = buf.toInt();
  if (step == 0) step = 10; // some apps send 0 for 10
  step = constrain(step, 1, 10);
  writeSpeed(stepToPwm(step));
  lastPWM = speedPWM;
}

void loop() {
  // 1) Handle incoming commands (update mode or speed)
  while (Serial.available()) {
    char c = Serial.read();
    if (c >= '0' && c <= '9') {
      handleSpeedFromChar(c);
      continue;
    }
    switch (c) {
      case 'F': mode = MODE_FWD;   break;
      case 'B': mode = MODE_BACK;  break;
      case 'L': mode = MODE_LEFT;  break;
      case 'R': mode = MODE_RIGHT; break;
      case 'S': stopAll();         break;
      default: break;
    }
  }

  // 2) Enforce behavior continuously based on mode
  if (mode == MODE_FWD) {
    int d = distanceMedian3();
    int STOP_CM = stopThresholdForSpeed(speedPWM);
    int RESUME_CM = resumeThresholdForSpeed(speedPWM);

    static bool blocked = false;

    if (!blocked) {
      if (d <= STOP_CM) {
        motorStopKeepMode();  // stop near obstacle but keep MODE_FWD
        blocked = true;
      } else {
        forward();
      }
    } else {
      // blocked: only resume when clear enough
      if (d >= RESUME_CM) {
        blocked = false;
        forward();
      } else {
        motorStopKeepMode();  // stay stopped but keep MODE_FWD
      }
    }
  }
  else if (mode == MODE_BACK) { backward(); }
  else if (mode == MODE_LEFT) { leftTurn(); }
  else if (mode == MODE_RIGHT){ rightTurn(); }
  // MODE_STOP already stopped

  // small cycle delay (optional)
  delay(10);
}
