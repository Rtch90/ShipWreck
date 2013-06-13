/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _POINT_H_

#define _POINT_H_

// A point in 3d space
class Point {
public:
  // Generic constructor
  Point();

  // Copy contrustor
  Point(Point *copy);

  // Construct a point at (xIn, yIn, zIn);
  Point(GLfloat xIn, GLfloat yIn, GLfloat zIn);

  GLfloat x;
  GLfloat y;
  GLfloat z;

  // Dump the coords
  void dump();

  // Give the glVertex3f call for this point
  void draw();

  // Average two points, store the result
  static void average(Point *a, Point *b, Point *result);
};

#endif
