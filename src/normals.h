/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _NORMALS_H_
#define _NORMALS_H_

double vectorLength(float vector[3]);

void normalize(float normalVector[3]);

void crossProduct(float point1[3], float point2[3], float point3[3], float normal[3]);

#endif
