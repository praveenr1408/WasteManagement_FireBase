#include <WiFi.h>
#include <FirebaseESP32.h>

// Set up your Firebase credentials
#define FIREBASE_HOST "my-app-3de95-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "YwOotL0dOX4oiqAPoWYnik5Rn8fAq8nmkjOLmPXA"

// Set up your WiFi credentials
const char* ssid = "Redmi";
const char* password = "876543210";

const int trigPin = 12;
const int echoPin = 14;
const int totalValue = 20;  // Total value for percentage calculation

const int red = 26;
const int yellow = 25;
const int green = 33;

// Create Firebase objects
FirebaseData firebaseData1;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

void setup() {
  Serial.begin(115200);
  pinMode(15, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    digitalWrite(15, HIGH);
    delay(200);
    digitalWrite(15, LOW);
    delay(100);
  }
  Serial.println("Connected to WiFi");
  digitalWrite(15, HIGH);

  // Initialize Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true); // Ensure WiFi reconnects automatically
}

void loop() {
  ultrasonic_reading();
}

void ultrasonic_reading() {
  digitalWrite(trigPin, LOW);
  delay(1);
  digitalWrite(trigPin, HIGH);
  delay(1);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.0343 / 2;
  if (distance >= 1000) {
    distance = 0;
  }

  // Map the distance to a percentage based on the total value
  int percentage = map(distance, 0, totalValue, 100, 0);

  // Ensure the percentage is within the valid range (0 to 100)
  percentage = constrain(percentage, 0, 100);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Percentage: ");
  Serial.print(percentage);
  Serial.println("%");

  Firebase.setInt(firebaseData1, "/IOT/Height(CM)/", distance);
  Firebase.setInt(firebaseData1, "/IOT/Percentage/", percentage);

  // Control the LEDs based on the percentage
  if (percentage <= 50) {
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
  } else if (percentage > 50 && percentage < 80) {
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    digitalWrite(yellow, HIGH);
  } else if (percentage >= 80 && percentage <= 100) {
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
    digitalWrite(yellow, LOW);
  }
}
