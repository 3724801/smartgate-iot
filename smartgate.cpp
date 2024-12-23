#include <WiFi.h>
#include <HTTPClient.h>
#include <Servo.h>

// Define pins
#define TRIG_PIN 25   // GPIO for Trigger pin of Ultrasonic Sensor
#define ECHO_PIN 26   // GPIO for Echo pin of Ultrasonic Sensor
#define SERVO_PIN 27  // GPIO for Servo Signal

// Wi-Fi Credentials
const char* ssid = "Your_SSID";         // Replace with your Wi-Fi SSID
const char* password = "Your_PASSWORD"; // Replace with your Wi-Fi Password

// Server URL
const char* serverURL = "http://yourserver.com/api/distance"; // Replace with your server endpoint

// Servo object
Servo myServo;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Attach the servo motor
  myServo.attach(SERVO_PIN);
  myServo.write(90); // Initialize servo to 90 degrees

  // Connect to Wi-Fi
  connectToWiFi();
}

void loop() {
  // Measure distance using the ultrasonic sensor
  long duration = measureDistance();
  int distance = duration * 0.034 / 2; // Convert to cm

  // Print distance
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Control the servo motor based on distance
  if (distance <= 200 && distance > 0) {
    int angle = map(distance, 0, 200, 0, 180);
    myServo.write(angle);
    Serial.print("Servo Angle: ");
    Serial.println(angle);
  } else {
    myServo.write(90); // Default position
  }

  // Send distance data to the server
  sendDataToServer(distance);

  delay(1000);
}

// Function to measure distance
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH);
}

// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Function to send data to the server
void sendDataToServer(int distance) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Configure the server URL
    http.begin(serverURL);

    // Set content type as JSON
    http.addHeader("Content-Type", "application/json");

    // Create the JSON payload
    String jsonPayload = "{\"distance\": " + String(distance) + "}";

    // Send POST request
    int httpResponseCode = http.POST(jsonPayload);

    // Print server response
    if (httpResponseCode > 0) {
      Serial.print("Server Response: ");
      Serial.println(http.getString());
    } else {
      Serial.print("Error Sending Data: ");
      Serial.println(httpResponseCode);
    }

    // End the HTTP request
    http.end();
  } else {
    Serial.println("Wi-Fi Disconnected!");
  }
}