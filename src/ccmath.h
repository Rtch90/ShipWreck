/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _CCMATH_H_

#define _CCMATH_H_

#include <cmath>
#include <cstdlib>

// Make sure M_PI is defined, since MSVC++ cmath doesn't define it
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#define TWO_PI (M_PI*2)

inline double radiansToDegrees(double radians) {
  return radians*(360.0/TWO_PI);

}

inline double degreesToRadians(double degrees) {
  return degrees*(TWO_PI/360.0);
}

inline double randBellCurve() {
  return ((rand()/(double)RAND_MAX)+(rand()/(double)RAND_MAX)+(rand()/(double)RAND_MAX))/ 3.0;
}

inline int randInt(int range) {
  return(int) ((float)(range + 1)*rand()/(RAND_MAX+1.0));
}

#endif
