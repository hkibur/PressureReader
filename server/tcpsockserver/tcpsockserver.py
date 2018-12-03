import socket
import threading
import sys
import os

def globalImport(mod):
    globals()[mod] = __import__(mod)

class TCPSockServer(object):
    def __init__(self, ip, port):
        self.psocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.psocket.bind((ip, port))
        self.psocket.listen(1)
        self.psocket.setblocking(0)

        self.connectionList = [pressureconnection.PressureConnection] * 0

        self.closing = False

    def mainLoop(self):
        while not self.closing:
            try:
                conn, addr = self.psocket.accept()
            except socket.error as e:
                continue
            self.connectionList.append(pressureconnection.PrintPressureConnection(conn, addr))

    def connectionCleaner(self):
        while not self.closing:
            for ind, connection in enumerate(self.connectionList):
                if connection.clean:
                    common.infoPrint("cleaned connection")
                    del self.connectionList[ind]

    def run(self):
        loopThread = threading.Thread(target = self.mainLoop)
        loopThread.start()
        cleanerThread = threading.Thread(target = self.connectionCleaner)
        cleanerThread.start()

        common.infoPrint(socket.gethostbyname(socket.gethostname()))

        while True:
            print "TCP Sock Server>",
            command = raw_input()
            if command == "exit":
                for connection in self.connectionList:
                    connection.close()
                self.closing = True
                loopThread.join()
                cleanerThread.join()
                self.psocket.close()
                break

TCP_IP = "0.0.0.0"
TCP_PORT = 111

if __name__ == "__main__":
    curPath = os.path.dirname(os.path.abspath(__file__))
    sys.path.append(curPath + "/../dep")

    globalImport("filelock")
    globalImport("common")
    globalImport("pressureconnection")

    TCPSockServer(TCP_IP, TCP_PORT).run()