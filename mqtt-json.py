import pymongo
import paho.mqtt.client as mqtt
from datetime import datetime
import json

# MongoDB configuration
mongo_client = pymongo.MongoClient("mongodb://danial:1234@localhost:27017/admin")
db = mongo_client["smarthome"]
collection = db["iot"]

# MQTT configuration
mqtt_broker_address = '34.123.36.95'
mqtt_topic = 'watering-system'

def on_message(client, userdata, message):
    payload = message.payload.decode('utf-8')
    print(f'Received message: {payload}')

    # Convert MQTT timestamp to datetime
    timestamp = datetime.utcnow() # Use current UTC time
    datetime_obj = timestamp.strftime("%Y-%m-%dT%H:%M:%S.%fZ")

    # Process the payload and insert into MongoDB with proper timestamp
    data = {
    'timestamp': datetime_obj,
    'soil-moisture': None,
    'raindrop': None,
    'temperature': None,
    'humidity': None,
    }

    try:
        # Parse JSON payload
        payload_json = json.loads(payload)

        # Update data dictionary with actual values
        data['soil-moisture'] = payload_json.get('soil-moisture')
        data['raindrop'] = payload_json.get('raindrop')
        data['humidity'] = payload_json.get('humidity')
        data['temperature'] = payload_json.get('temperature')

    except json.JSONDecodeError as e:
        print(f'Error decoding JSON: {e}')

    collection.insert_one(data)
    print('Data ingested into MongoDB')

client = mqtt.Client()
client.on_message = on_message

# Connect to MQTT broker
client.connect(mqtt_broker_address, 1883, 60)

# Subscribe to MQTT topic
client.subscribe(mqtt_topic)

# Start the MQTT loop
client.loop_forever()