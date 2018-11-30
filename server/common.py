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