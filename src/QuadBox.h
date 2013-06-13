/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _QUADBOX_H_
#define _QUADBOX_H_

#include <GL/gl.h>
#include "./Point.h"

class QuadBox {
public:
  // Contructor
  QuadBox();

  // Destructor
  ~QuadBox();

  Point *b1, *b2, *b3, *b4;
  Point *t1, *t2, *t3, *t4;

  void setHeight(GLfloat low, GLfloat high);

  // Debuggind purposes :)
  void draw();
  void dump();
};

#endif
