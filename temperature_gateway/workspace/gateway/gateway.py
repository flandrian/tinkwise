import socket
import time
from serial import Serial
import re
from time import sleep

CARBON_SERVER = '0.0.0.0'
CARBON_PORT = 2003

#serial_file = file("test_data.txt")
serial_file = Serial(port="/dev/ttyUSB0", baudrate=115200)
scan_expression = re.compile('(?P<node>\d+):(?P<temp>\d+\.?\d*)')

while True:
	line = serial_file.readline()
	print 'processing ' + line + '\n'
	match = scan_expression.match(line)
	if match is None:
		sleep(1)
	else:
		node_number = match.group('node')
		temperature = match.group('temp')
		
		message = '{node}.temperatur {temp} {time}\n'.format(node = node_number, temp=temperature, time=int(time.time()))
		print 'message: ' + message
	
		sock = socket.socket()
		sock.connect((CARBON_SERVER, CARBON_PORT))
		sock.sendall(message)
		sock.close()
