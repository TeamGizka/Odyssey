"""Generator of the swmain filler block. See README.md in this directory.

    python generate.py [--empty | --count] <output directory>

Writes keeperfill_appmanager.inc and keeperfill_main.inc, the two files dev/game/swmain/appmanager.cpp and main.cpp
include in front of their first function.

  default   KOTOR_KEEPER_FILL=ON: the unreferenced functions listed in keys.json (--count prints how many).
  --empty   KOTOR_KEEPER_FILL=OFF: comments only, no declaration, no code.

The output is a pure function of keys.json and the mode: no time stamps, LF line ends, and a file is rewritten only
when its content changes. Python standard library only.

keys.json: "appmanager" / "main" = ordered token lists for the two files:
  ["k", r, s]            function KFill_<n> whose code has r relocations and raw size s
  ["x"] | ["x", c, m]    next function KFillX_<n> with destructible locals (unwind code size = next entry of "eh",
                         locals from "eh_shapes"); c extra self calls and m stores set the size of its own code
  ["p", i]               helper function i (0..5; "eh_form": 2 declares the helper types first)
  ["pre"]                single-block helper preamble (form 1)

Function forms (sizes as compiled by cl 13.00.9466 /O2 /Gy):
  C  void F(int *p, ...) { if (p) { F(p); ... r calls; p[1] = 0x1001; ... m stores } }
  D  int F(...) { F(); ... r calls; return 1; }                     code length 5r + 6
  E  int F(...) { F(); ...; int n = F(); n = (n ^ c) + F(); ...; return n; }
  X  void F(int *p, ...) { KFillXH h0; ... KFillXB b0; ... KFillXS a0; ... if (p) F(p); }
Every function is variadic (never inlined) and references only itself and the helpers."""
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
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
    """Form 2 ("eh_form": 2): declarations only, no code. The six helper functions follow as ["p", i] entries."""
    return ['void KFillXPre(void *m, ...);', 'void KFillXD(int *p, ...);',
            'struct KFillXW { CExoString s; ~KFillXW(); };',
            'struct KFillXS { int v; ~KFillXS(); };', 'struct KFillXB { int v[4]; ~KFillXB(); };',
            'struct KFillXH { int v[40]; ~KFillXH(); };']


def eh_helper(i, calls=3):
    """Form 2 helper i (0..5). Helpers 2..5 make `calls` calls ("eh_helper_calls", default 3)."""
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


HEAD = ['// KEEPERFILL-BEGIN',
        '// Generated by cmake/keeperfill/generate.py from keys.json. Do not edit.',
        '// Nothing between the KEEPERFILL markers is game code: the functions are never referenced and the linker',
        '// discards them. See cmake/keeperfill/README.md.']
TAIL = '// KEEPERFILL-END'


def generate_empty():
    """KOTOR_KEEPER_FILL=OFF: the markers and nothing between them."""
    text = '\n'.join(HEAD + ['// Mode: KOTOR_KEEPER_FILL=OFF. Nothing is emitted.', TAIL]) + '\n'
    return {'keeperfill_appmanager.inc': text, 'keeperfill_main.inc': text}


def count(keys):
    """-> {'text', 'eh', 'helpers', 'total'}: generated functions (the form-1 preamble holds 6 helpers)."""
    tokens = keys['appmanager']
    out = {'text': sum(1 for t in tokens if t[0] == 'k') + len(keys['main']),
           'eh': sum(1 for t in tokens if t[0] == 'x'),
           'helpers': sum(1 for t in tokens if t[0] == 'p') + 6 * sum(1 for t in tokens if t[0] == 'pre')}
    out['total'] = out['text'] + out['eh'] + out['helpers']
    return out


def generate(keys):
    """-> {file name: text}"""
    head = HEAD + ['// Mode: KOTOR_KEEPER_FILL=ON.',
                   '// Unreferenced functions of chosen code sizes; they affect only which copy of identical functions',
                   '// the linker keeps when it folds them.']
    eh, shapes = keys['eh'], keys['eh_shapes']
    one = list(head)
    if keys.get('eh_form') == 2:
        one += eh_decls()
    nk = nx = 0
    for t in keys['appmanager']:
        if t[0] == 'k':
            one += text_filler('KFill_%d' % nk, t[1], t[2])
            nk += 1
        elif t[0] == 'pre':
            one += eh_preamble()
        elif t[0] == 'p':
            one += eh_helper(t[1], keys.get('eh_helper_calls', 3))
        elif t[0] == 'x':
            one += eh_filler('KFillX_%d' % nx, *(list(shapes['%d,%d' % tuple(eh[nx])]) + list(t[1:3])))
            nx += 1
        else:
            raise ValueError(t)
    if nx != len(eh):
        raise ValueError('EH functions: %d placed, %d keys' % (nx, len(eh)))
    two = list(head)
    for r, s in keys['main']:
        two += text_filler('KFill_%d' % nk, r, s)
        nk += 1
    return {'keeperfill_appmanager.inc': '\n'.join(one + [TAIL]) + '\n',
            'keeperfill_main.inc': '\n'.join(two + [TAIL]) + '\n'}


def main(argv):
    args = [a for a in argv[1:] if not a.startswith('--')]
    flags = {a for a in argv[1:] if a.startswith('--')}
    if flags - {'--empty', '--count'} or (len(args) != 1 and '--count' not in flags):
        print('usage: generate.py [--empty | --count] <output directory>', file=sys.stderr)
        return 2
    with open(os.path.join(HERE, 'keys.json'), encoding='ascii') as f:
        keys = json.load(f)
    if '--count' in flags:
        print('keeperfill: %(total)d generated functions = %(text)d plain + %(eh)d with destructible locals '
              '+ %(helpers)d helpers' % count(keys))
        return 0
    out = args[0]
    files = generate_empty() if '--empty' in flags else generate(keys)
    os.makedirs(out, exist_ok=True)
    for name, text in sorted(files.items()):
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
