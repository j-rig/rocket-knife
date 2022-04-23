import sys
import struct
# import io

# uint32_t signature; // 0x06054b50
# uint16_t diskNumber; // unsupported
# uint16_t centralDirectoryDiskNumber; // unsupported
# uint16_t numEntriesThisDisk; // unsupported
# uint16_t numEntries;
# uint32_t centralDirectorySize;
# uint32_t centralDirectoryOffset;
# uint16_t zipCommentLength;

erf="<LHHHHLLH"
ers=0x06054b50

print('opening', sys.argv[1])
with open(sys.argv[1],'ab') as fhout:
    # fhout.seek(0, io.SEEK_END)
    offset=fhout.tell()
    print('offset', offset)
    print('opening', sys.argv[2])
    with open(sys.argv[2],'rb') as fhin:
        dat=bytearray(fhin.read())
        i=len(dat)-struct.calcsize(erf)
        while(1):
            blk=struct.unpack_from(erf, dat, i)
            if blk[0]== ers:
                print('found end of central directory record')
                print('offset', blk[6])
                blk=list(blk)
                blk[6]+=offset
                print('new offset', blk[6])
                # dat=io.BytesIO(dat)
                struct.pack_into(erf,dat,i,*blk)
                fhout.write(dat)
                break
            i-=1

#todo mod global file headers
