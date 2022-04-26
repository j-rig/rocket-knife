# @file
# @author Josh Righetti
# @date 2022

import unittest
import subprocess
import os
#import os.path
import zipfile
import zlib
import gzip

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

    def test_compress(self):
        # data='testa'*10000
        # os.environ['TEST_PUFF']=os.path.join(os.environ['TESTDIR'], 'data.puff')
        # os.environ['TEST_TEXT']=os.path.join(os.environ['TESTDIR'], 'data.txt')
        # with open(os.environ['TEST_PUFF'],'wb') as fh:
        #     fh.write(zlib.compress(data))
        # with open(os.environ['TEST_TEXT'],'wb') as fh:
        #     fh.write(data)
        self.lua('test_compress.lua')

    def test_crypt(self):
        self.lua('test_crypt.lua')

    def test_sqlite(self):
        os.environ['TEST_SQLITE']=os.path.join(os.environ['TESTDIR'], 'test.sqlite')
        self.lua('test_sqlite.lua')

    def test_unzip(self):
        os.environ['TEST_ZIP']=os.path.join(os.environ['TESTDIR'], 'unzip.zip')
        os.environ['TEST_ZIP2']=os.path.join(os.environ['TESTDIR'], 'unzip2.zip')
        with zipfile.ZipFile(os.environ['TEST_ZIP'], 'w') as zf:
            zf.writestr('testa', 'testa'*1000)
            zf.writestr('testb', 'testb'*10000, zipfile.ZIP_DEFLATED)
            zf.writestr('testc', 'testc')
        with open(os.environ['TEST_ZIP2'],'w') as fho:
            fho.write('blah'*1001)
            with open(os.environ['TEST_ZIP'],'r') as fhi:
                fho.write(fhi.read())
        self.lua('test_unzip.lua')

    def test_rkbasic(self):
        self.lua('test_rkbasic.lua')

    def test_rkrpc(self):
        self.lua('test_rkrpc.lua')
