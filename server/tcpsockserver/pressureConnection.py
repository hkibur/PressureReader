import threading
import sys

import common
import socket
import filelock

JOIN_TIMEOUT = 5

class PressureConnection(object): #Only subclass this
    def __init__(self, conn, addr, **kwargs):
        self.conn = conn
        self.addr = addr
        self.closing = False
        self.clean = False
        self.messLen = kwargs.get("messlen", 1024)

        common.infoPrint("new connection:", self.addr)

        self.workerThread = threading.Thread(target = self.subWorker)
        self.workerThread.start()

    def subWorker(self):
        self.worker()
        self.conn.close()
        self.clean = True

    def worker(self):
        raise NotImplementedError

    def close(self):
        self.closing = True
        self.workerThread.join(JOIN_TIMEOUT)
        if self.workerThread.isAlive():
            raise RuntimeError("%s took more than %d second to terminate" % (self.__class__.__name__, JOIN_TIMEOUT))
        self.conn.close()
        self.clean = True

class PrintPressureConnection(PressureConnection):
    def __init__(self, *args, **kwargs):
        PressureConnection.__init__(self, *args, **kwargs)

    def worker(self):
        while not self.closing:
            try:
                data = self.conn.recv(self.messLen)
            except socket.error as e:
                continue
            if not data:
                common.infoPrint("connection closed by client")
                break
            sys.stdout.write(data)

class FileLogPressureConnection(PressureConnection):
    def __init__(self, *args, **kwargs):
        PressureConnection.__init__(self, *args, **kwargs)

    def worker(self):
        while not self.closing:
            try:
                data = self.conn.recv(self.messLen)
            except socket.error as e:
                continue
            if not data:
                common.infoPrint("connection closed by client")
                break
            #TODO: think about architecture, find where file should be.