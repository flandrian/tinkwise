from serial import Serial
from time import sleep
import json

CARBON_SERVER = '0.0.0.0'
CARBON_PORT = 2003

serial_file = file("test_data.txt")
#serial_file = Serial(port="/dev/ttyUSB0", baudrate=115200)

while True:
	line = serial_file.readline()
	if line == '':
		continue
	print 'processing ' + line
	try:
		sample = json.loads(line)
	except Exception, e:
		print 'could not parse JSON string ' + line
		print e
		continue
	if not 'node' in sample:
		print 'no node found in JSON string' + line
		continue
	node_index = int(sample['node'])
	print 'node ' + str(node_index)
	for item in sample.items():
		if item[0] == 'temperature':
			print 'temp: ' + str(item[1])