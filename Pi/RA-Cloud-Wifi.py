#!/usr/bin/python

import serial
import paho.mqtt.client as mqtt
import os
import json
import threading
import time
import string
import requests

### Settings
serialdev = '/dev/ttyUSB1' # for BBB
# serialdev = '/dev/ttyAMA0' # for RPi

broker = "cloud.reefangel.com" # mqtt broker
port = 1883 # mqtt broker port
in_topic = "<CHANGE TO INTOPIC>"
pub_topic = "<CHANGE TO OUTTOPIC>"
mqttUsername = "<CHANGE TO USERNAME>"
mqttPassword = "<CHANGE TO PASSWORD>"
mqttClientID = "<CHANGE TO CLIENTID>"
debug = True  ## set this to True for lots of prints
# buffer of data to output to the serial port
#outputData = []
outputData = ''

 
####  MQTT callbacks
def on_connect(client, userdata, flags, rc):
	if rc == 0:
	#rc 0 successful connect
		print "Connected"
	else:
		raise Exception
	#subscribe to the output MQTT messages
	output_mid = client.subscribe(in_topic)

def on_publish(client, userdata, mid):
	if(debug):
		print "Published. mid:", mid

def on_subscribe(client, userdata, mid, granted_qos):
	if(debug):
		print "Subscribed. mid:", mid

def on_message_output(client, userdata, msg):
	if(debug):
		print "Output Data: ", msg.topic, "data:", msg.payload
	#add to outputData list
	#outputData.append(msg)
	outputData = msg.payload
	print("Payload Length :")
	print(len(outputData))
	ser.write(outputData)

def on_message(client, userdata, message):
	if(debug):
		print "Unhandled Message Received: ", message.topic, message.paylod		

#called on exit
#close serial, disconnect MQTT
def cleanup():
	print "Ending and cleaning up"
	ser.close()
	mqttc.disconnect()

### Can probably remove this since we are no longer calling this function
def mqtt_to_JSON_output(mqtt_message):
	topics = mqtt_message.topic.split('/');
	## JSON message in ninjaCape form
	json_data = '{"DEVICE": [{"G":"0","V":0,"D":' + topics[2] + ',"DA":"' + mqtt_message.payload + '"}]})'
	return json_data
### Can probably remove this since we are no longer calling this function


#thread for reading serial data and publishing to MQTT client
def serial_read_and_publish(ser, mqttc):
	ser.flushInput()
	while True:
		line = ser.readline() # this is blocking
		if(debug):
			print "Read from Serial Port:",line
		try:
			if 'GET' in line:
				#print ("Sending to Reefangel.com: ")
				PARAMS = string.replace(UPDATE, 'GET /status/submitp.aspx?', '')
				portalupdate = requests.get('http://forum.reefangel.com/status/submitp.aspx', params=PARAMS)
			if 'CLOUD' in line:
				#print ('Sending to MQTT Reefangel.com')
				mqttupdate = string.replace(line, 'CLOUD:', '')
				mqttc.publish(pub_topic, mqttupdate)
		except(KeyError):
			# TODO should probably do something here if the data is malformed
			pass

############ MAIN PROGRAM START
try:
	print "Connecting... ", serialdev
	#connect to serial port
	ser = serial.Serial(serialdev, 57600, timeout=None) #timeout 0 for non-blocking. Set to None for blocking.

except:
	print "Failed to connect serial"
	#unable to continue with no serial input
	raise SystemExit

try:
	#create an mqtt client
	mqttc = mqtt.Client(mqttClientID)
	#attach MQTT callbacks
	mqttc.on_connect = on_connect
	mqttc.on_publish = on_publish
	mqttc.on_subscribe = on_subscribe
	mqttc.on_message = on_message
	mqttc.message_callback_add(in_topic, on_message_output)
	#connect to broker
	mqttc.username_pw_set(mqttUsername, mqttPassword)    #set username and password
	mqttc.connect(broker, port, 60)
	# start the mqttc client thread
	mqttc.loop_start()
	serial_thread = threading.Thread(target=serial_read_and_publish, args=(ser, mqttc))
	serial_thread.daemon = True
	serial_thread.start()
	while True: # main thread
		if (debug):
			print("Main Thread Looping with")
			print(outputData)
		#writing to serial port if there is data available
		if( len(outputData) > 0 ):
			#print "***data to OUTPUT:",mqtt_to_JSON_output(outputData[0])
			#ser.write(mqtt_to_JSON_output(outputData.pop()))
			print("MQTT Server Response")
			print(outputData)
			#ser.write(outputData.pop())
			ser.write(outputData)
			#outputData = ''
		time.sleep(0.5)

# handle app closure
except (KeyboardInterrupt):
	print "Interrupt received"
	cleanup()
except (RuntimeError):
	print "uh-oh! time to die"
	cleanup()