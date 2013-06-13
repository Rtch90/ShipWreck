/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL_image.h>
#include "Log.h"
#include "Config.h"
#include "image.h"

using namespace std;

// Based on Nehe Lesson 6
int BuildTexture(char const *filename, GLuint *texId, GLint param, bool genMips) {
  // Create storage space for the texture
  SDL_Surface *TextureImage; 

  // Format to pass to texture generation function
  GLint textureFormat;

  // Load the image, check for errors, if image is not found quit
  TextureImage = IMG_Load(filename);

  if(!TextureImage) {
     Log::logger->message("Warning: Could not load %s", filename);
	 return false;
  }

  // Create the texture
  glGenTextures(1, texId);
  
  // Typical texture generation using data from the bitmap
  glBindTexture(GL_TEXTURE_2D, *texId);
  
  // Setup filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if(Config::configuration->checkCompressTextures()) {
    textureFormat = GL_COMPRESSED_RGB;
  } else {
    textureFormat = GL_RGB;
  }
  
  if(genMips) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		    GL_LINEAR_MIPMAP_LINEAR);
    // Generate the texture and mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, textureFormat, TextureImage->w,
		      TextureImage->h, GL_RGB, GL_UNSIGNED_BYTE,
		      TextureImage->pixels);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		    GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, TextureImage->w, 
    		 TextureImage->h, 0, GL_RGB, GL_UNSIGNED_BYTE, 
    		 TextureImage->pixels);
  }
  
  // Free up memory we used
  SDL_FreeSurface(TextureImage);
  
  return true;
}

// LoadTGAFile()
// desc: loads the TGA file "filename" into the tgaFile data structure
int LoadTGAFile(char const *filename, TGAFILE *tgaFile) {
  FILE *filePtr;
  unsigned char   ucharBad;	// garbage unsigned char data
  short int	  sintBad;	// garbage short int data
  long		  imageSize;	// size of the TGA image
  int		  colorMode;	// 4 for RGBA or 3 for RGB
  long		  imageIdx;	// counter variable
  unsigned char   colorSwap;	// swap variable
  
  // open the TGA file
  filePtr = fopen(filename, "rb");
  if(!filePtr)
    return 0;
  
  // read first two bytes of garbage
  fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
  fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
  
  // read in the image type
  fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr);
  
  // for our purposes, the image type should be either a 2 (color) or a 3 (greyscale)
  if((tgaFile->imageTypeCode != 2) && (tgaFile->imageTypeCode != 3))
    {
      fclose(filePtr);
      return 0;
    }
  
  // read 13 bytes of garbage data
  fread(&sintBad, sizeof(short int), 1, filePtr);
  fread(&sintBad, sizeof(short int), 1, filePtr);
  fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
  fread(&sintBad, sizeof(short int), 1, filePtr);
  fread(&sintBad, sizeof(short int), 1, filePtr);
  
  // read image dimensions
  fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr);
  fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr);
  
  // read image bit depth
  fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr);
  
  // read 1 byte of garbage data
  fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
  
  // colorMode -> 3 = BGR, 4 = BGRA
  colorMode = tgaFile->bitCount / 8;
  imageSize = tgaFile->imageWidth * tgaFile->imageHeight * colorMode;
  
  // allocate memory for image data
  tgaFile->imageData = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);
  
  // read in image data
  fread(tgaFile->imageData, sizeof(unsigned char), imageSize, filePtr);
  
  // change BGR to RGB so OpenGL can read the image data
  for(imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
      colorSwap = tgaFile->imageData[imageIdx];
      tgaFile->imageData[imageIdx] = tgaFile->imageData[imageIdx + 2];
      tgaFile->imageData[imageIdx + 2] = colorSwap;
    }
  
  // close the file
  fclose(filePtr);
  
  return 1;
}

// saves an array of pixels as a TGA image
int WriteTGAFile(char const *filename, short int width, short int height, unsigned char* imageData) {
  unsigned char byteSkip;  // used to fill in the data fields that we don't care about
  short int     shortSkip;
  unsigned char imageType; // type of image we're writing to file
  int           colorMode;
  unsigned char colorSwap;
  int		imageIdx;
  unsigned char bitDepth;
  long		imageSize;
  FILE *filePtr;
  
  // create file for writing binary mode
  filePtr = fopen(filename, "wb");
  if(!filePtr)
    {
      fclose(filePtr);
      return 0;
    }
  
  imageType = 2;  // RGB, uncompressed
  bitDepth  = 24; // 24-bitdepth
  colorMode = 3;  // RGB color mode
  
  byteSkip = 0;
  shortSkip = 0;
  
  // write 2 bytes of blank data
  fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);
  fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);
  
  // write imageType
  fwrite(&imageType, sizeof(unsigned char), 1, filePtr);
  
  fwrite(&shortSkip, sizeof(short int), 1, filePtr);
  fwrite(&shortSkip, sizeof(short int), 1, filePtr);
  fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);
  fwrite(&shortSkip, sizeof(short int), 1, filePtr);
  fwrite(&shortSkip, sizeof(short int), 1, filePtr);
  
  // write image dimensions
  fwrite(&width, sizeof(short int), 1, filePtr);
  fwrite(&height, sizeof(short int), 1, filePtr);
  fwrite(&bitDepth, sizeof(unsigned char), 1, filePtr);
  
  // write 1 byte of blank data
  fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);
  
  // calculate the image size
  imageSize = width * height * colorMode;
  
  // change image data from RGB to BGR
  for(imageIdx = 0; imageIdx < imageSize ; imageIdx += colorMode) 
    {
      colorSwap = imageData[imageIdx];
      imageData[imageIdx] = imageData[imageIdx + 2];
      imageData[imageIdx + 2] = colorSwap;
    }
  
  // write the image data
  fwrite(imageData, sizeof(unsigned char), imageSize, filePtr);
  
  // close the file
  fclose(filePtr);
  
  return 1;
}

// SaveScreenshot()
// desc: takes a snapshot of the current window contents by retrieving
//       the screen data with glReadPixels() and writing the data to a file
void SaveScreenshot(char const *filename, int x, int y) {
  void *imageData;
  
  imageData = malloc(x*y*3);		// allocate memory for the imageData
  memset(imageData, 0, x*y*3);	// clear imageData memory contents
  
  // read the image data from the window
  glReadPixels(0, 0, x-1, y-1, GL_RGB, GL_UNSIGNED_BYTE, imageData);
  
  // write the image data to a file
  WriteTGAFile(filename, x, y, (unsigned char*)imageData);
  
  // free the image data memory
  free(imageData);
}
