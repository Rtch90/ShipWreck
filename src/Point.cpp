/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <cstdio>
#include <GL/gl.h>
#include "./Point.h"

// Generic constructor
Point::Point() {
  x = 0;
  y = 0;
  z = 0;
}

// Copy constructor
Point::Point(Point *copy) {
  x = copy->x;
  y = copy->y;
  z = copy->z;
}

// Construct point at (xIn, yIn, zIn)
Point::Point(GLfloat xIn, GLfloat yIn, GLfloat zIn) {
  x = xIn;
  y = yIn;
  z = zIn;
}

void Point::dump() {
  printf("(%f, %f, %f)\n", x, y, z);
}

// Average two points, store in result
void Point::average(Point *a, Point *b, Point *result) {
  result->x = (int) (a->x + b->x)/2;
  result->y = (int) (a->y + b->y)/2;
  result->z = (int) (a->z + b->z)/2;
}

void Point::draw() {
  glVertex3f(x, y, z);
}
