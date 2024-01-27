#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Sensor Pins
const int MOISTURE_SENSOR_PIN = A5;
const int RAIN_SENSOR_PIN = A3;
const int DHT_PIN = A1;
const int DHT_TYPE = DHT11;

// Actuator Pins
const int GREEN_LED_PIN = 14;
const int RED_LED_PIN = 21;
const int MOTOR_PIN = 47;

// Thresholds for watering and rain avoidance
const int MOISTURE_THRESHOLD = 40;
const int RAIN_THRESHOLD = 20;

// Connecting to WiFi Access Point
const char WIFI_SSID[] = "cs-mtg-room";
const char WIFI_PASSWORD[] = "bilik703";
const char *MQTT_SERVER = "34.123.36.95";  // your VM instance public IP address 
const int MQTT_PORT = 1883; 
const char *MQTT_TOPIC = "watering-system"; // MQTT topic 

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay (500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while(!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT server");
    }
    else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  //intialize serial monitior
  Serial.flush();
  Serial.begin(9600);

  pinMode(MOISTURE_SENSOR_PIN, INPUT);
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  dht.begin();
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);

  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  float moistureLevel = (1.00 - (analogRead(MOISTURE_SENSOR_PIN) / 4095.00)) * 100;
  float rainLevel = (1.00 - (analogRead(RAIN_SENSOR_PIN) / 4095.00)) * 100;
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Create JSON object
  StaticJsonDocument<200> jsonDocument;
  jsonDocument["soil-moisture"] = moistureLevel;
  jsonDocument["raindrop"] = rainLevel;
  jsonDocument["humidity"] = humidity;
  jsonDocument["temperature"] = temperature;

  // Serialize JSON document into string
  char jsonString[200];
  serializeJson(jsonDocument, jsonString);

  if (moistureLevel < MOISTURE_THRESHOLD && rainLevel < RAIN_THRESHOLD) {
    Serial.println("Watering the plant!");
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(MOTOR_PIN, HIGH);
  } else if (rainLevel > RAIN_THRESHOLD) {
    Serial.println("It is currently raining. No watering needed!");
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(MOTOR_PIN, LOW);
  } else {
    Serial.println("Plant is perfectly watered, though there is no rain!");
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(MOTOR_PIN, LOW);
  }

  Serial.print("Moisture Level: ");
  Serial.println(moistureLevel);
  Serial.print("Rain Presence: ");
  Serial.println(rainLevel);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  client.publish(MQTT_TOPIC, jsonString);

  delay(5000);
}
