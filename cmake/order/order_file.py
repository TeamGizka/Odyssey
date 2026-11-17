"""The order file of swkotor2.exe: cmake/order/swkotor2.txt, used when KOTOR_LINK_ORDER is ON.

    python cmake/order/order_file.py from-map build/tier-a/dev/game/swmain/swkotor2.map cmake/order/swkotor2.txt
    python cmake/order/order_file.py resolve cmake/order/swkotor2.txt <output> <build directory>

from-map writes the order file from the link map of a build whose functions are in the original order, such as the
tier-a build. It lists the functions of .text in address order, one decorated name per line, up to the first
function the build does not compile itself (an import thunk or the C runtime): the linker places the listed
functions first and everything else after them in its default order, which keeps the functions behind the C runtime
behind it, as in the original. Every name at an address is listed: the linker folds identical functions into one
copy and places that copy where the name of the copy it keeps is listed. C names are written without their leading
underscore, which the linker adds. Static functions cannot be named in an order file and are left out.

Names in an anonymous namespace carry a number that the compiler derives from the source file's location
(?A0x1234abcd); from-map writes it as ?A0x00000000. resolve, run by the build before the link, writes the order file
the linker reads with the numbers of this build, taken from the libraries and objects in the build directory."""
import os
import re
import sys

ROW = re.compile(r'^\s*0001:([0-9a-f]{8})\s+(\S+)\s+[0-9a-f]{8}\s+f\s+(?:i\s+)?(\S+)\s*$')
TEXT = re.compile(r'^\s*0001:([0-9a-f]{8})\s+([0-9a-f]{8})H\s+\.text\s+CODE\s*$')
ANONYMOUS = re.compile(r'\?A0x[0-9a-f]{8}@')
ANONYMOUS_NAME = re.compile(rb'\?[\x21-\x7e]*?\?A0x[0-9a-f]{8}@[\x21-\x7e]*')
UNNUMBERED = '?A0x00000000@'


def from_map(map_path, out_path):
    text_end, rows = None, []
    with open(map_path, encoding='ascii', errors='replace') as f:
        for line in f:
            if line.startswith(' Static symbols'):
                break
            m = TEXT.match(line)
            if m:
                text_end = int(m.group(1), 16) + int(m.group(2), 16)
                continue
            m = ROW.match(line)
            if m and text_end is not None and int(m.group(1), 16) < text_end:
                rows.append((int(m.group(1), 16), m.group(2), m.group(3).endswith(('.cpp.obj', '.c.obj'))))
    if text_end is None:
        raise SystemExit('%s: no .text section' % map_path)
    names = []
    for _, name, compiled in sorted(rows, key=lambda row: row[0]):
        if not compiled:
            break
        names.append(ANONYMOUS.sub(UNNUMBERED, name[1:] if name.startswith('_') else name))
    write(out_path, names)


def resolve(order_path, out_path, build_dir):
    numbered = {}
    for root, _, files in os.walk(build_dir):
        for file in files:
            if file.endswith(('.lib', '.obj')):
                with open(os.path.join(root, file), 'rb') as f:
                    for name in ANONYMOUS_NAME.findall(f.read()):
                        name = name.decode('ascii')
                        numbered.setdefault(ANONYMOUS.sub(UNNUMBERED, name), set()).add(name)
    with open(order_path, encoding='ascii') as f:
        names = f.read().split()
    for i, name in enumerate(names):
        if UNNUMBERED in name:
            found = numbered.get(name, set())
            if len(found) != 1:
                raise SystemExit('%s: %s matches %d names in %s' % (order_path, name, len(found), build_dir))
            names[i] = found.pop()
    write(out_path, names)


def write(path, names):
    text = ''.join(name + '\n' for name in names)
    try:
        with open(path, encoding='ascii', newline='') as f:
            if f.read() == text:
                return
    except OSError:
        pass
    with open(path, 'w', encoding='ascii', newline='\n') as f:
        f.write(text)


if __name__ == '__main__':
    if len(sys.argv) == 4 and sys.argv[1] == 'from-map':
        from_map(sys.argv[2], sys.argv[3])
    elif len(sys.argv) == 5 and sys.argv[1] == 'resolve':
        resolve(sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        raise SystemExit(__doc__)
