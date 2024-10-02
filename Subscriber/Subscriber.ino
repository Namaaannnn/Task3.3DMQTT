// Subscriber code

#include <WiFiNINA.h> // Library for WiFi connectivity
#include <ArduinoMqttClient.h> // Library for MQTT protocol

// WiFi credentials
char wifiSSID[] = "Naman"; // Your WiFi SSID
char wifiPassword[] = "12345678"; // Your WiFi password

// Pin definition for the LED
const int ledPin = 5; // Pin connected to the LED

// MQTT client setup
WiFiClient wifiClient; // Create a WiFi client
MqttClient mqttClient(wifiClient); // Create an MQTT client using the WiFi client

// MQTT broker configuration
const char mqttBroker[] = "broker.emqx.io"; // MQTT broker address
int mqttPort = 1883; // MQTT broker port
const char mqttTopic[] = "SIT210/waves"; // Topic to subscribe to for messages

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud
  pinMode(ledPin, OUTPUT); // Set the LED pin as output

  connectToWiFi(); // Connect to the WiFi network
  connectToMQTTBroker(); // Connect to the MQTT broker

  mqttClient.subscribe(mqttTopic); // Subscribe to the specified MQTT topic
  Serial.print("Subscribed to Topic: ");
  Serial.println(mqttTopic); // Print the subscribed topic
  Serial.println("Waiting for messages..."); // Indicate readiness to receive messages
}

void loop() {
  // Reconnect to MQTT broker if disconnected
  if (!mqttClient.connected()) {
    connectToMQTTBroker();
  }
  
  mqttClient.poll(); // Keep the MQTT client active and process incoming messages

  // Check for available messages
  while (mqttClient.available()) {
    String message = mqttClient.readString(); // Read the incoming message as a string
    Serial.print("Received Message: "); // Print the received message
    Serial.println(message);
    processMessage(message); // Process the received message
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

// Function to process the received message
void processMessage(String message) {
  // Check if the message matches "Wave"
  if (message == "Wave") {
    Serial.println("Wave Detected"); // Indicate that a wave message was received
    flashLEDWavePattern(); // Call function to flash LED in a wave pattern
  } 
}

// Function to flash the LED in a wave pattern
void flashLEDWavePattern() {
  // Flash the LED 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH); // Turn on the LED
    delay(500); // Wait for 500 milliseconds
    digitalWrite(ledPin, LOW); // Turn off the LED
    delay(500); // Wait for 500 milliseconds
  }
}