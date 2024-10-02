// publisher code 

#include <WiFiNINA.h> // Library for WiFi connectivity
#include <ArduinoMqttClient.h> // Library for MQTT protocol

// WiFi credentials
char wifiSSID[] = "Naman"; // Your WiFi SSID
char wifiPassword[] = "12345678"; // Your WiFi password

// Pin definitions for the ultrasonic sensor and LED
const int triggerPin = 9; // Trigger pin for the ultrasonic sensor
const int echoPin = 10; // Echo pin for the ultrasonic sensor

// Variables for distance measurement
float duration, distance;
float speedOfSound = 0.034; // Speed of sound in cm/μs

// MQTT client setup
WiFiClient wifiClient; // Create a WiFi client
MqttClient mqttClient(wifiClient); // Create an MQTT client using the WiFi client

String lastMessage = ""; // Variable to store the last message sent

// MQTT broker configuration
const char mqttBroker[] = "broker.emqx.io"; // MQTT broker address
int mqttPort = 1883; // MQTT broker port
const char mqttTopic[] = "SIT210/waves"; // Topic to publish messages

const long interval = 1000; // Interval for distance measurement in milliseconds
long prevMills = 0; // Variable to track previous time for measurement

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud

  // Set up pins
  pinMode(triggerPin, OUTPUT); // Set trigger pin as output
  pinMode(echoPin, INPUT); // Set echo pin as input

  connectToWiFi(); // Connect to the WiFi network
  connectToMQTTBroker(); // Connect to the MQTT broker
}

void loop() {
  // Reconnect to MQTT broker if disconnected
  if (!mqttClient.connected()) {
    connectToMQTTBroker();
  }
  
  mqttClient.poll(); // Keep the MQTT client active

  long currMills = millis(); // Get the current time

  // Check if the interval time has passed
  if (currMills - prevMills >= interval) {
    prevMills = currMills; // Update the previous time

    distance = MeasureDistance(); // Measure the distance using ultrasonic sensor

    Serial.print("Distance: "); // Print the distance
    Serial.println(distance);

    sendMessage(); // Send a message based on the measured distance
  }
}

// Function to connect to WiFi
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(wifiSSID); // Print the WiFi SSID being connected to

  // Attempt to connect to WiFi
  while (WiFi.begin(wifiSSID, wifiPassword) != WL_CONNECTED) {
    Serial.print("."); // Indicate connection attempt
    delay(1000); // Wait 1 second before retrying
  }

  Serial.println("WiFi Connected"); // Confirmation of WiFi connection
}

// Function to connect to the MQTT broker
void connectToMQTTBroker() {
  Serial.print("Connecting to MQTT Broker: ");
  Serial.println(mqttBroker); // Print the MQTT broker being connected to

  // Attempt to connect to the MQTT broker
  while (!mqttClient.connect(mqttBroker, mqttPort)) {
    Serial.print("."); // Indicate connection attempt
    delay(1000); // Wait 1 second before retrying
  }

  Serial.println("MQTT Broker Connected"); // Confirmation of MQTT connection
}

// Function to measure distance using ultrasonic sensor
float MeasureDistance() {
  digitalWrite(triggerPin, LOW); // Ensure trigger pin is low
  delayMicroseconds(3); // Wait for 3 microseconds
  digitalWrite(triggerPin, HIGH); // Send a trigger signal
  delayMicroseconds(10); // Keep the trigger high for 10 microseconds
  digitalWrite(triggerPin, LOW); // Set trigger pin low again

  // Measure the duration of the echo pulse
  duration = pulseIn(echoPin, HIGH, 25000); // Wait for the echo and measure duration

  // Calculate and return the distance
  return (duration * speedOfSound) / 2; // Distance = (duration * speed of sound) / 2
}

// Function to send a message via MQTT based on distance
void sendMessage() {
  // Check if the measured distance is within a certain range
  if (distance > 0 && distance < 15) {
    mqttClient.beginMessage(mqttTopic); // Start a new message to the specified topic
    
    mqttClient.print("Wave"); // Send the message content
    Serial.println("Wave Message Sent!"); // Confirmation of message sent

    mqttClient.endMessage(); // End the message
    delay(3000); // Wait for 3 seconds before sending the next message
  }
}
