"""Write the Bink 1.6b and Miles import libraries swkotor2.exe links, from the DLLs the game ships.

The SDK releases that match these libraries are not at hand (3rdparty/Bink/1.6b/README.md and
3rdparty/Miles/6.6g/README.md), so each is rebuilt the way its vendor produced it: as the by-product of linking the
DLL with Visual Studio .NET 2003 (RAD built binkw32.dll 1.6b and mss32.dll with LINK 7.10). A stub DLL that exports
the names (and stdcall argument sizes) of the real export table is compiled and linked; LINK writes the import
library and the stub DLL is discarded. An import library holds no code: export names, hints (the export name table
index) and the DLL name. Rebuilding the 1.5v and 6.5h libraries the same way reproduces every import record of the
vendor files.

    python scripts/make_implibs.py "<KOTOR II folder>"     (needs VS2003_ROOT, the Visual Studio .NET 2003 folder)

reads binkw32.dll (1.6b) and Mss32.dll (6.5h) of the game folder, and 3rdparty/Miles/6.5h/lib/mss32.lib (how each
Miles export is declared); writes 3rdparty/Bink/1.6b/lib/binkw32.lib and 3rdparty/Miles/6.6g/lib/mss32.lib.
"""
import hashlib
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# The Miles 6.6g library swkotor2.exe was linked with exports three names more than mss32.dll 6.5h, which the
# game ships: two added in 6.6g and one added in 6.6f (the export table of mss32.dll 6.6g).
MILES_ADDED = ['_AIL_find_3D_provider@8', '_AIL_find_filter@8', '_AIL_set_sample_info@8']


# The DLLs of the Steam release (binkw32.dll 1.6b of 2003-12-10, Mss32.dll 6.5h).
DLL_SHA256 = {
    'binkw32.dll': 'd963112ea8545c8aaa6bd48a9d2d229605806ae1bbc03f90b0106db3034466ed',
    'Mss32.dll': '858d6066bab4445f56c2d1713f16bfa742fb43a149c6afcb83460efe5bdec7da',
}


def checked(game, name):
    path = os.path.join(game, name)
    with open(path, 'rb') as handle:
        digest = hashlib.sha256(handle.read()).hexdigest()
    if digest != DLL_SHA256[name]:
        sys.exit('%s is not the DLL of the Steam release (sha256 %s)' % (path, digest))
    return path


def exports(path):
    """export names of a DLL, in export name table order"""
    with open(path, 'rb') as handle:
        blob = handle.read()
    pe = struct.unpack_from('<I', blob, 0x3C)[0]
    sections, optional_size = struct.unpack_from('<H12xH', blob, pe + 6)
    optional = pe + 24
    export_rva = struct.unpack_from('<I', blob, optional + 96)[0]
    spans = []
    for i in range(sections):
        header = optional + optional_size + 40 * i
        vsize, rva, rawsize, rawptr = struct.unpack_from('<IIII', blob, header + 8)
        spans.append((rva, max(vsize, rawsize), rawptr))

    def at(rva):
        for start, length, rawptr in spans:
            if start <= rva < start + length:
                return rawptr + rva - start
        raise ValueError('address 0x%x outside %s' % (rva, path))

    count, names_rva = struct.unpack_from('<I4xI', blob, at(export_rva) + 24)
    out = []
    for i in range(count):
        offset = at(struct.unpack_from('<I', blob, at(names_rva) + 4 * i)[0])
        out.append(blob[offset:blob.index(b'\0', offset)].decode('ascii'))
    return out


def archive_imports(path):
    """{import name: (symbol, name type)} of the short import members of an import library"""
    with open(path, 'rb') as handle:
        blob = handle.read()
    if blob[:8] != b'!<arch>\n':
        raise ValueError('%s is not a library' % path)
    out = {}
    offset = 8
    while offset + 60 <= len(blob):
        size = int(blob[offset + 48:offset + 58])
        data = blob[offset + 60:offset + 60 + size]
        if data[:4] == b'\0\0\xff\xff':
            kind = struct.unpack_from('<H', data, 18)[0]
            symbol = data[20:data.index(b'\0', 20)].decode('ascii')
            name_type = (kind >> 2) & 7
            if name_type == 1:
                name = symbol
            elif name_type == 2:
                name = symbol.lstrip('?@_')
            else:
                name = symbol.lstrip('?@_').split('@')[0]
            out[name] = (symbol, name_type)
        offset += 60 + size + (size & 1)
    return out


