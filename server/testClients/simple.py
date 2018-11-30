import socket
import time

TCP_IP = '127.0.0.1'
TCP_PORT = 111
BUFFER_SIZE = 1024
MESSAGE = "Hello, World! wow"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
while True:
    s.send(MESSAGE)
    cmd = raw_input()
    if cmd == "exit":
        break
s.shutdown(socket.SHUT_RDWR)
s.close()