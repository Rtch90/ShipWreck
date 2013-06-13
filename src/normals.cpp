/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#include "ccmath.h"
#include "normals.h"

double vectorLength(float vector[3]) {
  return sqrt((vector[0]*vector[0]) +
	      (vector[1]*vector[1]) +
	      (vector[2]*vector[2]));
}

void normalize(float normalVector[3]) {
  double length;

  length = vectorLength(normalVector);

  for (int x = 0; x < 3; x++) {
    normalVector[x] /=length;
  }
}

void crossProduct(float point1[3], float point2[3], float point3[3], float normal[3]) {
  float vector1[3], vector2[3];

  vector1[0] = point1[0] - point2[0];
  vector1[1] = point1[1] - point2[1];
  vector1[2] = point1[2] - point2[2];

  vector2[0] = point2[0] - point3[0];
  vector2[1] = point2[1] - point3[1];
  vector2[2] = point2[2] - point3[2];

  normal[0] = vector1[1]*vector2[2] - vector1[2]*vector2[1];
  normal[1] = vector1[2]*vector2[0] - vector1[0]*vector2[2];
  normal[2] = vector1[0]*vector2[1] - vector1[1]*vector2[0];
}
