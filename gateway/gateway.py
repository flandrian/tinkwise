#!/usr/bin/python

from serial import Serial
from time import sleep
import json
from ConfigParser import ConfigParser
import argparse
import logging.handlers
from RrdHelper import RrdHelper

class TinkwiseGateway(object):
	def __init__(self):
		self._logger = logging.getLogger()
		self._logger.setLevel(logging.INFO)
		self._logger.addHandler(logging.handlers.SysLogHandler(address='/dev/log'))

		parser = argparse.ArgumentParser(description='tinkwise gateway')
		parser.add_argument('-c','--config_file', type=str, dest='config_file', default='/etc/tinkwise.conf')
		args = parser.parse_args()
		
		config = ConfigParser()
		config.read(args.config_file)
		connection_type = config.get('connection', 'type')
		connection_file = config.get('connection', 'file')
		self._rrd_dir = config.get('rrd', 'path')
		
		self._rrd = RrdHelper(self._rrd_dir)
		
		if connection_type == 'file':
			self._input_file = file(connection_file)
		elif connection_type == 'serial':
			try:
				self._input_file = Serial(port=connection_file, baudrate=115200)
			except Exception as e:
				self._logger.critical('failed to open serial port ' + connection_file + ':' + e.message)
				raise
			
	def run(self):
		while True:
			line = self._input_file.readline()
			if line == '':
				sleep(1)
				continue
			self._logger.info('processing ' + line)
			try:
				sample = json.loads(line)
			except Exception, e:
				self._logger.error('could not parse JSON string ' + line)
				self._logger.error(e.message)
				continue
			if not 'node' in sample:
				self._logger.error('no node found in JSON string' + line)
				continue
			node_index = int(sample['node'])
			self._logger.info('node ' + str(node_index))
			
			del sample['node']
			
			data_sources_spaces = map(lambda o: o.encode('ascii', 'ignore'), sample.keys())
			data_sources = map(lambda s: s.replace(' ', '_'), data_sources_spaces)
			data_values = map(str, sample.values())
			
			self._rrd.create_if_not_existing(node_index, data_sources)
			
			try:
				self._rrd.update(node_index, data_sources, data_values)
			except RrdHelper.DataSourceNotFoundException:
				self._logger.warning('data source reveived is missing in config file')
					


if __name__ == "__main__":
	gateway = TinkwiseGateway()
	gateway.run()
