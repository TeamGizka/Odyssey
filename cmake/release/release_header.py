"""Write the release form of a linked swkotor.exe.

    python release_header.py <linked swkotor.exe> <output>

The shipped 1.03 executable differs from the linker's output in three header fields only; they are settings of the
release process, not code, and are applied here:

  * file offsets 0x40..0x10F (the DOS stub program and the linker's tool signature) are zero;
  * the PE TimeDateStamp is the release link time, 2004-02-12 18:15:53 UTC;
  * the PE CheckSum is 0.

Nothing else is changed. The PE header must start at file offset 0x110, as it does in every link of this tree.
Prints the SHA-256 of the output.
"""
import hashlib
import os
import struct
import sys

STUB_START = 0x40
PE_OFFSET = 0x110
TIME_DATE_STAMP = 0x402BC2D9
CHECKSUM = 0


def release_image(linked):
    pe = struct.unpack_from('<I', linked, 0x3C)[0]
    if pe != PE_OFFSET or linked[pe:pe + 4] != b'PE\0\0':
        raise ValueError('PE header at %#x, expected %#x' % (pe, PE_OFFSET))
    out = bytearray(linked)
    out[STUB_START:PE_OFFSET] = bytes(PE_OFFSET - STUB_START)
    struct.pack_into('<I', out, pe + 8, TIME_DATE_STAMP)
    struct.pack_into('<I', out, pe + 0x58, CHECKSUM)
    return bytes(out)


def main(argv):
    if len(argv) != 3:
        print(__doc__, file=sys.stderr)
        return 2
    with open(argv[1], 'rb') as f:
        data = release_image(f.read())
    os.makedirs(os.path.dirname(os.path.abspath(argv[2])), exist_ok=True)
    with open(argv[2], 'wb') as f:
        f.write(data)
    print('%s  sha256 %s' % (argv[2], hashlib.sha256(data).hexdigest()))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
