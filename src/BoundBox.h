/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _BOUNDBOX_H_
#define _BOUNDBOX_H_

class Point;

class BoundBox {
public:
  // Contructor
  BoundBox();

  // Pass in max/min x/y/z
  BoundBox(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

  // Destructor
  ~BoundBox();

  Point *b1, *b2, *b3, *b4;
  Point *t1, *t2, *t3, *t4;

  void setHeight(GLfloat low, GLfloat high);

  // Debuggind purposes :)
  void draw();
  void dump();
};

#endif
