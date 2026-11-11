"""Add compiled resources to a linked executable with the Win32 resource-update API.

    python release_resources.py <linked image> <resources.res> <output>

The shipped swkotor2.exe carries the marks of a post-link resource update: every resource directory has version
4.0 and the section is exactly what EndUpdateResource writes (directories, then data sorted by type, name and
language, each item on a 4-byte boundary), and its cursor groups are not what any resource compiler writes. The
cursors were therefore added after the link, through BeginUpdateResource / UpdateResource / EndUpdateResource,
which rewrites the whole .rsrc section. This step does the same: it copies the image, adds every resource of the
.res file (keeping the linked ones) and lets EndUpdateResource write the section. Windows only.
"""
import ctypes
import os
import shutil
import struct
import sys
from ctypes import wintypes


def res_entries(blob):
    """(type, name, language, data) of every record of a .res file (numeric types and names)"""
    out = []
    offset = 0
    while offset + 8 <= len(blob):
        data_size, header_size = struct.unpack_from('<II', blob, offset)
        header = blob[offset + 8:offset + header_size]
        position = 0
        ids = []
        for _ in range(2):
            if header[position:position + 2] == b'\xff\xff':
                ids.append(struct.unpack_from('<H', header, position + 2)[0])
                position += 4
            else:
                end = position
                while header[end:end + 2] != b'\0\0':
                    end += 2
                ids.append(header[position:end].decode('utf-16le'))
                position = end + 2
            position += -position % 4
        language = struct.unpack_from('<H', header, position + 6)[0]
        data = blob[offset + header_size:offset + header_size + data_size]
        if data_size or ids[0] != 0:
            out.append((ids[0], ids[1], language, data))
        offset += header_size + data_size
        offset += -offset % 4
    return out


def main(argv):
    if len(argv) != 4:
        sys.exit(__doc__)
    linked, res, output = argv[1:]
    with open(res, 'rb') as handle:
        entries = res_entries(handle.read())
    output = os.path.abspath(output)
    shutil.copyfile(linked, output)
    kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel32.BeginUpdateResourceW.restype = wintypes.HANDLE
    kernel32.BeginUpdateResourceW.argtypes = [wintypes.LPCWSTR, wintypes.BOOL]
    kernel32.UpdateResourceW.argtypes = [wintypes.HANDLE, ctypes.c_void_p, ctypes.c_void_p, wintypes.WORD,
                                         ctypes.c_void_p, wintypes.DWORD]
    kernel32.EndUpdateResourceW.argtypes = [wintypes.HANDLE, wintypes.BOOL]
    handle = kernel32.BeginUpdateResourceW(output, False)
    if not handle:
        sys.exit('BeginUpdateResource failed (%d)' % ctypes.get_last_error())

    def ident(value):
        return ctypes.c_void_p(value) if isinstance(value, int) else ctypes.cast(ctypes.c_wchar_p(value),
                                                                                 ctypes.c_void_p)
    keep = []
    for kind, name, language, data in entries:
        buffer = ctypes.create_string_buffer(data, len(data))
        keep.append(buffer)
        if not kernel32.UpdateResourceW(handle, ident(kind), ident(name), language, buffer, len(data)):
            kernel32.EndUpdateResourceW(handle, True)
            sys.exit('UpdateResource %s/%s failed (%d)' % (kind, name, ctypes.get_last_error()))
    if not kernel32.EndUpdateResourceW(handle, False):
        sys.exit('EndUpdateResource failed (%d)' % ctypes.get_last_error())
    print('%s: %d resources added' % (output, len(entries)))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
