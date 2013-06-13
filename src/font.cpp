/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Log.h"
#include "image.h"
#include "font.h"

using namespace std;

// Display lists for characters
GLuint fontBase;

// Texture used to make characters
GLuint fontTexture;

bool BuildFont(char const *file) {
  // Holds Character Coords
  float cx;
  float cy;

  // Create a list of each character
  fontBase = glGenLists(256);
  if(fontBase == 0) {
    return false;
  }

  Log::logger->message("Font lists generated");

  // Load the font texture
  BuildTexture(file, &fontTexture, GL_CLAMP, false);
  
  Log::logger->message("Font texture loaded");  

  // Select font texture
  glBindTexture(GL_TEXTURE_2D, fontTexture);

  Log::logger->message("Font texture bound");
  
  // Create each of the 256 characters
  for(int i=0; i<256; i++) {
    /* NOTE:
     *  BMPs are stored with the top-leftmost pixel being the
     * last byte and the bottom-rightmost pixel being the first
     * byte. So an image that is displayed as
     *    1 0
     *    0 0
     * is represented data-wise like
     *    0 0
     *    0 1
     * And because SDL_LoadBMP loads the raw data without
     * translating to how it is thought of when viewed we need
     * to start at the bottom-right corner of the data and work
     * backwards to get everything properly. So the below code has
     * been modified to reflect this. Examine how this is done and
     * how the original tutorial is done to grasp the differences.
     *
     * As a side note BMPs are also stored as BGR instead of RGB
     * and that is why we load the texture using GL_BGR. It's
     * baackwards I know.
     */
    
    // x position of character
    cx = 1 - (float)(i % 16) / 16.0;
    // y position of character
    cy = 1 - (float)(i / 16) / 16.0;
    
    // Start this character's list
    glNewList(fontBase + (255 - i), GL_COMPILE );
    // Each character is a quad
    glBegin(GL_QUADS);
    // Texture Coord (Top Left)
    glTexCoord2f(cx - 0.0625f, cy - 0.0625f);
    glVertex2i(0, 0);
    
    // Texture Coord (Bottom Left)
    glTexCoord2f(cx - 0.0625, cy);
    glVertex2i(0, 16);
    
    // Texture Coord (Bottom Right)
    glTexCoord2f(cx, cy);
    glVertex2i(16, 16);
    
    // Texture Coord (Top Right)
    glTexCoord2f(cx, cy - 0.0625f);
    glVertex2i(16, 0);
    glEnd( );
    
    // Move To The Left Of The Character 
    glTranslated(10, 0, 0);
    glEndList();

    //Log::logger->message("Font character %d built", i);
  }
  
  return true;
}

void DestroyFont() {
  // Delete the display lists
  glDeleteLists(fontBase, 256);
  fontBase = 0;

  // Delete the texture
  glDeleteTextures(1, &fontTexture);
  fontTexture = 0;
}

void glPrint(char const *s, int set) {
  // Make sure set is valid
  if(set > 1) set = 1;

  // Bind the font texture
  glBindTexture(GL_TEXTURE_2D, fontTexture);

  glPushAttrib(GL_LIST_BIT);
  glListBase(fontBase - 32 + (128 * set));

  glPushMatrix();

  // Draw the actual text
  glCallLists(strlen(s), GL_BYTE, s);

  glPopMatrix();

  glPopAttrib();
}

/* This is to augment the glPrint above.
 * it allows for multiple arguments so you can display
 * text just like you would in a normal printf function.
 */

void glPrintf(int set, const char *s, ...) {

  va_list vargList; // This is to handle the variable arguments ...
  char outBuf[1024];
  unsigned short outLen;

  // This section takes the arguments and puts them into
  // the character array.
  va_start(vargList, s);
#ifdef WIN32 
  outLen = _vsnprintf(outBuf, sizeof(outBuf), s, vargList);
#else
  outLen = vsnprintf(outBuf, sizeof(outBuf), s, vargList);
#endif
  va_end(vargList);

  if(outLen >= sizeof(outBuf)) {
    // vsnprintf had to truncate the data, set outLen to correct value
    outLen = sizeof(outBuf);
  }

  // copied from glPrint()
  if (set > 1) set = 1;

  // Bind the font texture
  glBindTexture(GL_TEXTURE_2D, fontTexture);

  glPushAttrib(GL_LIST_BIT);
  glListBase(fontBase - 32 + (128 * set));

  glPushMatrix();

  // Draw the actual text
  glCallLists(strlen(outBuf), GL_BYTE, outBuf); // Changed to use correct variables

  glPopMatrix();

  glPopAttrib();
}
  

void EnterTextMode(int width, int height) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0, width, height, 0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
}

void ExitTextMode() {
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_BLEND);
}

void glNewLine() {
  glTranslatef(0, 18, 0);
}
