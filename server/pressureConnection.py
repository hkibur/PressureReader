import threading

import common

JOIN_TIMEOUT = 5

class PressureConnection(object): #Only subclass this
    def __init__(self, conn, addr, **kwargs):
        self.conn = conn
        self.addr = addr
        self.closing = False
        self.messLen = kwargs.get("messlen", 1024)

        common.debugPrint("new connection:", self.addr)

        self.workerThread = threading.Thread(target = self.worker)
        self.workerThread.start()

    def worker(self):
        raise NotImplementedError

    def close(self):
        self.closing = True
        self.workerThread.join(JOIN_TIMEOUT)
        if self.workerThread.isAlive():
            raise RuntimeError("%s took more than %d second to terminate" % (self.__class__.__name__, JOIN_TIMEOUT))
        self.conn.close()

class PrintPressureConnection(PressureConnection):
    def __init__(self, *args, **kwargs):
        PressureConnection.__init__(self, *args, **kwargs)

    def worker(self):
        while not self.closing:
            data = self.conn.recv(self.messLen)
            #do stuff