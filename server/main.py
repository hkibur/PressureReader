import socket

import pressureConnection
import common

TCP_IP = "127.0.0.1"
TCP_PORT = 80
BUF_LEN = 1024
DEBUG = True

connectionList = [pressureConnection.PressureConnection] * 0

psocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
psocket.bind((TCP_IP, TCP_PORT))
psocket.listen()

def serverMainLoop():
    while True:
        conn, addr = psocket.accept()
        connectionList.append(pressureConnection.PrintPressureConnection())