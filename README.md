** Smart IoT Watering System **

A watering system that utilizes Google Cloud Platform to store the data read from the sensors into MongoDB.

Utilizes these sensors:
- Soil Moisture Sensor
- Raindrop Module Sensor
- DHT11 Sensor

Utilizes these actuators:
- LEDs
- Relay & Motor Fan

The sprinklers act as the main source of water for the plants in the case of a watering system. The system will read the moisture of the soil and if it is below a certain threshold, the "sprinklers" will be activated.
On top of that, the raindrop module sensor will assist by deactivating the sprinkler system when it reads that there is rain present. Colored LEDs will indicate whether the sprinkler is running or not.

For the sake of a demo IoT project, the sprinklers will be in place of the motor fan.

-- Configurations --
* Change the PINs according to own board connections
* Important to change the WIFI_SSID & WIFI_PASSWORD according to user's own network details
* Need to change the MQTT_SERVER & MQTT_TOPIC based on user's details on the Google Cloud Platform
