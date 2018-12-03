import socket
import threading

import pressureConnection
import common

TCP_IP = "0.0.0.0"
TCP_PORT = 111
BUF_LEN = 1024

connectionList = [pressureConnection.PressureConnection] * 0

serverClosing = False

psocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
psocket.bind((TCP_IP, TCP_PORT))
psocket.listen(1)
psocket.setblocking(0)

def serverMainLoop():
    while not serverClosing:
        try:
            conn, addr = psocket.accept()
        except socket.error as e:
            continue
        connectionList.append(pressureConnection.PrintPressureConnection(conn, addr))

def connectionCleaner():
    while not serverClosing:
        for ind, connection in enumerate(connectionList):
            if connection.clean:
                common.infoPrint("cleaned connection")
                del connectionList[ind]

loopThread = threading.Thread(target = serverMainLoop)
loopThread.start()
cleanerThread = threading.Thread(target = connectionCleaner)
cleanerThread.start()

common.infoPrint(socket.gethostbyname(socket.gethostname()))

while True:
    print "Server>",
    command = raw_input()
    if command == "exit":
        for connection in connectionList:
            connection.close()
        serverClosing = True
        loopThread.join()
        cleanerThread.join()
        psocket.close()
        break