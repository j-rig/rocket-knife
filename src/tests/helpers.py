# @file
# @author Josh Righetti
# @date 2022

import os
import time


def stdoutstderrpath():
    t = time.strftime("%m%d%y_%H%M%S")
    pid = os.getpid()
    td = './build/test'
    i = '%s/%s.%i.stdin.txt' % (td, t, pid)
    o = '%s/%s.%i.stdout.txt' % (td, t, pid)
    e = '%s/%s.%i.stderr.txt' % (td, t, pid)
    return i, o, e
