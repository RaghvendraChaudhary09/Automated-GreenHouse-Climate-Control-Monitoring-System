/*
  Greenhouse Controller - Manual Control Only
  - All relays are controlled manually from the web app.
  - Includes a simulated CO2 sensor.
*/

// ========== LIBRARIES ==========
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// ========== WI-FI & ADAFRUIT IO CONFIGURATION ==========
#define WLAN_SSID       "RaghvendraF" // Apna Wi-Fi Name daalein
#define WLAN_PASS       "6392027660s"     // Apna Wi-Fi Password daalein

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "GreenhouseRNA" // <-- Apna ADAFRUIT IO USERNAME YAHAN DAALEIN
#define AIO_KEY         "aio_VyBt48nDAywmDNrKvWawMuwmCJVs"      // <-- Apna ADAFRUIT IO KEY YAHAN DAALEIN

// ========== PIN & SENSOR CONFIGURATION ==========
#define I2C_SDA 21
#define I2C_SCL 22
#define PIN_DHT 4
#define PIN_SOIL 34

#define RELAY_PUMP        25
#define RELAY_EXHAUST_FAN 26
#define RELAY_HUMIDIFIER  5
#define RELAY_SOLENOID    18
#define RELAY_GROW_LIGHT  19

// Sensor Objects & Clients
DHT dht(PIN_DHT, DHT22);
BH1750 lightMeter;
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// ========== ADAFRUIT IO FEEDS ==========
Adafruit_MQTT_Publish soilFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil-moisture");
Adafruit_MQTT_Publish tempFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish lightFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/light-level");
Adafruit_MQTT_Publish co2Feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/co2-level");

Adafruit_MQTT_Subscribe pumpSub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay-pump");
Adafruit_MQTT_Subscribe fanSub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay-exhaust-fan");
Adafruit_MQTT_Subscribe humidifierSub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay-humidifier");
Adafruit_MQTT_Subscribe solenoidSub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay-solenoid");
Adafruit_MQTT_Subscribe growLightSub = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/relay-grow-light");

// Timers and State Variables
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL_MS = 15000; // Har 15 second mein data bhejega

// CO2 Simulation variables
float simulatedCO2 = 500.0; // <-- BADLAAV YAHAN KIYA GAYA HAI
bool co2Spiked = false;
unsigned long co2SpikeTime = 40000; 
unsigned long lastCo2DecreaseTime = 0;

// Helper Function for Relays
void setRelay(int pin, bool on) {
  bool signal = on;
  if (pin == RELAY_GROW_LIGHT) {
    signal = !on; // Grow light ke liye inverted logic
  }
  digitalWrite(pin, signal ? HIGH : LOW);
}

// MQTT Connection Function
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) return;
  Serial.print("Connecting to MQTT... ");
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
  }
  Serial.println("MQTT Connected!");
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PUMP, OUTPUT);
  pinMode(RELAY_EXHAUST_FAN, OUTPUT);
  pinMode(RELAY_HUMIDIFIER, OUTPUT);
  pinMode(RELAY_SOLENOID, OUTPUT);
  pinMode(RELAY_GROW_LIGHT, OUTPUT);

  Wire.begin(I2C_SDA, I2C_SCL);
  dht.begin();
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  mqtt.subscribe(&pumpSub);
  mqtt.subscribe(&fanSub);
  mqtt.subscribe(&humidifierSub);
  mqtt.subscribe(&solenoidSub);
  mqtt.subscribe(&growLightSub);
}

// CO2 Simulation Logic
void handleCo2Simulation() {
  unsigned long currentTime = millis();
  if (!co2Spiked && currentTime >= co2SpikeTime) {
    simulatedCO2 = 1500.0;
    co2Spiked = true;
    lastCo2DecreaseTime = currentTime; 
  }
  if (co2Spiked && (currentTime - lastCo2DecreaseTime >= 20000)) {
    lastCo2DecreaseTime = currentTime;
    // Ab 800 se neeche nahi jayega
    if (simulatedCO2 > 800) simulatedCO2 -= 5;
  }
}

void loop() {
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    // Sirf manual commands ko sunega
    if (subscription == &pumpSub) setRelay(RELAY_PUMP, strcmp((char *)subscription->lastread, "ON") == 0);
    if (subscription == &fanSub) setRelay(RELAY_EXHAUST_FAN, strcmp((char *)subscription->lastread, "ON") == 0);
    if (subscription == &humidifierSub) setRelay(RELAY_HUMIDIFIER, strcmp((char *)subscription->lastread, "ON") == 0);
    if (subscription == &solenoidSub) setRelay(RELAY_SOLENOID, strcmp((char *)subscription->lastread, "ON") == 0);
    if (subscription == &growLightSub) setRelay(RELAY_GROW_LIGHT, strcmp((char *)subscription->lastread, "ON") == 0);
  }

  handleCo2Simulation(); 

  if (millis() - lastSensorRead > SENSOR_INTERVAL_MS) {
    lastSensorRead = millis();
    int soil = analogRead(PIN_SOIL);
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    float lux = lightMeter.readLightLevel();

    // Hamesha data publish karo
    if (soil > 0) soilFeed.publish((int32_t)soil);
    if (!isnan(temp)) tempFeed.publish(temp);
    if (!isnan(hum)) humFeed.publish(hum);
    if (lux >= 0) lightFeed.publish(lux);
    co2Feed.publish(simulatedCO2);
  }
}

