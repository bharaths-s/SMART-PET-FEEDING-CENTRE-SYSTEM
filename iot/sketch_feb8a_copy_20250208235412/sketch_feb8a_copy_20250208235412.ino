#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Servo.h>

// Pin Definitions
#define TRIG_PIN 12    // GPIO12 (D6 on NodeMCU)
#define ECHO_PIN 14    // GPIO14 (D5 on NodeMCU)
#define SERVO_PIN 4    // GPIO4 (D2 on NodeMCU)
#define RELAY_PIN 13   // GPIO13 (D7 on NodeMCU)
#define MOISTURE_PIN A0

Servo servo;

// Wi-Fi Credentials
const char* ssid = "OPPO A76";
const char* password = "123456789";

// ThingSpeak Details
unsigned long channelID = 2804055;  // Replace with your ThingSpeak Channel ID
const char* writeAPIKey = "XF1GD9JUZA0YAXIU";  // Replace with your ThingSpeak Write API Key

WiFiClient client;

void setup() {
  Serial.begin(115200);
  Serial.println("Setup started...");

  // Initialize Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(MOISTURE_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  // Attach Servo
  servo.attach(SERVO_PIN);
  servo.write(0); // Ensure servo starts in closed position
  Serial.println("Servo attached");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
  Serial.println("ThingSpeak initialized");
}

void loop() {
  // Food Level Check
  int distance = getDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    controlServo(distance);

    delay(2000); // Delay for 2 seconds

  // Water Level Check
  int moistureLevel = getMoistureLevel();
  Serial.print("Moisture Level: ");
  Serial.println(moistureLevel);

  if (moistureLevel>800) {
    refillWater();
  } else {
    Serial.println("Moisture level is sufficient");
  }

  // Send Data to ThingSpeak
  ThingSpeak.setField(1, distance);
  ThingSpeak.setField(2, moistureLevel);
  int response = ThingSpeak.writeFields(channelID, writeAPIKey);

  if (response == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.print("Error sending data to ThingSpeak: ");
    Serial.println(response);
  }

  delay(15000); // Update every 15 seconds
}

int getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout after 30ms
  if (duration == 0) {
    Serial.println("Ultrasonic sensor timeout!");
    return -1; // Indicate error
  }

  int distance = duration * 0.034 / 2; // Convert duration to distance (cm)
  return distance;
}

int getMoistureLevel() {
  int moistureValue = analogRead(MOISTURE_PIN);
  return moistureValue;
}

void controlServo(int distance) {
    if (distance > 5 && distance <= 100) {
        Serial.println("Moving servo to 90°...");
        servo.write(90);
    
    } else {
        Serial.println("Moving servo to 0°...");
        servo.write(0);
    }
}

void refillWater() {
  Serial.println("Refilling water...");
  digitalWrite(RELAY_PIN, HIGH);
  delay(2000); // Run pump for 5 seconds
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Water refilled");
}
