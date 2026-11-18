#pragma once

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#include <GL/gl.h>
#else
#include "platform.h"
#endif
#include <aurlist.h>
#include "../libsource/math3d/math3d.h"

#ifndef _XBOX
template List<Vector>::List(Vector*);
#endif
template List<Vector>::~List();
