#!/usr/bin/python

from serial import Serial
from time import sleep
import json
import rrdtool
from ConfigParser import ConfigParser
import os
import argparse

class TinkwiseGateway(object):
	def __init__(self):
		parser = argparse.ArgumentParser(description='tinkwise gateway')
		parser.add_argument('-c','--config_file', type=str, dest='config_file', default='/etc/tinkwise.conf')
		args = parser.parse_args()
		
		config = ConfigParser()
		config.read(args.config_file)
		connection_type = config.get('connection', 'type')
		connection_file = config.get('connection', 'file')
		self._rrd_dir = config.get('rrd', 'path')
		
		if connection_type == 'file':
			self._input_file = file(connection_file)
		elif connection_type == 'serial':
			self._input_file = Serial(port=connection_file, baudrate=115200)
			
	def run(self):
		while True:
			line = self._input_file.readline()
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
			
			data_sources = map(lambda o: o.encode('ascii', 'ignore'), sample.keys())
			data_values = map(str, sample.values())
			rrd_path = self._rrd_dir + '/{}.rrd'.format(node_index)
			
			if not os.path.isfile(rrd_path):
				self._create_database(rrd_path, data_sources)
			
			try:
				rrdtool.update(rrd_path, '--template', data_sources, 'N:' + ':'.join(data_values))
			except Exception as e:
				if e.message.find('unknown DS name') >= 0:
					print 'data source reveived is missing in config file'
				else:
					raise
					
	def _create_database(self, path, data_sources):
		print 'creating database {} with sources {}'.format(path, ','.join(data_sources))
		data_source_specs = map(lambda s: 'DS:'+s+':GAUGE:600:U:U', data_sources)
		rrdtool.create(path, '--step', '600', data_source_specs, 'RRA:LAST:0.5:1:80000')

if __name__ == "__main__":
	gateway = TinkwiseGateway()
	gateway.run()
