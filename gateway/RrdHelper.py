import rrdtool
import logging
import os

class RrdHelper(object):
    '''
    Helper methods to access RRDs as used by tinkwise
    '''

    class DataSourceNotFoundException(Exception):
        pass

    def __init__(self, database_directory):
        self._logger = logging.getLogger()
        self._rrd_directory = database_directory    
    
    def create_if_not_existing(self, node_index, data_sources):
        rrd_path = self._rrd_directory + '/{}.rrd'.format(node_index)
        if os.path.isfile(rrd_path):
            return
        self._logger.info('creating database {} with sources {}'.format(rrd_path, ','.join(data_sources)))
        data_source_specs = map(lambda s: 'DS:'+s+':GAUGE:600:U:U', data_sources)
        try:
            rrdtool.create(rrd_path, '--step', '600', data_source_specs, 'RRA:LAST:0.5:1:80000')
        except Exception as e:
            self._logger.error(e.message)
            raise    
    
    def update(self, node_number, data_sources, data_values):
        rrd_path = self._rrd_directory + '/{}.rrd'.format(node_number)
        try:
            data_sources_colons = ':'.join(data_sources)
            rrdtool.update(rrd_path, '--template', data_sources_colons, 'N:' + ':'.join(data_values))
        except Exception as e:
            if e.message.find('unknown DS name') >= 0:
                self._logger.warning('data source reveived is missing in config file')
            else:
                raise