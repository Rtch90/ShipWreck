/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _EXTENSIONS_H_ 
#define _EXTENSIONS_H_

// Don't need any of this if it's not a Win32 environment
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

#ifdef WIN32
// Multitexturing functions
// Note: these are actually declared in extensions.cpp
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2f;
extern PFNGLACTIVETEXTUREARBPROC glActiveTexture;
extern PFNGLACTIVETEXTUREARBPROC glClientActiveTexture;
extern PFNGLDRAWRANGEELEMENTSWINPROC glDrawRangeElements;
#endif

// Vertex buffer object extension function pointers
extern PFNGLGENBUFFERSARBPROC glGenBuffers;
extern PFNGLBINDBUFFERARBPROC glBindBuffer;
extern PFNGLBUFFERDATAARBPROC glBufferData;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;


void initExtensions();
bool checkExtension(const char *str);

#endif
