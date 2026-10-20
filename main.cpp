#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Pin Definitions (Joystick Only) ---
const int JOY_HORZ   = A0;  // Left/Right: Adjust Focus Minutes (when paused)
const int JOY_VERT   = A1;  // Up/Down: Toggle 60x Demo Speed vs 1x Real Time
const int JOY_SEL    = 2;   // Joystick Center Click: Start / Pause Timer

const int LED_GREEN  = 4;   // Break Mode (Available / Free)
const int LED_YELLOW = 5;   // Paused / Standby
const int LED_RED    = 6;   // Focus Mode (Do Not Disturb!)

const int BUZZER_PIN = 8;
const int SERVO_PIN  = 9;

Servo dndFlagServo;

// --- Pomodoro State Variables ---
bool isRunning = false;
bool isFocusPhase = true;   // true = Focus, false = Break
bool turboDemoMode = true;  // true = 1 real sec counts as 1 min for fast demo

int focusDurationMin = 15;  // Adjustable 5 to 45 mins
int breakDurationMin = 5;
int remainingSeconds = 15 * 60;
int completedSessions = 0;
int flagAngle = 0;

unsigned long lastTickMs = 0;
unsigned long lastInputMs = 0;

void setup() {
  pinMode(JOY_SEL, INPUT_PULLUP);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  dndFlagServo.attach(SERVO_PIN);
  dndFlagServo.write(0);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;);
  }
  display.setTextColor(SSD1306_WHITE);
  updateHardwareOutputs();
}

void loop() {
  handleJoystickInput();
  updatePomodoroTimer();
  drawPomodoroScreen();
  delay(30);
}

void handleJoystickInput() {
  // Click Joystick Center (SEL) to Start / Pause
  if (digitalRead(JOY_SEL) == LOW && (millis() - lastInputMs > 300)) {
    lastInputMs = millis();
    isRunning = !isRunning;
    lastTickMs = millis();
    tone(BUZZER_PIN, isRunning ? 1200 : 600, 100);
    updateHardwareOutputs();
  }

  if (millis() - lastInputMs > 220) {
    int horz = analogRead(JOY_HORZ);
    int vert = analogRead(JOY_VERT);

    // Joystick Up/Down toggles 60x Demo Speed vs 1x Real Time
    if (vert > 700) {
      turboDemoMode = true;
      tone(BUZZER_PIN, 1400, 40);
      lastInputMs = millis();
    } else if (vert < 300) {
      turboDemoMode = false;
      tone(BUZZER_PIN, 800, 40);
      lastInputMs = millis();
    }

    // When Paused, Joystick Left/Right adjusts Focus Duration (5 to 45 mins)
    if (!isRunning && isFocusPhase) {
      if (horz < 300 && focusDurationMin < 45) { // Right = Increase
        focusDurationMin += 5;
        remainingSeconds = focusDurationMin * 60;
        tone(BUZZER_PIN, 1050, 30);
        lastInputMs = millis();
      } else if (horz > 700 && focusDurationMin > 5) { // Left = Decrease
        focusDurationMin -= 5;
        remainingSeconds = focusDurationMin * 60;
        tone(BUZZER_PIN, 850, 30);
        lastInputMs = millis();
      }
    }
  }
}

void updatePomodoroTimer() {
  if (!isRunning) return;

  if (millis() - lastTickMs >= 1000) {
    lastTickMs = millis();
    int stepSec = turboDemoMode ? 60 : 1;
    remainingSeconds -= stepSec;

    if (remainingSeconds <= 0) {
      if (isFocusPhase) {
        completedSessions++;
        isFocusPhase = false;
        remainingSeconds = breakDurationMin * 60;
        playPhaseChime(true);
      } else {
        isFocusPhase = true;
        remainingSeconds = focusDurationMin * 60;
        playPhaseChime(false);
      }
      updateHardwareOutputs();
    }
  }
}

void updateHardwareOutputs() {
  if (!isRunning) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_GREEN, LOW);
    flagAngle = 0;
  } else if (isFocusPhase) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);
    flagAngle = 90; // Raise Do Not Disturb Flag
  } else {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, HIGH);
    flagAngle = 0;  // Lower Do Not Disturb Flag
  }
  dndFlagServo.write(flagAngle);
}

void drawPomodoroScreen() {
  int mins = remainingSeconds / 60;
  int secs = remainingSeconds % 60;
  int totalSec = (isFocusPhase ? focusDurationMin : breakDurationMin) * 60;
  int barWidth = map(remainingSeconds, 0, totalSec, 0, 108);

  display.clearDisplay();

  // Top Header Bar
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (!isRunning) {
    display.print("POMODORO : PAUSED");
  } else if (isFocusPhase) {
    display.print("FOCUS MODE (DND ON)");
  } else {
    display.print("BREAK MODE (FREE)");
  }
  display.drawLine(0, 9, 128, 9, SSD1306_WHITE);

  // Large Countdown Clock + Servo Flag Status
  display.setTextSize(2);
  display.setCursor(6, 15);
  if (mins < 10) display.print("0");
  display.print(mins);
  display.print(":");
  if (secs < 10) display.print("0");
  display.print(secs);

  display.setTextSize(1);
  display.setCursor(78, 15);
  display.print("FLAG:");
  display.setCursor(78, 25);
  display.print(flagAngle == 90 ? "UP 90d" : "DN  0d");

  // Progress Bar
  display.drawRect(8, 36, 112, 7, SSD1306_WHITE);
  display.fillRect(10, 38, constrain(barWidth, 0, 108), 3, SSD1306_WHITE);

  // Stats & Controls Footer
  display.setCursor(4, 46);
  display.print("Done:");
  display.print(completedSessions);
  display.print(" | Spd:");
  display.print(turboDemoMode ? "60xDemo" : "1xReal");

  display.setCursor(4, 56);
  display.print("Click:Run L/R:Min U/D");

  display.display();
}

void playPhaseChime(bool toBreak) {
  if (toBreak) {
    tone(BUZZER_PIN, 880, 120);
    delay(150);
    tone(BUZZER_PIN, 1320, 200);
  } else {
    tone(BUZZER_PIN, 660, 120);
    delay(150);
    tone(BUZZER_PIN, 880, 200);
  }
}