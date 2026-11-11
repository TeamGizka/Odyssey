"""Write the release form of a linked executable.

    python release_header.py --time-date-stamp <value> --checksum <value> <linked image> <output>

A shipped executable differs from the linker's output in three header fields; they are settings of the release
process, not code, and are applied here:

  * the bytes from file offset 0x40 up to the PE header (the DOS stub program and the linker's tool signature) are zero;
  * the PE TimeDateStamp is the release link time (--time-date-stamp);
  * the PE CheckSum is --checksum.

Nothing else is changed. Values are decimal or 0x-prefixed hexadecimal. Prints the PE header offset and the SHA-256
of the output.
"""
import argparse
import hashlib
import os
import struct
import sys

STUB_START = 0x40


def release_image(linked, time_date_stamp, checksum):
    pe = struct.unpack_from('<I', linked, 0x3C)[0]
    if pe < STUB_START or linked[pe:pe + 4] != b'PE\0\0':
        raise ValueError('no PE header at %#x' % pe)
    out = bytearray(linked)
    out[STUB_START:pe] = bytes(pe - STUB_START)
    struct.pack_into('<I', out, pe + 8, time_date_stamp)
    struct.pack_into('<I', out, pe + 0x58, checksum)
    return pe, bytes(out)


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--time-date-stamp', required=True, type=lambda v: int(v, 0))
    ap.add_argument('--checksum', required=True, type=lambda v: int(v, 0))
    ap.add_argument('linked')
    ap.add_argument('output')
    a = ap.parse_args(argv[1:])
    with open(a.linked, 'rb') as f:
        pe, data = release_image(f.read(), a.time_date_stamp, a.checksum)
    os.makedirs(os.path.dirname(os.path.abspath(a.output)), exist_ok=True)
    with open(a.output, 'wb') as f:
        f.write(data)
    print('%s  PE header at %#x  sha256 %s' % (a.output, pe, hashlib.sha256(data).hexdigest()))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
