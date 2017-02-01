#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLu.h>
#pragma comment(lib, "opengl32") //Link in the OpenGL32.lib static library
#pragma comment(lib, "GLu32") //Link in the GLu32.lib static library
#include "Thirdparty/OpenGL/glext.h"
#include "Thirdparty/OpenGL/wglext.h"

extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;