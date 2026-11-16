"""Keeper-election dead filler generator (debt: keeper-election-dead-filler).

    python scripts/keeperfill/generate.py [<output dir>]      default: dev/game/swmain

Writes keeperfill_appmanager.inc, the file dev/game/swmain/appmanager.cpp includes in front of its first function.
It holds DEAD functions only (never referenced, discarded by /OPT:REF, absent from the map and from the image):
.text fillers, EH fillers and six EH helper functions; the counts are those of levers.json.

Why: LINK 7.00 /OPT:ICF elects the keeper of every folded group by an unstable qsort over ALL COMDATs of the link,
dead ones included (sort key = relocation count, raw size), once for .text and once for the EH funclets (.text$x).
The original's dead COMDAT inventory is not recoverable, so both elections are steered with dead filler of chosen
keys in front of the first link object until every keeper is the original's. The lists are solved for one frozen
tree (aurora-k2 tools/k2/icf/icf1/icfjoint.sh); any change of a COMDAT anywhere in the link needs a new solve.
Tier B literal finish with declared filler; the dead code is to be reconstructed later.

The text is about 86 MB, so it is generated: the repository keeps this script and levers.json. The output is a pure
function of levers.json (no time stamps, LF line ends, rewritten only when its content changes). CMake runs this at
configure time (dev/game/swmain/CMakeLists.txt).

Filler forms (measured under cl 13.00.9466 /O2 /Gy; names do not enter the sort key):
  .text  C  void F(int *p, ...) { if (p) { F(p); ... r calls; p[1] = 0x1001; ... m stores } }
         D  int F(...) { F(); ... r calls; return 1; }
         E  int F(...) { F(); ...; int n = F(); n = (n ^ c) + F(); ...; return n; }
         every form is variadic (never inlined) and references only itself: no undefined symbol, no data, no EH.
  EH     void F(int *p, ...) { KFillXH h0; ... KFillXB b0; ... KFillXS a0; ... if (p) F(p); }
         destructible locals; the unwind funclets form one .text$x COMDAT whose key is set by the local counts
         (levers.json eh_shapes); the function's own .text key is tuned with extra self calls and stores.
         The preamble references operator delete, then ~CExoString, which keeps appmanager.obj's undefined-symbol
         order."""
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
DEBT = 'keeper-election-dead-filler'
I = '    '


def _stores(m):
    return 7 * min(m, 31) + 10 * max(m - 31, 0)


