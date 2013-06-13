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
#include "BoundBox.h"
#include "Point.h"

BoundBox::BoundBox() {
  b1 = NULL;
  b2 = NULL;
  b3 = NULL;
  b4 = NULL;

  t1 = NULL;
  t2 = NULL;
  t3 = NULL;
  t4 = NULL;
}

BoundBox::BoundBox(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
  b1 = new Point(minX, minY, minZ);
  b2 = new Point(maxX, minY, minZ);
  b3 = new Point(minX, minY, maxZ);
  b4 = new Point(maxX, minY, maxZ);

  t1 = new Point(minX, maxY, minZ);
  t2 = new Point(maxX, maxY, minZ);
  t3 = new Point(minX, maxY, maxZ);
  t4 = new Point(maxX, maxY, maxZ);
}

BoundBox::~BoundBox() {
  delete b1;
  delete b2;
  delete b3;
  delete b4;

  delete t1;
  delete t2;
  delete t3;
  delete t4;
}

void BoundBox::setHeight(GLfloat low, GLfloat high) {
  if ((b1 == NULL) || (b2 == NULL) || (b3 == NULL) || (b4 == NULL)) {
    printf("NULL!\n");
  }
  b1->y = low;
  b2->y = low;
  b3->y = low;
  b4->y = low;

  t1 = new Point(b1->x, high, b1->z);
  t2 = new Point(b2->x, high, b2->z);
  t3 = new Point(b3->x, high, b3->z);
  t4 = new Point(b4->x, high, b4->z);
}

void BoundBox::draw() {
  glBegin(GL_LINES);
  glNormal3f(0, 1, 0);
  b1->draw();
  b2->draw();

  b2->draw();
  b4->draw();

  b4->draw();
  b3->draw();

  b3->draw();
  b1->draw();

  b1->draw();
  t1->draw();

  b2->draw();
  t2->draw();

  b3->draw();
  t3->draw();

  b4->draw();
  t4->draw();

  t1->draw();
  t2->draw();

  t2->draw();
  t4->draw();

  t4->draw();
  t3->draw();

  t3->draw();
  t1->draw();
  glEnd();
}

void BoundBox::dump() {
  b1->dump();
  b2->dump();
  b3->dump();
  b4->dump();

  t1->dump();
  t2->dump();
  t3->dump();
  t4->dump();
  printf("\n");
}
