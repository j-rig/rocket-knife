# @file
# @author Josh Righetti
# @date 2022

import unittest
import subprocess
import os
#import os.path

import helpers

i, o, e = helpers.stdoutstderrpath()
o = open(o, 'w+')
e = open(e, 'w+')
l = './build/x86/macbin/rk'


class rk_tests(unittest.TestCase):

    def lua(self, s):
        s = './tests/%s' % s
        #s= os.path.abspath(s)
        o.write('python running rk on %s...\n' % s)
        o.flush()
        r = subprocess.call([l, s], stdout=o, stderr=e)
        o.flush()
        e.flush()
        self.assertEqual(r, 0)

    def test_basic(self):
        self.lua('test_basic.lua')
