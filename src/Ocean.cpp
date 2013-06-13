/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "ccmath.h"
#include <vector>
#include <cstdio>
#include <GL/gl.h>
#include "image.h"
#include "normals.h"
#include "Ocean.h"
#include "extensions.h"
#include "Log.h"
#include "WaveEmitter.h"

#define OCEAN_TEXSCALE 6

using namespace std;


Ocean::Ocean(int sizeX, int sizeZ) {
  width  = sizeX;
  height = sizeZ;

  count  = 0;
 
  // Set up the texture
  glEnable(GL_TEXTURE_2D);
  BuildTexture("./data/water.png", &texture, GL_REPEAT, true);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // Set up the wave texture
  BuildTexture("./data/surf.png", &waveTexture, GL_CLAMP_TO_EDGE, true);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // Set up the material;
  material[0] = 0.23;
  material[1] = 0.35;
  material[2] = 0.7;
  material[3] = 0.45;

  // Generate the display list
  list = glGenLists(1);
  if(list == 0) {
    printf("Could not generate waves\n");
  }
  glNewList(list, GL_COMPILE);

  // Draw the actual water
  // Use the material
  glMaterialfv(GL_FRONT, GL_AMBIENT, material);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material);

  // Enable the texture
  glBindTexture(GL_TEXTURE_2D, texture);

  glNormal3f(0, 1, 0);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex3f(-1*width/2, 0, -1*height/2);
  glTexCoord2f(0, height/OCEAN_TEXSCALE);
  glVertex3f(-1*width/2, 0, height/2);
  glTexCoord2f(width/OCEAN_TEXSCALE, height/OCEAN_TEXSCALE);
  glVertex3f(width/2, 0, height/2);
  glTexCoord2f(width/OCEAN_TEXSCALE, 0);
  glVertex3f(width/2, 0, -1*height/2);
  glEnd();

  // Done with the list
  glEndList();

  // Generate the wave list
  WaveEmitter::generateWaveList();
}

Ocean::~Ocean() {
  // Delete the texture
  glDeleteTextures(1, &texture);
  
  // Delete the display list
  glDeleteLists(list, 1);


  // The wave emitters will be deleted by the quad tree
  waveEmitters.clear();

  WaveEmitter::deleteActiveWaves();

  WaveEmitter::deleteWaveList();

  Log::logger->message("Ocean deleted");
}

void Ocean::draw() {
  float mult  = sin(count);
  float mult2 = cos(count);

  GLfloat shift  = (.25*mult)/OCEAN_TEXSCALE;
  GLfloat shift2 = (.25*mult2*(width/height))/OCEAN_TEXSCALE;

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glBindTexture(GL_TEXTURE_2D, waveTexture);
  WaveEmitter::drawActiveWaves();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Get ready to manipulate the texture matrix
  glMatrixMode(GL_TEXTURE);
  glPushMatrix();

  // Move around the texture
  glTranslatef(shift, shift2, 0);

  // Draw the actual waves
  glCallList(list);

  // Reset the matrix
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void Ocean::update(unsigned int ticks) {
  unsigned int numEmitters = waveEmitters.size();

  count += .002*ticks;
  if(count > TWO_PI) count -= TWO_PI;

  for(unsigned int i = 0; i < numEmitters; i++) {
    waveEmitters[i]->update(ticks);
  }

  WaveEmitter::updateActiveWaves(ticks);
}

void Ocean::addEmitter(WaveEmitter*emitter) {
  // This just puts a pointer to a wave emitter into the vector
  waveEmitters.push_back(emitter);
}