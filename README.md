# Greenhouse Control Dashboard

A web-based IoT dashboard for real-time monitoring and manual control of greenhouse environmental conditions using MQTT and Adafruit IO.

This project provides a responsive, dark-themed user interface that displays sensor data and allows direct control of connected greenhouse devices from any modern web browser.

---

## Overview

The Greenhouse Control Dashboard connects to Adafruit IO via MQTT over WebSockets. It subscribes to sensor feeds for live data visualization and publishes control commands to relay feeds for actuator management.

---

## Features

- Real-time environmental monitoring
- Manual device control via toggle switches
- MQTT over WebSockets (secure SSL connection)
- Responsive, dark-mode user interface
- No backend or server required

---

## Sensors Monitored

- Temperature (°C)
- Humidity (%)
- Soil Moisture
- Light Intensity (Lux)
- CO₂ Concentration (ppm)

---

## Actuators Controlled

- Water Pump
- Exhaust Fan
- Humidifier
- Solenoid Valve
- Grow Light

---

## Technology Stack

- **Frontend:** HTML5, CSS3, JavaScript
- **Messaging Protocol:** MQTT
- **MQTT Client:** Eclipse Paho JavaScript
- **Cloud Platform:** Adafruit IO

---

## MQTT Feed Structure

### Sensor Feeds
- `temperature`
- `humidity`
- `soil-moisture`
- `light-level`
- `co2-level`

### Control Feeds
- `relay-pump`
- `relay-exhaust-fan`
- `relay-humidifier`
- `relay-solenoid`
- `relay-grow-light`

**Payload Values**
- `ON`
- `OFF`

---

## Getting Started

### Prerequisites
- Adafruit IO account
- MQTT-enabled microcontroller (ESP32 / ESP8266)

### Setup

1. Create all required feeds on Adafruit IO
2. Obtain your Adafruit IO Username and AIO Key
3. Update credentials in `index.html`:

4. Open `index.html` in a modern web browser

---

## Hardware Compatibility

- ESP32 / ESP8266
- DHT11 / DHT22
- Soil moisture sensors
- Light sensors (LDR)
- CO₂ sensors (MQ-135, MH-Z19)

---

## Roadmap

- Automated control logic
- Historical data visualization
- Role-based user access
- Progressive Web App (PWA) support
- Cloud deployment support

---

## License

This project is released under the MIT License and is intended for educational, research, and personal use.