def stub(names, declared):
    """the stub source and the .def lines: decorated names are dllexports, undecorated names go through the .def"""
    code, lines = [], []
    for name in names:
        symbol, name_type = declared.get(name, (name, 1))
        match = re.match(r'^_(\w+)@(\d+)$', symbol)
        if match:
            params = ', '.join('int p%d' % i for i in range(int(match.group(2)) // 4)) or 'void'
            export = '__declspec(dllexport) ' if name_type == 1 else ''
            code.append('extern "C" %svoid __stdcall %s(%s) {}' % (export, match.group(1), params))
            if name_type != 1:
                lines.append(name)
        elif re.match(r'^[A-Za-z]\w*$', name):
            code.append('extern "C" void __cdecl %s(void) {}' % name)
            lines.append(name)
        else:
            raise ValueError('export %r has no C spelling' % name)
    return '\n'.join(code) + '\n', lines


def link_import_library(dll_name, names, declared, out_lib):
    vs = os.environ.get('VS2003_ROOT')
    if not vs or not os.path.isdir(os.path.join(vs, 'Vc7', 'bin')):
        sys.exit('set VS2003_ROOT to the Visual Studio .NET 2003 folder (it holds Vc7/bin)')
    env = dict(os.environ)
    env['PATH'] = os.pathsep.join([os.path.join(vs, 'Vc7', 'bin'), os.path.join(vs, 'Common7', 'IDE'), env['PATH']])
    env['INCLUDE'] = os.path.join(vs, 'Vc7', 'include')
    env['LIB'] = os.path.join(vs, 'Vc7', 'lib')
    base = os.path.splitext(dll_name)[0].lower()
    work = tempfile.mkdtemp(prefix='implib_')
    try:
        code, lines = stub(names, declared)
        with open(os.path.join(work, 'stub.cpp'), 'w') as handle:
            handle.write(code)
        with open(os.path.join(work, 'stub.def'), 'w') as handle:
            handle.write('LIBRARY %s\nEXPORTS\n%s' % (dll_name, ''.join('  %s\n' % n for n in lines)))
        tools = os.path.join(vs, 'Vc7', 'bin')
        for command in ([os.path.join(tools, 'cl.exe'), '/nologo', '/c', '/O2', 'stub.cpp'],
                        [os.path.join(tools, 'link.exe'), '/nologo', '/DLL', '/NOENTRY', '/NODEFAULTLIB', '/MACHINE:X86', '/DEF:stub.def',
                         '/OUT:%s' % dll_name, '/IMPLIB:%s.lib' % base, 'stub.obj']):
            result = subprocess.run(command, cwd=work, env=env, capture_output=True, text=True)
            if result.returncode:
                sys.exit('%s failed:\n%s%s' % (command[0], result.stdout, result.stderr))
        os.makedirs(os.path.dirname(out_lib), exist_ok=True)
        shutil.copyfile(os.path.join(work, '%s.lib' % base), out_lib)
    finally:
        shutil.rmtree(work, ignore_errors=True)
    built = archive_imports(out_lib)
    if sorted(built) != sorted(names):
        sys.exit('%s: the import records do not match the export list' % out_lib)
    print('wrote %s (%d imports)' % (os.path.relpath(out_lib, ROOT), len(built)))


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: python scripts/make_implibs.py "<KOTOR II folder>"')
    game = sys.argv[1]
    bink = exports(checked(game, 'binkw32.dll'))
    miles = exports(checked(game, 'Mss32.dll'))
    declared = archive_imports(os.path.join(ROOT, '3rdparty', 'Miles', '6.5h', 'lib', 'mss32.lib'))
    link_import_library('binkw32.dll', sorted(bink), {},
                        os.path.join(ROOT, '3rdparty', 'Bink', '1.6b', 'lib', 'binkw32.lib'))
    link_import_library('mss32.dll', sorted(set(miles) | set(MILES_ADDED)), declared,
                        os.path.join(ROOT, '3rdparty', 'Miles', '6.6g', 'lib', 'mss32.lib'))


if __name__ == '__main__':
    main()
