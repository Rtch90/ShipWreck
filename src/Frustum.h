/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

class BoundBox;

// Frustum used for culling quadtree
class Frustum {
public:
  // Extracts projection data
  void getFrustum();

  bool checkBox(BoundBox *box);

private:
  float frustum[6][4];
};

#endif
