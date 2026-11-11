# Miles Sound System 6.5h

The SDK the game links against: `include/mss.h` and the import library `lib/mss32.lib`.

`include/eax.h` is the EAX 3.0 header (`m3d/ds3dbase/eax3.h` in the Miles source): exosoundinternal.cpp uses its
`EAXLISTENERPROPERTIES`, and the `EAX_ENVIRONMENT_*` enum of mss.h is guarded by `#ifndef EAX_H_INCLUDED` for this
include order.
