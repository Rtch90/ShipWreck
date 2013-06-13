/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _WAVE_H
#define _WAVE_H

#include <GL/gl.h>

class Wave {
public:
  Wave(float xIn, float zIn, float angleIn);

  ~Wave();

  void update(int ticks);

  void prepDraw();

  bool isActive();

private:
  float x;
  float z;
  float angle;

  bool active;
  float counter;
};

#endif
