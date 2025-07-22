import asyncio
import json
import time
from typing import Dict, Callable, Any, Optional
import paho.mqtt.client as mqtt
from config.mqtt_config import MQTTConfig


## TO-Do: Setup proper logging

class NailaMQTTClient:
  def __init__(self, config: MQTTConfig):
    self.config = config
    self.client: Optional[mqtt.Client] = None
    self.connected = False
    self.message_handlers: Dict[str, Callable] = {}
    self.reconnect_attempts = 0
    self.max_reconnect_attempts = 5
    self.on_message_callback: Optional[Callable] = None
    self.subscriptions = []
  
  def setup(self):
    """Initialize the MQTT Client"""
    self.client = mqtt.Client(
      client_id=self.config.client_id,
      clean_session=self.config.clean_session
    )

    if self.config.username and self.config.password:
      self.client.username_pw_set(self.config.username, self.config.password)
    
    self.client.on_connect = self._on_connect
    self.client.on_disconnect = self._on_disconnect
    self.client.on_message = self._on_message
    self.client.on_subscribe = self._on_subscribe
    self.client.on_publish = self._on_publish
  
  def _on_connect(self, client, userdata, flags, rc):
    """Callback for when the client connects"""
    if rc == 0:
      self.connected = True
      print(f"Connected to MQTT broker at {self.config.broker_host}:{self.config.broker_port}")

      for topic, qos in self.subscriptions:
        result = self.client.subscribe(topic, qos)
        print(f"Subscribed to {topic} with QoS {qos}")
    else:
      print(f"Failed to connect. Return code: {rc}")
  
  def _on_disconnect(self, client, userdata, rc):
    """Called when client disconnects"""
    self.connected = False
    if rc != 0:
      print(f"Unexpected disconnection. Return code: {rc}")
    else:
      print("Disconnected from broker")
  
  def _on_message(self, client, userdata, msg):
    """Called when message is received"""
    topic = msg.topic
    payload = msg.payload.decode('utf-8')
    print(f"Received: {topic} -> {payload}")

    if self.on_message_callback:
      self.on_message_callback(topic, payload)
  
  def _on_subscribe(self, client, userdata, mid, granted_qos):
    """Called when subscription is acknowledged"""
    print(f"Subscription acknowledged: {mid}")
  
  def _on_publish(self, client, userdata, mid):
    """Called when publish is complete"""
    print(f"Message published: {mid}")

  def connect(self):
    """Connect to the MQTT broker"""
    if not self.client:
      self.setup()
    
    try:
      result = self.client.connect(self.config.broker_host, self.config.broker_port, self.config.keepalive)
      if result == 0:
        self.client.loop_start()
        return True
      else:
        print(f"Connection failed with result: {result}")
        return False
    except Exception as e:
      print(f"Exception during connection: {e}")
      return False

  def disconnect(self):
    """Disconnect from broker"""
    if self.client and self.connected:
      self.client.loop_stop()
      self.client.disconnect()
  
  def subscribe(self, topic, qos=0):
    """Subscribe to a topic"""
    self.subscriptions.append((topic, qos))

    if self.connected:
      result = self.client.subscribe(topic, qos)
      print(f"Subscribed to {topic}")
      return result
    else:
      print(f"Not connected yet. Will subscirbe to {topic} when connected")
      return True
  
  def publish(self, topic, message, qos=0):
    """Publish a message"""
    if self.connected:
      result = self.client.publish(topic, message, qos)
      print(f"Published to {topic}: {message}")
      return result
    else:
      print("Not connected. Can't publish")
      return False
  
  def set_message_callback(self, callback: Callable):
    """Set custom message handler"""
    self.on_message_callback = callback