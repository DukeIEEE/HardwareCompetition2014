"""
Serial communications manager for the Raspberry Pi to communicate between Arduino Uno and Pi and between Pi and external computer

Commands from Arduino to Pi start with the ! character
All other output from Arduino is considered debugging output and is sent to the external computer.
"""
import serial
import socket
import sys

import subprocess

PORT = 8888
EXE = 'ls'
DEBUG = True

#command expected to take arbitrary number of arguments and return a string representing the output
command_list = {}
  
def command(f):
  command_list[f.__name__] = f
  return f

@command
def ping(*x):
  return 'Success'
  
@command
def target_coords(*x):
  s = subprocess.Popen([EXE], shell=True, stdout=subprocess.PIPE)
  return s.stdout.read()
  
@command
def exe(x):
  s = subprocess.Popen(x, shell=True, stdout=subprocess.PIPE)
  return s.stdout.read()

def output(text, log=True):
  if DEBUG:
    global conn
    conn.sendall(text)
  
  if log:
    global logger
    logger.write(text)

#initialize socket first so that we can print Arduino connection data 

if DEBUG:
  try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('',PORT))
    s.listen(10)
    conn, addr = s.accept()
    print('Connected with ' + addr[0] + ':' + str(addr[1]))
  except: 
    print('Socket connection failed')
    sys.exit()

logger = open('log.txt', 'w')
    
output('Connecting to Arduino...')
ser = serial.begin('/dev/ttyACM0', 9600, timeout=2) #Arduino Uno is /dev/ttyACM0
output('Arduino connected...')

while True:
  line = ser.readline()
  if not line:
    continue
  
  output('Uno:' + line)
  #we cannot afford to have serial_manager crash during execution!
  try:
    if line.startswith('!'): #command from Arduino
      if line == '!exit': break #done with everything
      tokens = line.split(' ')
      if tokens[0] in command_list:
        result = command_list[tokens[0]](tokens[1:])
        output('Pi:' + result)
        ser.write(result)
      else:
        output('Unrecognized command: ' + tokens[0])
  except:
    e = sys.exec_info()[0]
    output('Exception: ' + e)
  output('\n')
  ser.write('\n')

if DEBUG:  
  conn.close()
logger.close()