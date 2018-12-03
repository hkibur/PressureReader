DEBUG = True

def taggedPrint(tag, *args):
    if DEBUG:
        for mess in args:
            print "%s:" % tag, mess,
        print

def debugPrint(*args):
    taggedPrint("DEBUG", *args)

def infoPrint(*args):
    taggedPrint("INFO", *args)

class TrapdoorBuffer(object):
    def __init__(self, target, limit):
        self.buffer = []
        self.limit = limit
        self.target = target

    def append(self, val):
        self.buffer.append(val)
        if len(self.buffer) >= self.limit:
            while len(self.buffer) > 0:
                self.target(self.buffer.pop(0))