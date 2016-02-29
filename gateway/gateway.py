#!/usr/bin/python

from serial import Serial
from time import sleep
import json
import rrdtool
from ConfigParser import ConfigParser

def main():
	config = ConfigParser()
	config.read('/etc/tinkwise.conf')
	connection_type = config.get('connection', 'type')
	connection_file = config.get('connection', 'file')
	
	if connection_type == 'file':
		input_file = file(connection_file)
	elif connection_type == 'serial':
		input_file = Serial(port=connection_file, baudrate=115200)
	
	while True:
		line = input_file.readline()
		if line == '':
			sleep(1)
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
				rrdtool.update('rrd/{}.rrd'.format(node_index), 'N:{}'.format(item[1]))
				
if __name__ == "__main__":
	main()
