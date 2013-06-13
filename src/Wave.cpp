/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <iostream>
#include "ccmath.h"
#include <GL/gl.h>
#include "Log.h"
#include "Wave.h"

#define WAVE_END  -.33
#define WAVE_RATE .002

Wave::Wave(float xIn, float zIn, float angleIn) {
  counter = 0.0;

  x     = xIn;
  z     = zIn;
  angle = angleIn;

  active = true;
}

Wave::~Wave() {
}

void Wave::update(int ticks) {
  counter += WAVE_RATE *ticks;

  if(counter > 10) active = false;
}

void Wave::prepDraw() {
  float dZ;
  float alpha;

  if(counter > 8) {
    dZ = WAVE_END;
    alpha = (10 - counter) / 2;
  } else {
    dZ = 1+WAVE_END+(counter/8);
    alpha = counter/7;
  }

  glTranslatef(x, 0, z);
  glRotatef(angle, 0, 1, 0);
  glTranslatef(0, 0, dZ);

  glColor4f(1.0, 1.0, 1.0, alpha);
}

bool Wave::isActive() { 
  return active;
}
