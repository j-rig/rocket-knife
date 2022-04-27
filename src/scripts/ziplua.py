import zipfile
import os.path
import sys
import struct

print('creating', sys.argv[1])
with zipfile.ZipFile(sys.argv[1], 'w') as zf:
    fs=os.path.getsize(sys.argv[2])
    print(sys.argv[2], fs)
    #zf.comment=str(fs)
    zf.comment=struct.pack("!L",fs)
    for f in sys.argv[3:]:
        print('adding', f)
        base=os.path.basename(f)
        n,e=base.split('.')
        dat=open(f,'rb').read()
        zf.writestr(n, dat, zipfile.ZIP_DEFLATED)
print('done')
