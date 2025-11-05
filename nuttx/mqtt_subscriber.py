import paho.mqtt.client as mqtt

broker = "broker.hivemq.com"
port = 1883
topic = "si-labs"

def on_connect(client, userdata, flags, rc, properties=None):
    print("Connected with result code:", rc)
    print("Subscribing to topic:", topic)
    client.subscribe(topic)

def on_message(client, userdata, msg):
    print("Received message:", str(msg.payload.decode("utf-8")))

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

client.connect(broker, port, 60)
client.loop_forever()
