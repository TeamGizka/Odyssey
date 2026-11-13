"""Write the release form of a linked executable.

    python release_header.py --time-date-stamp <value> --checksum <value> [--clear-stub] [--up-system-only]
        <linked image> <output>

A shipped executable differs from the linker's output in its header fields; they are settings of the release
process, not code, and are applied here:

  * the PE TimeDateStamp is the release link time (--time-date-stamp);
  * the PE CheckSum is --checksum;
  * with --clear-stub, the bytes from file offset 0x40 up to the PE header (the DOS stub program and the linker's
    tool signature) are zero, as in the copy-protected release;
  * with --up-system-only, FILE_HEADER.Characteristics carries IMAGE_FILE_UP_SYSTEM_ONLY (0x4000), the uniprocessor
    mark of the Steam build 427.

Nothing else is changed. Values are decimal or 0x-prefixed hexadecimal. Prints the PE header offset and the SHA-256
of the output.
"""
import argparse
import hashlib
import os
import struct
import sys

STUB_START = 0x40
IMAGE_FILE_UP_SYSTEM_ONLY = 0x4000


def release_image(linked, time_date_stamp, checksum, clear_stub=False, up_system_only=False):
    pe = struct.unpack_from('<I', linked, 0x3C)[0]
    if pe < STUB_START or linked[pe:pe + 4] != b'PE\0\0':
        raise ValueError('no PE header at %#x' % pe)
    out = bytearray(linked)
    if clear_stub:
        out[STUB_START:pe] = bytes(pe - STUB_START)
    struct.pack_into('<I', out, pe + 8, time_date_stamp)
    if up_system_only:
        characteristics = struct.unpack_from('<H', out, pe + 0x16)[0]
        struct.pack_into('<H', out, pe + 0x16, characteristics | IMAGE_FILE_UP_SYSTEM_ONLY)
    struct.pack_into('<I', out, pe + 0x58, checksum)
    return pe, bytes(out)


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--time-date-stamp', required=True, type=lambda v: int(v, 0))
    ap.add_argument('--checksum', required=True, type=lambda v: int(v, 0))
    ap.add_argument('--clear-stub', action='store_true')
    ap.add_argument('--up-system-only', action='store_true')
    ap.add_argument('linked')
    ap.add_argument('output')
    a = ap.parse_args(argv[1:])
    with open(a.linked, 'rb') as f:
        pe, data = release_image(f.read(), a.time_date_stamp, a.checksum, a.clear_stub,
                                 a.up_system_only)
    os.makedirs(os.path.dirname(os.path.abspath(a.output)), exist_ok=True)
    with open(a.output, 'wb') as f:
        f.write(data)
    print('%s  PE header at %#x  sha256 %s' % (a.output, pe, hashlib.sha256(data).hexdigest()))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
