"""
Serial client operates on a computer that connects to serial manager running on Pi. Currently just echoes what Pi outputs.
"""
import socket
import sys

PORT = 8888

if len(sys.argv) <= 1:
  print('Usage: python serial_client.py [server_ip]')
  sys.exit()
  
HOST = sys.argv[1]

print('Connecting to {0} at port {1}'.format(HOST, PORT))
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
print('Connection established')

while True:
  data = s.recv(4096)
  if not data: continue
  if data == '!exit': break
  print(data)

s.close()
print('Connection closed')