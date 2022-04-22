import zipfile
import os.path
import sys

print('creating', sys.argv[1])
with zipfile.ZipFile(sys.argv[1], 'w') as zf:
    for f in sys.argv[2:]:
        print('adding', f)
        base=os.path.basename(f)
        n,e=base.split('.')
        dat=open(f,'r').read()
        zf.writestr(n, dat, zipfile.ZIP_DEFLATED)
print('done')