def _len_c(r, m):
    if r == 0 and m == 0:
        return 1
    body = 6 * r + 3 * ((r + 15) // 16) + _stores(m)
    jcc = 2 if body + (1 if r else 0) <= 127 else 6
    return (5 if r else 4) + 2 + jcc + body + (2 if r else 1)


def _r16(n):
    return (n + 15) // 16 * 16


def plan(r, s):
    """Form of the .text filler with r relocations and raw size s: ('C', stores) | ('D', 0) | ('E', (a, b)) | None."""
    if s <= 0 or s % 16 or r < 0:
        return None
    m = 1 if r else 0
    while _len_c(r, m) <= s:
        if _r16(_len_c(r, m)) == s:
            return ('C', m)
        m += 1
    if r >= 1 and _r16(5 * r + 6) == s:
        return ('D', 0)
    if r >= 2:
        best = None
        for a in range(0, r):
            for b in range(0, r - a):
                if a + b == 0:
                    continue
                length = 5 * r + 5 + 5 * a + 8 * b
                if length <= s and _r16(length) == s and (best is None or a + b < best[0] + best[1]):
                    best = (a, b)
        if best:
            return ('E', best)
    return None


def text_filler(name, r, s):
    p = plan(r, s)
    if p is None:
        raise ValueError('key (%d,%d) is not hittable' % (r, s))
    form, m = p
    out = []
    if form == 'C':
        out += ['void %s(int *p, ...)' % name, '{']
        if r or m:
            out += [I + 'if (p)', I + '{']
            out += ['%s%s%s(p);' % (I, I, name)] * r
            out += ['%s%sp[%d] = 0x%x;' % (I, I, i, 0x1000 + i) for i in range(1, m + 1)]
            out.append(I + '}')
        out.append('}')
    elif form == 'D':
        out += ['int %s(...)' % name, '{']
        out += ['%s%s();' % (I, name)] * r
        out += [I + 'return 1;', '}']
    else:
        a, b = m
        out += ['int %s(...)' % name, '{']
        out += ['%s%s();' % (I, name)] * (r - 1 - a - b)
        out.append('%sint n = %s();' % (I, name))
        out += ['%sn = (n ^ %d) + %s();' % (I, 5 + i % 100, name) for i in range(a)]
        out += ['%sn = (n ^ 0x%x) + %s();' % (I, 0x1001 + i, name) for i in range(b)]
        out += [I + 'return n;', '}']
    return out


def eh_preamble():
    return ['void KFillXPre(void *m, ...)', '{', '    if (m)', '        operator delete(m);', '}',
            'struct KFillXW { CExoString s; ~KFillXW(); };', 'KFillXW::~KFillXW()', '{', '}',
            'void KFillXD(int *p, ...)', '{', '    if (p)', '    {', '        KFillXD(p);', '        p[1] = 0x1001;',
            '    }', '}',
            'struct KFillXS { int v; ~KFillXS(); };', 'struct KFillXB { int v[4]; ~KFillXB(); };',
            'struct KFillXH { int v[40]; ~KFillXH(); };',
            'KFillXS::~KFillXS()', '{', '    KFillXD(&v);', '}', 'KFillXB::~KFillXB()', '{', '    KFillXD(v);', '}',
            'KFillXH::~KFillXH()', '{', '    KFillXD(v);', '}']


def eh_decls():
    """Form 2 (levers "eh_form": 2): declarations only, no code. The six helper functions follow as ["p", i] entries."""
    return ['void KFillXPre(void *m, ...);', 'void KFillXD(int *p, ...);',
            'struct KFillXW { CExoString s; ~KFillXW(); };',
            'struct KFillXS { int v; ~KFillXS(); };', 'struct KFillXB { int v[4]; ~KFillXB(); };',
            'struct KFillXH { int v[40]; ~KFillXH(); };']


def eh_helper(i, calls=3):
    """Form 2 helper i (0..5). Helpers 0 and 1 have the ICF sort key (1,16) and stand in two (1,16) guard slots of
    the owned frame; they come first in the file: operator delete, then ~CExoString are the first undefined symbols
    of appmanager.obj, as before. Helpers 2..5 have `calls` relocations (levers "eh_helper_calls", default 3), so their keys lie above the owned key band
    (they are among the entries the root partition exchanges)."""
    if i == 0:
        return ['void KFillXPre(void *m, ...)', '{', '    operator delete(m);', '}']
    if i == 1:
        return ['KFillXW::~KFillXW()', '{', '}']
    if i == 2:
        return ['void KFillXD(int *p, ...)', '{', '    if (p)', '    {'] + ['        KFillXD(p);'] * calls + ['        p[1] = 0x1001;', '    }', '}']
    cls, arg = (('KFillXS', '&v'), ('KFillXB', 'v'), ('KFillXH', 'v'))[i - 3]
    return ['%s::~%s()' % (cls, cls), '{'] + ['    KFillXD(%s);' % arg] * calls + ['}']


def eh_filler(name, nh, nb, ns, calls=0, stores=0):
    """calls / stores (list token ["x", calls, stores]) tune the function's own .text key, measured
    (5 + calls, size(calls, stores)); the funclet key (.text$x) depends on the locals only."""
    out = ['void %s(int *p, ...)' % name, '{']
    out += ['    KFillXH h%d;' % k for k in range(nh)]
    out += ['    KFillXB b%d;' % k for k in range(nb)]
    out += ['    KFillXS a%d;' % k for k in range(ns)]
    if calls == 0 and stores == 0:
        out += ['    if (p)', '        %s(p);' % name, '}']
    else:
        out += ['    if (p)', '    {'] + ['        %s(p);' % name] * (1 + calls)
        out += ['        p[%d] = 0x%x;' % (i, 0x1000 + i) for i in range(1, stores + 1)] + ['    }', '}']
    return out


def generate(levers):
    """-> {file name: text}"""
    head = ['// GENERATED by tools/reconstruct_next/keeperfill/generate.py from levers.json. Do not edit, do not commit.',
            '// Keeper-election dead filler (debt %s): dead functions, never referenced, discarded by' % DEBT,
            '// /OPT:REF. They only steer the ICF keeper election of LINK 7.00 (.text, and .text$x through the EH fillers).']
    eh, shapes = levers['eh'], levers['eh_shapes']
    one = list(head)
    if levers.get('eh_form') == 2:
        one += eh_decls()
    nk = nx = 0
    for t in levers['appmanager']:
        if t[0] == 'k':
            one += text_filler('KFill_%d' % nk, t[1], t[2])
            nk += 1
        elif t[0] == 'pre':
            one += eh_preamble()
        elif t[0] == 'p':
            one += eh_helper(t[1], levers.get('eh_helper_calls', 3))
        elif t[0] == 'x':
            one += eh_filler('KFillX_%d' % nx, *(list(shapes['%d,%d' % tuple(eh[nx])]) + list(t[1:3])))
            nx += 1
        else:
            raise ValueError(t)
    if nx != len(eh):
        raise ValueError('EH fillers: %d placed, %d keys' % (nx, len(eh)))
    two = list(head)
    for r, s in levers['main']:
        two += text_filler('KFill_%d' % nk, r, s)
        nk += 1
    return {'keeperfill_appmanager.inc': '\n'.join(one) + '\n', 'keeperfill_main.inc': '\n'.join(two) + '\n'}


def main(argv):
    out = argv[1] if len(argv) > 1 else os.path.join(ROOT, 'dev', 'game', 'swmain')
    with open(os.path.join(HERE, 'levers.json'), encoding='ascii') as f:
        levers = json.load(f)
    os.makedirs(out, exist_ok=True)
    for name, text in sorted(generate(levers).items()):
        if name == 'keeperfill_main.inc' and not levers['main']:
            continue
        path = os.path.join(out, name)
        data = text.encode('ascii')
        try:
            with open(path, 'rb') as f:
                same = f.read() == data
        except OSError:
            same = False
        if not same:
            with open(path, 'wb') as f:
                f.write(data)
        print('keeperfill: %s %d bytes %s' % (name, len(data), 'unchanged' if same else 'written'))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
