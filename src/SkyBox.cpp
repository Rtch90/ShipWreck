/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdlib.h>
#include "ccmath.h"
#include <cstdio>
#include <GL/gl.h>
#include "image.h"
#include "normals.h"
#include "SkyBox.h"

// Get's index in waves array, based on (x, y) coords.
#define WAVES(X, Z) waves[((Z) * w) + (X)]

SkyBox::SkyBox(char const *texturedir) {
  // Set up the texture
  glEnable(GL_TEXTURE_2D);
  BuildTexture("./data/skybox1/FT.png", &textures[0], GL_CLAMP, false);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  BuildTexture("./data/skybox1/LT.png", &textures[1], GL_CLAMP, false);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  BuildTexture("./data/skybox1/BK.png", &textures[2], GL_CLAMP, false);
  glBindTexture(GL_TEXTURE_2D, textures[2]);
  BuildTexture("./data/skybox1/RT.png", &textures[3], GL_CLAMP, false);
  glBindTexture(GL_TEXTURE_2D, textures[3]);
  BuildTexture("./data/skybox1/UP.png", &textures[4], GL_CLAMP, false);
  glBindTexture(GL_TEXTURE_2D, textures[4]);
  BuildTexture("./data/skybox1/DN.png", &textures[5], GL_CLAMP, false);
  glBindTexture(GL_TEXTURE_2D, textures[5]);
}

SkyBox::~SkyBox() {
  // Delete the textures
  glDeleteTextures(6, &textures[0]);
}

void SkyBox::draw(float x,float y,float z) {
  //Turn our world a bit, but first push the matrixes so others won't get into a strange position :P
  glPushMatrix();

  glPushAttrib(GL_ENABLE_BIT);
  //Disable the depth test so we can draw this in infinity
  glDepthMask(false);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);

  glTranslatef(0.0f, y, 0.0f);

  //Draw some nice _large_ sky (the 100.0f scale might seem a bit large, but you have to prepare for odd camera settings too)
  glEnable(GL_TEXTURE_2D);

  glScalef(100.0f, 100.0f, 100.0f);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  const float cz = -0.0f;
  const float cx = 1.0f;
  const float r = 1.0f;

  // FRONT Side
  glBindTexture(GL_TEXTURE_2D,textures[0]);
  glBegin(GL_QUADS);
  glTexCoord2f(cx, cz); glVertex3f( -r,  r,  r);
  glTexCoord2f(cz, cz); glVertex3f(  r,  r,  r);
  glTexCoord2f(cz, cx); glVertex3f(  r, -r,  r); 
  glTexCoord2f(cx, cx); glVertex3f( -r, -r,  r);
  glEnd();

  // BACK side
  glBindTexture(GL_TEXTURE_2D,textures[2]);
  glBegin(GL_QUADS);
  glTexCoord2f(cx, cz);  glVertex3f(  r,  r, -r);
  glTexCoord2f(cz, cz);  glVertex3f( -r,  r, -r);
  glTexCoord2f(cz, cx);  glVertex3f( -r, -r, -r); 
  glTexCoord2f(cx, cx);  glVertex3f(  r, -r, -r);
  glEnd();

  // Left side
  glBindTexture(GL_TEXTURE_2D,textures[3]);
  glBegin(GL_QUADS);
  glTexCoord2f(cx,cz); glVertex3f( -r,  r, -r);
  glTexCoord2f(cz,cz); glVertex3f( -r,  r,  r); 
  glTexCoord2f(cz,cx); glVertex3f( -r, -r,  r);
  glTexCoord2f(cx,cx); glVertex3f( -r, -r, -r);
  glEnd();
 
  // Right side
  glBindTexture(GL_TEXTURE_2D,textures[1]);
  glBegin(GL_QUADS);
  glTexCoord2f(cx, cz); glVertex3f(  r,  r,  r);
  glTexCoord2f(cz, cz); glVertex3f(  r,  r, -r); 
  glTexCoord2f(cz, cx); glVertex3f(  r, -r, -r);
  glTexCoord2f(cx, cx); glVertex3f(  r, -r,  r);
  glEnd();

  // Up side
  glBindTexture(GL_TEXTURE_2D,textures[4]);
  glBegin(GL_QUADS);
  glTexCoord2f(cx, cz); glVertex3f( -r,  r, -r);
  glTexCoord2f(cz, cz); glVertex3f(  r,  r, -r); 
  glTexCoord2f(cz, cx); glVertex3f(  r,  r,  r);
  glTexCoord2f(cx, cx); glVertex3f( -r,  r,  r);
  glEnd();
 
  // Down side
  glBindTexture(GL_TEXTURE_2D,textures[5]);
  glBegin(GL_QUADS);
  glTexCoord2f(cx, cz); glVertex3f( -r, -r,  r);
  glTexCoord2f(cz, cz); glVertex3f(  r, -r,  r); 
  glTexCoord2f(cz, cx); glVertex3f(  r, -r, -r);
  glTexCoord2f(cx, cx); glVertex3f( -r, -r, -r);
  glEnd();

  //Back to where we were
  glDepthMask(true);
  glPopAttrib();
  glPopMatrix();
}
