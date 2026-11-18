#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <aurlist.h>
#include "../libsource/math3d/math3d.h"

#ifndef _XBOX
template List<Vector>::List(Vector*);
#endif
template List<Vector>::~List();
