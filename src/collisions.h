/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_

// See http://jgt.akpeters.com/

#define TEST_CULL

int
intersect_triangle(double orig[3], double dir[3],
                   double vert0[3], double vert1[3], double vert2[3],
                   double *t, double *u, double *v);

#endif // _COLLISIONS_H 
