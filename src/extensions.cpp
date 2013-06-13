/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <string>
#include <GL/gl.h>
#include <GL/glext.h>
#include <SDL/SDL.h> // Should this be #include "SDL.h" ?
#include "Config.h"
#include "extensions.h"

#ifdef WIN32

PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2f = 0;
PFNGLACTIVETEXTUREARBPROC glActiveTexture = 0;
PFNGLACTIVETEXTUREARBPROC glClientActiveTexture = 0;
PFNGLDRAWRANGEELEMENTSWINPROC glDrawRangeElements = 0;
#endif

PFNGLGENBUFFERSARBPROC glGenBuffers       = 0;
PFNGLBINDBUFFERARBPROC glBindBuffer       = 0;
PFNGLBUFFERDATAARBPROC glBufferData       = 0;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffers = 0;

void initExtensions() {

#ifdef WIN32
  // Initialize the multi-texturing functions
  glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FARBPROC) SDL_GL_GetProcAddress("glMultiTexCoord2fARB");
  glActiveTexture   = (PFNGLCLIENTACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glActiveTextureARB");
  glClientActiveTexture = (PFNGLACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glClientActiveTextureARB");
  glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSWINPROC) SDL_GL_GetProcAddress("glDrawRangeElements");
#endif

  if(Config::configuration->checkVBO()) {
    glGenBuffers = (PFNGLGENBUFFERSARBPROC) SDL_GL_GetProcAddress("glGenBuffersARB");
    glBindBuffer = (PFNGLBINDBUFFERARBPROC) SDL_GL_GetProcAddress("glBindBufferARB");
    glBufferData = (PFNGLBUFFERDATAARBPROC) SDL_GL_GetProcAddress("glBufferDataARB");
    glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC) SDL_GL_GetProcAddress("glDeleteBuffersARB");
  }
}

bool checkExtension(const char *str) {
  // Get the list of supported extensions
  char* extensionList = (char*) glGetString(GL_EXTENSIONS);

  if(!str || !extensionList)
    return false;

  while(*extensionList) {
    // Find the length of the first extension substring
    unsigned int firstExtensionLength = strcspn(extensionList, " ");

    if(strlen(str) == firstExtensionLength && strncmp(str, extensionList, firstExtensionLength) == 0) {
      return true;
    }

    // Move to the next substring
    extensionList += firstExtensionLength + 1;
  }    
  return false;
}
