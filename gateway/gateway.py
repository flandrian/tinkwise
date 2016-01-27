#!/usr/bin/python

from serial import Serial
from time import sleep
import json
import rrdtool
from ConfigParser import ConfigParser
import argparse

def main():
	parser = argparse.ArgumentParser(description='tinkwise gateway')
	parser.add_argument('-c','--config_file', type=str, dest='config_file', default='/etc/tinkwise.conf')
	args = parser.parse_args()
	
	config = ConfigParser()
	config.read(args.config_file)
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
		
		del sample['node']
		
		rrdtool.update('rrd/{}.rrd'.format(node_index), '--template', map(lambda o: o.encode('ascii', 'ignore'), sample.keys()), 'N:' + ':'.join(map(str, sample.values())))
				
if __name__ == "__main__":
	main()
