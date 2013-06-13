/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _IMAGE_H_ 
#define _IMAGE_H_


#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#define GL_COMPRESSED_RGB 0x84ED
#include <GL/gl.h>

typedef struct {
  unsigned char imageTypeCode;
  short int     imageWidth;
  short int     imageHeight;
  unsigned char bitCount;
  unsigned char *imageData;
} TGAFILE;

// Uses SDL_image library
int BuildTexture(char const *filename, GLuint *texId, GLint param, bool genMips);

// Uses the TGAFILE struct, so use BuildTexture instead
int LoadTGAFile(char const *filename, TGAFILE *tgaFile);

// Uses the TGAFILE struct, should rewrite to use SDL
int WriteTGAFile(char const *filename, short int width, short int height, unsigned char* imageData);

void SaveScreenshot(char const *filename, int x, int y);

#endif
