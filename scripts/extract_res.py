"""Extract the cursors and the icon of swkotor2.exe into dev/game/swmain/res/.

icon101.ico is compiled by dev/game/swmain/swmain.rc; cursors.res, the cursors as compiled resources (their
directories are not what a resource compiler writes for .cur files), is linked as it is. Neither is distributed
with the source. Run once, with your own copy of the game executable (the Steam release, file version 2.2.0.0):

    python scripts/extract_res.py "C:/path/to/swkotor2.exe"

Every written file is checked against scripts/extract_res.sha256; a mismatch means the executable is not that
release (or its resources were modified), and the build would not reproduce the original.
"""
import hashlib
import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_DIR = os.path.join(ROOT, 'dev', 'game', 'swmain', 'res')
MANIFEST = os.path.join(ROOT, 'scripts', 'extract_res.sha256')

RT_CURSOR, RT_ICON, RT_GROUP_CURSOR, RT_GROUP_ICON = 1, 3, 12, 14


class Image:
    def __init__(self, blob):
        self.blob = blob
        if blob[:2] != b'MZ':
            raise ValueError('not an executable')
        pe = struct.unpack_from('<I', blob, 0x3C)[0]
        if blob[pe:pe + 4] != b'PE\0\0':
            raise ValueError('not a PE image')
        sections, optional_size = struct.unpack_from('<H12xH', blob, pe + 6)
        optional = pe + 24
        if struct.unpack_from('<H', blob, optional)[0] != 0x10B:
            raise ValueError('not a 32-bit Windows executable')
        self.resource_rva = struct.unpack_from('<I', blob, optional + 96 + 2 * 8)[0]
        if not self.resource_rva:
            raise ValueError('the executable has no resources')
        self.sections = []
        for i in range(sections):
            header = optional + optional_size + 40 * i
            vsize, rva, rawsize, rawptr = struct.unpack_from('<IIII', blob, header + 8)
            self.sections.append((rva, max(vsize, rawsize), rawptr))

    def read(self, rva, size):
        for start, length, rawptr in self.sections:
            if start <= rva < start + length:
                offset = rawptr + rva - start
                return self.blob[offset:offset + size]
        raise ValueError('address 0x%x outside the image' % rva)

    def resources(self):
        """{type: {(id, lang): data}}"""
        root = self.resource_rva
        table = {}

        def walk(offset, path):
            names, ids = struct.unpack_from('<HH', self.read(root + offset + 12, 4))
            for i in range(names + ids):
                ident, child = struct.unpack_from('<II', self.read(root + offset + 16 + 8 * i, 8))
                key = ident if not ident & 0x80000000 else None
                if child & 0x80000000:
                    walk(child & 0x7FFFFFFF, path + (key,))
                else:
                    rva, size = struct.unpack_from('<II', self.read(root + child, 8))
                    kind, ident_key = path[0], path[1]
                    table.setdefault(kind, {})[(ident_key, key)] = self.read(rva, size)

        walk(0, ())
        return table


def _res_entry(kind, ident, lang, flags, data):
    """One record of a .res file: header (numeric type and name) and data, padded to a DWORD boundary."""
    header = struct.pack('<IIHHHHIHHII', len(data), 32, 0xFFFF, kind, 0xFFFF, ident, 0, flags, lang, 0, 0)
    return header + data + bytes(-len(data) % 4)


def build_files(image):
    """The icon group as an .ico file and the cursors, images and directories as they are in the executable, as a
    compiled resource file: {filename: bytes}."""
    table = image.resources()
    files = {}
    for (ident, lang), data in table.get(RT_GROUP_ICON, {}).items():
        _reserved, kind, count = struct.unpack_from('<HHH', data, 0)
        entries = [struct.unpack_from('<BBBBHHIH', data, 6 + 14 * i) for i in range(count)]
        directory = b''
        body = b''
        offset = 6 + 16 * count
        for width, height, colors, reserved, planes, bits, _size, image_id in entries:
            blob = table[RT_ICON][(image_id, lang)]
            directory += struct.pack('<BBBBHHII', width, height, colors, reserved, planes, bits, len(blob), offset)
            body += blob
            offset += len(blob)
        files['icon%s.ico' % ident] = struct.pack('<HHH', 0, kind, count) + directory + body
    res = _res_entry(0, 0, 0, 0, b'')[:32]
    for (ident, lang), data in sorted(table.get(RT_GROUP_CURSOR, {}).items()):
        count = struct.unpack_from('<H', data, 4)[0]
        for i in range(count):
            image_id = struct.unpack_from('<H', data, 6 + 14 * i + 12)[0]
            res += _res_entry(RT_CURSOR, image_id, lang, 0x1010, table[RT_CURSOR][(image_id, lang)])
        res += _res_entry(RT_GROUP_CURSOR, ident, lang, 0x1030, data)
    files['cursors.res'] = res
    return files


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: python scripts/extract_res.py <path to swkotor2.exe>')
    try:
        with open(sys.argv[1], 'rb') as handle:
            files = build_files(Image(handle.read()))
    except (OSError, ValueError, KeyError, struct.error) as error:
        sys.exit('cannot read resources from %s: %s' % (sys.argv[1], error))
    expected = {}
    with open(MANIFEST, encoding='utf-8') as handle:
        for line in handle:
            if line.strip():
                digest, name = line.split()
                expected[name] = digest
    missing = sorted(set(expected) - set(files))
    if missing:
        sys.exit('the executable lacks %d expected resources (first: %s); is it swkotor2.exe?'
                 % (len(missing), missing[0]))
    wrong = [name for name in expected if hashlib.sha256(files[name]).hexdigest() != expected[name]]
    if wrong:
        sys.exit('%d resources differ from the expected release (first: %s); nothing written' % (len(wrong), wrong[0]))
    os.makedirs(OUT_DIR, exist_ok=True)
    for name in sorted(expected):
        with open(os.path.join(OUT_DIR, name), 'wb') as handle:
            handle.write(files[name])
    print('wrote %d files to %s' % (len(expected), os.path.relpath(OUT_DIR, ROOT)))


if __name__ == '__main__':
    main()
