/*
 * @file main.cpp
 * @brief IoT Military & Railway Surveillance Robot Firmware
 * @details Features ESP32-based remote driving via Arduino IoT Cloud, 
 *          obstacle avoidance using Ultrasonic sensor, and fire detection.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <LiquidCrystal.h>

/* ================= LCD PIN CONFIGURATION ================= */
// Pins: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(2, 15, 19, 18, 5, 4);

/* ================= MOTOR DRIVER (L298N) ================= */
#define IN1 32
#define IN2 33
#define IN3 21
#define IN4 22

/* ================= ULTRASONIC SENSOR ================= */
#define TRIG_FRONT 13
#define ECHO_FRONT 12
#define OBSTACLE_THRESHOLD 15 // Distance threshold in cm

/* ================= FIRE SENSOR ================= */
#define FIRE_PIN 34 // Digital Output (DO) pin of fire sensor

/* ================= IOT CREDENTIALS (PLACEHOLDERS FOR GITHUB) ================= */
// NOTE: Replace these placeholders with your actual credentials in your local working copy.
const char DEVICE_LOGIN_NAME[] = "YOUR_DEVICE_LOGIN_NAME_HERE";
const char SSID[]              = "YOUR_WIFI_SSID_HERE";
const char PASS[]              = "YOUR_WIFI_PASSWORD_HERE";
const char DEVICE_KEY[]        = "YOUR_SECRET_DEVICE_KEY_HERE";

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

/* ================= CLOUD VARIABLES ================= */
String contri;     // Movement commands: "F", "B", "L", "R", "S"
int distancei;     // Distance reading in cm
bool firei;        // Fire detection state flag
bool obstaclei;    // Obstacle detection state flag

/* ================= TIMING VARIABLES ================= */
unsigned long lastSensorReadTime = 0;
const unsigned long SENSOR_INTERVAL = 250; // Non-blocking delay for LCD & Sensors (250ms)

/* ================= FUNCTION DECLARATIONS ================= */
void initProperties();
void updateSensors();
void onContriChange();

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Railway Robot");
  delay(2000);
  lcd.clear();

  // Initialize Motor Pins
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); 
  pinMode(IN4, OUTPUT);

  // Initialize Sensor Pins
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  pinMode(FIRE_PIN, INPUT);

  // Ensure motors are initially turned off
  digitalWrite(IN1, LOW); 
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); 
  digitalWrite(IN4, LOW);

  // Initialize Arduino IoT Cloud Setup
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

/* ================= MAIN LOOP ================= */
void loop() {
  // Keep IoT Cloud connection active continuously
  ArduinoCloud.update();

  // Read sensors every 250ms using non-blocking timer to prevent control lag
  if (millis() - lastSensorReadTime >= SENSOR_INTERVAL) {
    lastSensorReadTime = millis();
    updateSensors();
  }
}

/* ================= SENSOR READINGS & LCD DISPLAY ================= */
void updateSensors() {
  long duration;
  long distance;

  // Trigger Ultrasonic Sensor Pulse
  digitalWrite(TRIG_FRONT, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_FRONT, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_FRONT, LOW);

  // Measure Echo Pulse Duration
  duration = pulseIn(ECHO_FRONT, HIGH, 30000);
  distance = duration * 0.034 / 2;

  distancei = (int)distance;
  obstaclei = (distance > 0 && distance <= OBSTACLE_THRESHOLD);

  // Read Digital Fire Sensor (LOW state indicates fire detected)
  int fireState = digitalRead(FIRE_PIN);
  firei = (fireState == LOW);

  // Update LCD Display without screen flicker
  lcd.setCursor(0, 0);
  lcd.print("C:");
  lcd.print(contri);
  lcd.print(" D:");
  lcd.print(distancei);
  lcd.print("cm ");
  lcd.print(obstaclei ? "OD" : "NO");
  lcd.print("  ");

  lcd.setCursor(0, 1);
  lcd.print(firei ? "FIRE: YES " : "FIRE: NO  ");

  // Serial Debug Logging
  Serial.print("CMD: "); Serial.print(contri);
  Serial.print(" | Distance: "); Serial.print(distancei);
  Serial.print(" | Obstacle: "); Serial.print(obstaclei);
  Serial.print(" | Fire: "); Serial.println(firei);
}

/* ================= IOT MOTOR CONTROL CALLBACK ================= */
void onContriChange() {
  Serial.print("Command Received: ");
  Serial.println(contri);

  // Safety Over-ride: Auto-stop moving forward if an obstacle is detected
  if (obstaclei && contri == "F") {
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
    Serial.println("AUTO STOP - Obstacle Detected!");
    return;
  }

  // Directional Driving Logic
  if (contri == "F") {        // Forward
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  } 
  else if (contri == "B") {   // Backward
    digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  } 
  else if (contri == "L") {   // Turn Left
    digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  } 
  else if (contri == "R") {   // Turn Right
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  } 
  else if (contri == "S") {   // Stop
    digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
  }
}

/* Unused Cloud Callbacks required by ArduinoIoTCloud */
void onDistanceiChange() {}
void onFireiChange() {}
void onObstacleiChange() {}

/* ================= CLOUD PROPERTIES SETUP ================= */
void initProperties() {
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);

  ArduinoCloud.addProperty(contri, READWRITE, ON_CHANGE, onContriChange);
  ArduinoCloud.addProperty(distancei, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(firei, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(obstaclei, READ, ON_CHANGE, NULL);
}