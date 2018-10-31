def debugPrint(*args):
    if DEBUG:
        for mess in args:
            print mess,
        print