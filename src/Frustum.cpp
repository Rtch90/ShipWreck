/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <GL/gl.h>
#include "Point.h"
#include "BoundBox.h"
#include "Frustum.h"

void Frustum::getFrustum() {
  float proj[16];
  float modl[16];
  float clip[16];
  //float t;
  
  // Get the current PROJECTION matrix
  glGetFloatv(GL_PROJECTION_MATRIX, proj);
  
  // Get the current MODELVIEW matrix
  glGetFloatv(GL_MODELVIEW_MATRIX, modl);
  
  // Combine the two matrices (multiply projection by modelview)
  clip[0] = modl[0]*proj[0] + modl[1]*proj[4] + modl[2]*proj[8]  + modl[3]*proj[12];
  clip[1] = modl[0]*proj[1] + modl[1]*proj[5] + modl[2]*proj[9]  + modl[3]*proj[13];
  clip[2] = modl[0]*proj[2] + modl[1]*proj[6] + modl[2]*proj[10] + modl[3]*proj[14];
  clip[3] = modl[0]*proj[3] + modl[1]*proj[7] + modl[2]*proj[11] + modl[3]*proj[15];
  
  clip[4] = modl[4]*proj[0] + modl[5]*proj[4] + modl[6]*proj[8]  + modl[7]*proj[12];
  clip[5] = modl[4]*proj[1] + modl[5]*proj[5] + modl[6]*proj[9]  + modl[7]*proj[13];
  clip[6] = modl[4]*proj[2] + modl[5]*proj[6] + modl[6]*proj[10] + modl[7]*proj[14];
  clip[7] = modl[4]*proj[3] + modl[5]*proj[7] + modl[6]*proj[11] + modl[7]*proj[15];
  
  clip[8]  = modl[8]*proj[0] + modl[9]*proj[4] + modl[10]*proj[8]  + modl[11]*proj[12];
  clip[9]  = modl[8]*proj[1] + modl[9]*proj[5] + modl[10]*proj[9]  + modl[11]*proj[13];
  clip[10] = modl[8]*proj[2] + modl[9]*proj[6] + modl[10]*proj[10] + modl[11]*proj[14];
  clip[11] = modl[8]*proj[3] + modl[9]*proj[7] + modl[10]*proj[11] + modl[11]*proj[15];
  
  clip[12] = modl[12]*proj[0] + modl[13]*proj[4] + modl[14]*proj[8]  + modl[15]*proj[12];
  clip[13] = modl[12]*proj[1] + modl[13]*proj[5] + modl[14]*proj[9]  + modl[15]*proj[13];
  clip[14] = modl[12]*proj[2] + modl[13]*proj[6] + modl[14]*proj[10] + modl[15]*proj[14];
  clip[15] = modl[12]*proj[3] + modl[13]*proj[7] + modl[14]*proj[11] + modl[15]*proj[15];
  
  // Extract the numbers for the RIGHT plane
  frustum[0][0] = clip[3]  - clip[0];
  frustum[0][1] = clip[7]  - clip[4];
  frustum[0][2] = clip[11] - clip[8];
  frustum[0][3] = clip[15] - clip[12];
  
  // Normalize the result (not needed for boxes)
  /*
    t = sqrt(frustum[0][0]*frustum[0][0] + frustum[0][1]*frustum[0][1] + frustum[0][2]*frustum[0][2]);
    frustum[0][0] /= t;
    frustum[0][1] /= t;
    frustum[0][2] /= t;
    frustum[0][3] /= t;
  */
  
  // Extract the numbers for the LEFT plane
  frustum[1][0] = clip[3]  + clip[0];
  frustum[1][1] = clip[7]  + clip[4];
  frustum[1][2] = clip[11] + clip[ 8];
  frustum[1][3] = clip[15] + clip[12];
  
  // Normalize the result (not needed for boxes)
  
  // Extract the BOTTOM plane
  frustum[2][0] = clip[3]  + clip[1];
  frustum[2][1] = clip[7]  + clip[5];
  frustum[2][2] = clip[11] + clip[9];
  frustum[2][3] = clip[15] + clip[13];
  
  // Normalize the result (not needed for boxes)
  
  // Extract the TOP plane
  frustum[3][0] = clip[3]  - clip[1];
  frustum[3][1] = clip[7]  - clip[5];
  frustum[3][2] = clip[11] - clip[ 9];
  frustum[3][3] = clip[15] - clip[13];
  
  // Normalize the result (not needed for boxes)
  
  // Extract the FAR plane
  frustum[4][0] = clip[3]  - clip[2];
  frustum[4][1] = clip[7]  - clip[6];
  frustum[4][2] = clip[11] - clip[10];
  frustum[4][3] = clip[15] - clip[14];
  
  // Normalize the result (not needed for boxes)
  
  // Extract the NEAR plane
  frustum[5][0] = clip[3]  + clip[2];
  frustum[5][1] = clip[7]  + clip[6];
  frustum[5][2] = clip[11] + clip[10];
  frustum[5][3] = clip[15] + clip[14];
  
  // Normalize the result (not needed for boxes)
}

bool Frustum::checkBox(BoundBox *box) {
  // Check each frustum plane
  for(int p=0; p<6; p++) {
    // Check each point of the box
    // Bottom points
    if(frustum[p][0]*box->b1->x + frustum[p][1]*box->b1->y + frustum[p][2]*box->b1->z + frustum[p][3] > 0)
      continue;
    if(frustum[p][0]*box->b2->x + frustum[p][1]*box->b2->y + frustum[p][2]*box->b2->z + frustum[p][3] > 0)
      continue;
    if(frustum[p][0]*box->b3->x + frustum[p][1]*box->b3->y + frustum[p][2]*box->b3->z + frustum[p][3] > 0)
      continue;
    if(frustum[p][0]*box->b4->x + frustum[p][1]*box->b4->y + frustum[p][2]*box->b4->z + frustum[p][3] > 0)
      continue;

    // Top points
    if(frustum[p][0]*box->t1->x + frustum[p][1]*box->t1->y + frustum[p][2]*box->t1->z + frustum[p][3] > 0)
      continue;
    if(frustum[p][0]*box->t2->x + frustum[p][1]*box->t2->y + frustum[p][2]*box->t2->z + frustum[p][3] > 0)
      continue;
    if(frustum[p][0]*box->t3->x + frustum[p][1]*box->t3->y + frustum[p][2]*box->t3->z + frustum[p][3] > 0)
      continue;
    if(frustum[p][0]*box->t4->x + frustum[p][1]*box->t4->y + frustum[p][2]*box->t4->z + frustum[p][3] > 0)
      continue;

    // It's not in front of ANY plane, return false
    return false;
  }

  // Got through all the checks, it passed
  return true;
}
